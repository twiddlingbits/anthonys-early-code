#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/sprite.h"
#include "si.h"

struct SimpleSprite *msl_sp;
short msl_y, msl_x;

start_msl(w,x)

struct Window *w;
int x;
{
msl_sp=createmsl(w);
if (msl_sp == 0)
   return(TRUE);     /** error **/
msl_y = 173;
msl_x = (x+16) >> 1;
MoveSprite(0, msl_sp, msl_x, msl_y);
return(FALSE);
}

bump_msl(w)
struct Window *w;
{
if (hit())
   return(TRUE);
msl_y -= 8;
MoveSprite(0, msl_sp, msl_x, msl_y);
if (msl_y < 30)
   return(TRUE);
return(FALSE);
}

end_msl(w)
struct Window *w;
{
/* msl_x=0;
msl_y=0;
MoveSprite(0, msl_sp, msl_x, msl_y); */
FreeSprite(2);
}

erase_vader(w,vx,vy)

struct Window *w;
short vx,vy;

{
short dx, dy;

dx = (msl_x<<1) - vx;
dx = dx/BOXWIDTH;
dx = dx*BOXWIDTH;
dx = dx+ vx;

dy = (msl_y) - vy;
dy = dy/BOXHEIGHT;
dy = dy*BOXHEIGHT;
dy = dy + vy;

printf("%d %d\n",dx,dy);

SetDrMd(w->RPort,JAM1);
SetAPen(w->RPort,0);
RectFill(w->RPort, dx, dy,dx+ BOXWIDTH, dy+BOXHEIGHT);
}

