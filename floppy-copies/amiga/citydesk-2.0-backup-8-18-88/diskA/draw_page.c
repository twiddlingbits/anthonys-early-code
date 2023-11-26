#include "exec/types.h"
#include "exec/nodes.h"
#include "intuition/intuition.h"
#include "desk.h"

/*******************/
/**** draw page ****/
/*******************/

extern COORD columnwidth;
extern COORD x_gridsize;
extern COORD y_gridsize;
extern COORD outlinespc;
extern SHORT displaygrid;
extern SHORT displayguides;


draw_page(pg)

int pg;

{
SHORT x,y,mag;
LONG i;
LCOORD j;
SHORT pw,ph;
struct Window *Window;
struct object obj;
extern struct pageinfo page[MAXPAGES];
struct object *objptr;
struct Node *node;
extern SHORT std_width, screensize;


if (page[pg].view==0) return(0);

Window=page[pg].view->window;
mag=page[pg].view->mag;          /* page magnigication factor */
SetDrMd(Window->RPort,JAM1);

if (pg==CB) {
   SetAPen(Window->RPort,GRAY2);    /** light gray **/
   RectFill(Window->RPort,0,0,std_width-1,screensize-1);
   for (node=page[CB].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      objptr = (struct object *)node;
      makedisplay(objptr);
      displayobject(page[CB].view,objptr);
      }
   return(AOK);
   }

/* clear screen */

SetAPen(Window->RPort,COLORBACK);

RectFill(Window->RPort,XBORDER,YBORDER,
         Window->Width - XBORDER_AREA + XBORDER - 1,
         Window->Height - YBORDER_AREA + YBORDER - 1);

pw=ctosx(page[pg].pw);
ph=ctosy(page[pg].ph);

draw_columns(pg);

if ((page[pg].flags & DRAWRULES) && (page[pg].nc > 1)) {
   j=page[pg].lm+columnwidth+(page[pg].cg>>1);
   for (i=1; i < page[pg].nc; i++) {
      x=ctomousex(page[pg].view,j);  /** COORD -> screen pixel **/
      Move(Window->RPort,x,ctomousey(page[pg].view,page[pg].tm));
      Draw(Window->RPort,x,ctomousey(page[pg].view,page[pg].ph - page[pg].bm));
      j=j+columnwidth+page[pg].cg;
      }
   }

/** draw grid **/

if (displaygrid) {
   SetAPen(Window->RPort, BLACK);
   for (i=0; i <= page[pg].pw*mag; i=i + x_gridsize*mag)
      for (j=0; j <= page[pg].ph*mag; j=j + y_gridsize*mag)
         if (i>page[pg].view->x && j>page[pg].view->y &&
            i < page[pg].view->x+page[pg].view->w &&
            j < page[pg].view->y+page[pg].view->h)
            WritePixel(Window->RPort,
               ctosx(i-page[pg].view->x)+XBORDER,
               ctosy(j-page[pg].view->y)+YBORDER);
   }


if (page[pg].flags & OUTLINEPAGE) {
   x=ctomousex(page[pg].view,page[pg].lm-outlinespc);
   y=ctomousey(page[pg].view,page[pg].tm-outlinespc);
   ph=ctosy(page[pg].ph-page[pg].tm-page[pg].bm+(outlinespc<<1))*mag;
   pw=ctosx(page[pg].pw-page[pg].lm-page[pg].rm+(outlinespc<<1))*mag;
   drawbox(Window->RPort,x,y,ph,pw);
   }

/************************************/
/*** re-draw any object in window ***/
/************************************/

obj.x=page[pg].view->x/page[pg].view->mag;
obj.y=page[pg].view->y/page[pg].view->mag;
obj.w=page[pg].view->w/page[pg].view->mag;
obj.h=page[pg].view->h/page[pg].view->mag;
obj.pn=page[pg].pn;

update(page[pg].view,&page[pg].objects,&obj);

}
