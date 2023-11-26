
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "adventure.h"


getx2 (wprint)
struct winprint *wprint;
{

struct IntuiMessage *mess;
LONG mclass;

ReportMouse(TRUE,wprint->WIN);
ModifyIDCMP(wprint->WIN, VANILLAKEY);

do {

   Wait(1<<wprint->WIN->UserPort->mp_SigBit);
   while(mess = GetMsg(wprint->WIN->UserPort)) {

      mclass = mess->Class;
      ReplyMsg(mess);

      if (mclass == VANILLAKEY || mclass == RAWKEY)
        goto ret1;
      }
   } while(TRUE) ;
   ret1:
   while(mess = GetMsg(wprint->WIN->UserPort))
      ReplyMsg(mess);
   ReportMouse(FALSE,wprint->WIN);

}



