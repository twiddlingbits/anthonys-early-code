#include <exec/types.h>
#include "graphics/gfxbase.h"
#include <intuition/intuition.h>
#include "adventure.h"

struct GfxBase *GfxBase;

printx(wprint,text)
struct winprint *wprint;
char *text;
{

SHORT i,x,y;
SHORT w,h,stp,len;
char single,nulchar;
char *text2;
int charcount;

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
               Move(wprint->WIN->RPort, 10+(wprint->x*8), 10+(wprint->y*8));
               Text(wprint->WIN->RPort," ",1);
               wprint->x++;
               wprint->stopnum++;
               }
            wprint->x = 0;
            wprint->y++;
            while(wprint->y > h )  {
               ScrollRaster(wprint->WIN->RPort,0,8,10,4,w*8,(h+1)*8+4);
               wprint->y=wprint->y-1;
               }
            }
         charcount++;
         text2++;
         }

      if (*text ==' ' && wprint->x == 0)
         i=i;
      else {
         Move(wprint->WIN->RPort, 10+(wprint->x*8), 10+(wprint->y*8));
         Text(wprint->WIN->RPort,text,1);
         wprint->x++;
         if (wprint->x > w-2) {
            wprint->x = 0;
            wprint->y++;
            }
         wprint->stopnum++;
         }
      }

   if (*text == '^')  {
      --len;
      text++;

      if (*text == 'n') {
         wprint->y++;
         wprint->stopnum = wprint->stopnum + w - wprint->x;
         wprint->x = 0;
         }
      if (*text == 'c') {
         SetAPen(wprint->WIN->RPort,0);
         RectFill(wprint->WIN->RPort, 4, 2, w*8, (h+1)*8+4);
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
            }
         wprint->stopnum = wprint->stopnum - 1;
         }
      if(*text =='h')  {
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
   text++;
   }
}


