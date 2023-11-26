
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


casewait (wprint,comcase,rk)
int comcase,rk;
struct winprint *wprint;
{
extern struct sequence *sequences[MAXLIST];
extern UBYTE *code;
extern int secnum;
int num,k,ii,jj,ll;
struct sequence *a;
struct Image *b,*bb;
struct BitMap *c;

struct IntuiMessage *mess;
LONG mclass;

if (comcase == 0) {
   num = code[rk + 1];
   a = sequences[num-1];
   if ((a->flags & ON) == 0 || (a->flags & REPEAT) ||(a->flags & ACTIVATED) == 0)
      goto ret1;

   ReportMouse(TRUE,wprint->WIN);
   ModifyIDCMP(wprint->WIN, INTUITICKS);

   do {

      Wait(1<<wprint->WIN->UserPort->mp_SigBit);
      while(mess = GetMsg(wprint->WIN->UserPort)) {

         mclass = mess->Class;
         ReplyMsg(mess);

         if (mclass == INTUITICKS) {
            updatesec(wprint);
            if ((a->flags & ACTIVATED) == 0)
               goto ret1;
            } /* intuiticks end */
         }
      } while(TRUE) ;
      ret1:
      while(mess = GetMsg(wprint->WIN->UserPort))
         ReplyMsg(mess);
      ReportMouse(FALSE,wprint->WIN);
   }
   return(2);
}




