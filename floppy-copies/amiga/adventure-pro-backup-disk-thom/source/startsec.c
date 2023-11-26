
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "adventure.h"

startsec(win,num,x1,y1)
int num,x1,y1;
struct Window *win;
{
   extern struct sequence *sequences[MAXLIST];
   extern int planemask,numplanes,offx[100],offy[100];
   int k,i,j,l,x,y;
   struct sequence *a;
   struct Image *b,*bb;
   struct BitMap *c;

   a = sequences[num-1];
   c = &(a->background);
   if (a->flags & ON)
      return(-1);
   a->flags = a->flags | ON;
   if ((a->flags & PERM) == 0) {
      i = 0;
      while (i < (a->framenum)) {
         if (a->graphics[i] == 0) {
            a->graphics[i] = AllocMem(sizeof(struct Image),0);
            if (a->graphics[i] == 0) {
               printf("startsec. error 1 !");
               exit(10);
               }
            l = getgraphic(a->graphics[i],a->names[i],0);
            if (l != 0) {
               printf("startsec. error 2 !");
               exit(10);
               }
            }
         i++;
         }
      }
   i = 0;
   k = 0;
   while (i < (a->framenum)) {
      b = a->graphics[i];
      if ((RASSIZE((b->Width),(b->Height))) >
          (RASSIZE((a->graphics[k]->Width),(a->graphics[k]->Height))))
         k = i;
      i++;
      }

bb = a->graphics[k];
a->backnum = (char) k;
/* c->BytesPerRow = bb->Width / 8 + 1; */
c->BytesPerRow = RASSIZE(bb->Width,1);
c->Rows = bb->Height;
c->Depth = numplanes;
c->Flags = 0;
c->Planes[0] = AllocMem(c->BytesPerRow * c->Rows *
                                numplanes ,MEMF_CHIP | MEMF_CLEAR);
l = 1;
while (l < numplanes) {
   c->Planes[l] = c->Planes[l-1] + (c->BytesPerRow * c->Rows);
   l++;
   }

offx[num-1] = x1;
offy[num-1] = y1;

if ((a->flags & (SIMPLE | SUPER))==0)   {

      b = a->graphics[k];
      x =(offx[num-1]) + (a->x[0]) ;
      y = (offy[num-1]) + (a->y[0]);
      BltBitMap (win->RPort->BitMap,x,y,
           c,0,0,c->BytesPerRow * 8 ,c->Rows ,0x0c0,planemask,0);
      }

if (a->sec[0] > 0)   {

   x =(offx[num-1]) + (a->x[0]) ;
   y = (offy[num-1]) + (a->y[0]);
   b = a->graphics[a->sec[0] - 1];
   if (a->flags & SUPER) {
      BltBitMap (win->RPort->BitMap,x,y,
           c,0,0,b->Width ,b->Height ,0x0c0,planemask,0);
      }
   DrawImage (win->RPort,b,x,y);

   }
}



