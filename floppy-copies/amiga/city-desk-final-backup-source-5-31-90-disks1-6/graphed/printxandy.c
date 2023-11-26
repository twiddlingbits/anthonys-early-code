

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"



printxandy(kitwin,Mx,My,numx1,numx2)
struct Window *kitwin;
int numx1,numx2;
LONG Mx,My;

{

char ts[4];
int x,y;

x = (int) Mx;
y = (int) My;

Move(kitwin->RPort,numx1,20);
sprintf(&ts[0],"%d",x);
if (x<100)
   ts[2] = 32;
if (x<10)
   ts[1] = 32;
Text(kitwin->RPort,&ts[0],3);
Move(kitwin->RPort,numx2,20);
sprintf(&ts[0],"%d",y);
if (y<100)
   ts[2] = 32;
if (y<10)
   ts[1] = 32;
Text(kitwin->RPort,&ts[0],3);



}


