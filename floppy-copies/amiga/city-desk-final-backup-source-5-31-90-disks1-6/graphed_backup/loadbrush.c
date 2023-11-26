
#include "stdio.h"
#include "libraries/dos.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

#define SafeFreeMem(a,b) if(a)FreeMem(a,b)

char gooblepop1[50],gooblepop2[50];

loadbrush(win,screen,brush,cutw,cuth,Mx,My,X1,Y1,holdrp,holdbm,b)

struct Screen *screen;
struct Window *win;
struct BitMap *brush,*holdbm;
struct RastPort *holdrp;
int *cutw,*cuth,b;
LONG Mx,My,*X1,*Y1;
{

struct graphic g;
char temp[8],temp2[100];
int error,x,i;
extern int yeoldmanx[6],yeoldmany[6];

if (b == 5)
   ClipBlit (holdrp,0,0,win->RPort,*X1 - yeoldmanx[5],*Y1 - yeoldmany[5]
          ,brush->BytesPerRow * 8 ,brush->Rows ,0xc0,1,0);
else if (b != 0)
   ClipBlit (holdrp,0,0,win->RPort,*X1 - yeoldmanx[b],*Y1 - yeoldmany[b]
          ,11 ,4 ,0xc0,1,0);
error = 0;
newpointer(screen,NORMAL);
x = get_fname(win,screen,"Load Brush",&gooblepop2[0],&gooblepop1[0]);
if (x==0){
   newpointer(screen,CROSS);
   return(1);
   }
sprintf(temp,"");
if (gooblepop1[strlen(gooblepop1)-1] != ':' && strlen(gooblepop1) > 2)
   strncat(temp,"/",2);
sprintf(temp2,"%s%s%s",gooblepop1,temp,gooblepop2);

newpointer(screen,BUSY);

x = load_ilbm(&temp2[0],&g,screen,TRUE);
if (x != AOK){
   telluser2(screen,"This file cannot be loaded!",NULL,FALSE);
   newpointer(screen,CROSS);
   return(1);
   }
else {

   holdrp->BitMap = brush;
   clearbitmap(holdrp);
   brush->Depth = g.numplanes;
   i = 0;
   while (i < g.numplanes)  {
      brush->Planes[i] = g.planes[i++];
/*      brush->Planes[i] = AllocMem(RASSIZE(g.w,g.h),MEMF_CHIP);
      movmem(brush->Planes[i],g.planes[i++],RASSIZE(g.w,g.h));
      printf("plane %d at %d\n",i-1,g.planes[i-1]);  */
      }
   brush->BytesPerRow = RASSIZE(g.w,1);
   brush->Rows = g.h;
   *cutw = g.w;
   *cuth = g.h;
   *X1 = Mx;
   *Y1 = My;
   SetAPen(holdrp,0);
/*   if (g.w < brush->BytesPerRow * 8)
      RectFill(holdrp,g.w,0,brush->BytesPerRow*8,brush->Rows);
*/   yeoldmanx[5] = *cutw/2;
   yeoldmany[5] = *cuth/2;
   holdrp->BitMap = holdbm;
   clearbitmap(holdrp);
   holdbm->Depth = win->RPort->BitMap->Depth;
   initbitmap(win->RPort,holdrp,*X1 - yeoldmanx[5],*Y1 - yeoldmany[5],
              *cutw,*cuth);


   }
newpointer(screen,CROSS);
return(0);


}



