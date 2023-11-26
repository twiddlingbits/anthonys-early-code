

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

putbm2(win,bitmap,rp,cbyte,x,y)
int x,y;
struct BitMap *bitmap;
struct Window *win;
struct RastPort *rp;
UBYTE cbyte;
{

rp->BitMap = bitmap;

   ClipBlit (rp,0,0,win->RPort,x,
         y,bitmap->BytesPerRow * 8 ,bitmap->Rows ,cbyte,1,0);

}

