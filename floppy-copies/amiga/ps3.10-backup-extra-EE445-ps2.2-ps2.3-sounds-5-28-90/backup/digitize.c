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
int rate;
ULONG k;
static UBYTE *pa;
pa=(UBYTE *)0xBFE001;

ScreenToBack(screen);
rate = toperiod(speed);
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
/*   k=digi_right(lp,maxnum,rate); */
   k=new_right(lp,maxnum,speed-1);
   break;

case LEFT:
/*   k=digi_left(lp,maxnum,rate); */
   k=new_left(lp,maxnum,speed-1);
   break;

case STEREO:
/*   k=digi_stereo(lp,rp,maxnum,rate); */
   k=new_stereo(lp,rp,maxnum,speed/2-1);
   break;
   }
Enable();
ScreenToFront(screen);
return(k);
}

