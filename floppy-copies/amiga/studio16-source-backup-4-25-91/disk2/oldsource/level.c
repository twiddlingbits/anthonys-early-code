#include "exec/types.h"
#include "/include/psound.h"

/**************************************/
/**   show input levels             **/
/*************************************/

void level(Window)
struct Window *Window;
{
extern short digi_type;
void level3();
static int k,right,left,rmin,rmax,lmin,lmax,i;
UBYTE *data,*direction,*pa;
UBYTE *data2,*direction2;
char line[80];

set_digi_type();
if (digi_type!=PS_2) {
   level3(Window);
   return;
   }

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

pa=(UBYTE *)0xBFE001;
*direction=(UBYTE)0;           /* all bits are read */
*data=(UBYTE)0;
*direction2=(UBYTE)2+4;      /* pa1 & pa2 write */
*data2=(UBYTE)2;            /* pa2 -- select right channel */

while (1==1) {
   lmax=0;
   rmax=0;
   lmin=999;
   rmin=999;
   for (k=0;k<30;k++) {
      if ((*pa&64)!=64) {
         msg(Window,"");
         return(0);
         }
      *data2=(UBYTE)RIGHT;       /* select right channel **/
      for (i=0;i<50;i++) ;      /* let signal settel */
      i=*data;                   /* start conversion    */
      for(i=0;i<2000;i++) ;      /* wait till finished */
      right=*data;               /* get right channel value */
      *data2=(UBYTE)LEFT;        /* select left channel */
      for (i=0;i<50;i++) ;      /* let signal settel */
      i=*data;                   /* start conversion    */
      for(i=0;i<2000;i++) ;      /* wait till finished */
      left=*data;                /* get left channel value */
      if (left<lmin) lmin=left;
      if (left>lmax) lmax=left;
      if (right<rmin) rmin=right;
      if (right>rmax) rmax=right;
      sprintf(line,get_msg_ptr(220),
              lmin,lmax,left,rmin,rmax,right,right-left);
      msg(Window,line);
      }
   }
}

void level3(Window)
{
char line[80];
UBYTE *data2,*data,*direction,*direction2,*pa;
short k,x;
short l,r;

data=(UBYTE *)0xBFE101;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;
pa=(UBYTE *)0xBFE001;

*direction=64+128;
*direction2 = 4;

*data2 &= (~4);         /** select left channel */
*data |= 64;           /** pulse RD high **/
*data &= (~64);        /** pulse RD low **/

while (TRUE) {
   for (k=0;k<30;k++) {
      if ((*pa&64)!=64) {
         msg(Window,"");
         return(0);
         }
      }
   for (x=0; x<20; x++) ;        /** pause roughly 100 usecs **/
   l = (*data)&0x3f;             /** get upper 6 bits **/
   l = l << 2;                   /** move bits to correct positions **/
   *data2 |= 4;                  /** select right channel **/
   *data |= 64;                  /** start next sample **/
   *data &= (~64);

   for (x=0; x<20; x++) ;        /** pause roughly 100 usecs **/
   r = (*data)&0x3f;             /** get upper 6 bits **/
   r = r << 2;                   /** move bits to correct positions **/
   sprintf(line,get_msg_ptr(221),l,r);
   msg(Window,line);
   *data2 &= (~4);               /** select left **/
   *data |= 64;                  /** start next sample **/
   *data &= (~64);
   }
}
