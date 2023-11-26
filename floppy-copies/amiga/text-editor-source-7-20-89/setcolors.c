
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)


setcolors(vp,type,scnd)
struct ViewPort *vp;
int type,scnd;
{

int i,j,k;

if (type == 1) {
   SetRGB4(vp,2,2,2,13);
   SetRGB4(vp,1,3,3,3);
   }

if (type == 0) {
   i = 16>>scnd;
   j = 0;
   k = 15;
   while (j < 1<<scnd) {
      SetRGB4(vp,j++,k,k,k);
      k = k - i;
      if (scnd == 1)
         k = 0;
      }
   }

}
