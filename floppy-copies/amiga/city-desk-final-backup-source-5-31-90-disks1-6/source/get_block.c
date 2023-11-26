#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/rastport.h"
#include "stdio.h"
#include "desk.h"


/*******************************************************/
/*** get_block  Gets a block object (filled rectangle **/
/*******************************************************/

get_block(pg,w,startx,starty,gotobj)
int pg;
struct Window *w;
struct object *gotobj;
SHORT startx,starty;

{
extern struct pageinfo page[MAXPAGES];
extern struct Window *clipboard;
extern COORD x_gridsize;
extern COORD y_gridsize;
extern SHORT snaptogrid;
struct object *obj,*createobject();
struct RastPort *rp;
struct IntuiMessage *message;
SHORT endx,newx,endy,newy;
SHORT type;
ULONG class;
USHORT code;
struct viewmsg msg;

if (page[pg].view->window!=w)
   return(CANCEL);

if (gotobj==0) {
   obj=createobject(&page[pg].objects);
   if (obj==0) return(OUTOFMEM);
   obj->pn=pg;
   if (obj->density==0)    /** Bypass default if it's zero **/
      obj->density = 5;
   }
else {
   obj=gotobj;
   erase_obj_image(obj);
   startx=ctomousex(page[pg].view,obj->x);
   starty=ctomousey(page[pg].view,obj->y);
   }

rp=w->RPort;

/* endx=w->MouseX + XBORDER;
endy=w->MouseY + YBORDER; */

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

      obj->x=XSNAPGRID(mousetocx(&page[pg],startx));
      obj->y=YSNAPGRID(mousetocy(&page[pg],starty));
      obj->h=YSNAPGRID(stocy(endy-starty)/page[pg].view->mag);
      obj->w=XSNAPGRID(stocx(endx-startx)/page[pg].view->mag);
      if (obj->h < 0 || obj->h > 10000) obj->h = 16;
      if (obj->w < 0 || obj->w > 10000) obj->w = 16;
      obj->type=BLOCK;
      obj->mag=page[pg].view->mag;
      msg.obj=obj;
      msg.page=&page[pg];
      msg.command=DRAWIT;
      viewmonitor(&msg);
      return(AOK);
      }


   if (class==RAWKEY) {
      ReportMouse(FALSE,w);
      if (gotobj == 0) {
         Remove(obj);
         FreeMem(obj,sizeof(struct object));
         }
      return(CANCEL);
      }

   endx = newx;
   endy = newy;
   clipdrawbox(rp, startx, starty, endy-starty, endx-startx);
   }
}

