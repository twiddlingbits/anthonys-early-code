
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

activate(num)
int num;
{
   extern struct sequence *sequences[MAXLIST];
   struct sequence *a;

   a = sequences[num-1];
   if ((a->flags & ON) == 0)
      return(-1);
   if (a->flags & ACTIVATED)
      return(-1);
   a->cell = 0;
   a->flags = a->flags | ACTIVATED;
   a->count = a->speed;

}



