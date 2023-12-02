#include "si.h"

void showvaders(w,si_x,y)
struct Window *w;

{

short i,k,x;


for (i=0; i < NUMROW; i++) {
   x=si_x;
   for (k=0; k< NUMCOL; k++) {

      switch (i) {            /** select correct kind of invader **/
         case 0:
            drawsi3(w,x,y);
            break;
         case 1:
         case 2:
            drawsi2(w,x,y);
            break;
         case 3:
         case 4:
            drawsi1(w,x,y);
            break;
         }

      x=x+BOXWIDTH;
      }
   y=y+BOXHEIGHT;
   }

k = (USE_WIDTH - 40 - 50)/3;         /** 50 is width of bunker **/

for (i=40; i<USE_WIDTH; i=i+k)
   drawbunker(w, i, 160);
}

