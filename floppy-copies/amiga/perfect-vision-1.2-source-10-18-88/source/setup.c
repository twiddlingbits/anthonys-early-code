#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "vid.h"

#define LUTSIZE 16

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

extern short lace;
extern short fourbits;
extern UBYTE *grab_buffer;
extern struct Window *menu_window;
extern struct Screen *main_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern UBYTE lutred[LUTSIZE], lutgreen[LUTSIZE], lutblue[LUTSIZE];

static struct NewWindow nw2 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   320,200,   /* window width and height */
   0,1,   /* detail and block pens */
   0,   /* IDCMP flags */
   BACKDROP|RMBTRAP|NOCAREREFRESH|BORDERLESS,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   320,200,   /* minimum width and height */
   320,400,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

setup()

{
static struct NewWindow NewWindowStructure = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   320,180,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP|RAWKEY|MOUSEBUTTONS,   /* IDCMP flags */
   RMBTRAP|ACTIVATE|NOCAREREFRESH|BORDERLESS,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   320,180,   /* minimum width and height */
   320,180,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};
char *notice;

notice = "THIS PROGRAM IS © COPYRIGHT 1988 SUNRIZE INDUSTRIES. ALL RIGHTS RESERVED.";

IntuitionBase=0;
GfxBase     = 0;
grab_buffer = 0;
menu_window = 0;
main_window = 0;
main_screen = 0;
menu_screen = 0;


if (fourbits) {
   grab_buffer = (UBYTE *)AllocMem(BUFSIZE,MEMF_CHIP|MEMF_CLEAR);
   }
else {
   grab_buffer = (UBYTE *)AllocMem(BUFSIZE*2,MEMF_CHIP|MEMF_CLEAR);
   }

if (grab_buffer==0) {
   exit(0);
   }

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);

if (IntuitionBase==NULL) {
   close_all();
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   close_all();
   exit(0);
   }

main_screen = openmain();
if (main_screen == 0) {
   close_all();
   }

menu_screen = (struct Screen *)openscreen(320,180,2,199-MENUHEIGHT);
if (menu_screen == 0) {
   close_all();
   }

setrp();

SetRGB4(&menu_screen->ViewPort, 0, 0, 0, 0);       /** black **/
SetRGB4(&menu_screen->ViewPort, 2, 15, 15, 15);    /** white **/
SetRGB4(&menu_screen->ViewPort, 1, 0, 0, 15);      /** blue  **/
SetRGB4(&menu_screen->ViewPort, 3, 15, 0, 0);      /** red   **/

NewWindowStructure.Screen = menu_screen;
menu_window = (struct Window *)OpenWindow(&NewWindowStructure);
if (menu_window == 0) {
   close_all();
   }
   setlut(0,0,0);       /*** Set lookup tabels to normal gray levels ***/
}

setrp()
{
setrp3(0,0,0);
}

setrp3(r,g,b)
int r,g,b;
{
int i;
extern struct RastPort *rp;
struct ViewPort *vp;

vp = &main_screen->ViewPort;
rp = &main_screen->RastPort;

for (i = 0; i < 16; i++)
   SetRGB4(vp, i, min(15,r+i), min(15,g+i), min(15,b+i));

}

set_false()
{
int i;
struct ViewPort *vp;

vp = &main_screen->ViewPort;

SetRGB4(vp, 0, 0, 0,0 );
SetRGB4(vp, 1, 1, 1, 1);
SetRGB4(vp, 2, 6, 2, 2);
SetRGB4(vp, 3, 8, 3, 3);
SetRGB4(vp, 4, 5, 4, 9);
SetRGB4(vp, 5, 6, 5, 10);
SetRGB4(vp, 6, 7, 6, 11);
SetRGB4(vp, 7, 7, 10, 11);
SetRGB4(vp, 8, 8, 11, 11);
SetRGB4(vp, 9, 9, 12, 11);
SetRGB4(vp, 10, 14, 13, 10);
SetRGB4(vp, 11, 15, 13, 3);
SetRGB4(vp, 12, 12, 12, 12);
SetRGB4(vp, 13, 13, 12, 13);
SetRGB4(vp, 14, 14, 13, 14);
SetRGB4(vp, 15, 15, 15, 15);
}

close_all()
{
if (grab_buffer) {
   if (fourbits)
      FreeMem(grab_buffer,BUFSIZE);
   else
      FreeMem(grab_buffer,2*BUFSIZE);
   }
if (menu_window)
   CloseWindow(menu_window);
if (main_window)
   CloseWindow(main_window);
if (main_screen)
   CloseScreen(main_screen);
if (menu_screen)
   CloseScreen(menu_screen);
if (GfxBase)
   CloseLibrary(GfxBase);
if (IntuitionBase)
   CloseLibrary(IntuitionBase);
exit(0);
}

openmain()
{
short height;

if (lace)
   height = 400;
else
   height = 200;

main_screen = (struct Screen *)openscreen(320,height,4,0);

if (main_screen == 0)
   close_all();

nw2.Screen = main_screen;
nw2.Height = height;
main_window = (struct Window *)OpenWindow(&nw2);
if (main_window == 0) {
   close_all();
   }

return(main_screen);
}

