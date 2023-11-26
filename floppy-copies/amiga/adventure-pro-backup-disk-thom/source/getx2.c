
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


getx2 (wprint)
struct winprint *wprint;
{
extern struct sequence *sequences[MAXLIST];
extern int secnum;
int k,ii,jj,ll;
struct sequence *a;
struct Image *b,*bb;
struct BitMap *c;
struct IntuiMessage *mess;
LONG mclass;
USHORT mcode;

newpointer(wprint->WIN->WScreen,NORMAL);

ReportMouse(TRUE,wprint->WIN);
ModifyIDCMP(wprint->WIN, INTUITICKS | VANILLAKEY | MOUSEBUTTONS);

do {

   Wait(1<<wprint->WIN->UserPort->mp_SigBit);
   while(mess = GetMsg(wprint->WIN->UserPort)) {

      mclass = mess->Class;
      mcode = mess->Code;
      ReplyMsg(mess);

      if (mclass == VANILLAKEY || (mclass == MOUSEBUTTONS &&
         mcode == SELECTDOWN))
        goto ret1;

      if (mclass == INTUITICKS) {
         updatesec(wprint);
         } /* intuiticks end */
      }
   } while(TRUE) ;
   ret1:
   while(mess = GetMsg(wprint->WIN->UserPort))
      ReplyMsg(mess);
   ReportMouse(FALSE,wprint->WIN);
   newpointer(wprint->WIN->WScreen,BUSY);

}




