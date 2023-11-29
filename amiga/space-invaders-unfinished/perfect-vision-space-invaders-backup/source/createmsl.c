#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/sprite.h"

createmsl(w)
struct Window *w;

{
static struct SimpleSprite sp;
struct ViewPort *vp;
int i,k,spgot;
static UWORD data[] = {
0,0,
0x4000,0x0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0xE000,0X0000,
0,0
};

spgot = GetSprite(&sp, 2);
if (spgot==-1)
   return(0);

sp.x = 100;
sp.y = 100;

sp.height=10;

k = ((spgot&0x06)*2)+16;

vp = &w->WScreen->ViewPort;
SetRGB4(vp,k+1,12,3,8);
ChangeSprite(vp,&sp,data);
return(&sp);
}

