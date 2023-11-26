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
char *reg1,*reg2;
ULONG k;
static int i;
static UBYTE *data,*direction,*pa,*direction2,*data2;

char *base;
unsigned short *dmaconw;

base=0xDEF000;
dmaconw=(unsigned short *)(base+0x096);

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
pa=(UBYTE *)0xBFE001;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

reg1=lp;
reg2=rp;
*direction=0;           /* all bits are read */
*data=0;
*direction2=6;  /* pa1 and pa2 write */
*data2=chan;


msg(Window,"RECORDING... Press the left mouse button to stop.");
printf("rate = %d\n",rate);
if ((*pa&64)==0) while((*pa&64)==0) ;
for (k=0;k<20;k++) ;    /* skip key bbbounce */
Disable();
k=0;
if (chan==RIGHT) {
   k=digi_right(lp,maxnum,rate);
   }
else if (chan==LEFT) {
   printf("bytes=%d\n",(k=digi_left(lp,maxnum,rate)));
   }
else if (chan!=STEREO)
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);   /* delay loop */
      *reg1++=(*data)-128;
      }
else {
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);    /* delay loop */
      *data2=RIGHT;
      *reg2++=(*data)-128;
      *data2=LEFT;
      *reg1++=(*data)-128;
      }
   }

Enable();
printf("bytes=%d\n",k);


/* reg1=findzero(lp);
reg2=reg1;

for (i=0;i<2000;i++)
   reg2=findzero(reg2);

   printf("%d Samps/Sec at rate %d.\n",((ULONG)reg2-(ULONG)reg1),rate);


   */

for (i=0;i<2000;i++) ;
return((int)(k-1));
}

/* findzero(p)
char *p;
{
char v1,v2;
do {
   v1=*p;
   if (v1==0) return(++p);
   v2=*(++p);
   if (v2==0) return(++p);
} while ( (v1 > 0 && v2 > 0) || (v1 <0 && v2 < 0));
return(p);
}
   */
