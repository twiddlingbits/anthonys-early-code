#include "exec/types.h"
#include "intuition/intuition.h"


void cleargun(window,x,y)

struct Window *window;
int x,y;

{
SetDrMd(window->RPort,JAM1);
SetAPen(window->RPort,0);
RectFill(window->RPort, x, y, x+43, y+10);   /** zap old gun */
}

