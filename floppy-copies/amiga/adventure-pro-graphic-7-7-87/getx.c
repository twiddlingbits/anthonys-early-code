
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "adventure.h"


getx (wprint)
struct winprint *wprint;
{

struct IntuiMessage *mess;
int i,l,j;
extern char command[80];
LONG mclass;
SHORT mcode[2],Mx,My;
char temp1[2];

ReportMouse(TRUE,wprint->WIN);
ModifyIDCMP(wprint->WIN, VANILLAKEY);

j = 0;
l = 0;
temp1[1] = 0;
printx(wprint,"_^b");
do {

   Wait(1<<wprint->WIN->UserPort->mp_SigBit);
   while(mess = GetMsg(wprint->WIN->UserPort)) {

      mclass = mess->Class;
      mcode[0] = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == VANILLAKEY || mclass == RAWKEY) {
         if (mcode[0] == 10 || mcode[0] == 13) {
            ReportMouse(FALSE,wprint->WIN);
            printx(wprint," ^n");
            ReportMouse(TRUE,wprint->WIN);
            command[l++] = 0;
            goto ret1;
            }
         else if (mcode[0] == 8 && l > 0) {
            ReportMouse(FALSE,wprint->WIN);
            printx(wprint," ^b^b_^b");
            ReportMouse(TRUE,wprint->WIN);
            l--;
            }
         else if (mcode[0] != 8)  {
            temp1[0] = (char) mcode[0];
            ReportMouse(FALSE,wprint->WIN);
            printx(wprint,temp1);
            ReportMouse(TRUE,wprint->WIN);
            command[l++] = mcode[0];
            printx(wprint,"_^b");
            }
         }
      }
   } while(TRUE) ;
   ret1:
   ReportMouse(FALSE,wprint->WIN);

}



