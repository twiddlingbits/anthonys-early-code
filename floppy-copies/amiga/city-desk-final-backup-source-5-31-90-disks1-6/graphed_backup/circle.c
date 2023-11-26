

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"



circlea(mclass,mcode,Mx,My,mode,tmode,X1,Y1,rx,ry,rpt,hrp,Brush,brush)
int *mode,brush,tmode,rx,ry;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*hrp;
struct BitMap *Brush;
LONG Mx,My,*X1,*Y1;

{

extern int ickyx,ickyy,yeoldmanx[5],yeoldmany[5],gibblecolor;

SetDrMd(rpt,COMPLEMENT);
DrawEllipse(rpt,*X1,*Y1,rx,ry);
*X1 = Mx;
*Y1 = My;
DrawEllipse(rpt,*X1,*Y1,rx,ry);
SetDrMd(rpt,JAM1);

if (mclass == MOUSEBUTTONS ) {
   if (mcode == SELECTDOWN ) {
      SetAPen(rpt,gibblecolor);
      rpt->FgPen = gibblecolor;
      rpt->BgPen = 0;
      rpt->AOlPen = gibblecolor;
      DrawEllipse(rpt,*X1,*Y1,rx,ry);
      *mode = tmode;
/*      printf("tmode = %d\n",tmode);    */
      ClipBlit (rpt,*X1 - yeoldmanx[brush],*Y1 - yeoldmany[brush]
          ,hrp,0,0,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,1,0);
      }
   if (mcode == MENUDOWN ) {
      *mode = CIRCLEB;
      ickyx = *X1 - rx;
      ickyy = *Y1 - ry;
      }
   }

}



circleb(mclass,mcode,Mx,My,mode,X1,Y1,rx,ry,rpt)
int *mode,*rx,*ry;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt;
LONG Mx,My,*X1,*Y1;

{

extern int ickyx,ickyy;

SetDrMd(rpt,COMPLEMENT);
DrawEllipse(rpt,*X1,*Y1,*rx,*ry);
*rx = Mx - ickyx;
*ry = My - ickyy;
*X1 = Mx;
*Y1 = My;
if (*rx <1) {
  *rx = 1;
  *X1 = ickyx + 1;
  }
if (*ry <1) {
  *ry = 1;
  *Y1 = ickyy + 1;
  }

DrawEllipse(rpt,*X1,*Y1,*rx,*ry);
SetDrMd(rpt,JAM1);

if (mclass == MOUSEBUTTONS ) {
   if (mcode == MENUUP )
      *mode = CIRCLEA;
   }

}



