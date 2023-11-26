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
static UBYTE *pa;
pa=(UBYTE *)0xBFE001;

Disable();

#if 0
for (k=0;k<900;k++) ;    /* skip key bbbounce */
while((*pa&64)==0) ;

for (k=0;k<900;k++) ;    /* skip key bbbounce */
while((*pa&64)==0) ;
#endif

k=1;

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

