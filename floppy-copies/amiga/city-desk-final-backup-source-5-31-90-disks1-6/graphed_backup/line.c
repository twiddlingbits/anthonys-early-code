

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"


USHORT colorholder;


linea(win,mclass,mcode,Mx,My,gadg,mode,X1,Y1,brush,rpt,line,holdrp,Brush)
struct Window *win;
int *mode,brush,gadg;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*holdrp;
LONG Mx,My,*X1,*Y1;
struct Border *line;
struct BitMap *Brush;
{
extern int gibblecolor,gibbleplanes;
extern int yeoldmanx[6],yeoldmany[6];

if (mclass == MOUSEBUTTONS && (mcode == SELECTDOWN || mcode == MENUDOWN)) {
   ClipBlit (win->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
      holdrp,0,0,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,gibbleplanes,0);
   *mode = LINEB;
   *X1 = Mx;
   *Y1 = My;
   if (mcode == SELECTDOWN) {
      rpt->AOlPen = gibblecolor;
      SetAPen(rpt,gibblecolor);
      }
   else {
      rpt->AOlPen = 0;
      SetAPen(rpt,0);
      }
   line->XY[2] = Mx - *X1;
   line->XY[3] = My - *Y1;
   DrawBorder (rpt,line,*X1 ,*Y1 );
   colorholder = mcode;

   }
}



lineb(screen,win,mclass,mcode,Mx,My,gadg,mode,X1,Y1,Brush,brush,rp,rpt,line)
struct Screen *screen;
struct Window *win;
struct Brush *Brush;
int *mode,brush,gadg;
ULONG mclass;
USHORT mcode;
struct RastPort *rp,*rpt;
LONG Mx,My,*X1,*Y1;
struct Border *line;
{

int i,OFFX,OFFY,X2,Y2,X3,Y3,cop1;
extern int gibblecolor;

DrawBorder (rpt,line,*X1 ,*Y1 );
line->XY[2] = Mx - *X1;
line->XY[3] = My - *Y1;
DrawBorder (rpt,line,*X1 ,*Y1 );
if (mclass == MOUSEBUTTONS &&(mcode == SELECTUP || mcode == MENUUP)) {
   DrawBorder (rpt,line,*X1 ,*Y1 );
   rpt->AOlPen = gibblecolor;
   SetAPen(rpt,gibblecolor);
   if (colorholder == MENUDOWN) {
      rpt->AOlPen = 0;
      SetAPen(rpt,0);
   }
   *mode = LINEA;
   if (brush == BRUSHA) {
      Move(rpt,*X1,*Y1);
      Draw(rpt,Mx,My);
      }
   else {
   newpointer(screen,BUSY);
   OFFX = Mx - *X1;
   OFFY = My - *Y1;
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
   *X1 = *X1 << 16;
   *Y1 = *Y1 << 16;
   if (cop1 != 0) {
      X3 = (OFFX << 16) / cop1;
      Y3 = (OFFY << 16) / cop1;
      }
   while (++i <= cop1) {
      *X1 = *X1 + X3;
      *Y1 = *Y1 + Y3;
      if (colorholder == MENUDOWN)
         putbm(win,Brush,rp,0,(*X1 >> 16),(*Y1 >> 16),brush,gibblecolor);
      else
         putbm(win,Brush,rp,1,(*X1 >> 16),(*Y1 >> 16),brush,gibblecolor);
      }
     newpointer(screen,CROSS);
     } /* else end */

   }

}



