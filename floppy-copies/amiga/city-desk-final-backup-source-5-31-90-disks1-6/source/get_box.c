#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/rastport.h"
#include "stdio.h"
#include "desk.h"


/*******************************************************/
/*** get_box lets the user drag out a box             **/
/*******************************************************/

void get_box(pg,w,startx,starty,box_x, box_y, box_h, box_w)
int pg;
struct Window *w;
SHORT startx,starty;
COORD *box_x, *box_y, *box_h, *box_w;

{
extern struct pageinfo page[MAXPAGES];
extern struct Window *clipboard;
extern COORD x_gridsize;
extern COORD y_gridsize;
extern SHORT snaptogrid;
struct RastPort *rp;
struct IntuiMessage *message;
SHORT endx,newx,endy,newy;
ULONG class;
USHORT code;

if (page[pg].view->window!=w) {
   *box_x=0;
   *box_y=0;
   *box_h=0;
   *box_w=0;
   return;
   }

rp=w->RPort;

endx=w->MouseX;
endy=w->MouseY;

ReportMouse(TRUE,w);
SetDrMd(rp,COMPLEMENT);

clipdrawbox(rp, startx, starty, endy-starty, endx-startx);

while (TRUE) {

   while((message=(struct IntuiMessage *)GetMsg(clipboard->UserPort))==0) {
      Wait(1<<clipboard->UserPort->mp_SigBit);
      }

   class=message->Class;
   code=message->Code;
   newx=message->MouseX;
   newy=message->MouseY;
   ReplyMsg(message);

   clipdrawbox(rp, startx, starty, endy-starty, endx-startx);

   if (class==MOUSEBUTTONS && code==SELECTUP) {
      ReportMouse(FALSE,w);

      *box_x=XSNAPGRID(mousetocx(&page[pg],startx));
      *box_y=YSNAPGRID(mousetocy(&page[pg],starty));
      *box_h=YSNAPGRID(stocy(endy-starty)/page[pg].view->mag);
      *box_w=XSNAPGRID(stocx(endx-startx)/page[pg].view->mag);
      if (*box_h < 0 || *box_h > 10000) *box_h = 16;
      if (*box_w < 0 || *box_w > 10000) *box_w = 16;
      return;
      }


   if (class==RAWKEY) {
      ReportMouse(FALSE,w);
      *box_x=0;
      *box_y=0;
      *box_h=0;
      *box_w=0;
      return;
      }

   endx = newx;
   endy = newy;
   clipdrawbox(rp, startx, starty, endy-starty, endx-startx);
   }
}

