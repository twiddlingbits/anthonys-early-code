
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

SHORT wrestlingfish;

drawa(win,mclass,mcode,Mx,My,gadg,mode,X1,Y1,brush,rpt,Brush,rp,rp2,undorp)
struct Window *win;
int *mode,brush,gadg;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*rp,*rp2,*undorp;
LONG Mx,My,*X1,*Y1;
struct BitMap *Brush;
{

extern int yeoldmanx[6],yeoldmany[6];
extern int gibblecolor,gibbleplanes;
extern struct RastPort gibblerp;

if(brush != 0) {
   ClipBlit (rp2,0,0,win->RPort,*X1 - yeoldmanx[brush],*Y1 - yeoldmany[brush]
             ,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,gibbleplanes,0);
   *X1 = Mx;
   *Y1 = My;
   ClipBlit (win->RPort,*X1 - yeoldmanx[brush],*Y1 - yeoldmany[brush]
             ,rp2,0,0,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,gibbleplanes,0);
   if (brush < 5)
      putbm(win,Brush,rp,1,Mx,My,brush,gibblecolor);
   else
      putbm(win,Brush,rp,1,Mx,My,brush,15);
   }

if (mclass == MOUSEBUTTONS && (mcode == SELECTDOWN || mcode == MENUDOWN)) {
   if(brush != BRUSHA)
      ClipBlit (rp2,0,0,win->RPort,*X1 - yeoldmanx[brush],*Y1 - yeoldmany[brush]
             ,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,gibbleplanes,0);
   putundo(win,undorp);
   *mode = DRAWB;
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

   if (brush == BRUSHA) {
      Move(rpt,*X1,*Y1);
      Draw(rpt,Mx,My);
      }
   else {
      if (mcode == MENUDOWN)
         putbm(win,Brush,rp,0,Mx,My,brush,gibbleplanes);
      else {
         putbm(win,Brush,rp,0,Mx,My,brush,gibbleplanes);
/*         ClipBlit (&gibblerp,0,0, win->RPort,Mx - yeoldmanx[brush],
                   My - yeoldmany[brush], gibblerp.BitMap->BytesPerRow
                   * 8, gibblerp.BitMap->Rows ,0x20);     */
         putbm(win,Brush,rp,1,Mx,My,brush,gibblecolor);
         win->RPort->Mask = 15;
         }
      }
   }
   wrestlingfish = mcode;
}




drawb(win,rp,qdraw,mclass,mcode,Mx,My,gadg,mode,X1,Y1,brush,rpt,Brush,
      OFFX,OFFY,X2,Y2,X3,Y3,rp2)
struct Window *win;
int qdraw,*mode,brush,gadg;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*rp,*rp2;
LONG Mx,My,*X1,*Y1,*OFFX,*OFFY,*X2,*Y2,*X3,*Y3;
struct BitMap *Brush;

{

LONG cop1,x1,y1,x2,y2;
int i;
extern int yeoldmanx[6],yeoldmany[6],gibbleplanes,gibblecolor;

   if (brush == BRUSHA) {
      Move(rpt,*X1,*Y1);
      Draw(rpt,Mx,My);
      }
   else {
      if (qdraw == YES) {
         if (wrestlingfish == MENUDOWN)
            putbm(win,Brush,rp,0,Mx,My,brush,gibbleplanes);
         else {
            putbm(win,Brush,rp,0,Mx,My,brush,gibbleplanes);
            putbm(win,Brush,rp,1,Mx,My,brush,gibblecolor);
            }
         }
      else {
         x2 = Mx - *X1;
         y2 = My - *Y1;
         *X2 = x2;
         if (*X2 < 0)
            *X2 = (*X2) * -1;
         *Y2 = y2;
         if (*Y2 < 0)
            *Y2 = (*Y2) * -1;
         cop1 = *X2;
         if (*Y2 > *X2)
            cop1 = *Y2;

         i = 0;
         x1=*X1;
         y1=*Y1;
         x1 = (x1) << 16;
         y1 = (y1) << 16;
         if (cop1 != 0) {
         x2 = ((x2) << 16) / cop1;
         y2 = ((y2) << 16) / cop1;
            }
         while (++i <= cop1) {
            x1 = x1 + x2;
            y1 = y1 + y2;
            if (wrestlingfish == MENUDOWN)
               putbm(win,Brush,rp,0,(x1 >> 16),(y1 >> 16),brush,gibbleplanes);
            else {
               putbm(win,Brush,rp,0,(x1 >> 16),(y1 >> 16),brush,gibbleplanes);
               putbm(win,Brush,rp,1,(x1 >> 16),(y1 >> 16),brush,gibblecolor);
               }
            }
         }
      }
   *X1 = Mx;
   *Y1 = My;
   if (mclass == MOUSEBUTTONS) {
      if (mcode == SELECTUP || mcode == MENUUP) {
         *mode = DRAWA;
         if (brush != BRUSHA) {
            ClipBlit (win->RPort,*X1 - yeoldmanx[brush],*Y1 - yeoldmany[brush]
                   ,rp2,0,0,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,
                   gibbleplanes,0);
/*            putbm(win,Brush,rp,1,Mx,My,brush,gibblecolor);
            */
            }
         }
      }
}



