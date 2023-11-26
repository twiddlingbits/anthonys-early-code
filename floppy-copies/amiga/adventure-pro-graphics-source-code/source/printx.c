#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"


printx(wprint,text)
struct winprint *wprint;
char *text;
{

SHORT i,x,y;
SHORT w,h,len;
char single,nulchar;
char *text2;
int charcount;

x = wprint->X;
x = x +5;
y = wprint->Y;
y = y +6;
w = wprint->W;
w = w /8-1;
h = wprint->H;
h = h/8-1;

SetAPen(wprint->WIN->RPort,1);

len = strlen(text);
while (0 < len) {


   if (*text != '^')  {
      text2 = text;
      charcount = 0;
      while (*text2 != ' ' && len - charcount > 0) {
         if (*text2 == '^')
            text2++;
         else if (charcount + wprint->x >w-2) {
            while (charcount > 1) {
               charcount--;
               Move(wprint->WIN->RPort, x+(wprint->x*8), y+(wprint->y*8));
               Text(wprint->WIN->RPort," ",1);
               wprint->x++;
               }
            wprint->x = 0;
            wprint->y++;
            while(wprint->y > h )  {
               ScrollRaster(wprint->WIN->RPort,0,8,x,y-6,x+w*8-10,y+(h+1)*8-6);
               wprint->y=wprint->y-1;
               wprint->stopnum++;
               }
            }
         charcount++;
         text2++;
         }

      if (*text ==' ' && wprint->x == 0)
         i=i;
      else {
         Move(wprint->WIN->RPort, x+(wprint->x*8), y+(wprint->y*8));
         Text(wprint->WIN->RPort,text,1);
         wprint->x++;
         if (wprint->x > w-2) {
            wprint->x = 0;
            wprint->y++;
            wprint->stopnum++;
            }
         }
      }

   if (*text == '^')  {
      --len;
      text++;

      if (*text == 'n') {
         wprint->y++;
         wprint->stopnum++;
         wprint->x = 0;
         }
      if (*text == 'c') {
         SetAPen(wprint->WIN->RPort,0);
         RectFill(wprint->WIN->RPort, wprint->X,wprint->Y,
         wprint->X + wprint->W,wprint->Y + wprint->H);
         SetAPen(wprint->WIN->RPort,1);
         wprint->stopnum = 0;
         wprint->x = 0;
         wprint->y = 0;
         }
      if(*text =='b') {
         wprint->x=wprint->x - 1;
         if (wprint->x < 0) {
            wprint->x = w-2;
            wprint->y=wprint->y - 1;
            wprint->stopnum = wprint->stopnum - 1;
            }
         }
      if(*text =='h')  {
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
   text++;
   }
}


