
/*** USE -b0 WHEN COMPILING THIS MODULE ***/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"

/* Local defines */
     
#define NBUFS  200         /* Number of lines in buffer */
#define NSAMPS 320         /* Number of samples to display */
#define LINELEN 160        /* Number of samples in line byte packed */

extern struct Window *window;
extern struct Screen *screen;
extern struct NewWindow nw;

static struct NewScreen ns32 = {
   0, 0, 320, 200,      /* size */     /** overscan **/
   5,                   /* depth */
   0, 0,                /* pens */
   0,                 /* mode */
   SCREENQUIET, 0, 0, 0, 0
};

color32(bufsize, rbuf, gbuf, bbuf, lockpalette, numlock, numcolors)

UBYTE *rbuf,*gbuf,*bbuf;
long bufsize;
USHORT lockpalette[];
int numlock;

{
USHORT colormap[32];    /*** Dummy till bobby changes color routines **/
short i, line;
long bufn;
int nlines;

if (bufsize > 32000)
   nlines = 400;
else
   nlines = 200;

init_disp(NSAMPS);
lock_colors(lockpalette, numlock);
     
bufn = 0;

for (line=0; line<nlines; line++) {
   disp_analyze32(&rbuf[bufn], &gbuf[bufn], &bbuf[bufn]);
   bufn = bufn + LINELEN;
   }
  
disp_sort(colormap,numcolors);
     
/* LoadRGB4(&screen->ViewPort, colormap, 32); */

show_32(bufsize, rbuf, gbuf, bbuf, numcolors);

return(TRUE);
}

show_32(bufsize, rbuf, gbuf, bbuf,numcolors)

UBYTE *rbuf,*gbuf,*bbuf;
long bufsize;
int numcolors;

{
USHORT colormap[32];
short line,i;
long bufn;
int addr,nlines;

get_cmap(colormap,numcolors);

for (i=numcolors; i < 32; i++) {
   colormap[i] = 0;
   }

LoadRGB4(&screen->ViewPort, colormap, 32);

if (bufsize > 32000)
   nlines = 400;
else
   nlines = 200;

addr = 0;
bufn = 0;
     
for (line=0; line < nlines; line++) {
   disp_32(&rbuf[bufn], &gbuf[bufn], &bbuf[bufn],
             window->RPort->BitMap->Planes, &addr);
   bufn = bufn + LINELEN;
   }
}

open_32_screen(nlines)

{
int i;

ns32.Height = nlines;
nw.Height = nlines;

if (nlines==400)
   ns32.ViewModes |= LACE;
else
   ns32.ViewModes &= (~LACE);

screen = (struct Screen *)OpenScreen(&ns32);
if (!screen) {
   error("can't open screen");
   return(0);
   }
     
nw.Screen = screen;
window = (struct Window *)OpenWindow(&nw);
if (!window) {
   error("can't open window");
   CloseScreen(screen);
   return(0);
   }
return(screen);
}

close_32_screen()
{
close_ham_screen();     /** Exact same screen and window pointers **/
}


