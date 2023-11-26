

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "adventure.h"
#include "sed1.h"


updatesec(wprint)
struct winprint *wprint;
{

extern int offx[100],offy[100],secnum,planemask;
extern struct sequence *sequences[MAXLIST];
extern struct IOAudio IOB[4];
extern struct Samp samp[4];
extern int slave[4][2];
int ii,iii,jj,ll,x,y,z;
char k;
struct sequence *a;
struct Image *b,*bb;
struct BitMap *c;

ii = 0;
while (ii < secnum) {
   a = sequences[ii];
   c = &(a->background);
   if ((a->flags & ON) && (a->flags & ACTIVATED)) {
      a->count--;
      if (a->count == 0) {
         a->count = a->speed;
         k = a->cell;
         (a->cell)++;
         if (a->cell == a->number) {
            if ((a->flags & REPEAT) == 0) {
               a->flags = a->flags & (ON | REPEAT | PERM | SIMPLE | SUPER);
               goto jump33;
               }
            a->cell = 0;
            }
         b = a->graphics[a->sec[a->cell] - 1];
         bb = a->graphics[a->sec[k] - 1];
         if (a->sec[k] > 0 && (a->flags &SUPER)) {
            x = (offx[ii]) + (a->x[k]);
            y = (offy[ii]) + (a->y[k]);
            WaitTOF();
            z = BltBitMap (c,0,0,wprint->WIN->RPort->BitMap,x,
            y,bb->Width ,bb->Height ,0x0c0,planemask,0);
            }

         if (a->sec[a->cell] == 0 && (a->flags & SUPER) == 0 &&
             (a->flags & SIMPLE)==0) {
            x = (offx[ii]) + (a->x[a->cell]);
            y = (offy[ii]) + (a->y[a->cell]);
            WaitTOF();
            z = BltBitMap (c,0,0,wprint->WIN->RPort->BitMap,x,
            y,c->BytesPerRow * 8 ,c->Rows ,0x0c0,planemask,0);
            }

         if (a->sec[a->cell] > 0) {

            x = (offx[ii]) + (a->x[a->cell]);
            y = (offy[ii]) + (a->y[a->cell]);
            if (a->flags &SUPER) {
            WaitTOF();
               z = BltBitMap (wprint->WIN->RPort->BitMap,x,
               y,c,0,0,b->Width,b->Height,0x0c0,planemask,0);
               }
            WaitTOF();
            DrawImage (wprint->WIN->RPort,b,x,y);
            }
         iii = 0;
         while(iii <4) {
            if (slave[iii][0] == ii+1 && slave[iii][1] == a->cell+1)
               play(wprint->WIN,&IOB[iii],0,samp[iii].length,&samp[iii],1);
            iii++;
            }
         }
      }
   jump33:
   ii++;
   }

}



