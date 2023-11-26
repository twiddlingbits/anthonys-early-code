#include "exec/types.h"
#include "psound.h"

/***********************************/
/***        record a sample      ***/
/***********************************/

digitze (Window,lp,rp,rate,maxnum,chan)
char *rp,*lp;
int rate,maxnum,chan;
struct Window *Window;
{
ULONG k;

for (k=0;k<20;k++) ;    /* skip key bbbounce */
k=1;

Disable();

switch (chan) {

case RIGHT:
   k=digi_right(lp,maxnum,rate);
   break;

case LEFT:
   k=digi_left(lp,maxnum,rate);
   break;

case STEREO:
   k=digi_stereo(lp,rp,maxnum,rate);
   break;
   }
Enable();
return(k-1);
}

