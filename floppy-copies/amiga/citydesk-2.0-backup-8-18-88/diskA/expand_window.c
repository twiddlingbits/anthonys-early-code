#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

void expand_window(pg)

int pg;

{
extern SHORT screensize;
extern SHORT std_width;
extern struct pageinfo page[MAXPAGES];
struct Window *w;
short xoff, yoff;

if (pg<1)
   return;

w = page[pg].view->window;

xoff =  MIN(std_width,w->MaxWidth) - w->Width;
yoff =  MIN(screensize, w->MaxHeight) - w->Height;

MoveWindow(w,-w->LeftEdge,-w->TopEdge);
SizeWindow(w, xoff, yoff);
}

