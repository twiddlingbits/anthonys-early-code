
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


getclickonly (twin,x,y)
struct textwin *twin;
int *x, *y;
{
struct Window *win;
struct RastPort *rp;
LONG mclass;
SHORT Mx,My;
struct IntuiMessage *mess;


ReportMouse(TRUE,win);
ModifyIDCMP(win, NEWSIZE | MOUSEBUTTONS);

rp = win->RPort;
while (TRUE) {

   Wait(1<<win->UserPort->mp_SigBit);
   while(mess = (struct IntuiMessage *)GetMsg(win->UserPort)) {

      mclass = mess->Class;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

            
      if (mclass == MOUSEBUTTONS) {
         *x = Mx;
         *y = My;
         return(0);
         }
      if (mclass == NEWSIZE) {
         return(-1);
         }

      }
   }
}


