#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

/*******************************************************************/
/*** getclick()   Gets a mouse click and returns new window, x, y **/
/*******************************************************************/

struct click *getclick(Window)

struct Window *Window;

{
static struct click click;
struct IntuiMessage *message;
ULONG class;
USHORT code;

do {
   while((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
      Wait(1<<Window->UserPort->mp_SigBit);
      }
   class=message->Class;
   code=message->Code;
   click.x=message->MouseX - XBORDER;
   click.y=message->MouseY - YBORDER;
   click.Window=message->IDCMPWindow;

/*   if (click.Window->Flags&GIMMEZEROZERO) {
      click.x=click.Window->GZZMouseX;
      click.y=click.Window->GZZMouseY;
      }
*/
   ReplyMsg(message);

   } while (class!=MOUSEBUTTONS || code!=SELECTDOWN);
return(&click);
}
