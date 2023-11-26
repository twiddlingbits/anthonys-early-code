#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

#define MAX(a,b) ((a)>=(b) ? (a) : (b))
#define MIN(a,b) ((a)<=(b) ? (a) : (b))

static short old_window_x;
static short old_window_y;
static short old_window_h;
static short old_window_w=0;


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

if (w->Height==w->MaxHeight && w->Width==w->MaxWidth)
   return;

old_window_x = w->LeftEdge;
old_window_y = w->TopEdge;
old_window_h = w->Height;
old_window_w = w->Width;

xoff =  MIN(std_width,w->MaxWidth) - w->Width;
yoff =  MIN(screensize, w->MaxHeight) - w->Height;

MoveWindow(w,-w->LeftEdge,-w->TopEdge);
SizeWindow(w, xoff, yoff);
}

void restore_window(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];
struct Window *w;
short xoff, yoff;

if (pg<1)
   return;

if (old_window_w==0)
   return;

w = page[pg].view->window;

xoff =  old_window_w - w->Width;
yoff =  old_window_h - w->Height;
SizeWindow(w, xoff, yoff);
MoveWindow(w, old_window_x - w->LeftEdge, old_window_y - w->TopEdge);
old_window_w=0;
}

