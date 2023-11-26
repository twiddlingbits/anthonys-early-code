#include "exec/types.h"
#include "psound.h"

/**************************************/
/**   show input levels             **/
/*************************************/

level(Window)
struct Window *Window;
{
static int k,right,left,rmin,rmax,lmin,lmax,i;
UBYTE *data,*direction,*pa;
UBYTE *data2,*direction2;
char line[80];

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
      *data2=(UBYTE)0;           /* select both channels */
      i=*data;                   /* start conversion    */
      *data2=(UBYTE)RIGHT;       /* select right chennel */
      for(i=0;i<5000;i++) ;      /* wait till finished */
      right=*data;               /* get right channel value */
      *data2=(UBYTE)LEFT;        /* select left channel */
      left=*data;                /* get left channel value */
      if (left<lmin) lmin=left;
      if (left>lmax) lmax=left;
      if (right<rmin) rmin=right;
      if (right>rmax) rmax=right;
      sprintf(line,"L: %3.3d %3.3d %3.3d  R: %3.3d %3.3d %3.3d   D: %4.4d",
              lmin,lmax,left,rmin,rmax,right,right-left);
      msg(Window,line);
      }
   }
}

