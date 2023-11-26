
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

startrmsec(num,win)
int num;
struct Window *win;
{
   extern struct sequence *sequences[MAXLIST];
   int i;
   extern int secnum;
   struct sequence *a;

   i = 0;
   while(i < secnum) {
   a = sequences[i];
   if (a->room == num) {
      startsec(win,i+1,0,0);
      activate(i+1);
      }
   i++;
   }
}


