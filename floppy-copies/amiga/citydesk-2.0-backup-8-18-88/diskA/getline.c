#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/rastport.h"
#include "stdio.h"
#include "desk.h"


/****************************************************/
/*** getline -- let user draw a rule              ***/
/****************************************************/

getline(pg,w,startx,starty,gotobj)
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
extern struct rule defaultrule;
struct object *obj,*createobject();
struct rule *rule;
struct RastPort *rp;
struct IntuiMessage *message;
SHORT endx,newx,endy,newy;
SHORT type;
SHORT lm;
ULONG class;
USHORT code;

if (page[pg].view->window!=w)
   return(CANCEL);

if (gotobj==0) {
   obj=createobject(&page[pg].objects);
   if (obj==0) return(OUTOFMEM);
   obj->pn=pg;
   obj->mag = page[pg].view->mag;
   }
else {
   obj=gotobj;
   erase_obj_image(obj);
   startx=ctomousex(page[pg].view,obj->x);
   starty=ctomousey(page[pg].view,obj->y);
   }

rp=w->RPort;
rp->LinePtrn=defaultrule.pattern;
/* endx=w->MouseX + XBORDER;
endy=w->MouseY + YBORDER; */

endx=w->MouseX;
endy=w->MouseY;

setruletype(endy,starty,&endy, endx,startx,&endx, &type);

if (gotobj) {
   if (type==HORIZ && stocx(endx - startx) < gotobj->w/2)
      startx=ctomousex(page[pg].view,obj->x + obj->w);
   else if (type==VERT && stocy(endy - starty) < gotobj->h/2)
      starty=ctomousey(page[pg].view,obj->y + obj->h);
   }

ReportMouse(TRUE,w);
SetDrMd(rp,COMPLEMENT);

Move(rp,startx,starty);  /** draw 1st line **/
Draw(rp,endx,endy);

show_coord(obj, startx, starty, 0, 0, endx-startx, endy-starty);

while (TRUE) {

   while((message=(struct IntuiMessage *)GetMsg(clipboard->UserPort))==0) {
      Wait(1<<clipboard->UserPort->mp_SigBit);
      }

   class=message->Class;
   code=message->Code;
   newx=message->MouseX;
   newy=message->MouseY;
   lm = last_message(message);
   ReplyMsg(message);

   Move(rp,startx,starty);  /** erase line **/
   Draw(rp,endx,endy);

   if (class==MOUSEBUTTONS && code==SELECTUP) {
      ReportMouse(FALSE,w);
      if (gotobj==0) {   /** Not resizing **/
         rule=(struct rule *)AllocMem(sizeof(struct rule),0);
         if (rule==0) {
            Remove(obj);
            FreeMem(obj,sizeof(struct object));
            rp->LinePtrn=0xFFFF;
            return(OUTOFMEM);
            }
         *rule=defaultrule;    /** copy default rule structure **/
         }
      else
         rule=gotobj->data;

      toggle(&endx, &startx);  /* if endx < startx, swap */
      toggle(&endy, &starty);  /* if endy < starty, swap */

      obj->data=(APTR)rule;
      rule->flags=type;   /** horiz or vertic line ?? **/
      obj->x=XSNAPGRID(mousetocx(&page[pg],startx));
      obj->y=YSNAPGRID(mousetocy(&page[pg],starty));
      if (type==HORIZ) {
         obj->w=XSNAPGRID(stocx(endx-startx)/page[pg].view->mag);
         obj->h=max(40,defaultrule.width);
         }
      else {
         obj->h=YSNAPGRID(stocy(endy-starty)/page[pg].view->mag);
         obj->w=max(40,defaultrule.width);
         }
      obj->type=RULE;
      drawrule(page[pg].view,obj,BLACK);
      rp->LinePtrn=0xFFFF;
      return(AOK);
      }


   if (class==RAWKEY) {
      ReportMouse(FALSE,w);
      drawrule(page[pg].view,obj,BLACK);
      rp->LinePtrn=0xFFFF;
      if (gotobj == 0) {
         Remove(obj);
         FreeMem(obj,sizeof(struct object));
         }
      return(CANCEL);
      }

   setruletype(newy,starty,&endy, newx,startx,&endx, &type);
   Move(rp,startx,starty);  /** draw new line **/
   Draw(rp,endx,endy);
   if (lm)  /** don't update if lots of messages qued up **/
      show_coord(obj, startx, starty, 0, 0, endx-startx, endy-starty);
   }
}


setruletype(newy,starty,endy, newx,startx,endx, type)

SHORT newy, starty, *endy, newx, startx, *endx, *type;

{

if ( (iabs(newy-starty)<<1) < iabs(newx - startx)) {
   *endy=starty;
   *endx=newx;
   *type=HORIZ;
   }
else {
   *endx=startx;
   *endy=newy;
   *type=VERT;
   }
}

toggle(endx, startx)

SHORT *endx, *startx;

{
SHORT temp;

if (*endx < *startx) {
   temp=*startx;
   *startx=*endx;
   *endx=temp;
   }
}
