
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

loadperm(s)
struct sequence *s;
{
   int i,l;

   if ((s->flags & PERM)==0)
      return(-1);
   i = 0;
   while (i < (s->framenum)) {
      if (s->graphics[i] == 0) {
         s->graphics[i] = AllocMem(sizeof(struct Image),0);
         if (s->graphics[i] == 0) {
            printf("permsec. error 1 !");
            exit(10);
            }
         l = getgraphic(s->graphics[i],s->names[i],0);
         if (l != 0) {
            printf("permsec. error 2 !");
            exit(10);
            }
         }
      i++;
      }
   return(-1);

}
