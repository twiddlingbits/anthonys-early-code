#include "exec/types.h"
#include "intuition/intuition.h"

void pop_to_cursor(s, w)

struct Screen *s;
struct NewWindow *w;

{
short x,y;

x = s->MouseX - w->Width/2;
y = s->MouseY - w->Height/2;

if (x + w->Width >= s->Width)
   x = s->Width - w->Width - 1;

if (y + w->Height >= s->Height)
   y = s->Height - w->Height - 1;

if (x < 0) x=0;
if (y < 0) y=0;

w->LeftEdge = x;
w->TopEdge = y;
}

