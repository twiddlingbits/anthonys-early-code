#include <intuition/intuition.h>
#include "fcntl.h"

#define ABS(x) (((x)>=0) ? (x) : (-(x)))

#define NLINES 200

#define NBUFS  200

static struct NewScreen ns = {
   0, 0, 320, NLINES,   /* size */
   6,                   /* depth */
   1, 2,                /* pens */
   HAM,                 /* mode */
   CUSTOMSCREEN, 0, 0, 0, 0
};

static struct NewWindow nw = {
   0, 0, 320, NLINES,
   1, 2,
   0,
   BORDERLESS|BACKDROP,
   0,0,0,0,0,
   0,0,0,0,
   CUSTOMSCREEN
};

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct Window *window;
struct Screen *screen;

int rf, gf, bf;

main()
{

USHORT colormap[16];
static int colorfreq[16][16][16];
static int colordelta[16][16][16];
static int colorval[16][16][16];
int i, j, k, line;
int red, green, blue;
int rd, gd, bd;
int rpix, gpix, bpix;
int color, creg;
char fname[80], rname[80], gname[80], bname[80];
static UBYTE rbuf[NBUFS][512], gbuf[NBUFS][512], bbuf[NBUFS][512];
int bufn;
/* struct IntuiMessage *message; */

rf=-1;
gf=-1;
bf=-1;

if (NLINES==400)
   ns.ViewModes |= LACE;

IntuitionBase = OpenLibrary("intuition.library", 0);
GfxBase = OpenLibrary("graphics.library", 0);

screen = (struct Screen *)OpenScreen(&ns);
if (!screen)
   error("can't open screen");

nw.Screen = screen;
window = (struct Window *)OpenWindow(&nw);
if (!window)
   error("can't open window");

/* initialize color map to b/w */

for (i=0; i<16; i++) {
   colormap[i] = (i<<8) + (i<<4) + i;
}

screen->ViewPort.ColorMap = GetColorMap(16);

LoadRGB4(&screen->ViewPort, colormap, 16);

printf("Name:");
scanf("%s\n", fname);

strcpy(rname, fname);
strcat(rname, ".red");
strcpy(gname, fname);
strcat(gname, ".grn");
strcpy(bname, fname);
strcat(bname, ".blu");

rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't open red");
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't open green");
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't open blue");

for (i=0; i<16; i++) {
   for (j=0; j<16; j++) {
      for (k=0; k<16; k++) {
         colorfreq[i][j][k] = 0;
         colordelta[i][j][k] = 0;
         colorval[i][j][k] = (i<<8)+(j<<4)+k;
      }
   }
}

/* First time through: build tables, display with no color regs */

bufn = NBUFS;

for (line=0; line<NLINES; line++) {

   readline(&bufn, rbuf, gbuf, bbuf);

   red = 0;
   green = 0;
   blue = 0;

   for (i=0; i<320; i++) {

      rpix = rbuf[bufn][i]>>4;
      gpix = gbuf[bufn][i]>>4;
      bpix = bbuf[bufn][i]>>4;

      colorfreq[rpix][gpix][bpix]++;

      rd = ABS(red-rpix);
      gd = ABS(green-gpix);
      bd = ABS(blue-bpix);

      colordelta[rpix][gpix][bpix] += (rd+gd+bd);

#if 0
      printf("red  =%x, pix=%x, rd=%x\n", red  , rpix, rd);
      printf("green=%x, pix=%x, gd=%x\n", green, gpix, gd);
      printf("blue =%x, pix=%x, bd=%x\n", blue , bpix, bd);
#endif

      if (rd >= gd && rd >= bd) {
         red = rpix;
         SetAPen(window->RPort, 0x20 | red);
      }
      else if (gd >= rd && gd >= bd) {
         green = gpix;
         SetAPen(window->RPort, 0x30 | green);
      }
      else {
         blue = bpix;
         SetAPen(window->RPort, 0x10 | blue);
      }
      WritePixel(window->RPort, i, line);
   }
}

/* Sort based on most delta... colorfreq currently not used */

sort(&colordelta[0][0][0], &colorfreq[0][0][0], &colorval[0][0][0], 16*16*16);

for (i=1; i<16; i++) {
   colormap[i] = *(&colorval[0][0][0]+i-1);
   printf("colormap[%d]=%x, delta=%d, freq=%d\n", i, colormap[i], *(&colordelta[0][0][0]+i-1), *(&colorfreq[0][0][0]+i-1));
}

LoadRGB4(&screen->ViewPort, colormap, 16);

/* Second time: use color registers */

close(rf);
rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't reopen red");
close(gf);
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't reopen green");
close(bf);
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't reopen blue");

for (line=0; line<NLINES; line++) {

   readline(&bufn, rbuf, gbuf, bbuf);

   red = 0;
   green = 0;
   blue = 0;

   for (i=0; i<320; i++) {

      rpix = rbuf[bufn][i]>>4;
      gpix = gbuf[bufn][i]>>4;
      bpix = bbuf[bufn][i]>>4;

      color = (rpix<<8) + (gpix<<4) + bpix;

      creg = -1;

      for (j=0; j<16; j++) {
         if (color == colormap[j])
            creg = j;
      }

      if (creg >= 0) {
         SetAPen(window->RPort, creg);
         red = rpix;
         green = gpix;
         blue = bpix;
      }

      else {

         rd = ABS(red-rpix);
         gd = ABS(green-gpix);
         bd = ABS(blue-bpix);

#if 0
         printf("red  =%x, pix=%x, rd=%x\n", red  , rpix, rd);
         printf("green=%x, pix=%x, gd=%x\n", green, gpix, gd);
         printf("blue =%x, pix=%x, bd=%x\n", blue , bpix, bd);
#endif

         if (rd >= gd && rd >= bd) {
            red = rpix;
            SetAPen(window->RPort, 0x20 | red);
         }
         else if (gd >= rd && gd >= bd) {
            green = gpix;
            SetAPen(window->RPort, 0x30 | green);
         }
         else {
            blue = bpix;
            SetAPen(window->RPort, 0x10 | blue);
         }
      }
      WritePixel(window->RPort, i, line);
   }
}

/* Third pass (can be combined with second): use cumulative deltas */
/* instead of one-pixel deltas                                     */


close(rf);
rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't reopen red");
close(gf);
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't reopen green");
close(bf);
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't reopen blue");

for (line=0; line<NLINES; line++) {

   readline(&bufn, rbuf, gbuf, bbuf);

   red = 0;
   green = 0;
   blue = 0;

   rd = 0;
   gd = 0;
   bd = 0;

   for (i=0; i<320; i++) {

      rpix = rbuf[bufn][i]>>4;
      gpix = gbuf[bufn][i]>>4;
      bpix = bbuf[bufn][i]>>4;

      color = (rpix<<8) + (gpix<<4) + bpix;

      creg = -1;

      for (j=0; j<16; j++) {
         if (color == colormap[j])
            creg = j;
      }

      if (creg >= 0) {
         SetAPen(window->RPort, creg);
         red = rpix;
         green = gpix;
         blue = bpix;
      }

      else {

         rd += (red-rpix);
         gd += (green-gpix);
         bd += (blue-bpix);

#if 0
         printf("red  =%x, pix=%x, rd=%x\n", red  , rpix, rd);
         printf("green=%x, pix=%x, gd=%x\n", green, gpix, gd);
         printf("blue =%x, pix=%x, bd=%x\n", blue , bpix, bd);
#endif

         if (ABS(rd) >= ABS(gd) && ABS(rd) >= ABS(bd)) {
            red = rpix;
            rd = 0;
            SetAPen(window->RPort, 0x20 | red);
         }
         else if (ABS(gd) >= ABS(rd) && ABS(gd) >= ABS(bd)) {
            green = gpix;
            gd = 0;
            SetAPen(window->RPort, 0x30 | green);
         }
         else {
            blue = bpix;
            bd = 0;
            SetAPen(window->RPort, 0x10 | blue);
         }
      }
      WritePixel(window->RPort, i, line);
   }
}


#if 0
for (i=0; i<16; i++)
   for (j=0; j<16; j++)
      for (k=0; k<16; k++)
         printf("r=%x, g=%x, b=%x, freq=%d\n", i, j, k, colorfreq[i][j][k]);
#endif

#if 0
wait (1<<window->UserPort->mp_SigBit);
message = (struct IntuiMessage *)GetMsg(window->UserPort);
ReplyMsg(message);
#endif

scanf("%s\n", fname);

error("Done");
}



sort(delta, freq, table, n)
int *delta, *freq, *table;
int n;
{
int i, j, temp;
int max;

printf("sorting...\n");
for (i=0; i<16; i++) {
   max = *(delta+i);
   for (j=i; j<n; j++) {
      if (*(delta+j) > max) {
         temp = *(delta+j);
         *(delta+j) = *(delta+i);
         *(delta+i) = temp;
         temp = *(freq+j);
         *(freq+j) = *(freq+i);
         *(freq+i) = temp;
         max = *(delta+i);
         temp = *(table+j);
         *(table+j) = *(table+i);
         *(table+i) = temp;
      }
   }
}
printf("done sorting\n");
}


readline(bufn, rb, gb, bb)
int *bufn;
UBYTE *rb, *gb, *bb;
{
int stat;

if (++(*bufn) >= NBUFS) {
   stat = read(rf, rb, 320*NBUFS);
   if (stat == -1)
      error("error reading red");
   stat = read(gf, gb, 320*NBUFS);
   if (stat == -1)
      error("error reading green");
   stat = read(bf, bb, 320*NBUFS);
   if (stat == -1)
      error("error reading blue");
   *bufn = 0;
}
}


error(s)
char *s;
{

printf("%s\n", s);
if (rf != -1)
   close(rf);
if (gf != -1)
   close(gf);
if (bf != -1)
   close(bf);
if (window)
   CloseWindow(window);
if (screen)
   CloseScreen(screen);
if (GfxBase)
   CloseLibrary(GfxBase);
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

exit(0);
}

