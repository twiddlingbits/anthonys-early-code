
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

endsec(num,win)
int num;
struct Window *win;
{
   extern struct sequence *sequences[MAXLIST];
   int i,j,l,x,y;
   extern int planemask,offx[100],offy[100];
   struct sequence *a;
   struct Image *b;
   struct BitMap *c;

   a = sequences[num-1];
   c = &(a->background);
   if ((a->flags & ON) == 0)
      return(-1);
   a->flags = a->flags & (SIMPLE | SUPER | REPEAT | PERM);
   b = a->graphics[(a->sec[a->cell] - 1)];

   if ((a->flags & REPEAT) == 0 && a->cell == a->number)
      a->cell = 0;

   if ((a->flags & SIMPLE)==0) {
      x = (offx[num-1])+(a->x[a->cell]);
      y = (offy[num-1])+(a->y[a->cell]);
      BltBitMap (c,0,0,win->RPort->BitMap,x,
              y,c->BytesPerRow * 8 ,c->Rows ,0x0c0,planemask,0);
      }

   FreeMem(c->Planes[0],RASSIZE((c->BytesPerRow * 8),c->Rows)*c->Depth);
   c->Planes[0] = 0;

   if ((a->flags & PERM)==0) {
      i = 0;
      while (i < (a->framenum)) {
         if (a->graphics[i] != 0) {
            FreeMem(a->graphics[i]->ImageData,( ( (a->graphics[i]->Width+15)
            >>4) <<1)*(a->graphics[i]->Height)*(a->graphics[i]->Depth));
            FreeMem(a->graphics[i],sizeof(struct Image));
            a->graphics[i] = 0;
            }
         i++;
         }
      }
}
