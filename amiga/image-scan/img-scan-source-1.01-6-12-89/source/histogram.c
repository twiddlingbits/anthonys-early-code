#include <exec/types.h>
#include <graphics/rastport.h>
#include "scan.h"

extern struct config config;

/******************************************************************/
/** Draws a histogram, given rastport, position and hight/widht **/
/******************************************************************/

void do_hist(rp,x,y,h,w,data,do_label)

UBYTE *data;

{
char *numlab = "BK  2   4   6   8   10  12    WT";
int bar,xpos,this_height;
long k[16], maxval, i;

void histogram();

histogram(data, k);

maxval = 0;
for (i=0; i<16; i++)
   if (k[i] > maxval)
      maxval = k[i];


SetDrMd(rp,JAM1);
SetAPen(rp,15);         /** white **/

if (do_label) i=10;
else i = 1;

RectFill(rp, x, y, x+w-1, y+h+i-1);

bar = w/34;

SetAPen(rp, 0);      /** Black **/

for (i=0; i<16; i++) {
   xpos = x+(w*(i+1))/17;
   this_height = k[i]*h/(maxval+1);
   if (do_label) {
      if (i>0 && i<10)
         Move(rp, xpos-4, y+h+9);
      else 
         Move(rp, xpos-8, y+h+9);
      Text(rp, &numlab[i<<1],2);
      }

/* printf("xpos %d bar %d y %d h %d this_height %d\n",
   xpos,bar,y,h,this_height); */

   RectFill(rp, xpos-bar+1, y+h-this_height, xpos+bar-1, y+h);
   }
}

void histogram(data, k)

UBYTE *data;
long k[16];

{
register int i;

for (i=0; i<16; i++)
   k[i]=0;

for (i=0; i < 256000; i=i+3)
   k[config.translate[data[i]]]++;
}


