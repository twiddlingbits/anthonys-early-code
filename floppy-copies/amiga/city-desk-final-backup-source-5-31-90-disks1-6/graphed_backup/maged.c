
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphics/rastport.h"
#include "graphed.h"


Maged ( Window, MagWin, x, y,box)
struct Window *Window;
struct Window *MagWin;
LONG x,y;
struct Border *box;
{

struct IntuiMessage *mess;
int x2,y2,pic,sit,magcolor;
LONG mclass;
SHORT mcode,Mx,My;
extern int gibblecolor;
int i,j;

magcolor = gibblecolor;
ReportMouse(TRUE,MagWin);
ModifyIDCMP(MagWin, CLOSEWINDOW | MOUSEMOVE | MOUSEBUTTONS);
sit = 0;
j = 241 / (1<< (Window->RPort->BitMap->Depth));

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
      if (mclass == MOUSEBUTTONS) {
         if (mcode == MENUDOWN) {
            if (My < 115 && My > 10) {
               sit = 2;
               x2 = (Mx - 2) / 6;
               y2 = (My - 10) / 5;
               SetAPen(MagWin->RPort,0);
               SetAPen(Window->RPort,0);
               RectFill(MagWin->RPort,x2 * 6 + 2, y2 * 5 + 10,
                        x2 * 6 + 7, y2 * 5 + 14);
               WritePixel(Window->RPort,x2+x,y2+y);
               }
            }

         if (mcode == SELECTDOWN) {
            if (My < 115) {
               sit = 1;
               x2 = (Mx - 2) / 6;
               y2 = (My - 10) / 5;
               SetAPen(MagWin->RPort,magcolor);
               SetAPen(Window->RPort,magcolor);
               RectFill(MagWin->RPort,x2 * 6 + 2, y2 * 5 + 10,
                        x2 * 6 + 7, y2 * 5 + 14);
               WritePixel(Window->RPort,x2+x,y2+y);
               }
            else {
               DrawBorder (MagWin->RPort,box,j * magcolor+2,116);
               magcolor = (Mx -2)/j;
               if (magcolor == (1<< (Window->RPort->BitMap->Depth)))
                  magcolor--;
               DrawBorder (MagWin->RPort,box,j * magcolor+2,116);
               }
            }

         if (mcode == SELECTUP || mcode == MENUUP)
            sit = 0;
         }

      if (mclass == MOUSEMOVE && sit > 0) {
         if(Mx>0 && Mx <MagWin->Width-3 && My >9 && My <115) {
         x2 = (Mx - 2) / 6;
         y2 = (My - 10) / 5;
         RectFill(MagWin->RPort,x2 * 6 + 2, y2 * 5 + 10,
               x2 * 6 + 7, y2 * 5 + 14);
         WritePixel(Window->RPort,x2+x,y2+y);
         }
         }
      }
   }
   ret1:
   ReportMouse(FALSE,MagWin);
}

