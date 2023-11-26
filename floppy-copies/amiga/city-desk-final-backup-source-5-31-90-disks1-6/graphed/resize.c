
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)

int ickyx,ickyy;

resizea(win,mclass,mcode,Mx,My,gadg,mode,X1,Y1,brush,rpt,Brush,rp,rp2,undorp,box)
struct Window *win;
int *mode,brush,gadg;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*rp,*rp2,*undorp;
LONG Mx,My,*X1,*Y1;
struct BitMap *Brush;
struct Border *box;
{

extern int yeoldmanx[6],yeoldmany[6];
extern int gibblecolor,gibbleplanes,gibblex;

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

if (mclass == MOUSEBUTTONS && mcode == SELECTDOWN ) {
   ClipBlit (rp2,0,0,win->RPort,*X1 - yeoldmanx[5],*Y1 - yeoldmany[5]
             ,Brush->BytesPerRow * 8 ,Brush->Rows ,0xc0,1,0);
   *mode = RESIZEB;
   *X1 = Mx;
   *Y1 = My;
   putundo(win,undorp);
   rpt->AOlPen = 1;
   SetAPen(rpt,1);
   ickyx = *X1 - yeoldmanx[5];
   ickyy = *Y1 - yeoldmany[5];
   box->XY[3] = My-ickyy;
   box->XY[5] = box->XY[3];
   box->XY[4] = Mx-ickyx;
   box->XY[6] = box->XY[4];
   DrawBorder (rpt ,box ,ickyx ,ickyy );
   }
}






resizeb(win,mode,tmode,mclass,mcode,Mx,My,X1,Y1,cutw,cuth,rpt,Brush,rp,hrp,undorp,box)
struct Window *win;
int *mode,tmode,*cutw,*cuth;
ULONG mclass;
USHORT mcode;
struct RastPort *rpt,*rp,*hrp,*undorp;
LONG Mx,My,*X1,*Y1;
struct BitMap *Brush;
struct Border *box;
{

extern int yeoldmanx[6],yeoldmany[6],gibblex;
struct BitMap newbm;
struct RastPort nbrushrp;
LONG nx,ny,workbyt,holdbyt;
int error,i,j;

   DrawBorder (rpt ,box ,ickyx ,ickyy );
   box->XY[3] = My-ickyy;
   box->XY[5] = box->XY[3];
   box->XY[4] = Mx-ickyx;
   box->XY[6] = box->XY[4];
   DrawBorder (rpt ,box ,ickyx ,ickyy );


if (mclass == MOUSEBUTTONS && mcode == SELECTUP ) {
   DrawBorder (rpt ,box ,ickyx ,ickyy );
   nx = Mx - ickyx;
   ny = My - ickyy;
   if (nx <1 || ny <1)  goto end1;

   newpointer(win->WScreen,BUSY);
   InitRastPort(&nbrushrp);
   newbm.Depth = rpt->BitMap->Depth;
   nbrushrp.BitMap = &newbm;
   error = initbitmap(NULL,&nbrushrp,0,0,nx,ny);
   if (error != AOK) goto end1;

   holdbyt = RASSIZE(nx,1);
   workbyt = Brush->BytesPerRow;
   i = 0;
   while (i <newbm.Depth)
      fit (Brush->Planes[i],0,Brush->Rows,(workbyt * 8),workbyt,
           newbm.Planes[i++],0,ny,nx,holdbyt);
   nbrushrp.BitMap = Brush;
   clearbitmap(&nbrushrp);
   movmem(&newbm,Brush,sizeof(struct BitMap));

   clearbitmap(hrp);
   hrp->BitMap->Depth = rpt->BitMap->Depth;
   *X1 = Mx;
   *Y1 = My;
   error = initbitmap(win->RPort,hrp,*X1 - nx/2,*Y1 - ny/2,nx,ny);
   if (error != AOK) {
      telluser2(win->WScreen,"resize mem.err 1!",NULL,FALSE);
      goto end1;
      }
   *mode = tmode;
   *cutw = nx;
   gibblex = nx;
   *cuth = ny;
   yeoldmanx[5] = nx/2;
   yeoldmany[5] = ny/2;


   end1:
   newpointer(win->WScreen,CROSS);
   return(error);
   }
}




