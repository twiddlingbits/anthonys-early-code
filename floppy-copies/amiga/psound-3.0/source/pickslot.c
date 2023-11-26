#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

/**************************/
/**  get a clicked slot  **/
/**************************/

pickslot(Window,samp)
struct Window *Window;
struct Samp *samp;
{
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
   mousey=(mousey-11)/8;
   if (mousex>320) mousey+=MAX_SAMPLES/2;
   if (mousey>MAX_SAMPLES || samp->lmem[mousey]==0) {
      msg(Window,"Invalid Selection, try again.");
      goto zap;
      }
   return(mousey);
   }
goto zap;
}

