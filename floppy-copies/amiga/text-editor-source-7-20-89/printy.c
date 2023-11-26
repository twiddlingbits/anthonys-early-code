
#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"


printy(wprint,number)
struct winprint *wprint;
int number;
{

SHORT x,y;
SHORT w,h;
char single,nulchar,string[10],*str;
int temp1,temp2,len;

x = wprint->X;
x = x +5;
y = wprint->Y;
y = y +6;
w = wprint->W;
w = w /8-1;
h = wprint->H;
h = h/8-1;
str = &string[0];

SetAPen(wprint->WIN->RPort,1);

len = stcu_d(str,number,9);

while (0 < len) {


   if (*str != '^')  {
      Move(wprint->WIN->RPort, x+(wprint->x*8), y+(wprint->y*8));
      Text(wprint->WIN->RPort,str,1);
      wprint->x=wprint->x + 1;
      if (wprint->x > w-2) {
         wprint->x = 0;
         wprint->y=wprint->y + 1;
         wprint->stopnum++;
         }
      }

   if (*str == '^')  {
      --len;
      str++;

      if (*str == 'n') {
         wprint->y = wprint->y + 1;
         wprint->stopnum++;
         wprint->x = 0;
         }
      if (*str == 'c') {
         SetAPen(wprint->WIN->RPort,0);
         RectFill(wprint->WIN->RPort, wprint->X,wprint->Y,
         wprint->X + wprint->W,wprint->Y + wprint->H);
         SetAPen(wprint->WIN->RPort,1);
         wprint->stopnum = 0;
         wprint->x = 0;
         wprint->y = 0;
         }
      if(*str =='b') {
         wprint->x=wprint->x - 1;
         if (wprint->x < 0) {
            wprint->x = w-2;
            wprint->y=wprint->y - 1;
            wprint->stopnum++;
            }
         }
      if(*str =='h')  {
      Move(wprint->WIN->RPort, x, y + h * 8);
      Text(wprint->WIN->RPort,"<MORE>",6);
      getx2(wprint);
      Move(wprint->WIN->RPort, x, y + h * 8);
      Text(wprint->WIN->RPort,"      ",6);
      wprint->stopnum = 0;
      }
      }

   while(wprint->y > h )  {
      ScrollRaster(wprint->WIN->RPort,0,8,x,y-6,x+w*8-10,y+(h+1)*8-6);
      wprint->y=wprint->y-1;
      wprint->stopnum++;
      }

   if(wprint->stopnum >= h*2) {
      Move(wprint->WIN->RPort, x, y + h * 8);
      Text(wprint->WIN->RPort,"<MORE>",6);
      getx2(wprint);
      Move(wprint->WIN->RPort, x, y + h * 8);
      Text(wprint->WIN->RPort,"      ",6);
      wprint->stopnum = 0;
      }

   --len;
   str++;
   }
}


