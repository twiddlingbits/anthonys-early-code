#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "libraries/dosextens.h"

#define IDCMP ACTIVEWINDOW|GADGETUP|MENUPICK|MOUSEBUTTONS|RAWKEY

static APTR temp_ptr;
static struct Process *mproc;

struct Window *openwindow(screen)

struct Screen *screen;

{
struct Window *w;
struct Window *OpenWindow();
extern struct Menu Titles[];

static struct NewWindow nw = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   15,8,   /* detail and block pens */
   IDCMP,    /* idcmp flags */
   BACKDROP|BORDERLESS|NOCAREREFRESH|ACTIVATE,
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   0,  /** window title **/
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   640,200,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

if (screen==0)
   return(0);

nw.Screen = screen;
nw.Width  = screen->Width;
nw.Height =  screen->Height;

w=OpenWindow(&nw);

if (w) {
   SetMenuStrip(w,Titles);
/*   mproc = (struct Process *)FindTask(0L);    couldnt read on my screen
   temp_ptr = mproc->pr_WindowPtr;
   mproc->pr_WindowPtr = (APTR)w;   */
   }
return(w);
}

closewindow(w)
struct Window *w;
{
if (w) {
/*   mproc->pr_WindowPtr = temp_ptr; */    /** restore original pointer **/
   CloseWindow(w);
   }
}

