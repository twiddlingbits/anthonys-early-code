
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


void clearwindow(twin)
struct textwin *twin;
{

SetAPen(twin->win->RPort,0);
SetDrMd(twin->win->RPort,JAM2);
RectFill(twin->win->RPort,2,10,twin->win->Width-RIGHTOFFSET,twin->win->Height-10);

}

