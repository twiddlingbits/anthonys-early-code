#include "exec/types.h"
#include "exec/exec.h"
#include "graphics/text.h"
#include "graphics/gfx.h"
#include "graphics/rastport.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"
#include "stdio.h"

extern COORD xshadowsize;
extern COORD yshadowsize;
extern UWORD xinch, yinch;


/**********************************/
/** match range from object list **/
/**********************************/
                                     
/*
  list == list to search for an object that x,y covers
  x,y  == try and find an object that contains this coord.
  */

struct object *matchrange(list,x,y)

struct List *list;
COORD x,y;
{
struct Node *node;
struct object *obj;
extern SHORT pointer_mode;

for (node=list->lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
   if (x>=obj->x && y>=obj->y && x<(obj->x+max(8,obj->w)) &&
        y < (obj->y+max(25,obj->h))) {
      if ( (!(obj->flags&STAMPED)) || pointer_mode==PTR_STAMPED) {
         return (obj);
         }
      }
   }
return (0);
}

/**************************************************************************/
/** Update  objects with INV or  TOP flags set (objects on top)          **/
/**************************************************************************/

refresh_top(thisobj)

struct object *thisobj;

{
struct Node *node;
struct object *obj;
COORD x,y,h,w;
struct Window *Window;
extern struct pageinfo page[MAXPAGES];
struct pageview *view;
short do_exit;
int pg;

pg=thisobj->pn;
Window=page[pg].view->window;
view=page[pg].view;

x=thisobj->x;
y=thisobj->y;
h=thisobj->h;
w=thisobj->w;

do_exit = TRUE;
for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
   if (((obj->flags&TOP) || (obj->flags&INV)) && (obj->flags&DO_NOT_DRAW)==0) {
      if (overlap(x,y,h,w,obj->x,obj->y,obj->h,obj->w)) {
         do_exit = FALSE;
         }
      }
   }

if (do_exit)
   return(0);

x=page[pg].view->x/view->mag;
y=page[pg].view->y/view->mag;
h=page[pg].view->h/view->mag;
w=page[pg].view->w/view->mag;

for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
   if (((obj->flags&TOP) || (obj->flags&INV)) && (obj->flags&DO_NOT_DRAW)==0) {
      if (overlap(x,y,h,w,obj->x,obj->y,obj->h,obj->w)) {
        if (obj->mag != view->mag || obj->screendata == 0) {
           freedata(obj);
           obj->mag=view->mag;
           makedisplay(obj);   /* recalculate bitmap for new magnification */
           }
/* printf("rt %x\n",thisobj); */
        displayobject(view,obj);
        }
      }
   }
}

/*******************************************************************/
/** Update all non-screened objects with INV or TOP flags not set **/
/*******************************************************************/

refresh_reg (thisobj)

struct object *thisobj;

{
struct Node *node;
struct object *obj;
COORD x,y,h,w;
struct Window *Window;
extern struct pageinfo page[MAXPAGES];
struct pageview *view;
int pg;

pg=thisobj->pn;
Window=page[pg].view->window;
view=page[pg].view;
x=thisobj->x;
y=thisobj->y;
h=thisobj->h;
w=thisobj->w;

for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
  obj=(struct object *)node;
   if ((obj->flags&(TOP|INV))==0  && (obj->flags&DO_NOT_DRAW)==0) {
      if (overlap(x,y,h,w,obj->x,obj->y,obj->h,obj->w) && (obj!=thisobj)) {
        if (obj->mag != view->mag || obj->screendata == 0) {
           freedata(obj);
           obj->mag=view->mag;
           makedisplay(obj);   /* recalculate bitmap for new magnification */
           }
/* printf("rr %x\n",thisobj); */
        displayobject(view,obj);
        }
      }
   }
}

refresh_all (thisobj)

struct object *thisobj;

{
/* printf("refreshing: %x\n",thisobj); */
refresh_reg (thisobj);
refresh_top (thisobj);
draw_columns(thisobj->pn);
}

/**************************/
/** display an object    **/
/**************************/
displayobject(view,obj)

struct pageview *view;
struct object *obj;
{
struct BitMap bitmap;
struct RastPort rp;
struct rule r,*old_r;
WORD mag,density;
int i;
UBYTE minterm;
int pixwidth, pixheight, planesize;
short x1,x2,y1,y2;
COORD start;

if (obj->type==RULE) {
   old_r=(struct rule *)obj->data;
   r = *old_r;
   obj->data = &r;
   if (r.flags==HORIZ) {
      r.width = r.width*yinch/576;
      }
   else {
      r.width = r.width*xinch/576;
      }
   drawrule(view,obj,BLACK);
   obj->data = old_r;
   return(0);
   }

if (obj->flags & SHADOWRIGHT) {
   start=(obj->w*15)/100;
   drawhline(view,obj->x + start, /* shadow  85 % of box **/
      obj->y + obj->h,
      obj->w - start + xshadowsize,
      yshadowsize,(UWORD)0xFFFF,BLACK);

   start=(obj->h*15)/100;
   drawvline(view,obj->x + obj->w,
      obj->y + start,
      obj->h - start,
      xshadowsize,0xFFFF,BLACK);
   }

if (obj->flags & SHADOWLEFT) {
   drawhline(view,obj->x,
      obj->y + obj->h,
      (obj->w*85)/100,
      yshadowsize,(UWORD)0xFFFF,BLACK);

   start = obj->h*15/100;
   drawvline(view,obj->x - xshadowsize,
      obj->y + start,
      obj->h - start + yshadowsize,
      xshadowsize,0xFFFF,BLACK);
   }

if (obj->flags&TOP || obj->density > 0) {
   density = (WORD)obj->density;
   i = GRAY2;
   view->window->RPort->Mask = 1;
   if (density > 49) {
      i = GRAY1;
      view->window->RPort->Mask = 2;
      }
   if (density > 98) {
      i = BLACK;
      view->window->RPort->Mask = 0xFF;
      }
   if (density==0) {
      i = WHITE;
      view->window->RPort->Mask = 0;
      }
   if (obj->flags&TOP) {
      view->window->RPort->Mask = 0xFF;
      }
   SetDrMd(view->window->RPort, JAM1);
   SetAPen(view->window->RPort, i);

   x1=ctomousex(view, obj->x);
   y1=ctomousey(view, obj->y);
   x2=ctomousex(view, obj->x+obj->w)-1;
   y2=ctomousey(view, obj->y+obj->h)-1;
   if (x2 < x1) x2=x1;
   if (y2 < y1) y2=y1;
   RectFill(view->window->RPort, x1, y1, x2, y2);
   view->window->RPort->Mask = 0xFF;
   draw_outline(view, obj);
   if (obj->type==BLOCK)
      return(0);
   }

if (obj->screendata==0) {
   draw_outline(view, obj);
   return(0);
   }

mag=obj->mag;
InitRastPort(&rp);

pixwidth  = ctosx(obj->w*obj->mag);
pixheight = ctosy(obj->h*obj->mag);
planesize = RASSIZE(pixwidth, pixheight);

bitmap.BytesPerRow=RASSIZE(pixwidth,1);
bitmap.Rows=pixheight;
bitmap.Flags=0;
bitmap.Depth=2;

if (obj->numplanes==1) {
   bitmap.Planes[0]=obj->screendata;
   bitmap.Planes[1]=obj->screendata;
   }
else {
   bitmap.Planes[1]=obj->screendata;
   bitmap.Planes[0]=obj->screendata + planesize;
   }

rp.BitMap=&bitmap;

minterm=0xEE;           /** D = B + C  **/
if (obj->flags&INV)
   minterm=0x20;        /** D = B*C  **/
/* if (obj->flags&TOP)
   minterm=0xC0;*/        /** D = B    **/

/* printf("doing clipblit\n"); */
ClipBlit(&rp,0,0,view->window->RPort,
          ctomousex(view, obj->x),
          ctomousey(view, obj->y),
          pixwidth, pixheight, minterm, 0x03, 0);

/* ClipBlit(&rp,0,0,view->window->RPort,
          ctosx(obj->x*mag) - ctosx(view->x),
          XBORDER+ctosy(obj->y*mag) - ctosy(view->y),
          pixwidth, pixheight, minterm, 0x03, 0);  */

draw_outline(view, obj);
}

draw_outline(view, obj)

struct object *obj;
struct pageview *view;

{
int i,pat;

if (obj->flags & OUTLINEOBJ) {
   i=3;
   if (obj->flags & LINEWIDTH2)
      i = 8;
   else if (obj->flags & LINEWIDTH3)
      i = 16;
   else if (obj->flags & LINEWIDTH4)
      i = 24;

   i = max(1,(i*yinch/576));  /** make width closer to correct size **/
                              /** this gives wrong size vert lines  **/
   if (obj->flags&DASHEDLINE)
      pat = 0xF0F0;
   else
      pat = 0xFFFF;

   drawthickbox(view,obj->x , obj->y , obj->h ,
         obj->w, i, pat, BLACK);
   }
}


/****************************************/
/** clear display  of object's size    **/
/****************************************/
clearobject(view,obj)

struct pageview *view;
struct object *obj;
{
struct rule r,*old_r;
struct Window *Window;
WORD mag,color;
int i;
short x1,x2,y1,y2;
COORD start;

if (view->window->Flags&BACKDROP)
   color = GRAY2;
else
   color = WHITE;

if (obj->type==RULE) {
   old_r=(struct rule *)obj->data;
   r = *old_r;
   obj->data = &r;
   if (r.flags==HORIZ) {
      r.width = r.width*yinch/576;
      }
   else {
      r.width = r.width*xinch/576;
      }
   drawrule(view,obj,color);
   obj->data = old_r;
   return(0);
   }

if (obj->flags & SHADOWRIGHT) {
   start=(obj->w*15)/100;
   drawhline(view,obj->x + start, /* shadow  85 % of box **/
      obj->y + obj->h,
      obj->w - start + xshadowsize,
      yshadowsize,(UWORD)0xFFFF,color);

   start=(obj->h*15)/100;
   drawvline(view,obj->x + obj->w,
      obj->y + start,
      obj->h - start,
      xshadowsize,0xFFFF,color);
   }

if (obj->flags & SHADOWLEFT) {
   drawhline(view,obj->x,
      obj->y + obj->h,
      (obj->w*85)/100,
      yshadowsize,(UWORD)0xFFFF,color);

   start = obj->h*15/100;
   drawvline(view,obj->x - xshadowsize,
      obj->y + start,
      obj->h - start + yshadowsize,
      xshadowsize,0xFFFF,color);
   }

Window=view->window;
SetDrMd(Window->RPort, JAM1);
SetAPen(Window->RPort, color);

x1=ctomousex(view, obj->x);
y1=ctomousey(view, obj->y);
x2 = x1 + ctosx(obj->w*obj->mag);  /** took out -1 here.  Kludge to fix **/
y2 = y1 + ctosy(obj->h*obj->mag);  /** drawthickbox which draws too big **/
if (x2 < x1) x2=x1;                /** sometimes **/
if (y2 < y1) y2=y1;
RectFill(Window->RPort, x1, y1, x2, y2);
return(0);
}

/********************/
/** createobject   **/
/********************/

struct object *createobject(list)

struct List *list;

{
struct object *obj;
extern struct object default_object;
extern SHORT irregular;

obj=(struct object *)AllocMem(sizeof(struct object),MEMF_CLEAR);
if (obj==0) return(0);
obj->node.ln_Name="";
AddHead(list,&obj->node);
obj->dpi=default_object.dpi;
obj->flags=(default_object.flags&(~IRREGULAR));
if (irregular)
   obj->flags |= IRREGULAR;
obj->density=default_object.density;
obj->angle=default_object.angle;
obj->first = obj;
return(obj);
}

/*********************/
/*** delete object ***/
/*********************/

deleteobject(obj)

struct object *obj;

{
struct graphic *g;
char *text;

remove_link(obj);

if (obj->type==RULE) {
   Remove(obj);
   FreeMem(obj->data,sizeof(struct rule));
   FreeMem(obj,sizeof(struct object));
   return(TRUE);
   }

if (obj->type==GRAPHIC) {
   Remove(obj);
   g=(struct graphic *)obj->data;
   free_graphic_data(g);
   FreeMem(g,sizeof(struct graphic));
   freedata(obj);  /** free screendata if needed */
   FreeMem(obj,sizeof(struct object));
   return(TRUE);
   }

if (obj->type==TEXT) {
   obj=obj->first;      /** if chain, make sure we delete it all **/
   text = tobj_text(obj);
/**   FreeMem(text, strlen(text)+1); auto free'd by free_macro_chain **/
   deletetextobjlist(obj);
   return(TRUE);
   }

if (obj->type==BLOCK) {
   Remove(obj);
   FreeMem(obj,sizeof(struct object));
   return(TRUE);
   }

return(FALSE);
}

/******************************/
/**** Delete textobj list *****/
/******************************/

deletetextobjlist(obj)
struct object *obj;
{
extern struct pageinfo page[MAXPAGES];
struct object *next;
struct viewmsg msg;

do {
   if (obj->screendata!=0 && page[obj->pn].view!=0) {  /** this object being displayed? **/
      msg.obj=obj;
      msg.page=&page[obj->pn];
      msg.command=CLEAR;
      viewmonitor(&msg);
      }
   remove_link(obj);
   Remove(obj);
   next=((struct text_object *)obj->data)->next;
   free_text_object(obj);   /** free any text_line structures **/
   free_macro_chain(((struct text_object *)obj->data)->macro);
   ((struct text_object *)obj->data)->macro=NULL;
   free_macro_chain(((struct text_object *)obj->data)->endmacro);
   ((struct text_object *)obj->data)->endmacro=NULL;
   FreeMem(obj->data,sizeof(struct text_object));  
   freedata(obj);  /** free screendata if needed */
   FreeMem(obj,sizeof(struct object));
   obj=next;
   } while (next!=NULL);
}

