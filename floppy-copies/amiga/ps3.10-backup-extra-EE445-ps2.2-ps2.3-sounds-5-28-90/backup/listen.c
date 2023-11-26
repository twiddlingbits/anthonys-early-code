#include "exec/types.h"
#include "psound.h"

/*********************************************/
/***        listen to digitizer            ***/
/*********************************************/

void listen (chan)
int chan;
{
void quash_bounce();
unsigned char *dat;
unsigned char *cra;
unsigned char *talo;
unsigned char *tahi;
unsigned char oldh,oldl;
int val;
static UBYTE *base,*pa,*data,*direction,*data2,*direction2;
static unsigned short sam,i;
short sam1,sam2;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short *aud1lch,*aud1len,*aud1vol,*aud1per,*aud1dat;
unsigned short *intreq,*intreqr;
unsigned int *aud0lc,*aud1lc;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;
cra = (unsigned char *)0xbfee01;
talo = (unsigned char *)0xbfe401;
tahi = (unsigned char *)0xbfe501;
dat = (unsigned char *)0xbfe101;

/** PB0     data bit 2 **/
/** PB1     data bit 3 **/
/** PB2     data bit 4 **/
/** PB3     data bit 5 **/
/** PB4     data bit 6 **/
/** PB5     data bit 7 **/
/** PB6     pulse at sampling rate **/
/** PB7     pulse to change gain (PA2 specifies up/down) **/
/** PA0     data bit 1 **/
/** PA1     data bit 0 **/
/** PA2     channel select **/

*direction=64+128;     /* all bits are read except PB7 (gain) */
*data=255;
*direction2=4;         /* pa2 write (sel) */

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

oldl=*talo;
oldh=*tahi;

val = 10000;         /** 10 samples per second **/
val = 715909/val;    /** samp/sec to counter ticks **/
val = val/2;
*talo = val&0xFF;
*tahi = val/256;
*cra = 7;            /** 7 continous toggle **/


if (chan==STEREO)
   while ((*pa&64)==64) {
      }

/** select channel here ... *data2=chan; */

if (chan==RIGHT) {
   *data2 = 4;       /** right **/
   while ((*pa&64)==64) {
      while ( ((*data)&64)==0) ;      /** wait for PB6 to pulse high **/
      while ( ((*data)&64)==64) ;      /** wait for PB6 to pulse low **/
      sam1=*data;    /** get previous samples upper 6 data **/
      sam2=*data2;   /** get previos samples low 2 bits **/
/*  printf("1 %x 2 %x\n",sam1,sam2); */
      sam1 = sam1<<2;
/* printf("shift 1 %x 2 %x\n",sam1,sam2); */
      if (sam2&1) sam1 |= 1;
      if (sam2&2) sam1 |= 2;
/* printf("lowbits 1 %x 2 %x\n",sam1,sam2); */
      sam1=sam1&0xFF;
      sam = sam1 - 128;     /** convert to 2's comp **/
/* printf("subtract: sam: %x\n\n",sam); */
      sam=sam&0xFF;
/*  printf("sam: %x\n",sam); */
      i=sam|(sam<<8);
/* printf("sam: %x\n\n",i);    */
      *aud1dat=i;
      *intreq=256;
      }
   }

if (chan==LEFT) {
   *data2 = 0;    /** left **/
   while ((*pa&64)==64) {
      while ( ((*data)&64)==0) ;      /** wait for PB6 to pulse high **/
      while ( ((*data)&64)==64) ;      /** wait for PB6 to pulse low **/
      sam1=(*data)&0x3f;    /** get previous samples upper 6 data **/
      sam2=*data2;   /** get previos samples low 2 bits **/
/*  printf("1 %x 2 %x\n",sam1,sam2); */
      sam1 = sam1<<2;
/* printf("shift 1 %x 2 %x\n",sam1,sam2); */
      if (sam2&1) sam1 |= 1;
      if (sam2&2) sam1 |= 2;
/* printf("lowbits 1 %x 2 %x\n",sam1,sam2); */
      sam = sam1 - 128;     /** convert to 2's comp **/
/* printf("subtract: sam: %x\n\n",sam); */
      sam=sam&0xFF;
/*  printf("sam: %x\n",sam); */
      i=sam|(sam<<8);
/* printf("sam: %x\n\n",i);    */
      *aud1dat=i;
      *intreq=256;
      }
   }

*talo = oldl;        /** retore  timer mode **/
*tahi = oldh;
*cra = 0;

quash_bounce(0);     /** wait for button to go down **/
quash_bounce(64);    /** wait for button to come up **/

Enable();
}

/***************************************************************/
/** This routine waits for the mouse left button to stabilize **/
/** on the value passed                                       **/
/***************************************************************/

void quash_bounce(val)

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

void dec_gain()
{
UBYTE *data,*data2,*direction;

data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */

*direction=64+128;

*data2 &= (~4);
*data &= (~128);
*data |= 128;
}

void inc_gain()
{
UBYTE *data,*data2,*direction;

data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */

*direction=64+128;
*data2 |= 4;
*data &= (~128);
*data |= 128;
}
