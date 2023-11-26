
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"


erase(rpt,brush,X1,Y1,rp,hrp,Brush,bdatad)
int brush;
SHORT *bdatad;
struct RastPort *rpt,*hrp;
LONG X1,Y1;
struct BitMap *Brush;
{

extern int yeoldmanx[6],yeoldmany[6];


rpt->AOlPen = 0;
SetAPen(rpt,0);
RectFill(rpt, X1, Y1, X1 + bdatad[4], Y1 + bdatad[3]);
rpt->AOlPen = 1;
SetAPen(rpt,1);
ClipBlit (rpt,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
   ,hrp,0,0,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,1,0);

}


