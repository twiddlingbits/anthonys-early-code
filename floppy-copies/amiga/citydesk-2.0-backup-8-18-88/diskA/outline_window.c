#include "exec/types.h"
#include "intuition/intuition.h"

void outline_window(w,color)

struct Window *w;
int color;

{
struct RastPort *rp;

rp = w->RPort;

SetDrMd(rp, JAM1);
SetAPen(rp, color);
Move(rp, 0, 0);
Draw(rp,w->Width-1,0);
Draw(rp, w->Width-1, w->Height-1);
Draw(rp, 0,w->Height-1);
Draw(rp, 0, 0);
}

