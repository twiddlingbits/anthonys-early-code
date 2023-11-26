
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)

graphic_ed (g)
struct graphic *g;
{
void printmode();
struct Menu *Tit1,*getmenu1();
struct Menu *Tit2,*getmenu2();
struct MenuItem *nextitem;
struct Screen *screen;
struct Window *Window,*nwwin();
struct Window *MagWin,*magwin();
UBYTE *tempbm;
struct RastPort *rpt;
struct IntuiMessage *mess,*GetMsg();
ULONG mclass;
USHORT mcode;
LONG Mx,My,cutx,cuty,cuth,cutw;
SHORT end,*data1;
SHORT button,mn1,mn2,mn3;
LONG workbyt,holdbyt,X1,Y1,OFFX,OFFY,X2,Y2,X3,Y3,X4,Y4,cop1;
int errorck,ret3,rff,color,qdraw,i,ii,f,mode,mode2,brush;
int scnw,scnh,gmode,gadg,crossflag;

static SHORT pblock[] =
{
0xFC00,
0xFC00,
0xFC00,
0xFC00,
0xFC00,
};
static SHORT bdataa[] =
{
0,0,
0,0,
};
static SHORT bdatab[] =
{
-1,-1,
 2,-1,
 3,0,
 -2,0,
 -2,1,
 3,1,
 2,2,
 -1,2,
};
static SHORT bdatae[] =
{
-2,-1,
 3,-1,
 3,0,
 -2,0,
 -2,1,
 3,1,
 3,2,
 -2,2,
};
static SHORT bdatac[] =
{
-3,-3,
3,-3,
5,-2,
-5,-2,
-6,-1,
6,-1,
6,0,
-6,0,
-6,1,
6,1,
5,2,
-5,2,
-3,3,
3,3,
};
static SHORT bdataf[] =
{
-6,-3,
6,-3,
6,-2,
-6,-2,
-6,-1,
6,-1,
6,0,
-6,0,
-6,1,
6,1,
6,2,
-6,2,
-6,3,
6,3,
};
static SHORT bdatad[] =
{
0,0,
0,50,
50,50,
50,0,
0,0,
};

static SHORT bdatax[] =
{
0,0,
640,0,
};

static SHORT bdatay[] =
{
0,0,
0,200,
};

static struct Border Brush[5] =  {
{
0,0,
1,0,COMPLEMENT,
2,
&bdataa[0],
NULL,
},
{
0,0,
1,0,JAM1,
8,
&bdatab[0],
NULL,
},
{
0,0,
1,0,JAM1,
14,
&bdatac[0],
NULL,
},
{
0,0,
1,0,JAM1,
8,
&bdatae[0],
NULL,
},
{
0,0,
1,0,JAM1,
14,
&bdataf[0],
NULL,
},  };

static struct Border twocon =  {
0,0,
1,0,COMPLEMENT,
2,
&bdatay[0],
NULL,
};

static struct Border twolines =  {
0,0,
1,0,COMPLEMENT,
2,
&bdatax[0],
&twocon,
};

static struct Border Box =
{
0,0,
1,0,COMPLEMENT,
5,
&bdatad[0],
NULL,
};

static struct Image Block =
{
0,0,
6,5,1,
&pblock[0],
0x1,0x0,NULL,
};

data1 =(SHORT *) AllocMem(10,MEMF_CHIP);
movmem(&pblock[0],data1,10);
Block.ImageData = data1;
errorck = AOK;
scnw = 640;
scnh = 200;
gmode = 1;
screen = (struct Screen *) openscreen(scnw,scnh,1);
if (screen == NULL) {
   Window=NULL;
   errorck = OUTOFMEM;
   goto ENDSUB2;
   }
Window = nwwin(screen);
if (Window == NULL) {
   errorck = OUTOFMEM;
   goto ENDSUB2;
   }

newpointer(screen,CROSS);
rpt = Window->RPort;
tempbm=0;
cutw=0;
cuth=0;
rpt->AOlPen = 1;
brush = BRUSHA;
color = 1;          
SetAPen(rpt,color);
end = 0;
button = 0;
qdraw = YES;
crossflag = NO;
X1 = 0;
Y1 = 0;
mode = DRAWA;
printmode(rpt,mode,color);
blkwht(Window,gmode,color);
gadg = 540;

Tit1 = getmenu1();
Tit2 = getmenu2();
SetMenuStrip(Window,Tit1);
Brush[1].FrontPen = color;
Brush[2].FrontPen = color;
SetAPen(rpt,1);
changechecks(gmode,qdraw,brush);

/* goto ENDSUB1;  1st try */

if (g->image != NULL)
   disgraphic(rpt,g,scnw,scnh);


ReportMouse(TRUE,Window);
ModifyIDCMP(Window,RAWKEY | MENUPICK | MOUSEBUTTONS | MOUSEMOVE);

while (TRUE)  {

   blkwht(Window,gmode,color);
   Wait(1<<Window->UserPort->mp_SigBit);
   while(mess = GetMsg(Window->UserPort)) {

      mclass = mess->Class;
      mcode = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == RAWKEY && 0x40 == (char)mcode)  {

         if(mode == BOXB ||mode == CUTB ||mode == PASTEB ||mode == ERASEB ||
            mode == PASTEN || mode == MAGB ||mode == ROTB ||mode == FLIPB )
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
         if(mode == BOXA || mode == ROTA )
            DrawBorder (rpt ,&twolines,0,0);
         if(mode == LINEB)
         DrawBorder (rpt,&Brush[0],X1 ,Y1 );
         mode = DRAWA;
         blkwht(Window,gmode,color);
         printmode(rpt,mode,color);
         if (gmode ==0 || gmode == 2)
            SetMenuStrip(Window,Tit2);
         else
            SetMenuStrip(Window,Tit1);
         goto jump111;
         }

      if (mclass == RAWKEY && 0x51 == (char)mcode)  {
         if (color == 1)
            color = 0;
         else
            color = 1;
         blkwht(Window,gmode,color);
         Brush[1].FrontPen = color;
         Brush[2].FrontPen = color;
         Brush[3].FrontPen = color;
         Brush[4].FrontPen = color;
         SetAPen(rpt,color);
         }

      if (mclass == MENUPICK)  {
         while (mcode != MENUNULL) {

            mn1 = MENUNUM(mcode);
            mn2 = ITEMNUM(mcode);
            mn3 =  SUBNUM(mcode);

            if (mn1 == 2 && mn2 == 7 && mn3 == 0) {
               brush = BRUSHA;
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }
            if (mn1 == 2 && mn2 == 7 && mn3 == 1) {
               brush = BRUSHB;
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }
            if (mn1 == 2 && mn2 == 7 && mn3 == 2) {
               brush = BRUSHC;
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }
            if (mn1 == 2 && mn2 == 7 && mn3 == 3) {
               brush = BRUSHD;
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }
            if (mn1 == 2 && mn2 == 7 && mn3 == 4) {
               brush = BRUSHE;
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }

            if (mn1 == 0 && mn2 == 0) {
               ret3 = telluser(screen,"Are you sure","about this?");
               if (ret3 == FALSE)
                  goto jump1;
               rpt->AOlPen = 0;
               SetAPen(rpt,0);
               RectFill(rpt,0,0,scnw,scnh);
               rpt->AOlPen = 1;
               SetAPen(rpt,color);
               printmode(rpt,mode,color);
               blkwht(Window,gmode,color);
               goto jump1;
               }

            if (mn1 == 0 && mn2 == 1) {
               ret3 = telluser(screen,"Are you sure","about this?");
               if (ret3 == FALSE)
                  goto jump1;
               rpt->AOlPen = 0;
               SetAPen(rpt,0);
               RectFill(rpt,0,0,scnw,scnh);
               rpt->AOlPen = 1;
               SetAPen(rpt,color);
               if (g->image != NULL)
                  disgraphic(rpt,g,scnw,scnh);
               SetAPen(rpt,color);
               printmode(rpt,mode,color);
               blkwht(Window,gmode,color);
               goto jump1;
               }

            if (mn1 == 0 && mn2 == 3) {
               if (cutw ==0 && cuth ==0) {
                  telluser2(screen,
                  "You have not yet created",
                  "a graphic cut!",FALSE);
                  goto jump1;
                  }
               ret3 = telluser(screen,
               "The current cut buffer will be",
               "your new graphic. Are you sure?");
               if (ret3 == FALSE)
                  goto jump1;
               SafeFreeMem(g->image,RASSIZE(g->w,g->h));
               g->image = tempbm;
               g->w =cutw;
               g->h =cuth;
               g->usex = 0;
               g->usey =0;
               g->usew =g->w;
               g->useh =g->h;
               g->numplanes = 1;

               if (gmode == 0)
                  g->aspect=(ASPLOW*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 1)
                  g->aspect=(ASPMED*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 2)
                  g->aspect=(ASPLOWLACE*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 3)
                  g->aspect=(ASPHI*(ULONG)g->w)/((ULONG)g->h);
               goto ENDSUB2;
               }

            if (mn1 == 0 && mn2 == 4) {
               ret3 = telluser(screen,"Do you REALLY","want to exit?");
               if (ret3 == FALSE)
                  goto jump1;
               errorck = CANCEL;
               goto ENDSUB1;
               }

            if (3 == 2) {
               boxstart:
               printmode(rpt,mode2,color);
               bdatay[0] = Mx;
               bdatay[3] = scnh;
               bdatay[2] = Mx;
               bdatax[1] = My;
               bdatax[2] = scnw;
               bdatax[3] = My;
               DrawBorder (rpt ,&twolines ,0,0 );
               crossflag = YES;
               goto jump1;
               }

            if (mn1 == 2 && mn2 == 0) {
               mode = DRAWA;
               qdraw = YES;
               if (mn3 == 1)
                  qdraw = NO;
               printmode(rpt,mode,color);
               changechecks(gmode,qdraw,brush);
               goto jump1;
               }
            if (mn1 == 2 && mn2 == 1) {
               mode = LINEA;
               printmode(rpt,mode,color);
               goto jump1;
               }

            if (mn1 == 2 && mn2 == 2) {
               mode2 = BOXB;
               mode = BOXA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 2 && mn2 == 3) {
               mode = FILLA;
               printmode(rpt,mode,color);
               goto jump1;
               }

            if (mn1 == 1 && mn2 == 0) {
               mode2 = CUTB;
               mode = BOXA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 1 && mn2 == 1) {
               if (cutw ==0 && cuth ==0) {
                  telluser2(screen,
                  "You have not yet created",
                  "a graphic cut!",FALSE);
                  goto jump1;
                  }
               mode = PASTEA;
               printmode(rpt,mode,color);
               goto jump1;
               }

            if (mn1 == 1 && mn2 == 2) {
               if (cutw ==0 && cuth ==0) {
                  telluser2(screen,
                  "You have not yet created",
                  "a graphic cut!",FALSE);
                  goto jump1;
                  }
               mode2 = PASTEN;
               mode = BOXA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 1 && mn2 == 3) {
               mode2 = ERASEB;
               mode = BOXA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 2 && mn2 == 6) {
               mode = MAGA;
               printmode(rpt,mode,color);
               goto jump1;
               }

            if (mn1 == 2 && mn2 == 4) {
               rff = mn3;
               mode2 = ROTB;
               mode = ROTA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 2 && mn2 == 5) {
               rff = mn3;
               mode2 = FLIPB;
               mode = BOXA;
               printmode(rpt,mode2,color);
               goto boxstart;
               }

            if (mn1 == 0 && mn2 == 2) {
               if (mn3 == gmode)
                  goto jump1;
               ret3 = telluser(screen,"WARNING: this will","destroy your edits!");
               if (ret3 == FALSE)
                  goto jump1;
               if (mn3 == 0) {
                  scnw = 320;
                  scnh = 200;
                  gmode = 0;
                  }
               if (mn3 == 1) {
                  scnw = 640;
                  scnh = 200;
                  gmode = 1;
                  }
               if (mn3 == 2) {
                  scnw = 320;
                  scnh = 400;
                  gmode = 2;
                  }
               if (mn3 == 3) {
                  scnw = 640;
                  scnh = 400;
                  gmode = 3;
                  }

               ClearMenuStrip(Window);
               CloseWindow(Window);
               CloseScreen(screen);
               screen = (struct Screen *) openscreen(scnw,scnh,1);
               if (screen == NULL) {
                  errorck = OUTOFMEM;
                  goto ENDSUB1;
                  }
               Window = nwwin(screen);
               if (Window == NULL) {
                  errorck = OUTOFMEM;
                  goto ENDSUB1;
                  }
               newpointer(screen,CROSS);
               rpt = Window->RPort;
               rpt->AOlPen = 1;
               SetAPen(rpt,color);
               mode = DRAWA;
               if (gmode ==0 || gmode == 2)
                  SetMenuStrip(Window,Tit2);
               else
                  SetMenuStrip(Window,Tit1);
               ReportMouse(TRUE,Window);
               ModifyIDCMP(Window,RAWKEY | MENUPICK | MOUSEBUTTONS | MOUSEMOVE);
               if (g->image != NULL)
                  disgraphic(rpt,g,scnw,scnh);
               printmode(rpt,mode,color);
               blkwht(Window,gmode,color);
               changechecks(gmode,qdraw,brush);
               gadg = 540;
               if (gmode == 0 || gmode == 2)
                  gadg = 249;
               goto jump111;
               }

               jump1:
               if (gmode ==0 || gmode == 2)
                  nextitem = ItemAddress(Tit2,mcode);
               else
                  nextitem = ItemAddress(Tit1,mcode);
               mcode = nextitem->NextSelect;
         }
         jump111:
         }


      else switch(mode)  {

      case DRAWA:
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
         if (Mx <= gadg + 4 || Mx >= gadg + 53 || My <= 2 || My >= 14) {
            mode = DRAWB;
            X1 = Mx;
            Y1 = My;
            if (brush == BRUSHA) {
               Move(rpt,X1,Y1);
               Draw(rpt,Mx,My);
               }
            else
               DrawBorder (rpt,&Brush[brush],Mx,My);
            } ;
            } ;
         break;

      case DRAWB:
         if (brush == BRUSHA) {
            Move(rpt,X1,Y1);
            Draw(rpt,Mx,My);
            }
         else {
         if (qdraw == YES)
            DrawBorder (rpt,&Brush[brush],Mx,My);

         else {
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
         X3 = (OFFX << 16) / cop1;
         Y3 = (OFFY << 16) / cop1;
         while (++i <= cop1) {
            X1 = X1 + X3;
            Y1 = Y1 + Y3;
            DrawBorder (rpt,&Brush[brush], (X1 >> 16), (Y1 >> 16));
            }
           }
           } /* else end */
         X1 = Mx;
         Y1 = My;
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTUP)
               break;
            mode = DRAWA;
            };
         break;

      case LINEA:
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
         if (Mx <= gadg + 4 || Mx >= gadg + 53 || My <= 2 || My >= 14) {
            mode = LINEB;
            X1 = Mx;
            Y1 = My;
            bdataa[2] = Mx - X1;
            bdataa[3] = My - Y1;
            DrawBorder (rpt,&Brush[0],X1 ,Y1 );
            } ;
            } ;
         break;

      case LINEB:
         DrawBorder (rpt,&Brush[0],X1 ,Y1 );
         bdataa[2] = Mx - X1;
         bdataa[3] = My - Y1;
         DrawBorder (rpt,&Brush[0],X1 ,Y1 );
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTUP)
               break;
            mode = LINEA;
            if (brush == BRUSHA) {
               Move(rpt,X1,Y1);
               Draw(rpt,Mx,My);
               }
            else {
            newpointer(screen,BUSY);
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
            X3 = (OFFX << 16) / cop1;
            Y3 = (OFFY << 16) / cop1;
            while (++i <= cop1) {
               X1 = X1 + X3;
               Y1 = Y1 + Y3;
               DrawBorder (rpt,&Brush[brush], (X1 >> 16), (Y1 >> 16));
               }
              newpointer(screen,CROSS);
              } /* else end */

            } ;
         break;

      case BOXA:
         ClearMenuStrip(Window);
         DrawBorder (rpt ,&twolines ,0,0 );
         bdatay[0] = Mx;
         bdatay[2] = Mx;
         bdatax[1] = My;
         bdatax[3] = My;
         DrawBorder (rpt ,&twolines ,0,0 );
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
         if (Mx <= gadg + 4 || Mx >= gadg + 53 || My <= 2 || My >= 14) {
            DrawBorder (rpt ,&twolines ,0,0 );
            crossflag = NO;
            mode = BOXB;
            X1 = Mx;
            Y1 = My;

            bdatad[3] = My - Y1;
            bdatad[5] = bdatad[3];
            bdatad[4] = Mx - X1;
            bdatad[6] = bdatad[4];
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            } ;
            } ;
         break;

      case BOXB:
         DrawBorder (rpt ,&Box ,X1 ,Y1 );
         bdatad[3] = My - Y1;
         bdatad[5] = bdatad[3];
         bdatad[4] = Mx - X1;
         bdatad[6] = bdatad[4];
         DrawBorder (rpt ,&Box ,X1 ,Y1 );

         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTUP)
               break;
            mode = DRAWA;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            X2 = X1;
            X3 = Mx;
            Y2 = Y1;
            Y3 = My;
            if (X3 < X2) {
               Mx = X2;
               X1 = X3;
               }
            if (Y3 < Y2) {
               My = Y2;
               Y1 = Y3;
               }
            bdatad[3] = My - Y1;
            bdatad[5] = bdatad[3];
            bdatad[4] = Mx - X1;
            bdatad[6] = bdatad[4];

         /* branch for boxb,cutb,pasten,eraseb,flipb */
         switch(mode2)  {

         case BOXB:

            Box.FrontPen = color;
            if (brush == BRUSHA) {
               Box.DrawMode = JAM1;
               DrawBorder (rpt ,&Box ,X1 ,Y1 );
               Box.DrawMode = COMPLEMENT;
               }
            else {
               newpointer(screen,BUSY);
               ii = 0;
                  X4 = X1;
                  Y4 = Y1;
               while(ii <7)  {
                  X1 = X4 + bdatad[ii];
                  Y1 = Y4 + bdatad[ii+1];
                  Mx = X4 + bdatad[ii+2];
                  My = Y4 + bdatad[ii+3];

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
                  X3 = (OFFX << 16) / cop1;
                  Y3 = (OFFY << 16) / cop1;
                  while (++i <= cop1) {
                     X1 = X1 + X3;
                     Y1 = Y1 + Y3;
                     DrawBorder (rpt,&Brush[brush], (X1 >> 16), (Y1 >> 16));
                     }
                  ii++;
                  ii++;
                  }  /* while end */
               } /* else end */
            Box.FrontPen = 1;
            mode = DRAWA;
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            break;

         case CUTB:
            newpointer(screen,BUSY);
            SafeFreeMem(tempbm,RASSIZE(cutw,cuth));
            mode = DRAWA;
            cutx = X1;
            cuty = Y1;
            cuth = bdatad[3];
            cutw = bdatad[4];
            workbyt = rpt->BitMap->BytesPerRow;
            holdbyt = RASSIZE(cutw,1);
            mode = DRAWA;
            tempbm = (UBYTE *)AllocMem(RASSIZE(cutw,cuth),0);
            setmem(tempbm,RASSIZE(cutw,cuth),0);
            fit (rpt->BitMap->Planes[0]+workbyt *cuty,cutx,cuth,cutw,workbyt,
               tempbm,0,cuth,cutw,holdbyt);
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            break;

         case PASTEN:
            newpointer(screen,BUSY);
            mode = DRAWA;
            workbyt = rpt->BitMap->BytesPerRow;
            fit (tempbm,0,cuth,cutw,holdbyt,
                rpt->BitMap->Planes[0]+workbyt *Y1,X1,bdatad[3],bdatad[4],workbyt);
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            break;

         case ERASEB:
            mode = DRAWA;
            rpt->AOlPen = 0;
            SetAPen(rpt,0);
            RectFill(rpt, X1, Y1, X1 + bdatad[4], Y1 + bdatad[3]);
            rpt->AOlPen = 1;
            SetAPen(rpt,color);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            break;

         case FLIPB:
            newpointer(screen,BUSY);
            SafeFreeMem(tempbm,RASSIZE(cutw,cuth));
            mode = DRAWA;
            cutx = X1;
            cuty = Y1;
            cuth = bdatad[3];
            cutw = bdatad[4];
            workbyt = rpt->BitMap->BytesPerRow;
            holdbyt = RASSIZE(cutw,1);
            mode = DRAWA;
            tempbm = (UBYTE *)AllocMem(RASSIZE(cutw,cuth),0);
            setmem(tempbm,RASSIZE(cutw,cuth),0);
            fit (rpt->BitMap->Planes[0]+workbyt *cuty,cutx,cuth,cutw,workbyt,
               tempbm,0,cuth,cutw,holdbyt);
            rpt->AOlPen = 0;
            SetAPen(rpt,0);
            RectFill(rpt,X1,Y1,X1+cutw,Y1+cuth);
            rpt->AOlPen = 1;
            SetAPen(rpt,1);

            if (rff == 1) {
               i = 0;
               do {
                  ii = 0;
                  do {
                     OFFX = GETPIX(tempbm,holdbyt,i,ii);
                     if (OFFX == 1)
                        WritePixel(rpt,X1+cutw-i,Y1+ii);
                     } while (ii++ <cuth-1);
                  } while (i++ <cutw);
               };

            if (rff == 0) {
               i = 0;
               do {
                  ii = 0;
                  do {
                     OFFX = GETPIX(tempbm,holdbyt,i,ii);
                     if (OFFX == 1)
                        WritePixel(rpt,X1+i,Y1+cuth-ii);
                     } while (ii++ <cuth-1);
                  } while (i++ <cutw);
               };
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            SetAPen(rpt,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            break;

            } /* switch end */
            printmode(rpt,mode,color);
            } ;
         break;

      case FILLA:
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
            mode = DRAWA;
            newpointer(screen,BUSY);
            rpt->AOlPen = color;
            Flood (Window->RPort,0,Mx,My);
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            } ;
         break;


      case PASTEA:

         ClearMenuStrip(Window);
         bdatad[3] = cuth;
         bdatad[5] = bdatad[3];
         bdatad[4] = cutw;
         bdatad[6] = bdatad[4];
         DrawBorder (rpt ,&Box ,Mx ,My );
         X1 = Mx;
         Y1 = My;
         mode = PASTEB;
         break;

      case PASTEB:

            DrawBorder (rpt ,&Box ,X1 ,Y1 );
               X1 = Mx;
            if (Mx + cutw >scnw-1)
               X1 = scnw-1-cutw;
               Y1 = My;
            if (My + cuth >scnh-1)
               Y1 = scnh-1-cuth;

            DrawBorder (rpt ,&Box ,X1 ,Y1 );

         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
            newpointer(screen,BUSY);
            mode = DRAWA;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            workbyt = rpt->BitMap->BytesPerRow;
            fit (tempbm,0,cuth,cutw,holdbyt,
                rpt->BitMap->Planes[0]+workbyt *Y1,X1,cuth,cutw,workbyt);
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            } ;
         break;


      case MAGA:

         ClearMenuStrip(Window);
         bdatad[3] = 20;
         bdatad[5] = bdatad[3];
         bdatad[4] = 40;
         bdatad[6] = bdatad[4];
         DrawBorder (rpt ,&Box ,Mx ,My );
         X1 = Mx;
         Y1 = My;
         mode = MAGB;
         break;

      case MAGB:

            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            X1 = Mx;
            if (Mx + 40 >scnw-1)
               X1 = scnw-41;
            Y1 = My;
            if (My + 20 >scnh-1)
               Y1 = scnh-21;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );

         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
            mode = DRAWA;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            MagWin = magwin(screen);
            if (MagWin == NULL) {
               errorck=OUTOFMEM;
               goto ENDSUB1;
               }
            newpointer(screen,CROSS);
            i = 0;
            do {
               ii = 0;
               do {
                  f = ReadPixel(rpt,X1 + i,Y1 + ii);
                  if (f == 1)
                     DrawImage(MagWin->RPort,&Block,i * 6 + 2, ii * 5 + 10);
                  } while (ii++ <= 19);
               } while (i++ <= 39);

            ReportMouse(FALSE,Window);
            Maged( Window, MagWin, &Block, X1, Y1);
            CloseWindow(MagWin);
            Block.ImageData = data1;
            ReportMouse(TRUE,Window);
            if (gmode == 0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            printmode(rpt,mode,color);
            while(mess = GetMsg(Window->UserPort))
               ReplyMsg(mess);
            }
         break;

      case ROTA:
         ClearMenuStrip(Window);
         DrawBorder (rpt ,&twolines ,0,0 );
         bdatay[0] = Mx;
         bdatay[2] = Mx;
         bdatax[1] = My;
         bdatax[3] = My;
         DrawBorder (rpt ,&twolines ,0,0 );
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN)
               break;
            DrawBorder (rpt ,&twolines ,0,0 );
            mode = ROTB;
            X1 = Mx;
            Y1 = My;

            bdatad[4] = Mx - X1;
            bdatad[6] = bdatad[4];
            bdatad[3] = bdatad[4] / 2;
            bdatad[5] = bdatad[3];
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            i = YES;
            } ;
         break;

      case ROTB:
         if (i == YES)
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
         if (Mx < X1 || My < Y1)
            i = NO;
         else {
            bdatad[4] = Mx - X1;
            bdatad[6] = bdatad[4];
            bdatad[3] = bdatad[4] / 2;
            bdatad[5] = bdatad[3];
            if (bdatad[3]+Y1 >scnh-1) {
               bdatad[3] = scnh-1-Y1;
               bdatad[5] = bdatad[3];
               bdatad[4] = bdatad[3]*2;
               bdatad[6] = bdatad[4];
               }


            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            i = YES;
            }
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTUP)
               break;
            newpointer(screen,BUSY);
            SafeFreeMem(tempbm,RASSIZE(cutw,cuth));
            mode = DRAWA;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            cutx = X1;
            cuty = Y1;
            cuth = bdatad[3];
            cutw = bdatad[4];
            workbyt = rpt->BitMap->BytesPerRow;
            holdbyt = RASSIZE(cutw,1);
            mode = DRAWA;
            tempbm = (UBYTE *)AllocMem(RASSIZE(cutw,cuth),0);
            setmem(tempbm,RASSIZE(cutw,cuth),0);
            fit (rpt->BitMap->Planes[0]+workbyt *cuty,cutx,cuth,cutw,workbyt,
               tempbm,0,cuth,cutw,holdbyt);
            rpt->AOlPen = 0;
            SetAPen(rpt,0);
            RectFill(rpt,X1,Y1,X1+cutw,Y1+cuth);
            rpt->AOlPen = 1;
            SetAPen(rpt,1);

            if (rff == 1) {
               i = 0;
               do {
                  ii = 0;
                  do {
                     OFFX = GETPIX(tempbm,holdbyt,i,ii);
                     if (OFFX == 1)
                        WritePixel(rpt,X1+cutw-i,Y1+cuth-ii);
                     } while (ii++ <cuth-1);
                  } while (i++ <cutw);
               };

            if (rff == 0) {
               i = 0;
               do {
                  ii = 0;
                  do {
                     OFFX = GETPIX(tempbm,holdbyt, i,   ii);
                     OFFY = GETPIX(tempbm,holdbyt, i+1, ii);
                     if (OFFX == 1 || OFFY == 1) {
                        WritePixel(rpt, X1+cutw-ii*2,   Y1+i/2);
                        WritePixel(rpt, X1+cutw-ii*2+1, Y1+i/2);
                        }

                     } while (ii++ <cuth-1);
                  i++;
                  } while (i++ <cutw);
               };

            if (rff == 2) {
               i = 0;
               do {
                  ii = 0;
                  do {
                     OFFX = GETPIX(tempbm,holdbyt, i,   ii);
                     OFFY = GETPIX(tempbm,holdbyt, i+1, ii);
                     if (OFFX == 1 || OFFY == 1) {
                        WritePixel(rpt, X1+ii*2,   Y1+cuth-i/2);
                        WritePixel(rpt, X1+ii*2+1, Y1+cuth-i/2);
                        }

                     } while (ii++ <cuth-1);
                  i++;
                  } while (i++ <cutw);
               };

            SetAPen(rpt,color);
            newpointer(screen,CROSS);
            printmode(rpt,mode,color);
            if (gmode ==0 || gmode == 2)
               SetMenuStrip(Window,Tit2);
            else
               SetMenuStrip(Window,Tit1);
            }

         break;



      } /* switch end */

      if (mclass == MOUSEBUTTONS) {
         if (mcode != SELECTDOWN)
            break;
         if (Mx > gadg + 4 && Mx < gadg + 53 && My >2 && My < 14) {
            if (color == 1)
               color = 0;
            else
               color = 1;
            blkwht(Window,gmode,color);
            }
         else break;

         Brush[1].FrontPen = color;
         Brush[2].FrontPen = color;
         Brush[3].FrontPen = color;
         Brush[4].FrontPen = color;
         jumpcol:
         SetAPen(rpt,color);
         }

   }
  }
ENDSUB1:
SafeFreeMem(tempbm,RASSIZE(cutw,cuth));

ENDSUB2:
SafeFreeMem(data1,10);
ClearMenuStrip(Window);
newpointer(screen,NORMAL);
if (Window !=0)
   CloseWindow(Window);
if (screen!=0)
   CloseScreen(screen);

return(errorck);

} /*  end of subroutine   */



