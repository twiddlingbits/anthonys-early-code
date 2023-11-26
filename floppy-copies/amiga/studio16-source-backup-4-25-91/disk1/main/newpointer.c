/** WARNING: you must compile this module with -ad and no -b option **/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "/include/studio16.h"

/*****************************/
/*** change pointer **********/
/*****************************/

newpointer(mode)

SHORT mode;
{
/* static short menus_on = TRUE; */
static short lastmode = PTR_NORMAL;
static short thismode = PTR_NORMAL;
struct Window *w;
struct Screen *screen;
extern struct studioBase *studioBase;

static unsigned short chip paste[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x8000, 0x8000,
   0x7c00, 0x7c00,
   0x3e00, 0x3e00,
   0x1d00, 0x1f00,
   0x180, 0x280,
   0xc0, 0x140,
   0x60, 0xa0,
   0x30, 0x50,
   0x18, 0x28,
   0xc, 0x14,
   0x6, 0xa,
   0x2, 0x5,
   0x0, 0x3,
   0x0, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -1, YOFFSET = 0 */



static unsigned short chip busy[] = {
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

static unsigned short chip crop[] = {
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

static unsigned short chip hand[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x6c00, 0xc00,
   0x3680, 0x3680,
   0x1b40, 0x1b40,
   0x6da0, 0x6da0,
   0x36d0, 0x36d0,
   0x1b78, 0x1b78,
   0xdfc, 0xdfc,
   0x7fc, 0x7fc,
   0x63fe, 0x63fe,
   0x3bfe, 0x3bfe,
   0x1ffd, 0x1ffd,
   0xffb, 0xffb,
   0x3f6, 0x3f6,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -2 */


static unsigned short chip copy[] = {
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


static unsigned short chip cross[] = {
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

static unsigned short chip nothing[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x0, 0x0,
   0x0000, 0x00,
   0x0, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -7 */




if (mode==PTR_RESTORE) mode=lastmode;     /* restore previous mode */
lastmode=thismode;    /* need to remember last 2 modes for RESTORE */
thismode=mode;

/* if (!menus_on && mode!=BUSY) {
   setmenus(screen, TRUE);
   menus_on = TRUE;
   }  */

screen = studioBase->screen;

switch (mode) {

   case PTR_NORMAL:
      for (w=screen->FirstWindow;w!=0;w=w->NextWindow) {
         ClearPointer(w);
         }
      return(0);

   case PTR_BUSY:

      setall(screen,busy, 16, 16, -2, -1);
/*      setmenus(screen, FALSE);
      menus_on = FALSE; */
      break;


   case PTR_GRABIT:

      setall(screen,hand, 16, 16, -2, -2);
      break;


   case PTR_CROP:

      setall(screen,crop, 16, 16, -2, -0);
      break;


   case PTR_CROSS:

      setall(screen,cross, 16, 16, -9, -7);
      break;

   case PTR_PASTE:

      setall(screen,paste, 16, 16,-1, 0);
      break;

   }  /** end of switch **/
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

