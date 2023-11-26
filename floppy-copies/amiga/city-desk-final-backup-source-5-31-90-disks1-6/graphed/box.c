

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"


box(win,colorholder,Mx,My,X1,Y1,brush,rpt,Box,Brush,rp)
struct Window *win;
int colorholder,brush;
struct RastPort *rpt,*rp;
LONG X1,Y1,Mx,My;
struct Border *Box;
struct BitMap *Brush;
{

LONG X2,Y2,X3,Y3,i,ii,OFFX,OFFY,cop1,X4,Y4,cutx,cuty;
extern int gibblecolor;

if (brush == BRUSHA) {
   rpt->AOlPen = 1;
   SetAPen(rpt,1);
   if (colorholder == MENUDOWN) {
      rpt->AOlPen = 0;
      SetAPen(rpt,0);
      }
   Box->DrawMode = JAM1;
   DrawBorder (rpt ,Box ,X1 ,Y1 );
   Box->DrawMode = COMPLEMENT;
   }
else {
   ii = 0;
      X4 = X1;
      Y4 = Y1;
      cutx = X1;
      cuty = Y1;
   while(ii <7)  {
      X1 = X4 + Box->XY[ii];
      Y1 = Y4 + Box->XY[ii+1];
      Mx = X4 + Box->XY[ii+2];
      My = Y4 + Box->XY[ii+3];

      OFFX = Mx - X1;
      OFFY = My - Y1;
      X2 = OFFX;
      if (X2 < 0)
         X2 = X2 * -1;
      Y2 = OFFY;
      if (Y2 < 0)
         Y2 = Y2 * -1;
      cop1 = X2;
      if (Y2 > X2)
         cop1 = Y2;

      i = 0;
      X1 = X1 << 16;
      Y1 = Y1 << 16;
      if ( cop1 != 0) {
      X3 = (OFFX << 16) / cop1;
      Y3 = (OFFY << 16) / cop1;
      }
      while (++i <= cop1) {
         X1 = X1 + X3;
         Y1 = Y1 + Y3;
         if (colorholder == MENUDOWN)
            putbm(win,Brush,rp,0,(X1 >> 16),(Y1 >> 16),brush,gibblecolor);
         else
            putbm(win,Brush,rp,1,(X1 >> 16),(Y1 >> 16),brush,gibblecolor);
         }
      ii++;
      ii++;
      }  /* while end */
   } /* else end */
Box->FrontPen = 1;


}

