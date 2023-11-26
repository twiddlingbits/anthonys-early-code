#include "exec/types.h"
#include "exec/exec.h"
#include "graphics/rastport.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "desk.h"

/***********************************/
/** allow user to size an object ***/
/***********************************/

sizeobject(view,obj,nice,sizedir)

struct object *obj;
struct pageview *view;
int nice;     /* Force sizing to follow aspect ratios? */
int sizedir;  /* allowable size directions: ANYWAY XONLY YONLY */

{
extern UWORD xinch, yinch;
FIXED temp;
ULONG class;
USHORT code;
SHORT  x,y,oldx,oldy;
SHORT  cornerx,cornery;
SHORT  orgval,mag;
struct Window *Window;
struct IntuiMessage *message;
struct graphic *g;

mag=obj->mag;
Window=view->window;
if (obj->type!=GRAPHIC)
   nice=FALSE;
else
   g=obj->data;

if (nice==TRUE)
   sizedir=ANYWAY;  /* X or Y doesn't make sence when nice is on **/


oldx=Window->MouseX;
oldy=Window->MouseY;

ReportMouse(TRUE,Window);

SetDrMd(Window->RPort,COMPLEMENT);

orgval=(SHORT)obj->x*mag - (SHORT)view->x;
cornerx=ctosx(iabs(orgval));  /* calculate the upper left corner */
if (orgval < 0) cornerx= -cornerx;

orgval=(SHORT)obj->y*mag - (SHORT)view->y;
cornery=ctosy(iabs(orgval));  /* calculate the upper left corner */
if (orgval < 0) cornery= -cornery;

cornerx += XBORDER;
cornery += YBORDER;

if (sizedir==XONLY)
   oldy=cornery + ctosy(obj->h*mag);
if (sizedir==YONLY)
   oldx=cornerx + ctosx(obj->w*mag);

if (nice) {       /** If using aspect ration, set y to correct value **/
   temp=stocx(oldx-cornerx);
   temp = temp << 16;
   temp = temp/g->aspect;
   temp = (temp * yinch)/xinch;  /** switch from x coords to y coords **/
   oldy = cornery+ctosy(temp);
   }

drawbox(Window->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);

loop:
while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))!=0) {
   if(message->Class==MOUSEBUTTONS) break;
   ReplyMsg (message);   /* empty msg buffer, and wait */
   }
while (message==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   message=(struct IntuiMessage *)GetMsg(Window->UserPort);
   }

class=message->Class;
code=message->Code;
x=Window->MouseX;
y=Window->MouseY;
ReplyMsg(message);

if (x <= cornerx) {
   x=cornerx+2;   /** make sure we don't flip it **/
   }

if (nice) {
   temp=stocx(x-cornerx);
   temp = temp << 16;
   temp = temp/g->aspect;
   temp = (temp * yinch)/xinch;  /** switch from x coords to y coords **/
   y = cornery+ctosy(temp);
   }

if (y <= cornery) {
   y=cornery+2;
   }
if (sizedir==XONLY)
   y=oldy;

if (sizedir==YONLY)
   x=oldx;


drawbox(Window->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);

if (class==MOUSEBUTTONS && code==SELECTUP) {
   ReportMouse(FALSE,Window);
   if (sizedir!=XONLY) {
      obj->h=stocy(oldy-cornery)/mag;   /* new size of gaphic */
      obj->h=max(obj->h,1);
      }
   if (sizedir!=YONLY) {
      obj->w=stocx(oldx-cornerx)/mag;
      obj->w=max(obj->w,1);
      }

   return(TRUE);
   }

if (class==RAWKEY) {
   ReportMouse(FALSE,Window);
   return(FALSE);
   }

oldx=x;
oldy=y;
drawbox(Window->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);
show_coord(obj, x, y, 0, 0, oldx-cornerx, oldy-cornery);
goto loop;
}


