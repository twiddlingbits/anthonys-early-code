#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

/*****************/
/** drag a box  **/
/****************/

/* obj  Pointer to object to drag around screen
   clip TRUE means confine object to this window
   */

dragbox(obj,clip)

struct object *obj;
int clip;

{
extern struct pageinfo page[MAXPAGES];
struct IntuiMessage *message;
struct Window *Window;
struct RastPort *rp;
ULONG class;
USHORT code;
SHORT mousex, mousey;
SHORT oldx, oldy;
SHORT x, y;
SHORT pg;
/* SHORT lm; */

x = 0;
y = 0;

pg = obj->pn;
Window = page[pg].view->window;
if (clip) {
   rp = Window->RPort;
   }
else {
   rp = &Window->WScreen->RastPort;
   }

SetDrMd(rp,COMPLEMENT);
draw_link(obj, rp, x, y, clip);
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
      draw_link(obj, rp, x, y, clip);
      move_link(obj, x, y, clip);
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
/* lm = last_message(message); */
ReplyMsg(message);

/* if (lm)
   show_coord(obj, mousex, mousey, x, y, 0, 0);
else printf("didn't show\n"); */

if (mousey!=oldy || mousex!=oldx) {
   draw_link(obj, rp, x, y, clip);
   x=x+mousex-oldx;
   y=y+mousey-oldy;

   draw_link(obj, rp, x, y, clip);
   oldx=mousex;
   oldy=mousey;
   show_coord(obj, mousex, mousey, x, y, 0, 0);
   }

if (class==MOUSEBUTTONS && code==SELECTUP) {
   ReportMouse(FALSE,Window);
   draw_link(obj, rp, x, y, clip);
   move_link(obj, x, y, clip);
   return(0);
   }
goto loop;
}

draw_link(obj, rp, xoff, yoff, clip)

struct object *obj;
struct RastPort *rp;
SHORT xoff,yoff,clip;

{
extern SHORT pointer_mode;
struct object *p;
SHORT x,y,h,w;

if (pointer_mode==PTR_LINKED) {
   conv_obj_data(obj, &x,&y,&h,&w, clip);
   clipdrawbox(rp, x+xoff, y+yoff, h, w);
   return;
   }

p = obj->top_link;

if (p==0)
   p = obj;

while (p) {
   conv_obj_data(p,&x,&y,&h,&w, clip);
   clipdrawbox(rp, x+xoff, y+yoff, h, w);
   p = p->next;
   }
}

conv_obj_data(obj,x,y,h,w,clip)

struct object *obj;
SHORT *x,*y,*h,*w;

{
short pg;
extern struct pageinfo page[MAXPAGES];

pg = obj->pn;
*x=ctosx(obj->x*obj->mag)+XBORDER; /* obj positions are in COORD */
*y=ctosy(obj->y*obj->mag)+YBORDER; /* must change to screen pixles */
*x=*x-ctosx(page[pg].view->x);   /* adjust if not viewing upper */
*y=*y-ctosy(page[pg].view->y);   /* left corner */
*h = ctosy(obj->h*obj->mag);
*w = ctosx(obj->w*obj->mag);

if (!clip) {
   *x=*x+page[pg].view->window->LeftEdge;   /* make x&y screen coors */
   *y=*y+page[pg].view->window->TopEdge;    /* not window... */
/*    *x=*x+page[pg].view->window->BorderLeft; */
/*   *y=*y+page[pg].view->window->BorderTop; */
   }
}

