
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

rotate(tempbm,rmode,cutw,cuth,X1,Y1,Brush,rp2,rff,scnd)
int rff,rmode,scnd;
struct RastPort *rp2;
LONG *X1,*Y1,cutw,cuth;
struct BitMap *tempbm,*Brush;
{

int i,ii,offx,offy;
struct RastPort rp;

InitRastPort(&rp);
rp.BitMap = Brush;
rp2->BitMap = tempbm;
if (rmode == 0) {
*X1 = cutw;
*Y1 = cuth;
if (rff != 1) {
   *X1 = cuth;
   *Y1 = cutw;
   }
tempbm->Depth = scnd;
initbitmap(NULL,rp2,0,0,*X1,*Y1);

if (rff == 1) {
   i = 0;
   do {
      ii = 0;
      do {
         offx = ReadPixel(&rp,i,ii);
         if (offx >0) {
            SetAPen(rp2,offx);
            WritePixel(rp2,*X1-i-1,*Y1-ii-1);
            }
         } while (++ii <cuth);
      } while (++i <cutw);
   };

if (rff == 0) {
   i = 0;
   do {
      ii = 0;
      do {
         offx = ReadPixel(&rp,i,ii);
         if (offx >0) {
            SetAPen(rp2,offx);
            WritePixel(rp2,*X1-ii-1,i);
            }
         } while (++ii <cuth);
      } while (++i <cutw);
   };

if (rff == 2) {
   i = 0;
   do {
      ii = 0;
      do {
         offx = ReadPixel(&rp,i,ii);
         if (offx >0) {
            SetAPen(rp2,offx);
            WritePixel(rp2,ii,*Y1-i-1);
            }
         } while (++ii <cuth);
      } while (++i <cutw);
   };

}
else {

*X1 = cutw;
*Y1 = cuth;
if (rff != 1) {
   *X1 = cuth*2;
   *Y1 = cutw/2;
   }
tempbm->Depth = scnd;
initbitmap(NULL,rp2,0,0,*X1,*Y1);

   if (rff == 1) {
      i = 0;
      do {
         ii = 0;
         do {
            offx = ReadPixel(&rp,i,ii);
            if (offx >0) {
               SetAPen(rp2,offx);
               WritePixel(rp2,cutw-i-1,cuth-ii-1);
               }
            } while (++ii <cuth);
         } while (++i <cutw);
      };

   if (rff == 0) {
      i = 0;
      do {
         ii = 0;
         do {
            offx = ReadPixel(&rp,i,ii);
            offy = ReadPixel(&rp,i+1,ii);
            if (offx >0 || offy >0) {
               SetAPen(rp2,offx);
               WritePixel(rp2, *X1-ii*2-1, i/2);
               SetAPen(rp2,offy);
               WritePixel(rp2, *X1-ii*2, i/2);
               }

            } while (++ii <cuth);
         ++i;
         } while (++i <cutw);
      };

   if (rff == 2) {
      i = 0;
      do {
         ii = 0;
         do {
            offx = ReadPixel(&rp,i,ii);
            offy = ReadPixel(&rp,i+1,ii);
            if (offx >0 || offy >0) {
               SetAPen(rp2,offx);
               WritePixel(rp2, ii*2,   *Y1-i/2-1);
               SetAPen(rp2,offy);
               WritePixel(rp2, ii*2+1, *Y1-i/2-1);
               }

            } while (++ii <cuth);
         ++i;
         } while (++i <cutw);
      };


}

}


