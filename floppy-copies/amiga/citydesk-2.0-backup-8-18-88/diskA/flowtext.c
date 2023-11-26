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

struct object *flowtext(pg,w,x,y,flowobj,text,mode)
int pg;
struct Window *w;
SHORT x,y;
struct object *flowobj;
char *text;
SHORT *mode;
{
extern short force_end;
extern SHORT time_for_same;
extern struct text_state samestate;
extern struct pageinfo page[MAXPAGES];
extern struct Window *clipboard;
extern struct pageinfo defaultpage;
extern COORD y_gridsize;
extern SHORT snaptogrid;
extern SHORT snaptotop;
extern SHORT autoflow;
extern struct Screen *screen;
struct object *newflowobj;
struct viewmsg msg;
struct pageview *view;
struct text_object *tobj,*newtobj;
struct macro_text *mt;
COORD cw;               /* column width plus guter width */
COORD putx,puty;        /* positon of upper left corner of this text block */
COORD putw,puth;        /* size of text block to fill with text */
COORD xcoord;           /* mouse x pos in COORD's */
SHORT col;
SHORT hit,fudge;
SHORT new_col,new_pg;   /* [newcolumn] or [newpage] command */
SHORT show;             /** drawtext after flow? **/
SHORT stripnl;          /** strip \n at start of this object? **/
int done;


/* if (page[pg].view->window!=w) return(flowobj); */

view=page[pg].view;   /* for shorter expresions below */

xcoord=(stocx(x)+view->x)/view->mag;
if (xcoord <= page[pg].lm) col=0;
else col=(xcoord-page[pg].lm)/colwid(pg);

puty=YSNAPGRID(stocy(y/view->mag)+view->y/view->mag);
if (snaptotop && pg!=CB)
   puty=page[pg].tm;

fudge=0;
show = TRUE;
stripnl=TRUE;

flowagain:

new_col=FALSE;
new_pg=FALSE;
cw = colwid(pg);
putx=page[pg].lm+col*cw;

if (!getemptyrect(pg,putx,puty,&putx,&puty,&puth,&putw,&hit)) {
   *mode=FLOW;
   return(flowobj);
   }

puty -= fudge;
puth += fudge;

if ((newflowobj=(struct object *)createtextobj(&page[pg]))==NULL) {
   showstat(screen,OUTOFMEM);
   *mode=FLOW;
   return(flowobj);
   }

tobj=(struct text_object *)flowobj->data;
newtobj=(struct text_object *)newflowobj->data;

if (flowobj==NULL) {
   newtobj->flags=FIRSTOBJECT;   /** I'm trying to phase this flag out **/
/*   newtobj->textstart=text; */
   newflowobj->first=newflowobj;
   if (time_for_same)            /** If reflow wants to use samestate **/
      newtobj->state = samestate;
   mt = AllocMem(sizeof(struct macro_text), MEMF_CLEAR);
   if (mt==0) {
      deleteobject(newflowobj);
      showstat(screen,OUTOFMEM);
      *mode=FLOW;
      return(flowobj);
      }

   mt->start = text;
   mt->end = text + strlen(text)+1;
   mt->flags = MTEXT_FREEMTEXT|MTEXT_FREETEXT;  /** auto free on **/
   newtobj->macro = (struct macro *)start_macro(0, mt, 0);
   }
else {
   newflowobj->first=flowobj->first;  /** set pointer to 1st obj in chain **/
/*   newtobj->textstart=tobj->textend; */
   newtobj->state=tobj->endstate;
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
      newflowobj->w=576;      /** give user something to grab **/
      newflowobj->h=576;
      *mode = NORMAL;
      return(NULL);
      }

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

if (puty >= page[pg].ph - page[pg].bm || new_col || new_pg) {
   if (autoflow) {
      stripnl=TRUE;     /** tell format to strip \n at top of this obj */
      col++;
      if (col >= page[pg].nc || new_pg) {
         col=0;
         pg++;
         if (pg >=MAXPAGES)   /** no where to go to! */
            pg--;
         if ((page[pg].flags&PAGEINUSE)==0) {
            copypage(&defaultpage,&page[pg]);
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

/** graphic_border -- The space to seperate from graphic when **/
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


getemptyrect(pg,x_pos,y_pos,x,y,h,w,hit)

SHORT pg;
COORD x_pos, y_pos;
COORD *x,*y,*h,*w;
SHORT *hit;           /** blocked or exapanding? **/

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
for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
   if (!(obj->flags&TRANSPARENT) &&
        checklines(obj->x, obj->w, putx, putw)) {
     if (obj->y <= puty && obj->y + obj->h > puty) {

         *hit=FALSE;
         oldy = min (oldy,obj->y + obj->h);
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

