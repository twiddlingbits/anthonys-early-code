#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"



/************************/
/*** viewmonitor.c    ***/
/************************/



/** entry point for all newly created tasks--views  */

viewmonitor(msg)

struct viewmsg *msg;

{
struct Window *w,*findwindow();
extern struct pageinfo page[MAXPAGES];
extern SHORT snaptogrid;
extern COORD x_gridsize;
extern COORD y_gridsize;
int destpg;
SHORT val,mag;
COORD newh,neww;

if (msg->command==REFRESH) {
   draw_page(msg->page->pn);
   return(0);
   }

if (msg->command==SIZED) {
   newh=msg->obj->h;     /* change to old h,w to clear it */
   neww=msg->obj->w;
   msg->obj->h=msg->oldh;
   msg->obj->w=msg->oldw;
   clearobject(msg->page->view,msg->obj);
   freedata(msg->obj);
   msg->obj->h=YSNAPGRID(newh);  /* restore new size */
   msg->obj->w=XSNAPGRID(neww);

   destpg = msg->obj->pn;
   if (destpg!=CB && msg->obj->x + msg->obj->w > page[destpg].pw) /* must all be */
      msg->obj->x = page[destpg].pw - msg->obj->w;  /* one page */
   if (destpg!=CB && msg->obj->y + msg->obj->h > page[destpg].ph)
      msg->obj->y = page[destpg].ph - msg->obj->h;

   makedisplay(msg->obj);  /** recalculate new screendata */
   displayobject(msg->page->view,msg->obj);  /** draw it **/
   update(msg->page->view,&msg->page->objects,msg->obj);
   return(0);             /** update fixes objects that got chopped **/
   }

if (msg->command==DRAWIT) {
   newpointer(page[CB].view->window->WScreen, BUSY);
   destpg = msg->obj->pn;
   if (destpg!=CB && msg->obj->x + msg->obj->w > page[destpg].pw) /* must all be */
      msg->obj->x = page[destpg].pw - msg->obj->w;  /* one page */
   if (destpg!=CB && msg->obj->y + msg->obj->h > page[destpg].ph)
      msg->obj->y = page[destpg].ph - msg->obj->h;
   makedisplay(msg->obj);
   displayobject(msg->page->view,msg->obj);
   update_top(msg->obj);
   newpointer(page[CB].view->window->WScreen, NORMAL);
   return(0);
   }
if (msg->command==CLEAR) {
   clearobject(msg->page->view,msg->obj);
   update(msg->page->view,&msg->page->objects,msg->obj);
   return(0);
   }

printf("PGM_ERROR02\n");
}


