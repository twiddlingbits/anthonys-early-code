
#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)
int gibblecolor,gibbleplanes,gibblex;
struct RastPort gibblerp;


graphic_ed (g)
struct graphic *g;
{
void printmode();
struct Menu *Tit1,*getmenu1();
struct MenuItem *nextitem;
struct Screen *screen;
struct Window *Window,*nwwin(),*kitwin,*openkit();
struct Window *MagWin,*magwin();
struct RastPort *rpt;
struct TmpRas trast;
struct IntuiMessage tmess[2],*mess,*mess2,*GetMsg();
ULONG mclass,tempflags;
USHORT mcode;
LONG Mx,My,cutx,cuty,cuth,cutw;
SHORT end,colorholder;
SHORT button,mn1,mn2,mn3;
LONG X1,Y1,OFFX,OFFY,X2,Y2,X3,Y3,X4,Y4;
int errorck,ret3,rff,color,qdraw,i,ii,f,mode,mode2,brush,tempbrush;
int boxf,scnw,scnh,scnd,gmode,rmode,gadg,crossflag,tenflag,mouseflag;
int messflag,tmode,tmode2,topedge,pfflag,rx,ry;
int numx1,numx2,numflag,undoflag,curcolor,originalplanes;
struct RastPort rp,rp2,holdrp[3] /*  previously [4]  */ ;
extern int yeoldmanx[6],yeoldmany[6];
int j,k,l,tempii;
char test[30],pseudosub;
extern BYTE twolinesflag;
struct graphic gg;

#include "brushdata.c"

static struct BitMap tempbm =
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
};

static struct BitMap undobm =
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
};

static struct BitMap holdbm[] = {
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
},
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
},
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
},
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
},
{
0,0,0,1,0,
0,0,0,0,0,0,0,0
},
};

static SHORT bdatad[] =
{
0,0,
0,50,
50,50,
50,0,
0,0,
};

static SHORT bdatal[] =
{
0,0,
0,0,
};

static struct Border line =  {
0,0,
1,0,COMPLEMENT,
2,
&bdatal[0],
NULL,
};

static struct Border Box =
{
0,0,
1,0,COMPLEMENT,
5,
&bdatad[0],
NULL,
};

movmem(&Brush[6],&Brush[5],sizeof(struct BitMap));
setmem(&gg,sizeof(struct graphic),0);
Brush[0].BytesPerRow = RASSIZE(2,1);
Brush[1].BytesPerRow = RASSIZE(6,1);
Brush[2].BytesPerRow = RASSIZE(10,1);
Brush[3].BytesPerRow = RASSIZE(5,1);
Brush[4].BytesPerRow = RASSIZE(11,1);
errorck = AOK;
scnw = 320;
scnh = 200;
gmode = 0;
scnd = 1;
if (g->planes[0] != NULL ) {  /* set up screen as per graphic */
   if (g->aspect == (ASPMED*(ULONG)g->w)/((ULONG)g->h)) {
      scnw = 640;
      scnh = 200;
      gmode = 1;
      }
   if (g->aspect == (ASPLOWLACE*(ULONG)g->w)/((ULONG)g->h)) {
      scnw = 320;
      scnh = 400;
      gmode = 2;
      }
   if (g->aspect == (ASPLOW*(ULONG)g->w)/((ULONG)g->h)) {
      if (g->h > 200) {
         scnw = 640;
         scnh = 400;
         gmode = 3;
         }
      else {
         scnw = 320;
         scnh = 200;
         gmode = 0;
         }
      }
   if (g->numplanes < 5)
      scnd = g->numplanes;
   }
else {
   scnw = 640;
   scnh = 200;
   gmode = 1;
   }

undoflag = NO;
screen = 0;
Window = 0;
kitwin = 0;
screen = (struct Screen *) graphscreen(scnw,scnh,scnd);
if (screen == NULL) {
   Window=NULL;
   errorck = OUTOFMEM;
   goto ENDSUB2;
   }

setcolors(&(screen->ViewPort),0,scnd);

Window = nwwin(screen);
if (Window == NULL) {
   errorck = OUTOFMEM;
   goto ENDSUB2;
   }
InitRastPort(&rp);
InitRastPort(&rp2);
InitRastPort(&holdrp[0]);
InitRastPort(&holdrp[1]);
InitRastPort(&gibblerp);
InitRastPort(&holdrp[2]);
/* InitRastPort(&holdrp[3]);     */
holdrp[0].BitMap = &holdbm[0];
gibblerp.BitMap = &holdbm[2];

kitwin = openkit(screen,&rp,&topedge,((1<<scnd) - 1),&Box);
if (kitwin == NULL) {
   errorck = OUTOFMEM;
   goto ENDSUB2;
   }

holdrp[1].BitMap = &undobm;
undobm.Depth = scnd;
i = initbitmap(NULL,&holdrp[1],0,0,scnw,scnh);

if (i != OUTOFMEM)
   undoflag = YES;
else
   undoflag = NO;

newpointer(screen,CROSS);
mouseflag = CROSS;
rpt = Window->RPort;
numx1=490;
numx2=538;
cutw=0;
cuth=0;
gibblecolor = (1<<scnd) - 1;
rpt->AOlPen = gibblecolor;
brush = BRUSHA;
color = gibblecolor;
gibblecolor = gibblecolor;
gibbleplanes = gibblecolor;
SetAPen(rpt,gibblecolor);
rmode = 0;
end = 0;
button = 0;
qdraw = YES;
crossflag = NO;
messflag = NO;
numflag = NO;
Mx = 0;
My = 0;
X1 = 0;
Y1 = 0;
rx = 0;
ry = 0;
mode = DRAWA;
gadg = 540;
tenflag = NO;
pfflag = NO;
Move(rpt,0,0);
rpt->AreaPtrn = 0;
rpt->AreaPtSz = 0;
setbox(kitwin,&rp,DRAWA,0,gmode,qdraw);
setbox(kitwin,&rp,BRUSHF,BRUSHA,gmode,qdraw);
Tit1 = getmenu1();
SetMenuStrip(Window,Tit1);
changechecks(gmode,rmode);

/* copy and fill in graphic */
if (g->planes[0] != NULL) {
   movmem(g,&gg,sizeof(struct graphic));
   i = 4;
   while (i-- > g->numplanes) {
      gg.planes[i] = (PLANEPTR) AllocMem(RASSIZE(g->w,g->h),MEMF_CLEAR);
      if (gg.planes[i] == NULL) {
         while (++i < 4) {
            FreeMem(gg.planes[i],RASSIZE(g->w,g->h));
            gg.planes[i] = 0;
            }
         goto copyjump1;
         }
      }

   i = g->numplanes;
   while (i < 4) {
      movmem(gg.planes[g->numplanes-1],gg.planes[i++],RASSIZE(g->w,g->h));
      }

   gg.numplanes = 4;
   copyjump1:
   originalplanes = (int) g->numplanes;
   }


if (g->planes[0] != NULL) {
   WindowToBack(kitwin);
   disgraphic(rpt,&gg,scnw,scnh,scnd);
   WindowToFront(kitwin);
   }

ReportMouse(TRUE,Window);
ModifyIDCMP(Window, MENUVERIFY | RAWKEY | MENUPICK | MOUSEBUTTONS | MOUSEMOVE);

tempflags=Window->Flags;
if (Window->MouseY > 9)
   Window->Flags = tempflags | RMBTRAP;

while (TRUE)  {
   truejump:
   printxandy(kitwin,Mx,My,numx1,numx2);
   Wait((1<<Window->UserPort->mp_SigBit) |
        (1<<kitwin->UserPort->mp_SigBit));
   while(TRUE) {
      if (messflag != 0) {
         if (messflag == 2)
            mess = &tmess[1];
         else
            mess = &tmess[0];
         mess2 = 0;
         }
      else {
         mess = GetMsg(Window->UserPort);
         mess2 = GetMsg(kitwin->UserPort);
         }
      if(mess == 0 && mess2 == 0)
         goto truejump;
      mclass = mess->Class;
      mcode = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;

      if (mess == 0 && mess2->Code == SELECTDOWN) {
         Mx = mess2->MouseX + kitwin->LeftEdge;
         My = mess2->MouseY + kitwin->TopEdge;
         tempbrush = brush;
         tmode = mode;
         if (brush > 0 &&(mode == DRAWA /*|| mode == LINEA*/))
            ClipBlit (&holdrp[0],0,0,Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                 ,Brush[brush].BytesPerRow * 8 ,Brush[brush].Rows ,0xc0,
                 gibbleplanes,0);
         ReplyMsg(mess2);
         boxf=kitget(mess2,kitwin,&mode,&mode2,&brush,&qdraw,gmode,&rp,
                     &rff,screen,&Box,Window,X1,Y1);
         SetMenuStrip(Window,Tit1);

         if (boxf == YES) {
            if (twolinesflag == 0)
               drawtwolines(Window,Mx,My);
            X1 = Mx;
            Y1 = My;
            crossflag = YES;
            ClearMenuStrip(Window);
            }
         if (tempbrush != brush ) {
            if (tempbrush == BRUSHA && brush <= BRUSHE && holdbm[0].Rows == 0) {
               holdbm[0].Depth = 4;
               initbitmap(NULL,&holdrp[0],0,0,11,4);
/*               holdbm[2].Depth = 1;
               initbitmap(NULL,&gibblerp,0,0,11,4);

               holdbm[2].Planes[1] = holdbm[2].Planes[0];
               holdbm[2].Planes[2] = holdbm[2].Planes[0];
               holdbm[2].Planes[3] = holdbm[2].Planes[0];
               ClipBlit (&Brush[brush],0,0,&gibblerp,0,0,
                         holdbm[0].BytesPerRow * 8 ,holdbm[0].Rows ,0xee);
*/
               holdbm[2].Depth = 4;
               }
            else if (tempbrush == 5) {
               clearbitmap(&holdrp[0]);
               holdbm[0].Depth = scnd;
               initbitmap(NULL,&holdrp[0],0,0,11,4);
               }
            }
         ActivateWindow(Window);
         X1=Mx;
         Y1=My;
         if (brush > 0)
            ClipBlit (Window->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
                   &holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                   Brush[brush].Rows ,0xc0,gibbleplanes,0);
         if ((mode == DRAWA || mode == LINEA) && brush > 0 && brush < 5) {
            putbm(Window,&Brush[brush],&rp,1,Mx,My,brush,gibblecolor);
            }

         goto MVJUMP;
         }
      else if (mess == 0 && mess2->Code != SELECTDOWN)
         ReplyMsg(mess2);
      else if (mclass == MENUVERIFY && mouseflag == GENOTHING) {
         newpointer(screen,CROSS);
         mouseflag = CROSS;
         }
   /*   if (mess2 != 0) ReplyMsg(mess2); */
      if (mess != 0 && messflag == 0) ReplyMsg(mess);
         if (messflag > 0)
            messflag--;

      if (mclass == RAWKEY && 0x40 == (char)mcode)  {

         if((mode == BOXB && (mode2 == BOXB ||mode2 == CUTB)) ||
             mode == MAGB)
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
         if(mode == CIRCLEA || mode == CIRCLEB) {
            SetDrMd(rpt,COMPLEMENT);
            DrawEllipse(rpt,X1,Y1,rx,ry);
            SetDrMd(rpt,JAM1);
            mode = tmode;
            SetMenuStrip(Window,Tit1);
            }
         if(mode == BOXB &&(mode2 == ERASEB ||mode2 == PATB))
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
         else {
            if(mode == BOXA || mode == ROTA )
               if (twolinesflag == 1)
               drawtwolines(Window,X1,Y1);
            if(mode == LINEB)

               DrawBorder (rpt,&line,X1 ,Y1);
            if (mode2 != ERASEB && mode2 != PATB)
               setbox(kitwin,&rp,mode,0,gmode,qdraw);
            }
         if ((mode == BOXA || mode == BOXB) && mode2 == CUTB) {
            setbox(kitwin,&rp,mode,0,gmode,qdraw);
/*            if (tempbrush == 5)
               setbox(kitwin,&rp,DRAWA,0,gmode,qdraw);  */
            if (tempbrush < 5)
               setbox(kitwin,&rp,CUSTBRUSH,0,gmode,qdraw);
            }
         if (mode == MAGB) {
            X1 = Mx;
            Y1 = My;
            }
         if (mode == BOXA || /*mode == FILLA || */mode == MAGB) {
            ClipBlit (Window->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
                &holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                Brush[brush].Rows ,0xc0,gibbleplanes,0);
            }
         mode = DRAWA;
         setbox(kitwin,&rp,DRAWA,0,gmode,qdraw);
         SetMenuStrip(Window,Tit1);
         goto jump111;
         }
      if (mclass == RAWKEY && 89 == (char)mcode)  {
         if (tenflag == NO) {
            WindowToBack(kitwin);
            tenflag = YES;
            }
         else {
            WindowToFront(kitwin);
            tenflag = NO;
            }
         }

      if (mclass == MENUPICK )  {
         while (mcode != MENUNULL) {

            mn1 = MENUNUM(mcode);
            mn2 = ITEMNUM(mcode);
            mn3 =  SUBNUM(mcode);

            if (mn1 == 0 && mn2 == 0) {
/*  test area here

sprintf(test,"%d,%d",mode,mode2);
telluser2(screen,"m,m2",test,FALSE);

test area ends  */

               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser(screen,"Are you sure","about this?");
               setcolors(&(screen->ViewPort),0,scnd);
               if (ret3 == FALSE)
                  goto jump1;
/*               if (brush > 0 &&(mode == DRAWA || mode == LINEA))
                  ClipBlit (&holdrp[0],0,0,Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                       ,Brush[brush].BytesPerRow * 8 ,Brush[brush].Rows ,
                       0xc0,gibbleplanes,0);
*/               if (undoflag == YES)
                  putundo(Window,&holdrp[1]);
               rpt->AOlPen = 0;
               SetAPen(rpt,0);
               RectFill(rpt,0,0,scnw,scnh);
               if (brush != BRUSHA)
                  SetRast(&holdrp[0],0);
               rpt->AOlPen = gibblecolor;
               SetAPen(rpt,gibblecolor);
               if (mode == CIRCLEA) {
                  SetDrMd(rpt,COMPLEMENT);
                  DrawEllipse(rpt,X1,Y1,rx,ry);
                  SetDrMd(rpt,JAM1);
                  }
               goto jump1;
               }

            if (mn1 == 0 && mn2 == 1) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser(screen,"Are you sure","about this?");
               setcolors(&(screen->ViewPort),0,scnd);
               if (ret3 == FALSE)
                  goto jump1;
/*               if (brush > 0 &&(mode == DRAWA || mode == LINEA))
                  ClipBlit (&holdrp[0],0,0,Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                         ,Brush[brush].BytesPerRow * 8 ,Brush[brush].Rows ,
                         0xc0,gibbleplanes,0);
*/               rpt->AOlPen = 0;
               SetAPen(rpt,0);
               if (undoflag == YES)
                  putundo(Window,&holdrp[1]);
               RectFill(rpt,0,0,scnw,scnh);
               if (brush == 5)
                  SetRast(&holdrp[0],0);
               rpt->AOlPen = 1;
               SetAPen(rpt,gibblecolor);
               if (g->planes[0] != NULL) {
                  WindowToBack(kitwin);
                  disgraphic(rpt,&gg,scnw,scnh,scnd);
                  WindowToFront(kitwin);
                  }
               SetAPen(rpt,gibblecolor);
               goto jump1;
               }

            if (mn1 == 0 && mn2 == 5) {
               if (cutw ==0 && cuth ==0) {
                  setcolors(&(screen->ViewPort),1,scnd);
                  telluser2(screen,
                  "You have not yet created",
                  "a custom brush!",FALSE);
                  setcolors(&(screen->ViewPort),0,scnd);
                  goto jump1;

                  }
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser(screen,
               "The current brush will be",
               "your new graphic. Are you sure?");
               setcolors(&(screen->ViewPort),0,scnd);
               if (ret3 == FALSE)
                  goto jump1;
               i = 0;
               while(i < g->numplanes) {
                  if (g->planes[i++] != 0)
                     SafeFreeMem(g->planes[i-1],RASSIZE(g->w,g->h));
                  }
               i = 0;
               while(i < scnd)
                  g->planes[i] = Brush[5].Planes[i++];
               g->w =cutw;
               g->h =cuth;
               g->usex = 0;
               g->usey =0;
               g->usew =g->w;
               g->useh =g->h;
               g->numplanes = scnd;

               if (gmode == 0)
                  g->aspect=(ASPLOW*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 1)
                  g->aspect=(ASPMED*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 2)
                  g->aspect=(ASPLOWLACE*(ULONG)g->w)/((ULONG)g->h);
               if (gmode == 3)
                  g->aspect=(ASPHI*(ULONG)g->w)/((ULONG)g->h);
               errorck = AOK;
               clearbitmap(&holdrp[0]);

               goto ENDSUB2;
               }

            if (mn1 == 0 && mn2 == 6) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser(screen,"Do you REALLY","want to exit?");
               setcolors(&(screen->ViewPort),0,scnd);
               if (ret3 == FALSE)
                  goto jump1;
               errorck = CANCEL;
               goto ENDSUB1;
               }

            if (FALSE) {
               boxstart:
               if (twolinesflag == 0)
                  drawtwolines(Window,Mx,My);
               X1 = Mx;
               Y1 = My;
               crossflag = YES;
               goto jump1;
               }

            if (mn1 == 1 && mn2 == 2) {
               if (brush > 0 &&(mode == DRAWA || mode == LINEA))
                  ClipBlit (&holdrp[0],0,0,Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                  ,Brush[brush].BytesPerRow * 8 ,Brush[brush].Rows ,0xc0,
                  gibbleplanes,0);
               if (undoflag == YES)
                  putundo(Window,&holdrp[1]);
               setbox(kitwin,&rp,mode,0,gmode,qdraw);
               mode2 = ERASEB;
               mode = BOXA;
               goto boxstart;
               }

            if (mn1 == 1 && mn2 == 4) {
               if (brush == 5) {
                  if (undoflag == YES)
                     putundo(Window,&holdrp[1]);
                  tmode = mode;
                  mode = RESIZEA;
                  }
               }

            if (mn1 == 0 && mn2 == 4) {
               if (brush == 5) {
                  setcolors(&(screen->ViewPort),1,scnd);
                  savebrush(Window,screen,&Brush[5]);
                  setcolors(&(screen->ViewPort),0,scnd);
                  }
               else {
                  setcolors(&(screen->ViewPort),1,scnd);
                  telluser2(screen,"You cannot save","this brush.",FALSE);
                  setcolors(&(screen->ViewPort),0,scnd);
                  }
               }

            if (mn1 == 0 && mn2 == 3) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = loadbrush(Window,screen,&Brush[5],&cutw,&cuth,Mx,My,&X1,&Y1
                         ,&holdrp[0],&holdbm[0],brush);
               setcolors(&(screen->ViewPort),0,scnd);
               gibblex = cutw;
               if (brush != 5 && ret3 == 0) {
                  setbox(kitwin,&rp,BRUSHF,brush,gmode,qdraw);
                  setbox(kitwin,&rp,CUSTBRUSH,brush,gmode,qdraw);
                  brush = 5;
                  }
               }

            if (mn1 == 1 && mn2 == 1) {
               if (brush > 0 && mode == DRAWA)
                  ClipBlit (&holdrp[0],0,0,Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                 ,Brush[brush].BytesPerRow * 8 ,Brush[brush].Rows ,0xc0,
                 gibbleplanes,0);
               if (undoflag == YES)
               putundo(Window,&holdrp[1]);
               tmode2 = mode;
               mode = CIRCLEA;
               rx = 50;
               ry = 25;
               X1 = Mx;
               Y1 = My;
               SetDrMd(rpt,COMPLEMENT);
               DrawEllipse(rpt,X1,Y1,rx,ry);
               SetDrMd(rpt,JAM1);

               }

            if (mn1 == 1 && mn2 == 3) {
               if (mn3 == 0 && rmode == 1)
                  rmode = 0;
               if (mn3 == 1 && rmode == 0)
                  rmode = 1;
               }

            if (mn1 == 1 && mn2 == 0) {
               if (undoflag == YES) {
                  getundo(Window,&holdrp[1]);
                  if (brush != BRUSHA)
                     ClipBlit (Window->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
                         &holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                         Brush[brush].Rows ,0xc0,gibbleplanes,0);
                  }
               }

            if (mn1 == 0 && mn2 == 2) {
               j = gmode;
               k = scnd;
               l = scnreq(screen,&j,&k);
               if (l == -1 ||(j == gmode && k == scnd))
                  goto jump1;
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser(screen,"WARNING: this will","destroy your edits!");
               setcolors(&(screen->ViewPort),0,scnd);
               if (ret3 == FALSE)
                  goto jump1;
               scnd = k;
               if (j == 0) {
                  scnw = 320;
                  scnh = 200;
                  gmode = 0;
                  }
               if (j == 1) {
                  scnw = 640;
                  scnh = 200;
                  gmode = 1;
                  }
               if (j == 2) {
                  scnw = 320;
                  scnh = 400;
                  gmode = 2;
                  }
               if (j == 3) {
                  scnw = 640;
                  scnh = 400;
                  gmode = 3;
                  }

               if (undoflag == YES)
                  clearbitmap(&holdrp[1]);
               ClearMenuStrip(Window);
               CloseWindow(kitwin);
               CloseWindow(Window);
               CloseScreen(screen);
               kitwin = 0;
               Window = 0;
               screen = 0;

               screen = (struct Screen *) graphscreen(scnw,scnh,scnd);
               if (screen == NULL) {
                  errorck = OUTOFMEM;
                  goto ENDSUB1;
                  }
               i = 16>>scnd;
               j = 0;
               k = 15;
               while (j < (1<<scnd)) {
                  SetRGB4(&(screen->ViewPort),j++,k,k,k);
                  k = k - i;
                  if (scnd == 1)
                     k = 0;
                  }
               Window = nwwin(screen);
               if (Window == NULL) {
                  errorck = OUTOFMEM;
                  goto ENDSUB1;
                  }
               gibblecolor = (1<<scnd) -1;
               kitwin = openkit(screen,&rp,&topedge,gibblecolor,&Box);
               if (kitwin == NULL) {
                  errorck = OUTOFMEM;
                  goto ENDSUB1;
                  }
               newpointer(screen,CROSS);
               rpt = Window->RPort;
               rpt->AOlPen = 1;
               SetAPen(rpt,gibblecolor);
               gibbleplanes = (1<<scnd) -1;
               mode = DRAWA;
               SetMenuStrip(Window,Tit1);
               ReportMouse(TRUE,Window);
               ModifyIDCMP(Window,MENUVERIFY | RAWKEY | MENUPICK | MOUSEBUTTONS | MOUSEMOVE);
               undobm.Depth = scnd;
               if (brush != BRUSHA)
                  SetRast(&holdrp[0],0);
               i = initbitmap(NULL,&holdrp[1],0,0,scnw,scnh);

               if (i != OUTOFMEM)
                  undoflag = YES;
               else
                  undoflag = NO;

               if (g->planes[0] != NULL) {
                  WindowToBack(kitwin);
                  disgraphic(rpt,&gg,scnw,scnh,scnd);
                  WindowToFront(kitwin);
                  }
               if (gmode == 0 || gmode == 2) {
                  gadg = 249;
                  numx1=238;
                  numx2=265;
                  }
               else {
                  gadg = 540;
                  numx1=490;
                  numx2=538;
                  }

         /* expand bitplanes of brush if necessary */

         if (brush >0 && scnd > holdbm[0].Depth) {
            if (brush == 5) {
               rp.BitMap = &holdbm[3];
               holdbm[3].Depth = scnd;
               gibblerp.BitMap = &Brush[5];

               l = initbitmap(&gibblerp,&rp,0,0,cutw,cuth);
               }
            else
               l = AOK;

            if (l == OUTOFMEM) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser2(screen,"Warning:No memory to","expand this brush properly.",FALSE);
               setcolors(&(screen->ViewPort),0,scnd);
               }
            else {
               if (brush == 5) {
                  j = Brush[5].Depth;
                  rp.BitMap = &Brush[5];
                  clearbitmap(&rp);
                  movmem(&holdbm[3],&Brush[5],sizeof(struct BitMap));
                  if (scnd > j) {
                     k = j;
                     do {
                        movmem(Brush[5].Planes[j-1],Brush[5].Planes[k++],
                        RASSIZE(Brush[5].BytesPerRow * 8,Brush[5].Rows));
                        } while (k < scnd);
                     }
                  }
                  clearbitmap(&holdrp[0]);
                  holdbm[0].Depth = scnd;
                  X1 = Mx;
                  Y1 = My;
                  initbitmap(Window->RPort,&holdrp[0],X1-yeoldmanx[5],
                          Y1-yeoldmany[5],holdbm[3].BytesPerRow*8,cuth);
                  }
               }
            setbox(kitwin,&rp,mode,0,gmode,qdraw);
            if (brush == 5)
               setbox(kitwin,&rp,CUSTBRUSH,0,gmode,qdraw);
            else
               setbox(kitwin,&rp,BRUSHF,brush,gmode,qdraw);
            changechecks(gmode,rmode);
            goto jump111;
            }

         jump1:
         nextitem = (struct MenuItem *) ItemAddress(Tit1,mcode);
         mcode = nextitem->NextSelect;
         }
      jump111:
      changechecks(gmode,rmode);
      }


      else switch(mode)  {

      case DRAWA:
         drawa(Window,mclass,mcode,Mx,My,gadg,&mode,&X1,&Y1,
               brush,rpt,&Brush[brush],&rp,&holdrp[0],&holdrp[1]);
         if (messflag == NO)
            messflag = clearmess(Window,kitwin,tmess);
         break;

      case DRAWB:
         drawb(Window,&rp,qdraw,mclass,mcode,Mx,My,gadg,&mode,&X1,&Y1,
               brush,rpt,&Brush[brush],&OFFX,&OFFY,&X2,&Y2,&X3,&Y3,&holdrp[0]);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case LINEA:
         linea(Window,mclass,mcode,Mx,My,gadg,&mode,&X1,&Y1,
               brush,rpt,&line,&holdrp[0],&Brush[brush]);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case LINEB:
         lineb(screen,Window,mclass,mcode,Mx,My,gadg,&mode,&X1,&Y1,
               &Brush[brush],brush,&rp,rpt,&line);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case RESIZEA:
         newpointer(screen,CROSS);
         resizea(Window,mclass,mcode,Mx,My,gadg,&mode,&X1,&Y1,
               brush,rpt,&Brush[brush],&rp,&holdrp[0],&holdrp[1],&Box);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case RESIZEB:
         newpointer(screen,CROSS);
         resizeb(Window,&mode,tmode,mclass,mcode,Mx,My,&X1,&Y1,
               &cutw,&cuth,rpt,&Brush[5],&rp,&holdrp[0],&holdrp[1],&Box);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case CIRCLEA:
         newpointer(screen,CROSS);
         circlea(mclass,mcode,Mx,My,&mode,tmode2,&X1,&Y1,rx,ry,rpt,
                 &holdrp[0],&Brush[brush],brush);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case CIRCLEB:
         newpointer(screen,CROSS);
         circleb(mclass,mcode,Mx,My,&mode,&X1,&Y1,&rx,&ry,rpt);
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;
    
      case BOXA:
         drawtwolines(Window,X1,Y1);
         X1 = Mx;
         Y1 = My;
         drawtwolines(Window,X1,Y1);
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN && mcode != MENUDOWN)
               break;
            rpt->AOlPen = 1;
            if (twolinesflag == 1)
               drawtwolines(Window,X1,Y1);
            crossflag = NO;
            mode = BOXB;
            X1 = Mx;
            Y1 = My;
            putundo(Window,&holdrp[1]);
            ClipBlit (Window->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
                   &holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                   Brush[brush].Rows ,0xc0,gibbleplanes,0);
            bdatad[3] = My - Y1;
            bdatad[5] = bdatad[3];
            bdatad[4] = Mx - X1;
            bdatad[6] = bdatad[4];
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
            colorholder = mcode;
            }
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;

      case BOXB:
         DrawBorder (rpt ,&Box ,X1 ,Y1 );
         bdatad[3] = My - Y1;
         bdatad[5] = bdatad[3];
         bdatad[4] = Mx - X1;
         bdatad[6] = bdatad[4];
         DrawBorder (rpt ,&Box ,X1 ,Y1 );

         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTUP && mcode != MENUUP)
               break;
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

            Box.FrontPen = gibblecolor;
            mode = BOXA;
            box(Window,gibblecolor,Mx,My,X1,Y1,brush,rpt,&Box,&Brush[brush],&rp);
            if (twolinesflag == 0)
               drawtwolines(Window ,X1 ,Y1 );

/*            newpointer(screen,CROSS);
            SetMenuStrip(Window,Tit1);
            X1 = Mx;
            Y1 = My;
            if (brush != BRUSHA)
               ClipBlit (Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                  ,&holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                  Brush[brush].Rows ,0xc0,gibbleplanes,0);
  */       if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
            break;

         case CUTB:
            newpointer(screen,BUSY);
            rp.BitMap = &holdbm[3];
            holdbm[3].Depth = scnd;
            l = initbitmap(Window->RPort,&rp,X1,Y1,bdatad[4]+1,bdatad[3]+1);
            if (l == OUTOFMEM) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser2(screen,"No memory for cut.",NULL,FALSE);
               setcolors(&(screen->ViewPort),0,scnd);
               goto cutabort;
               }
            holdrp[2].BitMap = &holdbm[4];
            holdbm[4].Depth = scnd;
            l = initbitmap(Window->RPort,&holdrp[2],X1-(bdatad[4]+1)/2,
                       Y1-(bdatad[3]+1)/2,bdatad[4]+1,bdatad[3]+1);
            if (l == OUTOFMEM) {
               setcolors(&(screen->ViewPort),1,scnd);
               ret3 = telluser2(screen,"No memory for cut.",NULL,FALSE);
               setcolors(&(screen->ViewPort),0,scnd);
               clearbitmap(&rp);
               goto cutabort;
               }
            rp.BitMap = &Brush[5];
            clearbitmap(&rp);
            cutx = X1;
            cuty = Y1;
            cuth = bdatad[3]+1;
            cutw = bdatad[4]+1;
            gibblex = cutw;
            movmem(&holdbm[3],&Brush[5],sizeof(struct BitMap));
            rp.BitMap = &Brush[5];
            brush = 5;
            yeoldmanx[5] = cutw/2;
            yeoldmany[5] = cuth/2;
            clearbitmap(&holdrp[0]);
            holdbm[0].Depth = scnd;
            X1 = Mx;
            Y1 = My;
            movmem(&holdbm[4],&holdbm[0],sizeof(struct BitMap));
            cutabort:
            if (tempbrush != 5) {
               setbox(kitwin,&rp,BRUSHF,tempbrush,gmode,qdraw);
               }
               setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
               mode = DRAWA;

            if (brush != BRUSHA)
               ClipBlit (Window->RPort,X1 - yeoldmanx[brush],Y1 - yeoldmany[brush]
                  ,&holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                  Brush[brush].Rows ,0xc0,gibbleplanes,0);
            SetMenuStrip(Window,Tit1);
            newpointer(screen,CROSS);
            break;

         case ERASEB:
            erase(rpt,brush,X1,Y1,&rp,&holdrp[0],&Brush[brush],&bdatad[0]);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            SetMenuStrip(Window,Tit1);
            break;
         /*
         case PATB:
            pattern(rpt,brush,X1,Y1,&rp,&holdrp[0],&Brush[brush],
                    &bdatad[0],&areafill2[0]);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            SetMenuStrip(Window,Tit1);
            break;
              */
            }
            } ;
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;

      case FLIPA:
         if (rff == -1) {
            setbox(kitwin,&rp,FLIPA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            break;
            }
         if (brush != 5) {
            setcolors(&(screen->ViewPort),1,scnd);
            telluser2(screen,"You cannot manipulate this","brush!",FALSE);
            setcolors(&(screen->ViewPort),0,scnd);
            setbox(kitwin,&rp,FLIPA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            break;
            }
         ClearMenuStrip(Window);
         rpt->AOlPen = gibblecolor;
         SetAPen(rpt,gibblecolor);
         newpointer(screen,BUSY);
         rp2.BitMap = &tempbm;
         tempbm.Depth = scnd;
         i = initbitmap(NULL,&rp2,0,0,cutw,cuth);

         if (i == AOK) {
            flip(cutw,cuth,&Brush[5],&rp2,rff);
            rp.BitMap = &Brush[5];
            clearbitmap(&rp);
            movmem(&tempbm,&Brush[5],sizeof(struct BitMap));
            }
         setbox(kitwin,&rp,FLIPA,brush,gmode,qdraw);
         setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
         mode = DRAWA;
         newpointer(screen,CROSS);
         SetMenuStrip(Window,Tit1);
         break;

      case FILLA:
         if (mclass == MOUSEBUTTONS) {
            if (mcode != SELECTDOWN && mcode != MENUDOWN)
               break;
            trast.RasPtr = AllocMem(Window->RPort->BitMap->BytesPerRow *
                    Window->RPort->BitMap->Rows,MEMF_CHIP | MEMF_CLEAR);
            if (trast.RasPtr == NULL) {
               setcolors(&(screen->ViewPort),1,scnd);
               telluser2(screen,"No memory for fill.",NULL,FALSE);
               setcolors(&(screen->ViewPort),0,scnd);
               break;
               }
            trast.Size = Window->RPort->BitMap->BytesPerRow * Window->RPort->BitMap->Rows;
            rpt->TmpRas = &trast;
            putundo(Window,&holdrp[1]);
/*            mode = DRAWA;
            setbox(kitwin,&rp,FILLA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
  */          newpointer(screen,BUSY);
            SetAPen(rpt,gibblecolor);
            rpt->FgPen = gibblecolor;
            rpt->BgPen = 0;
            rpt->AOlPen = gibblecolor;
            if (mcode == MENUDOWN)
               rpt->AOlPen = 0;

            Flood(rpt,1,Mx,My);
            FreeMem(trast.RasPtr, rpt->BitMap->BytesPerRow * rpt->BitMap->Rows);
            rpt->TmpRas = NULL;

            newpointer(screen,CROSS);
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
            WindowToBack(kitwin);
            tenflag = YES;
            DrawBorder (rpt ,&Box ,X1 ,Y1 );
               putundo(Window,&holdrp[1]);
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
                  SetAPen(MagWin->RPort,f);
                  RectFill(MagWin->RPort,i * 6 + 2, ii * 5 + 10,
                           i * 6 + 7, ii * 5 + 14);
                  } while (ii++ <= 19);
               } while (i++ <= 39);
            i = 0;
            j = 241 / (1<<scnd);
            do {
               SetAPen(MagWin->RPort,i);
               RectFill(MagWin->RPort,j*i+2,116,j*i+j+2,124);
               } while (++i < (1<<scnd));
            Box.XY[3] = 124-116;
            Box.XY[5] = Box.XY[3];
            Box.XY[4] = j;
            Box.XY[6] = Box.XY[4];
            DrawBorder (MagWin->RPort,&Box,j * gibblecolor+2,116);

            ReportMouse(FALSE,Window);
            Maged( Window, MagWin, X1, Y1,&Box);
            CloseWindow(MagWin);
            ReportMouse(TRUE,Window);
            mode = DRAWA;
            setbox(kitwin,&rp,MAGA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            SetMenuStrip(Window,Tit1);
            messflag = clearmess(Window,kitwin,tmess);
            if (brush != BRUSHA)
               ClipBlit (Window->RPort,Mx - yeoldmanx[brush],My - yeoldmany[brush],
                  &holdrp[0],0,0,Brush[brush].BytesPerRow * 8 ,
                  Brush[brush].Rows ,0xc0,gibbleplanes,0);
            WindowToFront(kitwin);
            tenflag = NO;
            }
         if (messflag == NO)
         messflag = clearmess(Window,kitwin,tmess);
         break;

      case ROTA:
         if (rff == -1) {
            setbox(kitwin,&rp,ROTA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            break;
            }
         if (brush != 5) {
            setcolors(&(screen->ViewPort),1,scnd);
            telluser2(screen,
            "You cannot manipulate this",
            "brush!",FALSE);
            setcolors(&(screen->ViewPort),0,scnd);
            setbox(kitwin,&rp,ROTA,brush,gmode,qdraw);
            setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
            mode = DRAWA;
            break;
            }
         ClearMenuStrip(Window);
         newpointer(screen,BUSY);
         rpt->AOlPen = gibblecolor;

         rotate(&tempbm,rmode,cutw,cuth,&X4,&Y4,&Brush[5],&rp2,rff,scnd);

         rp.BitMap = &Brush[5];
         clearbitmap(&rp);
         clearbitmap(&holdrp[0]);

         movmem(&tempbm,&Brush[5],sizeof(struct BitMap));
         cutw = X4;
         gibblex = X4;
         cuth = Y4;
         Brush[5].BytesPerRow = RASSIZE(cutw,1);
         Brush[5].Rows = cuth;
         setbox(kitwin,&rp,ROTA,brush,gmode,qdraw);
         setbox(kitwin,&rp,DRAWA,brush,gmode,qdraw);
         yeoldmanx[5] = cutw/2;
         yeoldmany[5] = cuth/2;
         mode = DRAWA;
         holdbm[0].Depth = scnd;
         initbitmap(Window->RPort,&holdrp[0],X1 - yeoldmanx[5],Y1 - yeoldmany[5],
                    cutw,cuth);
         newpointer(screen,CROSS);
         SetMenuStrip(Window,Tit1);

         break;



      } /* switch end */

      MVJUMP:
      endstuff(screen,Window,kitwin,Mx,My,gmode,&tempflags,tenflag,&mouseflag,
               brush,mode,topedge,numx1,numx2);         

      }
   }
ENDSUB1:
rp.BitMap = &Brush[5];
clearbitmap(&rp);
/* clearbitmap(&holdrp[0]);   used to be here  */

ENDSUB2:
clearbitmap(&holdrp[0]);
clearbitmap(&holdrp[1]);
/* ClearMenuStrip(Window);  */

if (g->planes[0] != NULL && gg.numplanes == 4) {
   i = originalplanes;
   while (i < 4 && gg.planes != NULL) {
      FreeMem(gg.planes[i++],RASSIZE(gg.w,gg.h));
      }
   }

if (kitwin!=0)
   CloseWindow(kitwin);
if (Window !=0)
   CloseWindow(Window);
if (screen!=0)
   CloseScreen(screen);

return(errorck);

} /*  end of graphed main   */

