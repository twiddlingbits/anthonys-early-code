#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

/*******************************************************************/
/*** getclick()   Gets a mouse click and returns new window, x, y **/
/*******************************************************************/

verifypick(obj)

struct object *obj;

{
extern struct pageinfo page[MAXPAGES];
struct pageview *view;
struct IntuiMessage *message;
ULONG class;
struct Window *Window;
short mag;

view   = page[obj->pn].view;
Window = view->window;
mag    = view->mag;

SetDrMd(Window->RPort,COMPLEMENT);
drawbox(Window->RPort,ctomousex(view,obj->x),ctomousey(view,obj->y),
        ctosy(obj->h)*mag, ctosx(obj->w)*mag);

while((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   }
class=message->Class;
/* code=message->Code; */
ReplyMsg(message);

drawbox(Window->RPort,ctomousex(view,obj->x),ctomousey(view,obj->y),
        ctosy(obj->h)*mag, ctosx(obj->w)*mag);

if (class==MOUSEBUTTONS)
   return(AOK);
return(CANCEL);
}
