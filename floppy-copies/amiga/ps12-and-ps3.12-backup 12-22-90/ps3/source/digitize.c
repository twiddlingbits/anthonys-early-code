#include "exec/types.h"
#include "psound.h"

extern struct Screen *screen;

/***********************************/
/***        record a sample      ***/
/***********************************/

digitze (Window,lp,rp,speed,maxnum,chan)
char *rp,*lp;
int speed,maxnum,chan;
struct Window *Window;
{
extern short digi_type;
extern int vid_clock;
int rate;
ULONG k;
static UBYTE *pa;
pa=(UBYTE *)0xBFE001;

rate = toperiod((vid_clock/10)/speed);
Disable();

k=1;

switch (chan) {

case RIGHT:
   if (digi_type==PS_2)
      show_msg(Window,255);
/*       k=digi_right(lp,maxnum,rate); */
   else
      k=new_right(lp,maxnum,speed-1);
   break;

case LEFT:
   if (digi_type==PS_2)
      show_msg(Window,255);
   /*  k=digi_left(lp,maxnum,rate); */
   else
      k=new_left(lp,maxnum,speed-1);
   break;

case STEREO:
   if (digi_type==PS_2)
      show_msg(Window,255);
/*      k=digi_stereo(lp,rp,maxnum,rate); */
   else
      k=new_stereo(lp,rp,maxnum,speed/2-1);
   break;
   }
Enable();
return(k);
}

