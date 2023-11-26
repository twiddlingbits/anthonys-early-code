#include <exec/types.h>
#include "graphics/gfxbase.h"
#include <intuition/intuition.h>
#include "adventure.h"


printy(wprint,number)
struct winprint *wprint;
int number;
{

extern struct GfxBase *GfxBase;
SHORT x,y;
SHORT w,h,stp;
char single,nulchar,string[10],*str;
int temp1,temp2,len;

GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0);
if (GfxBase == NULL)  {
   printf("graph.lib. won't open!\n");
   exit(10);
   }

x = wprint->WIN->LeftEdge;
x = x +10;
y = wprint->WIN->TopEdge;
y = y +10;
w = wprint->WIN->Width;
w = w /8-1;
h = wprint->WIN->Height;
h = h/8-2;
stp = w*h;
str = &string[0];

SetAPen(wprint->WIN->RPort,1);

len = stcu_d(str,number,9);

while (0 < len) {


   if (*str != '^')  {
      Move(wprint->WIN->RPort, 10+(wprint->x*8), 10+(wprint->y*8));
      Text(wprint->WIN->RPort,str,1);
      wprint->x=wprint->x + 1;
      if (wprint->x > w-2) {
         wprint->x = 0;
         wprint->y=wprint->y + 1;
         }
      wprint->stopnum = wprint->stopnum + 1;
      }

   if (*str == '^')  {
      --len;
      str++;

      if (*str == 'n') {
         wprint->y = wprint->y + 1;
         wprint->stopnum = wprint->stopnum + w - wprint->x;
         wprint->x = 0;
         }
      if (*str == 'c') {
         SetAPen(wprint->WIN->RPort,0);
         RectFill(wprint->WIN->RPort, 4, 2, w*8, (h+1)*8+4);
         SetAPen(wprint->WIN->RPort,1);
         wprint->stopnum = 0;
         wprint->x = 0;
         wprint->y = 0;
         }
      if(*str =='b') {
         wprint->x=wprint->x - 1;
         if (wprint->x < 0) {
            wprint->x = w;
            wprint->y=wprint->y - 1;
            }
         wprint->stopnum = wprint->stopnum - 1;
         }
      if(*str =='h')  {
      Move(wprint->WIN->RPort, 10, 10 + h * 8);
      Text(wprint->WIN->RPort,"<MORE>",6);
      getx2(wprint);
      Move(wprint->WIN->RPort, 10, 10 + h * 8);
      Text(wprint->WIN->RPort,"      ",6);
      wprint->stopnum = 0;
      }
      }

   while(wprint->y > h )  {
      ScrollRaster(wprint->WIN->RPort,0,8,10,4,w*8,(h+1)*8+4);
      wprint->y=wprint->y-1;
      }

   if(wprint->stopnum >= stp) {
      Move(wprint->WIN->RPort, 10, 10 + h * 8);
      Text(wprint->WIN->RPort,"<MORE>",6);
      getx2(wprint);
      Move(wprint->WIN->RPort, 10, 10 + h * 8);
      Text(wprint->WIN->RPort,"      ",6);
      wprint->stopnum = 0;
      }

   --len;
   str++;
   }
}


