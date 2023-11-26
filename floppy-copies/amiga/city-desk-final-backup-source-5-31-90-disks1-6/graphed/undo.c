
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

putundo(win,undorp)
struct Window *win;
struct RastPort *undorp;
{

   int x,y;

   x = undorp->BitMap->BytesPerRow * 8;
   y = undorp->BitMap->Rows;

   ClipBlit (win->RPort,0,0,undorp,0,0
             , x, y, 0xc0, 1, 0);

}

getundo(win,undorp)
struct Window *win;
struct RastPort *undorp;
{

   int x,y;

   x = undorp->BitMap->BytesPerRow * 8;
   y = undorp->BitMap->Rows;

   ClipBlit (undorp,0,0,win->RPort,0,0
             , x, y, 0xc0, 1, 0);

}



