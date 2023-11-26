#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"

/****************************************************/
/*** flowtext. fill up area around pointer with text */
/****************************************************/

/** if flowobj==NULL then this is the first text block. text points
      to the text to use. If flowobj != NULL then text is ignored
      and flowobj points the the object that we should add a text
      object to.

      pg          The page to add this object to
      w           Current window
      x           where user clicked the mouse (GZZMouseX)
      y           where user clicked the mouse (GZZMouseY)
      flowobj     Last object in chain flowed
      text        text for first flow  (if flowobj==NULL)
      mode        status on return (FLOW, NORMAL)
      */

struct object *flowtext(pg,w,x,y,flowobj,text,mode,putx,puty,puth,putw)
int pg;
struct Window *w;
SHORT x,y;
struct object *flowobj;
char *text;
SHORT *mode;
COORD putx,puty;        /* positon of upper left corner of this text block */
COORD putw,puth;        /* size of text block to fill with text */
{
extern SHORT irregular;
extern short force_end;
extern SHORT time_for_same;
extern struct text_state samestate;
extern struct pageinfo page[MAXPAGES];
extern struct Window *clipboard;
extern struct pageinfo defaultpage;
extern COORD y_gridsize;
extern COORD x_gridsize;
extern SHORT snaptogrid;
extern SHORT snaptotop;
extern SHORT autoflow;
extern struct Screen *screen;
extern UWORD xinch, yinch;
struct object *newflowobj;
struct viewmsg msg;
struct pageview *view;
struct text_object *tobj,*newtobj;
COORD cw;               /* column width plus guter width */
COORD xcoord;           /* mouse x pos in COORD's */
SHORT col;
SHORT hit,fudge;
SHORT new_col,new_pg;   /* [newcolumn] or [newpage] command */
SHORT show;             /** drawtext after flow? **/
SHORT stripnl;          /** strip \n at start of this object? **/
SHORT irreg_flag;
SHORT use_given_box;    /** use putx, y, h, w, passed? **/
SHORT always_exit;   /** only do one pass when flowing **/
int done;


if (puth==0)
   use_given_box=FALSE;
else
   use_given_box=TRUE;

always_exit = use_given_box;

view=page[pg].view;   /* for shorter expresions below */

xcoord=XSNAPGRID(mousetocx(&page[pg],x));
if (xcoord <= page[pg].lm) col=0;
else col=(xcoord-page[pg].lm)/colwid(pg);

if (!use_given_box) {
   puty=YSNAPGRID(mousetocy(&page[pg],y));
   if (snaptotop && pg!=CB)
      puty=page[pg].tm;
   }

hit=TRUE;
fudge=0;
show = TRUE;
stripnl=TRUE;

flowagain:

new_col=FALSE;
new_pg=FALSE;
cw = colwid(pg);
if (!use_given_box)
   putx=page[pg].lm+col*cw;

if (flowobj)
   irreg_flag = flowobj->flags&IRREGULAR;
else
   irreg_flag = irregular;

if (!use_given_box) {
   if (!getemptyrect(pg,putx,puty,&putx,&puty,&puth,&putw,&hit,irreg_flag)) {
      *mode=FLOW;
      return(flowobj);
      }
   }

use_given_box = FALSE;     /** only can do this on 1st pass **/

puty -= fudge;
puth += fudge;

if ((newflowobj=(struct object *)createtextobj(&page[pg]))==NULL) {
   showstat(screen,OUTOFMEM);
   *mode=FLOW;
   return(flowobj);
   }

if (irreg_flag)
   newflowobj->flags |= IRREGULAR;

tobj=(struct text_object *)flowobj->data;
newtobj=(struct text_object *)newflowobj->data;

if (flowobj==NULL) {
   newflowobj->first=newflowobj;
   if (time_for_same) {           /** If reflow wants to use samestate **/
      newtobj->state = samestate;
      newtobj->state.objptr = newflowobj;
      }

   if (attach_text(newflowobj, text)!=AOK) {
      deleteobject(newflowobj);
      showstat(screen,OUTOFMEM);
      *mode=FLOW;
      return(flowobj);
      }
   }

else {
   newflowobj->first=flowobj->first;  /** set pointer to 1st obj in chain **/
   newtobj->state=tobj->endstate;
   newtobj->state.objptr = newflowobj;
   newtobj->macro = copy_macro_chain (tobj->endmacro);
   flowobj->flags &= (~MORETEXT);  /* remove flag on prev. obj */
   tobj->next=newflowobj;

   if (stripnl) {
      newtobj->flags |= STRIPNL;
      stripnl=FALSE;
      }
   }

/** Set checkwidow flag if this is an object that shouldn't have **/
/** Widows or orphans. Note this doesn't currently handle the case **/
/** where a block of text is blocked in the middle of a column **/

if (puty + puth >= page[pg].ph - page[pg].bm)
   newtobj->flags |= CHECKWIDOW;

newflowobj->x=putx;
newflowobj->y=puty;
newflowobj->w=putw;
newflowobj->h=puth;
newflowobj->numplanes=1;
newflowobj->mag=view->mag;
newflowobj->type=TEXT;
newflowobj->dpi=150;

newpointer(screen,BUSY);

if (hit)
   done=format(newflowobj,FALSE);  /* false if everything fit */
else
   done=format(newflowobj,TRUE);  /* false if everything fit */

/** Check and see if either [newpage] or [newcolumn] was used **/
/** in the last format... **/

if (force_end&TEXTS_FORCECOLEND)
   new_col = TRUE;
if (force_end&TEXTS_FORCEPAGEEND)
   new_pg = TRUE;

/** The following code handles the case when the user picked a spot **/
/** to put the text where there was not enuf room for even one line. **/

if (newtobj->height == 0 || newtobj->width == 0) {
   if (!done) {   /** here if no real text to place ! **/
      newflowobj->w=xinch;      /** give user something to grab **/
      newflowobj->h=yinch;
      *mode = NORMAL;
      return(NULL);
      }

   if (flowobj==NULL)
      autofree_off(newflowobj);       /** keep original text string **/
   deletetextobjlist(newflowobj);     /** get rid of object we just made **/
   *mode=FLOW;                        /** gona try again **/
   if (flowobj != NULL) {             /** if NULL this was 1st object **/
      tobj->next=NULL;
      flowobj->flags |= MORETEXT;
      return(flowobj);
      }
   return(NULL);                     /** flowobj==0 means 1st object **/
   }

/** OK, some text flowed. lets cleanup end get outa here **/

newflowobj->h=max(8,newtobj->height);
newflowobj->w=max(8,newtobj->width);

if (show) {
   msg.obj=newflowobj;
   msg.page=&page[pg];
   msg.command=DRAWIT;
   viewmonitor(&msg);
   }

if (!done) {   /* !done really means we are done. Confused? See above **/
   *mode=NORMAL;
   return(NULL);
   }
newflowobj->flags |= MORETEXT;


if (hit) {
   puty += puth;
   fudge = puth - newflowobj->h;
   }
else {
   puty=newflowobj->y + newflowobj->h;
   fudge=0;
   }

if (always_exit || puty >= page[pg].ph - page[pg].bm || new_col || new_pg) {
   if (autoflow && !always_exit) {
      stripnl=TRUE;     /** tell format to strip \n at top of this obj */
      col++;
      if (col >= page[pg].nc || new_pg) {
         col=0;
         pg++;
         if (pg >=LASTPAGE)   /** no where to go to! */
            pg--;
         else {
            check_page(pg);
            }
         }
      if (page[pg].view==0)
         show=FALSE;
      puty = page[pg].tm;
      fudge=0;
      }
   else {
      *mode=FLOW;
      return(newflowobj);
      }
   }

flowobj=newflowobj;
goto flowagain;
}

checklines(x1,w1,x2,w2)

SHORT x1,w1,x2,w2;

{
return ((x1 <= (x2 + w2)) && ((x1 >= x2) || ((x1 + w1) >= x2)));
}

/** graphic_border -- The space to separate from graphic when **/
/** flowing around graphics **/

#define GRAPHIC_BORDER 80


/********************/
/*** getemptyrect ***/
/********************/

/** On Entry:
   pg               Page to place object on
   x_pos, y_pos     Coord. of where to start flowing

   On Exit
   x,y,h,w          Free box that can be used for formating
                    text into.

   */


getemptyrect(pg,x_pos,y_pos,x,y,h,w,hit,irregular)

SHORT pg;
COORD x_pos, y_pos;
COORD *x,*y,*h,*w;
SHORT *hit;           /** blocked or exapanding? **/
int irregular;

{
extern struct pageinfo page[MAXPAGES];
struct object *obj;
struct Node *node;
SHORT cw;
COORD putx,puty,putw,puth;
COORD temp,oldy;

cw=colwid(pg);

putx=x_pos;
puty=y_pos;
putw=cw-page[pg].cg;
oldy=page[pg].ph-page[pg].bm;

/** find x and width to flow into **/

*hit=TRUE;
if (!irregular) { /** Only if not flowing arround irreguar graphics **/
   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;
      if (!(obj->flags&TRANSPARENT) &&
           checklines(obj->x, obj->w, putx, putw)) {
         if (obj->y <= puty && obj->y + obj->h > puty) {
            *hit=FALSE;
            oldy = min (oldy, obj->y + obj->h);
            if (obj->x  >  putx + 144) {
               putw = obj->x - putx - GRAPHIC_BORDER;
               }
            else {
               temp=putx;
               putx=obj->x+obj->w + GRAPHIC_BORDER;
               putw = putw - (putx - temp);
               }
            if (putw < 144)
                return(FALSE);
           }
         }
      }
   /** got x,y,w now find height **/

   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
     obj=(struct object *)node;
     if (!(obj->flags&TRANSPARENT) && checklines(obj->x, obj->w, putx, putw))
        if (obj->y > puty && obj->y < oldy)  {
            oldy=obj->y;
            *hit=TRUE;
            }
      }
   }
puth=oldy-puty;

if (puth < 48 || puth > page[pg].ph)   /** return if height invalid **/
   return(FALSE);


*x=putx;
*y=puty;
*h=puth;
*w=putw;
return(TRUE);
}


/** given page number, return cw + gutter of that page **/

colwid(pg)

{
int cw;
extern struct pageinfo page[MAXPAGES];

cw=(page[pg].pw-page[pg].rm-page[pg].lm-(page[pg].nc-1)*
              page[pg].cg)/page[pg].nc;
cw=cw+page[pg].cg; /* cw= col wid plus col gutter */
return(cw);
}

/*********************************************************************/
/** get_bounds  -- returns left & right bounds surrounding of line  **/
/** described by  x_pos, y_pos, width                               **/
/*********************************************************************/


get_bounds(flow_obj, y_off, x_left, x_right, margin)

COORD *x_left, *x_right;
COORD y_off, margin;
struct object *flow_obj;

{
COORD x_pos, y_pos, width, offset;
SHORT pg;
extern struct pageinfo page[MAXPAGES];
struct object *obj;
struct Node *node;
short left1,left2;
short right1,right2;

pg = flow_obj->pn;
width = flow_obj->w;
offset = width/4;
x_pos = flow_obj->x;
y_pos = flow_obj->y + y_off;
*x_left = x_pos;
*x_right = x_pos+width;

if (flow_obj->flags&IRREGULAR) {
   for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;

      /** this checks the special case where an object completly covers  **/
      /** the flowing object.  In this case, igonore obj.  This lets     **/
      /** you resize an object on top of a screened back ground, for ex. **/

      if (!(obj->x < flow_obj->x &&
          obj->x + obj->w > flow_obj->x + flow_obj->w &&
          obj->y < flow_obj->y &&
          obj->y + obj->h > flow_obj->y + flow_obj->h)) {

         left1 = is_point_bounded(x_pos+offset, y_pos, obj);
         left2 = is_point_bounded(x_pos, y_pos, obj);
         right1= is_point_bounded(x_pos+width-offset, y_pos, obj);
         right2= is_point_bounded(x_pos+width, y_pos, obj);

         if (flow_obj!=obj && !(obj->flags&TRANSPARENT)) {
            if (left1 || left2) {
               if (obj->type==GRAPHIC)
                  get_graphic_bounds(obj, x_pos, y_pos, width, x_left, x_right,margin);
               else
                  *x_left = obj->x+obj->w+margin;
               }
            else if (right1 || right2) {
               if (obj->type==GRAPHIC)
                  get_graphic_bounds(obj, x_pos, y_pos, width, x_left, x_right,margin);
                else
                  *x_right = obj->x-margin;
               }
            }
         }
      }
   }
*x_left -= flow_obj->x;
*x_right = width - (*x_right - flow_obj->x);
}

/*****************************************************************/
/** given an x,y pos return true if contained in a given object **/
/*****************************************************************/

is_point_bounded(x_pos, y_pos, obj)

COORD x_pos, y_pos;
struct object *obj;

{
return(x_pos >= obj->x
    && x_pos <= obj->x+obj->w
    && y_pos >= obj->y
    && y_pos <= obj->y+obj->h);
}

get_graphic_bounds(obj, x_pos, y_pos, width, x_left, x_right, margin)

struct object *obj;
COORD x_pos, y_pos, width;
COORD *x_left, *x_right;
COORD margin;
{
short y;
struct graphic *g;
COORD offset;

g = obj->data;
offset = y_pos - obj->y;
y = g->h*offset/obj->h;

if (is_point_bounded(x_pos, y_pos, obj) ||
    is_point_bounded(x_pos+width/4, y_pos, obj)) { /** obj on left column **/
   *x_left = obj->x + obj->w*scan_graphic(g, g->w, y, -1)/g->w;
   *x_left += margin;
   if (*x_left < x_pos)
      *x_left = x_pos;
   }
else {
   *x_right = obj->x + obj->w*scan_graphic(g, 0, y, 1)/g->w;
   *x_right -= margin;
   if (*x_right > x_pos + width)
      *x_right = x_pos + width;
   }
}

scan_graphic (g, start_x, y, inc)
struct graphic *g;
short start_x,y,inc;
{
short x;


for (x=start_x+inc; x!=0 && x!=g->w; x = x + inc)
   if (get_pix_val(g, x, y)!=0)
      break;

return (x);
}

