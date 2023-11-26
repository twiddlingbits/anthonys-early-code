#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

/**************************/
/**  get a clicked slot  **/
/**************************/

pickslot(Window)
struct Window *Window;
{
extern struct Samp samp[MAX_SAMPLES];
struct IntuiMessage *message;
int class,code,mousex,mousey;

zap:
while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
   Wait(1<<Window->UserPort->mp_SigBit);

class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
ReplyMsg(message);

if (class==MOUSEBUTTONS && code==SELECTDOWN) {
   mousey=(mousey-STAT_Y)/8;
   if (mousey>MAX_SAMPLES || samp[mousey].lmem==0) {
      msg(Window,"Invalid Selection, try again.");
      goto zap;
      }
   return(mousey);
   }
goto zap;
}

