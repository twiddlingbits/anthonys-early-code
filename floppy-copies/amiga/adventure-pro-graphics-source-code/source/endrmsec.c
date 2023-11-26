
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

endrmsec(num,win)
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
   if (a->room == num)
      endsec(i+1,win);
   i++;
   }
}


