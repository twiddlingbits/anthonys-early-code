
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)

/*
    initializes a bitmap, clears it, and fills it with given source
    unless source = NULL
            ***** takes RPorts as arguments ! *****
    returns OK, OUTOFMEM
            ***** set depth in BitMap.Depth ! *****
    BytesPerRow and Rows are re-initialized to the given dimensions.
    if OUTOFMEM, the Planes pointers and the BytesPerRow are zeroed out
    before returning.
*/

initbitmap(rp,rp2,x,y,w,h)
struct RastPort *rp,*rp2;
int x,y,w,h;
{

int i;

i = 0;
while (i < 6)
   rp2->BitMap->Planes[i++] = 0;
rp2->BitMap->BytesPerRow = RASSIZE(w,1);
rp2->BitMap->Rows = h;
i = rp2->BitMap->Depth;
while (i-- > 0) {
   rp2->BitMap->Planes[i] = (PLANEPTR) AllocMem(RASSIZE(w,h),MEMF_CHIP | MEMF_CLEAR);
   if (rp2->BitMap->Planes[i] == NULL) {
      while (++i < rp2->BitMap->Depth) {
         FreeMem(rp2->BitMap->Planes[i],RASSIZE(w,h));
         rp2->BitMap->Planes[i] = 0;
         }
      rp2->BitMap->BytesPerRow = 0;
      return(OUTOFMEM);
      }
   }
if (rp != NULL)
   ClipBlit (rp,x,y,rp2,0,0,RASSIZE(w,1)*8,h,0xc0);

return(AOK);
}

/*
      This routine frees the ram used in a bitmap, and returns
      with the Planes pointers and the BytesPerRow zeroed out.
            ***** takes RPorts as arguments ! *****
*/


clearbitmap(rp)
struct RastPort *rp;
{

int i;

i = 0;
while (i <rp->BitMap->Depth) {
   SafeFreeMem(rp->BitMap->Planes[i],rp->BitMap->BytesPerRow * rp->BitMap->Rows);
   rp->BitMap->Planes[i++] = 0;
   }
rp->BitMap->BytesPerRow = 0;
setmem(rp->BitMap,sizeof(struct BitMap),0);

}



