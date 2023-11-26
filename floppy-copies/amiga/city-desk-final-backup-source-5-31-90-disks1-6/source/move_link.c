#include "exec/types.h"
#include "desk.h"

move_link (obj,x,y,clip)

struct object *obj;
int x,y,clip;

{
struct Window *w;
struct object *orig_obj;
extern struct pageinfo page[MAXPAGES];
extern COORD x_gridsize;
extern COORD y_gridsize;
extern SHORT snaptogrid;
extern SHORT pointer_mode;
short source_pg, dest_pg;
short source_mag, dest_mag;
short val;
short newx, newy, dummy;
long adj_x, adj_y;

source_pg = obj->pn;
source_mag = page[source_pg].view->mag;
orig_obj = obj;

if (!clip) {   /** moving chain off this page **/
   w = find_link_window(obj,x,y);
   dest_pg = whatpage(w);

   if ((!w) || (dest_pg==-1))
      return(0);  /* not draged into a valid window */

   dest_mag = page[dest_pg].view->mag;
   x = x - page[dest_pg].view->window->LeftEdge +
           page[source_pg].view->window->LeftEdge;
   y = y - page[dest_pg].view->window->TopEdge +
           page[source_pg].view->window->TopEdge;

   conv_obj_data(obj, &newx, &newy, &dummy, &dummy, TRUE);
   newx += x;
   newy += y;
   adj_x = mousetocx(&page[dest_pg], newx) - obj->x;
   adj_y = mousetocy(&page[dest_pg], newy) - obj->y;

   if (obj->top_link)
      obj = obj->top_link;

   while (obj) {
      erase_obj_image(obj);
      Remove(&obj->node);
      AddHead(&page[dest_pg].objects, obj);
      obj->pn=dest_pg;      /** make sure obj knows where its at **/
      obj->x += adj_x;
      obj->y += adj_y;

      if (obj->mag != page[dest_pg].view->mag) {
         freedata(obj);
         obj->mag=page[dest_pg].view->mag;
         makedisplay(obj);
         }
      displayobject(page[dest_pg].view, obj);

      /* if (obj->type==BLOCK) */
         /* update(page[dest_pg].view,&page[dest_pg].objects,obj); */
     /*    refresh_all(obj); */
/*      else   */
         refresh_top(obj);

      obj = obj->next;
      }
   }

else {   /* MOVEDONPAGE */
   dest_pg=page[source_pg].pn;


   if (obj->top_link && pointer_mode!=PTR_LINKED)
      obj = obj->top_link;

   while (obj) {
      erase_obj_image(obj);
      conv_obj_data(obj, &newx, &newy, &dummy, &dummy, TRUE);
      newx += x;
      newy += y;
      Remove(&obj->node);
      AddHead(&page[dest_pg].objects,obj);
/*      val=(stocx(newx)+(SHORT)page[dest_pg].view->x)/mag;
      obj->x=XSNAPGRID((COORD)val); */
      val = mousetocx(&page[dest_pg], newx);
      if (val < 0 ) val=0;
      obj->x = val;
      if (dest_pg!=CB && obj->x + obj->w > page[dest_pg].pw) /* must all be */
         obj->x = page[dest_pg].pw - obj->w;  /* one page */
 
/*      val=(stocy(newy)+(SHORT)page[dest_pg].view->y)/mag;
      obj->y=YSNAPGRID((COORD)val); */
      val = mousetocy(&page[dest_pg], newy);
      if (val < 0 ) val=0; /* can't put it off the page. */
      obj->y = val;
      if (dest_pg !=CB && obj->y + obj->h > page[dest_pg].ph)
         obj->y = page[dest_pg].ph - obj->h;

      displayobject(page[dest_pg].view, obj);

/*      if (obj->type==BLOCK) */
         /* update(page[dest_pg].view,&page[dest_pg].objects,obj); */
    /*     refresh_all(obj); */
/*      else                   */
         refresh_top(obj);

      if (pointer_mode==PTR_LINKED)  /** PTR_LINKED means only move this **/
         obj = 0;
      else
         obj = obj->next;
      }
   }
return(0);
}

/** When moving an object (linked or not), all linked members
    must go to the same window.  This routine finds that window,
    or returns a zero if all members are not in the same window. */

find_link_window(obj,xoff,yoff)

struct object *obj;
int xoff,yoff;

{
struct Window *w1, *w2, *findwindow();
struct screen *s;
struct object *p;
extern struct pageinfo page[MAXPAGES];
SHORT x,y,h,w;
short first_pass;

s=page[CB].view->window->WScreen;

p = obj->top_link;
if (p==0)
   p = obj;

first_pass = TRUE;

while (p) {
   conv_obj_data(p,&x,&y,&h,&w, FALSE);
   w1=findwindow(s, x+xoff, y+yoff, h, w);
   if (first_pass) {
      first_pass = FALSE;
      w2 = w1;
      }
   if (w1!=w2)
      return(0);
   p = p->next;
   }
return(w2);
}


