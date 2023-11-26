

#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"



endstuff(screen,win,kitwin,Mx,My,gmode,tempflags,tenflag,mouseflag,
         brush,mode,topedge,numx1,numx2)
struct Screen *screen;
struct Window *win,*kitwin;
int numx1,numx2,gmode,tenflag,*mouseflag,brush,mode,topedge;
ULONG *tempflags;
LONG Mx,My;

{

char ts[4];
int x,y;
extern int gibblecolor;

x = (int) Mx;
y = (int) My;

*tempflags=win->Flags;
if (win->MouseY > 9)
   win->Flags = *tempflags | RMBTRAP;
else if ((*tempflags & RMBTRAP) && tenflag == NO)
   win->Flags = *tempflags - RMBTRAP;

if (tenflag == YES && *mouseflag == CROSS) {
   if ((mode == DRAWA || mode == DRAWB) && brush != BRUSHA && gibblecolor > 0) {
      newpointer(screen,GENOTHING);
      *mouseflag = GENOTHING;
      }
   }
else if (My > topedge && *mouseflag == GENOTHING && tenflag == NO) {
   newpointer(screen,CROSS);
   *mouseflag = CROSS;
   }
else if (My < topedge && *mouseflag == CROSS) {
   if ((mode == DRAWA || mode == DRAWB) && brush != BRUSHA && gibblecolor > 0) {
      newpointer(screen,GENOTHING);
      *mouseflag = GENOTHING;
      }
   }
/*
Move(kitwin->RPort,numx1,20);
Text(kitwin->RPort,"   ",3);
Move(kitwin->RPort,numx2,20);
Text(kitwin->RPort,"   ",3);
Move(kitwin->RPort,numx1,20);
sprintf(&ts[0],"%d",x);
if (x<100)
   ts[2] = 32;
if (x<10)
   ts[1] = 32;
Text(kitwin->RPort,&ts[0],3);
Move(kitwin->RPort,numx2,20);
sprintf(&ts[0],"%d",y);
if (y<100)
   ts[2] = 32;
if (y<10)
   ts[1] = 32;
Text(kitwin->RPort,&ts[0],3);

*/

}
