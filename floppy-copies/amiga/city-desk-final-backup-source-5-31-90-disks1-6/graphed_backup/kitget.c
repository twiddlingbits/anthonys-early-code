
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)

kitget(mess,kitwin,mode,mode2,brush,qdraw,gmode,rp,rff,screen,box,win,X1,Y1)
struct Screen *screen;
struct IntuiMessage *mess;
struct Window *kitwin,*win;
int *mode,*mode2,*brush,*qdraw,gmode,*rff;
struct RastPort *rp;
struct Border *box;
LONG X1,Y1;
{

ULONG mclass;
USHORT mcode;
LONG Mx,My;
int brushb,i,j,k;
int mode3,mode4,boxf;
extern int gibblecolor;
extern BYTE twolinesflag;

mclass = mess->Class;
mcode = mess->Code;
Mx = mess->MouseX;
My = mess->MouseY;

boxf=NO;

if (mcode == SELECTDOWN && (*mode != MAGB && *mode != MAGA))  {

   if (My < 10) {            /*   color switch handler    */
      j = kitwin->Width / (1<<screen->BitMap.Depth);
      if (screen->Width > 325)
         j++;
      box->XY[3] = 9;
      box->XY[5] = box->XY[3];
      box->XY[4] = j;
      box->XY[6] = box->XY[4];
      DrawBorder (kitwin->RPort ,box ,j * gibblecolor ,0);
      gibblecolor = Mx /j;
      if (gibblecolor > 15)
         gibblecolor = 15;
      DrawBorder (kitwin->RPort ,box ,j * gibblecolor ,0);

      return(AOK);
      }

   if (*mode == CIRCLEA)  {
      mode3 = DRAWA;
      goto jump2;
      }

   if ((Mx >= 480 && (gmode == 1 || gmode == 3)) ||
       (Mx >= 236 && (gmode == 0 || gmode == 2))) {
      boxf=NO;
      goto jump2;
      }

   if (twolinesflag == 1)
      drawtwolines(win,X1,Y1);
   if (*mode == BOXA) {
      if (*mode2 == CUTB) {
         if (*brush < 5)
            setbox(kitwin,rp,*mode2,0,gmode,*qdraw);
         setbox(kitwin,rp,*mode,0,gmode,*qdraw);
         }
      }

   if (gmode == 0 || gmode == 2) {
      if (Mx < 24) {
         mode3 = DRAWA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 47) {
         mode3 = DRAWB;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 71) {
         mode3 = LINEA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 94) {
         mode3 = BOXA;
         boxf=YES;
         goto jump1;
         }
      if (Mx < 118) {
         mode3 = FILLA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 141) {
         mode3 = FLIPA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 165) {
         mode3 = ROTA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 189) {
         mode3 = MAGA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 212) {
         if (My <17) {
            if (Mx < 195)
               brushb=BRUSHA;
            else
            if (Mx < 202)
               brushb=BRUSHB;
            else
               brushb=BRUSHC;
            }
         else {
            if (Mx < 200)
               brushb=BRUSHD;
            else
               brushb=BRUSHE;
            }

         mode3 = BRUSHF;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 236) {
         mode3 = CUTA;
         boxf=YES;
         goto jump1;
         }
      }
   else {
      if (Mx < 48) {
         mode3 = DRAWA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 96) {
         mode3 = DRAWB;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 143) {
         mode3 = LINEA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 191) {
         mode3 = BOXA;
         boxf=YES;
         goto jump1;
         }
      if (Mx < 239) {
         mode3 = FILLA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 287) {
         mode3 = FLIPA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 335) {
         mode3 = ROTA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 383) {
         mode3 = MAGA;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 431) {
         if (My <17) {
            if (Mx < 402)
               brushb=BRUSHA;
            else
            if (Mx < 414)
               brushb=BRUSHB;
            else
               brushb=BRUSHC;
            }
         else {
            if (Mx < 410)
               brushb=BRUSHD;
            else
               brushb=BRUSHE;
            }

         mode3 = BRUSHF;
         boxf=NO;
         goto jump1;
         }
      if (Mx < 480) {
         mode3 = CUTA;
         boxf=YES;
         }
      }
   jump1:
   mode4 = *mode;
   if (mode3==BRUSHF)
      mode4=BRUSHF;
   if (mode4 == DRAWA && *qdraw == NO)
      mode4=DRAWB;
   if (mode3 == CUTA && *brush == 5) {
      *mode = BOXA;
      *mode2 = CUTB;
      setbox(kitwin,rp,mode4,*brush,gmode,*qdraw);
      goto jump2;
      }
   if (mode3 == BRUSHF) {  /* was a normal brush selected?  */
      if (*brush == 5)
         setbox(kitwin,rp,CUSTBRUSH,*brush,gmode,*qdraw);
      else {
         setbox(kitwin,rp,BRUSHF,*brush,gmode,*qdraw);
         }
      if (*mode == BOXA) {
            setbox(kitwin,rp,BOXA,*brush,gmode,*qdraw);
         if (*qdraw == NO)
            mode4=DRAWB;
         else
            mode4=DRAWA;
         *mode = mode4;
         setbox(kitwin,rp,BRUSHF,brushb,gmode,*qdraw);
         }
      *brush = brushb;
      setbox(kitwin,rp,mode4,*brush,gmode,*qdraw);
      goto jump2;
      }
   setbox(kitwin,rp,mode4,*brush,gmode,*qdraw);
   *mode = mode3;
   if (mode3 == DRAWA)
      *qdraw = YES;

   if (mode3 == DRAWB) {
      *mode = DRAWA;
      *qdraw = NO;
      }
   if (mode3 == BOXA && *mode2 != BOXB)
      *mode2 = BOXB;

   setbox(kitwin,rp,*mode,*brush,gmode,*qdraw);

   if (mode3 == CUTA /* && *mode2 != CUTB */ ) {
      *mode = BOXA;
      *mode2 = CUTB;
      }

   jump2:
   if (mode3 == FLIPA)
      *rff = flipget(screen,rp);
   if (mode3 == ROTA) {
      *rff = rotget(screen,rp);
      }

   }
if (mcode == SELECTDOWN && (*mode == MAGA)) {
   if (Mx + 40 >win->Width-1)
   X1 = win->Width-41;
   if (My + 20 >win->Height-1)
   Y1 = win->Height-21;
   DrawBorder (rp,box ,X1 ,Y1 );
   }


return(boxf);
}




