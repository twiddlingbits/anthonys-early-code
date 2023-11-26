
#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"




void printmode(rpt,mode,color)
struct RastPort *rpt;
int mode,color;
{
SetAPen(rpt,1);
Move(rpt,170,7);
Text(rpt," mode: ",7);
if (mode == DRAWA || mode == DRAWB)
   Text(rpt,"Draw   ",7);

if (mode == BOXA || mode == BOXB)
   Text(rpt,"Box    ",7);

if (mode == LINEA || mode == LINEB)
   Text(rpt,"Line   ",7);

if (mode == FILLA )
   Text(rpt,"Fill   ",7);

if (mode == MAGA || mode == MAGB)
   Text(rpt,"Magnify ",8);

if (mode == ROTA || mode == ROTB)
   Text(rpt,"Rotate ",7);

if (mode == FLIPA || mode == FLIPB)
   Text(rpt,"Flip   ",7);

if (mode == ERASEA || mode == ERASEB)
   Text(rpt,"Erase  ",7);

if (mode == CUTB)
   Text(rpt,"Cut    ",7);

if (mode == PASTEA || mode == PASTEN )
   Text(rpt,"Paste  ",7);

SetAPen(rpt,color);

}

