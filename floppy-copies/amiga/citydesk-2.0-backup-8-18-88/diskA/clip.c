#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/regions.h"
#include "desk.h"


void add_clip(view)

struct pageview *view;

{
struct Rectangle rect;
struct Region *rgn;
struct Window *w;

w = view->window;

/* printf("bl %d bt %d\n",w->BorderLeft,w->BorderTop);
printf("br %d bb %d\n",w->BorderRight,w->BorderBottom); */

if (w==0)
   return;

rgn = NewRegion();
rect.MinY = YBORDER;
rect.MaxY = w->Height - YBORDER_AREA + YBORDER - 1;
rect.MinX = XBORDER;
rect.MaxX = w->Width - XBORDER_AREA + XBORDER - 1;
OrRectRegion(rgn, &rect);
InstallClipRegion(w->WLayer, rgn);
view->rgn = rgn;
}

void remove_clip(view)
struct pageview *view;
{
DisposeRegion(view->rgn);
InstallClipRegion(view->window->WLayer, 0);
}

