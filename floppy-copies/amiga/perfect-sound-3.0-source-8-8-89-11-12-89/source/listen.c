#include "exec/types.h"
#include "psound.h"

/*********************************************/
/***        listen to digitizer            ***/
/*********************************************/

listen (chan)
int chan;
{
static UBYTE *base,*pa,*data,*direction,*data2,*direction2;
static unsigned short sam,i;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short *aud1lch,*aud1len,*aud1vol,*aud1per,*aud1dat;
unsigned short *intreq,*intreqr;
unsigned int *aud0lc,*aud1lc;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

*direction=0;           /* all bits are read */
*data=0;
*direction2=2+4;      /* pa1 & pa2 write */

Disable();

base=(UBYTE *)0xDFF000;
/* channel 0 chip addresses */
dmaconw=(unsigned short *)(base+0x096);
aud0lch=(unsigned short *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
aud0lc=(int *)aud0lch;
/* channel 1 chip addresses */
aud1lch=(unsigned short *)(base+0x0B0);
aud1len=(unsigned short *)(base+0x0B4);
aud1vol=(unsigned short *)(base+0x0B8);
aud1per=(unsigned short *)(base+0x0B6);
aud1dat=(unsigned short *)(base+0x0BA);
aud1lc=(int *)aud1lch;
intreq=(unsigned short *)(base+0x09C);
intreqr=(unsigned short *)(base+0x1E);
pa=(UBYTE *)0xBFE001;

*aud0vol=64;
*aud0per=1;
*aud1vol=64;
*aud1per=1;
i=0;

if (chan==STEREO)
   while ((*pa&64)==64) {
      *data2=LEFT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;    /* not in manual, but required to resart ch 0 */

      *data2=RIGHT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;    /* needed to restart ch 1 */
      }
*data2=chan;
if (chan==RIGHT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;
      }

if (chan==LEFT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;
      }

quash_bounce(0);     /** wait for button to go down **/
quash_bounce(64);    /** wait for button to come up **/

/* while ((*pa&64)!=64) ;  */   /* wait for left mouse button release */
Enable();
}

/***************************************************************/
/** This routine waits for the mouse left button to stabilize **/
/** on the value passed                                       **/
/***************************************************************/

quash_bounce(val)

int val;

{
short bounce = TRUE;
short i;
UBYTE *pa;
pa=(UBYTE *)0xBFE001;

while (bounce) {
   bounce = FALSE;
   while ((*pa&64)!= val) ;
   for (i=0; i < 450; i++)     /* wait for a bbounce **/
      if ( (*pa&64)!=val)
         bounce = TRUE;
   }
}

