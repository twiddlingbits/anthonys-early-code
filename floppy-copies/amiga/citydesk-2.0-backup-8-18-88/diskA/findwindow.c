#include "exec/types.h"
#include "intuition/intuition.h"

/****************************************************************

search all windows in "screen" for one that contains box(x,y,h,w).
if there is more than one window, returns the one nearest the top.

*******************************************************************/

struct Window *findwindow(screen,x,y,h,wid)

struct Screen *screen;
SHORT x,y,h,wid;

{
struct Window *w;
struct Layer *l;

Forbid();
for (l=screen->LayerInfo.top_layer; l->back!=0; l=l->back) {
   w=(struct Window *)l->Window;
   if (w!=NULL && w->LeftEdge <=x && w->TopEdge <= y &&
      (w->LeftEdge+w->Width) >= (x+wid) && (w->TopEdge+w->Height) >= (y+h)) {
      Permit();
      return(w);
      }
   }
w=l->Window;
if (w!=NULL && w->LeftEdge <=x && w->TopEdge <= y &&
   (w->LeftEdge+w->Width) >= (x+wid) && (w->TopEdge+w->Height) >= (y+h)) {
   Permit();
   return(w);
   }
Permit();
return(0);
}

