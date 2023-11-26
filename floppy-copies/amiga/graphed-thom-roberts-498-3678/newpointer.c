#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

/*****************************/
/*** change pointer **********/
/*****************************/

newpointer(screen,mode)

SHORT mode;
struct Screen *screen;

{
static lastmode=NORMAL;
static thismode=NORMAL;
static char *pointerdata=0;
struct Window *w;
 static unsigned short flow[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0xED5C, 0x0,
   0x4888, 0x0,
   0x4C88, 0x0,
   0x4888, 0x0,
   0x4D48, 0x0,
   0x0, 0x0,
   0xFFFC, 0xFFFC,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -7, YOFFSET = -10 */


static unsigned short size[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x1E00, 0x0,
   0x1200, 0x0,
   0x1200, 0x0,
   0x1FF0, 0x0,
   0x210, 0x0,
   0x210, 0x0,
   0x210, 0x0,
   0x210, 0x0,
   0x3F0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -4, YOFFSET = -3 */

static unsigned short busy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x380, 0x380,
   0x1FE0, 0x1FE0,
   0x3FFC, 0x3FFC,
   0x3FFE, 0x31FE,
   0x3FFE, 0x3DFE,
   0x7FFE, 0x7B1E,
   0x7FFE, 0x77DE,
   0x7FFC, 0x71BC,
   0x7FFC, 0x7F7C,
   0x3FFC, 0x3F1C,
   0x7FF8, 0x7FF8,
   0x7F00, 0x7F00,
   0x3BC0, 0x3BC0,
   0x1C0, 0x1C0,
   0x80, 0x80,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -1 */

static unsigned short crop[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x4000, 0x4000,
   0x6000, 0x6000,
   0x7000, 0x7000,
   0x7800, 0x7800,
   0x7C00, 0x7C00,
   0x7400, 0x7400,
   0xF00, 0xD00,
   0x780, 0x80,
   0xFC0, 0x40,
   0x7E0, 0x20,
   0x3F0, 0x10,
   0x1F8, 0x8,
   0xF0, 0x0,
   0x60, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = 0 */

static unsigned short hand[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0xC00, 0xC00,
   0x3680, 0x3680,
   0x1B40, 0x1B40,
   0x6DA0, 0x6DA0,
   0x36D0, 0x36D0,
   0x1B78, 0x1B78,
   0xDF8, 0xDF8,
   0x7FC, 0x7FC,
   0x13FC, 0x13FC,
   0x19FE, 0x19FE,
   0xDFC, 0xDFC,
   0x7F9, 0x7F9,
   0x3F3, 0x3F3,
   0xE6, 0xE6,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -5, YOFFSET = -4 */


static unsigned short copy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x6EEA, 0x0,
   0x4AAA, 0x0,
   0x6EEE, 0x0,
   0x84, 0x0,
   0x84, 0x0,
   0x80, 0x0,
   0x2A0, 0x0,
   0x1C0, 0x0,
   0x80, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -14 */

static unsigned short pick[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x100, 0x0,
   0x300, 0x0,
   0x700, 0x0,
   0xF00, 0x0,
   0x1FFF, 0x0,
   0x3FFF, 0x0,
   0x7FFF, 0x1D6A,
   0xFFFF, 0x1D4C,
   0x7FFF, 0x116A,
   0x3FFF, 0x0,
   0x1FFF, 0x0,
   0xF00, 0x0,
   0x700, 0x0,
   0x300, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -1, YOFFSET = -8 */

static unsigned short pict[] = {
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x75DC, 0x0,
   0x5548, 0x0,
   0x7508, 0x0,
   0x4548, 0x0,
   0x45CA, 0x0,
   0x0, 0x0,
   0xFFFF, 0x0,
   0x100, 0x0,
   0x100, 0x0,
   0x7C0, 0x0,
   0x380, 0x0,
   0x100, 0x0000,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -8, YOFFSET = -15 */

static unsigned short line[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x7800, 0x0,
   0x7000, 0x1000,
   0x7000, 0x3000,
   0x5800, 0x1800,
   0xC00, 0xC00,
   0x7FF, 0x7FF,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -5 */

static unsigned short cross[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x0, 0x0,
   0x7EBF, 0x80,
   0x0, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -7 */



if (mode==RESTORE) mode=lastmode;     /* restore previous mode */
lastmode=thismode;    /* need to remember last 2 modes for RESTORE */
thismode=mode;

if (pointerdata==0 && mode!=NORMAL)
   if ((pointerdata=(char *)AllocMem(sizeof(size),MEMF_CHIP))==0) return(0);


if (mode==NORMAL) {
   for (w=screen->FirstWindow;w!=0;w=w->NextWindow) {
      ClearPointer(w);
      }
   
   if (pointerdata!=0) {
      FreeMem(pointerdata,sizeof(flow));
      pointerdata=0;
      }
   return(0);
   }

if (mode==SIZE) {
   movmem(size,pointerdata,sizeof(size));
   setall(screen,pointerdata, 16, 16, -4, -3);
   }

if (mode==FLOW) {
   movmem(flow,pointerdata,sizeof(flow));
   setall(screen,pointerdata, 16, 16, -7, -10);
   }

if (mode==BUSY) {
   movmem(busy,pointerdata,sizeof(busy));
   setall(screen,pointerdata, 16, 16, -2, -1);
   }
if (mode==GRABIT) {
   movmem(hand,pointerdata,sizeof(hand));
   setall(screen,pointerdata, 16, 16, -5, -4);
   }

if (mode==CROP) {
   movmem(crop,pointerdata,sizeof(crop));
   setall(screen,pointerdata, 16, 16, -2, -0);
   }

if (mode==DUPOBJECT) {
   movmem(copy,pointerdata,sizeof(copy));
   setall(screen,pointerdata, 16, 16, -9, -14);
   }

if (mode==PLACEGRAPHIC) {
   movmem(pict,pointerdata,sizeof(pict));
   setall(screen,pointerdata, 16, 16, -8, -15);
   }

if (mode==LINE) {
   movmem(line,pointerdata,sizeof(line));
   setall(screen,pointerdata, 16, 16, -2, -5);
   }

if (mode==CROSS) {
   movmem(cross,pointerdata,sizeof(cross));
   setall(screen,pointerdata, 16, 16, -9, -7);
   }

if (mode==INVERT || mode==CHEAPEDIT || mode==EDITFORMAT ||
    mode==BOX || mode==SAVEOBJ || mode==REFLOWTEXT) {
   movmem(pick,pointerdata,sizeof(pick));
   setall(screen,pointerdata, 16, 16, -1, -8);
   }
}

/************************************/
/** change pointers for all windows */
/************************************/

setall(s,data,a,b,c,d)

struct Screen *s;
char *data;
int a,b,c,d;
{
struct Window *w;

for (w=s->FirstWindow; w!=0; w=w->NextWindow) {
   SetPointer(w,data,a,b,c,d-1);
   }
}



