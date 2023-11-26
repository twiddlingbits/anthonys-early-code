
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

flip(cutw,cuth,Brush,rp2,rff)
int rff;
struct RastPort *rp2;
LONG cutw,cuth;
struct BitMap *Brush;
{

int i,ii,offx,offy;
struct RastPort rp;

InitRastPort(&rp);
rp.BitMap = Brush;


if (rff == 1) {
   i = 0;
   do {
      ii = 0;
      do {
         offx = ReadPixel(&rp,i,ii);
         if (offx >0) {
            SetAPen(rp2,offx);
            WritePixel(rp2,i,cuth-ii-1);
            }
         } while (++ii <cuth);
      } while (++i <cutw);
   }

if (rff == 0) {
   i = 0;
   do {
      ii = 0;
      do {
         offx = ReadPixel(&rp,i,ii);
         if (offx >0) {
            SetAPen(rp2,offx);
            WritePixel(rp2,cutw-i-1,ii);
            }
         } while (++ii <cuth);
      } while (++i <cutw);
   }



}


