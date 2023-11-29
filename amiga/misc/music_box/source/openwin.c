#include "exec/types.h"
#include "intuition/intuition.h"

extern struct Screen *screen;

openwin()


{
static struct NewWindow nw = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   1,0,   /* detail and block pens */
   ACTIVEWINDOW|CLOSEWINDOW|MOUSEBUTTONS|MOUSEMOVE,
   BACKDROP|BORDERLESS|NOCAREREFRESH|WINDOWCLOSE,
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   640,200,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};
struct Window *window;

nw.Screen=screen;
window=OpenWindow(&nw);
return(window);
}


