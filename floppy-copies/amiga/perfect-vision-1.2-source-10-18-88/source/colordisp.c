
/****** USE -b0 WHEN COMPILING THIS MODULE ******/

#include <intuition/intuition.h>
#include "fcntl.h"
     
/* Defines for display package */
     
#define RED    0
#define GREEN  1
#define BLUE   2
#define LUTSIZE 16
     
/* Local defines */
     
/* #define NLINES 400         Number of lines to display */
#define NBUFS  200         /* Number of lines in buffer */
#define NSAMPS 320         /* Number of samples to display */
#define LINELEN 160        /* Number of samples in line byte packed */
     
struct NewScreen ns = {
   0, 0, 320, 100,      /* size */
   6,                   /* depth */
   0, 0,                /* pens */
   HAM,                 /* mode */
   SCREENQUIET, 0, 0, 0, 0
};
     
struct NewWindow nw = {
   0, 0, 320, 100,
   1, 2,
   0,  /** was MOUSEBUTTONS **/
   BORDERLESS|BACKDROP,
   0,0,0,0,0,
   0,0,0,0,
   CUSTOMSCREEN
};
     
extern struct Window *window;
extern struct Screen *screen;
     
/* int rf, gf, bf; */
/* struct ColorMap *cm; */
     
colordisp(nlines, rbuf, gbuf, bbuf, fast, lockpalette, numlock)

UBYTE *rbuf;
UBYTE *gbuf;
UBYTE *bbuf;
int nlines;
short fast; /** Ham speed **/
USHORT lockpalette[];
int numlock;

{
     
USHORT colormap[16];
short line;
long bufn;
int addr;
VOID init_disp(), lock_colors(), disp_sort();

init_disp(NSAMPS);
lock_colors(lockpalette, numlock);

bufn = 0;
addr = 0;

if (fast) {
   for (line=0; line<nlines; line++) {
      disp_noreg(&rbuf[bufn], &gbuf[bufn], &bbuf[bufn],
                window->RPort->BitMap->Planes, &addr);
      bufn = bufn + LINELEN;
   }
}

else {      /** better ham! **/
   bufn = 0;
     
   for (line=0; line<nlines; line++) {
      disp_analyze(&rbuf[bufn], &gbuf[bufn], &bbuf[bufn]);
      bufn = bufn + LINELEN;
   }
     
   disp_sort(colormap,16);
     
   LoadRGB4(&screen->ViewPort, colormap, 16);
      
   addr = 0;
   bufn = 0;
     
   for (line=0; line<nlines; line++) {
      disp_line(&rbuf[bufn], &gbuf[bufn], &bbuf[bufn],
                window->RPort->BitMap->Planes, &addr);
      bufn = bufn + LINELEN;
   }
}
   
return(TRUE);
}


/******************************************/
/** This routine reports error msgs      **/
/******************************************/
     
error(s)
char *s;
{
extern struct Screen *menu_screen;

if (s) {
   pop_up();
   telluser2(menu_screen,"Error doing color:",s);
   pop_down();
   }

return(0);
}
     

/*######################################################################*/
/* HAM Delta Analysis and Display Package                               */
/* Written by Bob Deen                                                  */
/* Aug. 14, 1987                                                        */
/*                                                                      */
/* This is a package of routines that analyzes lines of Red, Green, and */
/* Blue data (8 bits per color), assigns color registers to the colors  */
/* with the highest delta values, and displays the image in a HAM       */
/* RastPort.  Delta values are the sum of the differences for each      */
/* color between the desired color and the best that HAM can provide.   */
/* Each value is run through a look-up table (lut) for things like      */
/* contrast enhancement.                                                */
/*######################################################################*/

#define ABS(x) (((x)>=0) ? (x) : (-(x)))
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
#define MIN(x,y) (((x) <= (y)) ? (x) : (y))
#define L(x) (MAX(MIN(x,15),0))

/* Global variables for delta analysis package */

int colordelta[16][16][16];
UBYTE colormapr[32], colormapg[32], colormapb[32];
UBYTE colordifftbl[4096], colorregtbl[4096];
UBYTE lutred[LUTSIZE], lutgreen[LUTSIZE], lutblue[LUTSIZE];
UBYTE savearea[32];
UWORD lutfullcolor[16][16][16];
short nsamps;

/************************************************************************/
/* init_disp(ns);                                                       */
/*                                                                      */
/* Initialize delta values, set up for ns samples per line, and init    */
/* lut's to a linear ramp.                                              */
/************************************************************************/

VOID init_disp(ns)
int ns;
{
short i, j, k;

nsamps = ns;

for (i=0; i<16; i++) {
   for (j=0; j<16; j++) {
      for (k=0; k<16; k++) {
         colordelta[i][j][k] = 0;
      }
   }
}
}


/************************************************************************/
/* lock_colors(pal, n);                                                 */
/*                                                                      */
/* Locks the first n colors into the color palette.  Must be called     */
/* right after init_disp.                                               */
/************************************************************************/

VOID lock_colors(pal, n)
USHORT pal[];
int n;
{
short i;
int r, g, b;

for (i=0; i<=n; i++) {
   r = (pal[i] >> 8) & 0x0F;
   g = (pal[i] >> 4) & 0x0F;
   b = (pal[i])      & 0x0F;
   colordelta[r][g][b] = 4000000 * (n-i+1);
}
}


/************************************************************************/
/* disp_set_lut(lut, table);                                            */
/*                                                                      */
/* Set lut number lut to the array table.                               */
/************************************************************************/

VOID disp_set_lut(lut, table)
int lut;
UBYTE *table;
{
int i;
UBYTE *lutdest;

if (lut == RED)
   lutdest = lutred;
else if (lut == GREEN)
   lutdest = lutgreen;
else
   lutdest = lutblue;

for (i=0; i<LUTSIZE; i++)
   lutdest[i] = table[i];

}


/************************************************************************/
/* disp_analyze(rb, gb, bb);                                            */
/*                                                                      */
/* Accumulate delta values for a single line.  Lines are byte format.   */
/************************************************************************/
/* replaced with assembler
VOID disp_analyze(rb, gb, bb)
UBYTE *rb, *gb, *bb;
{
short i;
short red, green, blue;
short rpix, gpix, bpix;
short rd, gd, bd;

red = 0;
green = 0;
blue = 0;

for (i=0; i<nsamps; i++) {

   rpix = lutred[rb[i]];
   gpix = lutgreen[gb[i]];
   bpix = lutblue[bb[i]];

   rd = ABS(red-rpix);
   gd = ABS(green-gpix);
   bd = ABS(blue-bpix);

   colordelta[rpix][gpix][bpix] += rd+gd+bd-MAX(rd,MAX(gd,bd));

   if (rd >= gd && rd >= bd)
      red = rpix;
   else if (gd >= bd)
      green = gpix;
   else
      blue = bpix;
}
}
*/

/************************************************************************/
/* disp_sort(cmap);                                                     */
/*                                                                      */
/* Sort the delta values in order of decreasing delta, and return the   */
/* top 15 in color map cmap.  Color 0 is always black.                  */
/************************************************************************/
VOID disp_sort(cmap,len)
USHORT *cmap;
int len;
{
short i, j, k, l;
int red, green, blue;
int max;
VOID disp_precalcreg(), disp_saturate();

for (i=0; i<len; i++) {
   if (i == 0) {
      red = 0;
      green = 0;
      blue = 0;
   }
   else {
      max = 0;
      for (j=0; j<16; j++)
         for (k=0; k<16; k++)
            for (l=0; l<16; l++)
               if (colordelta[j][k][l] > max) {
                  max = colordelta[j][k][l];
                  red = j;
                  green = k;
                  blue = l;
               }
   }
   colordelta[red][green][blue] = 0;

/*** THIS three stuff is optional ***/

   for (j=-3; j<=3; j++) {
      colordelta[L(red+j)][green][blue] >>= 1;
      colordelta[red][L(green+j)][blue] >>= 1;
      colordelta[red][green][L(blue+j)] >>= 1;
   }

   for (j=-2; j<=2; j++) {
      colordelta[L(red+j)][green][blue] >>= 1;
      colordelta[red][L(green+j)][blue] >>= 1;
      colordelta[red][green][L(blue+j)] >>= 1;
   }

   for (j=-1; j<=1; j++) {
      colordelta[L(red+j)][green][blue] >>= 1;
      colordelta[red][L(green+j)][blue] >>= 1;
      colordelta[red][green][L(blue+j)] >>= 1;
   }

   colormapr[i] = red;
   colormapg[i] = green;
   colormapb[i] = blue;
   cmap[i] = (red<<8) | (green<<4) | (blue);
  /* printf("cmap[%d] = %x\n", i, cmap[i]);   */ /*!!!!*/
}

disp_precalcreg(len);
}


/************************************************************************/
/* disp_precalcreg();                                                   */
/************************************************************************/

VOID disp_precalcreg(len)
int len;
{
short i, j;
short rpix, gpix, bpix;
short testrd, testgd, testbd;
short mindiff, testdiff;
short creg;

for (rpix=0; rpix<16; rpix++) {
   for (gpix=0; gpix<16; gpix++) {
      for (bpix=0; bpix<16; bpix++) {
         i = (rpix<<8) | (gpix<<4) | bpix;

/* Search for best color register match */

         mindiff = 4096;
         for (j=0; j<len; j++) {
            testrd = ABS((int)(colormapr[j] - rpix));
            testgd = ABS((int)(colormapg[j] - gpix));
            testbd = ABS((int)(colormapb[j] - bpix));
            testdiff = testrd+testgd+testbd;
            if (testdiff < mindiff) {
               mindiff = testdiff;
               creg = j;
            }
         }

         colordifftbl[i] = mindiff;
         colorregtbl[i] = creg;
      }
   }
}
}

/************************************************************************/
/* disp_saturate(saturation);                                           */
/*                                                                      */
/* Calculate the full color LUT using a saturation value.               */
/* Saturation is a WORD with 8 bits integer, 8 bits fraction so that    */
/* 1<<8 is 1.0 (no adjustment), less than 1.0 means fade the colors,    */
/* greater than 1.0 means enhance the colors.                           */
/************************************************************************/

VOID disp_saturate(saturation)
WORD saturation;
{
int i;
int r, g, b;
int nr, ng, nb;
WORD gray;                       /* avg gray value */

if (saturation == (1<<8)) {      /* Saturation == 1 (for efficiency) */
   for (i=0; i<4096; i++)
      *(&lutfullcolor[0][0][0] + i) = i;
   return;                      
}

for (r=0; r<16; r++) {
   for (g=0; g<16; g++) {
      for (b=0; b<16; b++) {

         gray = ((r+g+b)<<8) / 3;

         nr = (((((r<<8)-gray) * saturation) >> 8) + gray) >> 8;
         ng = (((((g<<8)-gray) * saturation) >> 8) + gray) >> 8;
         nb = (((((b<<8)-gray) * saturation) >> 8) + gray) >> 8;

         nr = L(nr);
         ng = L(ng);
         nb = L(nb);

         lutfullcolor[r][g][b] = (nr<<8) + (ng<<4) + nb;
      }
   }
}
}

/************************************************************************/
/* disp_bright_cont(bright, contrast);                                  */
/*                                                                      */
/* Calculate the per-color LUTs using brightness and contrast values.   */
/* Contrast is a fixed-point number with 8 bits integer and 8 bits      */
/* fraction, so $0100 is no stretch, <$0100 reduces contrast, and       */
/* >$0100 increases it.  Brightness is an integer (positive or negative)*/
/* that is added to the value after applying the contrast.              */
/************************************************************************/

VOID disp_bright_cont(bright, contrast)
int bright;
WORD contrast;
{
int i;
int lut;

for (i=0; i<LUTSIZE; i++) {
   lut = lutred[i];                    /* Makes it use signed arithmetic! */
   lutred[i] = L((((lut-7) * contrast) + (7<<8) + (bright<<8)) >> 8);
   lut = lutgreen[i];
   lutgreen[i] = L((((lut-7) * contrast) + (7<<8) + (bright<<8)) >> 8);
   lut = lutblue[i];
   lutblue[i] = L((((lut-7) * contrast) + (7<<8) + (bright<<8)) >> 8);
}
}

/************************************************************************/
/* setlut()                                                             */
/************************************************************************/

VOID setlut(radj, gadj, badj)
{
short i;
short temp;

for (i=0; i<LUTSIZE; i++) {        /*** Initialize look up tables ***/
   temp = i+radj;
   if (temp < 0) temp = 0;
   if (temp > 15) temp = 15;
   lutred[i] = temp;

   temp = i+gadj;
   if (temp < 0) temp = 0;
   if (temp > 15) temp = 15;
   lutgreen[i] = temp;

   temp = i+badj;
   if (temp < 0) temp = 0;
   if (temp > 15) temp = 15;
   lutblue[i] = temp;
   }
}

open_ham_screen(nlines)

{
ns.Height = nlines;
nw.Height = nlines;

if (nlines==400)
   ns.ViewModes |= LACE;
else
   ns.ViewModes &= (~LACE);
     
screen = (struct Screen *)OpenScreen(&ns);

if (!screen)  {
   error("Can't open screen");
   return(0);
   }
     
nw.Screen = screen;
window = (struct Window *)OpenWindow(&nw);
if (!window) {
   CloseScreen(screen);
   error("Can't open window");
   return(0);
   }
SetRGB4(&screen->ViewPort, 0, 0, 0, 0);
return(screen);
}

close_ham_screen()
{
CloseWindow(window);
CloseScreen(screen);
}

