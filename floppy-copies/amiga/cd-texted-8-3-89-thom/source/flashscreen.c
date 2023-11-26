
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "desk.h"


void flashscreen(vp,l,r,g,b)
struct ViewPort *vp;
int r,g,b,l;
{

UWORD tempcolor,t2;

tempcolor = GetRGB4(vp->ColorMap,0);

if (r == -1) {
   t2 = tempcolor;
   r = 15-((t2>>8)&15);
   }
if (g == -1) {
   t2 = tempcolor;
   g = 15-((t2>>4)&15);
   }
if (b == -1) {
   t2 = tempcolor;
   b = 15-(t2&15);
   }

SetRGB4(vp,0,r,g,b);

while(l-- > 0)
   WaitTOF();

t2 = tempcolor;
r = ((t2>>8)&15);
g = ((t2>>4)&15);
b = (t2&15);
SetRGB4(vp,0,r,g,b);

}


