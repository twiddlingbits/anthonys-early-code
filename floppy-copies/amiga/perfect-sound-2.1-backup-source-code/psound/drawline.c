#include "exec/types.h"
#include "intuition/intuition.h"

drawline(Window,fx,fy,tx,ty,pen)

struct Window *Window;
int fx,fy,tx,ty;
int pen;
{
Move(Window->RPort,fx,fy);
SetAPen(Window->RPort,pen);
Draw(Window->RPort,tx,ty);
}

