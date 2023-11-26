#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

/**************************/
/**  get a clicked slot  **/
/**************************/

pickslot(Window)
struct Window *Window;
{
extern struct Disk_Samp samp[MAX_SAMPLES];
struct IntuiMessage *message;
int class,code,mousex,mousey;

for (class=0; class < MAX_SAMPLES; class++)
   if (samp[class].flags&SAMPLE_OPEN) goto zap;

return(-1);       /** no samples to pick!! **/

zap:
while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
   Wait(1<<Window->UserPort->mp_SigBit);

class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
ReplyMsg(message);

if (class==RAWKEY && code==64) {       /** space **/
   show_msg(Window,193);
   return(-1);
   }

if (class==MOUSEBUTTONS && code==SELECTDOWN) {
   mousey=(mousey-STAT_Y)/8;
   if (mousey>MAX_SAMPLES || !(samp[mousey].flags&SAMPLE_OPEN)) {
      show_msg(Window,194);
      goto zap;
      }
   return(mousey);
   }
goto zap;
}

