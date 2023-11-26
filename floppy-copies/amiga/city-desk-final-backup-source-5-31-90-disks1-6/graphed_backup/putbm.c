

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

int yeoldmanx[6] = {0,2,2,2,3,0};
int yeoldmany[6] = {0,1,2,1,2,0};
BYTE twolinesflag;

void putbm(win,bitmap,rp,color,x,y,brush,planes)
int color,x,y,brush,planes;
struct BitMap *bitmap;
struct Window *win;
struct RastPort *rp;
{
extern int gibblex;
int thisx;
rp->BitMap = bitmap;
if (brush < 5)
   thisx = bitmap->BytesPerRow * 8;
else
   thisx = gibblex;

if (color > 0) {
   win->RPort->Mask = planes;
   ClipBlit (rp,0,0,win->RPort,x - yeoldmanx[brush],y - yeoldmany[brush]
         ,thisx ,bitmap->Rows ,0xee,planes,0);
   win->RPort->Mask = 15;
   }
else {
   win->RPort->Mask = 15;
   ClipBlit (rp,0,0,win->RPort,x - yeoldmanx[brush],y - yeoldmany[brush]
         ,thisx ,bitmap->Rows ,0x20,planes,0);
   }
}

void drawtwolines(win,x,y)
struct Window *win;
int x,y;
{
SetDrMd(win->RPort,COMPLEMENT);
Move(win->RPort,0,y);
Draw(win->RPort,win->Width,y);
Move(win->RPort,x,0);
Draw(win->RPort,x,win->Height);
SetDrMd(win->RPort,JAM1);

twolinesflag++;
if (twolinesflag == 2)
   twolinesflag = 0;

}


