

#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

setbox(win,rp,box,brush,gmode,qdraw)
int box,brush,gmode,qdraw;
struct Window *win;
struct RastPort *rp;
{
static struct BitMap big = {
0,15,0,4,0,
0,0,0,0,0,0,0,0};
static struct BitMap small = {
0,7,0,4,0,
0,0,0,0,0,0,0,0};

int hsizeb,hsizes,sx,sy;

if (box == DRAWA && qdraw == NO)
   box = DRAWB;
if (box == LINEB)
   box = LINEA;
if (box == BOXB)
   box = BOXA;
if (box == FLIPB)
   box = FLIPA;
if (box == ROTB)
   box = ROTA;
if (box == MAGB)
   box = MAGA;
if (box == CUTA || box == CUTB)
   box = CUSTBRUSH;

hsizeb = 49;
if (gmode == 0 || gmode == 2)
   hsizeb = 25;
big.BytesPerRow = RASSIZE(hsizeb,1);
hsizes = 13;
if (gmode == 0 || gmode == 2)
   hsizes = 7;
small.BytesPerRow = RASSIZE(hsizes,1);

big.Planes[0] = AllocMem(RASSIZE(hsizeb,15),MEMF_CHIP);
big.Planes[1] = big.Planes[0];
big.Planes[2] = big.Planes[0];
big.Planes[3] = big.Planes[0];

small.Planes[0] = AllocMem(RASSIZE(hsizes,7),MEMF_CHIP);
small.Planes[1] = small.Planes[0];
small.Planes[2] = small.Planes[0];
small.Planes[3] = small.Planes[0];

if (box == BRUSHF) {
   rp->BitMap = &small;
   if (gmode == 0 || gmode == 2) {
      sx = 188;
      sy = 0;
      if (brush == BRUSHB)
         sx = 194;
      if (brush == BRUSHC)
         sx = 201;
      if (brush == BRUSHD) {
         sx = 192;
         sy = 7;
         }
      if (brush == BRUSHE) {
         sx = 199;
         sy = 7;
         }
      }
   else {
      sx = 386;
      sy = 1;
      if (brush == BRUSHB)
         sx = 398;
      if (brush == BRUSHC)
         sx = 410;
      if (brush == BRUSHD) {
         sx = 393;
         sy = 7;
         }
      if (brush == BRUSHE) {
         sx = 406;
         sy = 7;
         }
      }
   ClipBlit (rp,0,0,win->RPort,sx,
             sy+10,hsizes ,7 ,0x50,1,0);
   }
else {

rp->BitMap = &big;
sy = 0;
sx = 0;
if (gmode == 0 || gmode == 2) {
   if (box == DRAWB)
      sx = 24;
   if (box == LINEA)
      sx = 47;
   if (box == BOXA)
      sx = 71;
   if (box == FILLA)
      sx = 94;
   if (box == FLIPA)
      sx = 118;
   if (box == ROTA)
      sx = 141;
   if (box == MAGA)
      sx = 165;
   if (box == CUSTBRUSH)
      sx = 212;
   }
else {
   if (box == DRAWB)
      sx = 48;
   if (box == LINEA)
      sx = 96;
   if (box == BOXA)
      sx = 143;
   if (box == FILLA)
      sx = 191;
   if (box == FLIPA)
      sx = 239;
   if (box == ROTA)
      sx = 287;
   if (box == MAGA)
      sx = 335;
   if (box == CUSTBRUSH)
      sx = 431;
   }
   ClipBlit (rp,0,0,win->RPort,sx+1,
             sy+1+10,hsizeb-1 ,13 ,0x50,1,0);
   }

FreeMem(big.Planes[0],RASSIZE(hsizeb,15));
FreeMem(small.Planes[0],RASSIZE(hsizes,7));

}

