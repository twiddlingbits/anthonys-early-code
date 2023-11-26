
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphics/rastport.h"
#include "graphed.h"


Maged ( Window, MagWin, Block, x, y)
struct Window *Window;
struct Window *MagWin;
struct Image *Block;
LONG x,y;
{

struct IntuiMessage *mess;
int x2,y2,pic;
LONG mclass;
SHORT *dataw,*datab,mcode,Mx,My;

static SHORT blk1[] =
{
0xFC00,
0xFC00,
0xFC00,
0xFC00,
0xFC00,
};

static SHORT blk2[] =
{
0x0000,
0x0000,
0x0000,
0x0000,
0x0000,
};

dataw = AllocMem(10,MEMF_CHIP);
datab = AllocMem(10,MEMF_CHIP);
movmem(&blk1[0],dataw,10);
movmem(&blk2[0],datab,10);

ReportMouse(TRUE,MagWin);
ModifyIDCMP(MagWin, CLOSEWINDOW | MOUSEBUTTONS);

while (TRUE)  {

   Wait(1<<MagWin->UserPort->mp_SigBit);
   while(mess = GetMsg(MagWin->UserPort)) {

      mclass = mess->Class;
      mcode = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == CLOSEWINDOW)
         goto ret1;
      if (mclass != MOUSEBUTTONS)
         goto endloop;
      if (mcode != SELECTDOWN)
         goto endloop;
      x2 = (Mx - 2) / 6;
      y2 = (My - 10) / 5;
      pic = ReadPixel(Window->RPort,x2+x,y2+y);
      if (pic == 0) {
         Block->ImageData = dataw;
         SetAPen(Window->RPort,1);
         }
      else  {
         Block->ImageData = datab;
         SetAPen(Window->RPort,0);
         }
      DrawImage(MagWin->RPort,Block,x2 * 6 + 2, y2 * 5 + 10);
      WritePixel(Window->RPort,x2+x,y2+y);

      endloop:
      }
   }
   ret1:
   FreeMem(dataw,10);
   FreeMem(datab,10);
   ReportMouse(FALSE,MagWin);
}

