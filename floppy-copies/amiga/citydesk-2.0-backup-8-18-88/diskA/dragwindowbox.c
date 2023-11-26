#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

/**********************************************/
/** drag a box inside a window with clipping **/
/**********************************************/

/* Window ==  ptr to window to use
   &x,&y  ==  ptr to SHORT x,y of box
   h,w    ==  height and width of the box to drag
   */

dragwindowbox(Window,x,y,h,w)

struct Window *Window;
SHORT *x,*y;
SHORT h,w;
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
SHORT mousex,mousey,oldx,oldy,newx,newy;
SHORT maxw,maxh;


maxw=638;
maxh=198;
SetDrMd(Window->RPort,COMPLEMENT);
drawbox(Window->RPort,*x,*y,h,w);
ReportMouse(TRUE,Window);
oldx=Window->MouseX;
oldy=Window->MouseY;

loop:    /** empty buffer then get a msg **/

while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))!=0) {
   class=message->Class;
   code=message->Code;
   ReplyMsg(message);
   if (class==MOUSEBUTTONS && code==SELECTUP) {
      ReportMouse(FALSE,Window);
      drawbox(Window->RPort,*x,*y,h,w);
      return(0);
      }
   }

while ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   }

class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
ReplyMsg(message);

show_coord(*x,*y);
SetDrMd(Window->RPort,COMPLEMENT);

if (class==MOUSEBUTTONS && code==SELECTUP) {
   ReportMouse(FALSE,Window);
   drawbox(Window->RPort,*x,*y,h,w);
   return(0);
   }

if (mousey!=oldy || mousex!=oldx) {
   drawbox(Window->RPort,*x,*y,h,w);
   newx=*x+mousex-oldx;
   newy=*y+mousey-oldy;
/*   if (newx>=0 && (newx+w)<=maxw) *x=newx;
   if (newy>=0 && (newy+h)<=maxh) *y=newy; */
   *x=newx;
   *y=newy;
   drawbox(Window->RPort,*x,*y,h,w);
   oldx=mousex;
   oldy=mousey;
   }

goto loop;
}
