#include "exec/types.h"
#include "exec/exec.h"
#include "graphics/text.h"
#include "graphics/gfx.h"
#include "graphics/rastport.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"
#include "stdio.h"

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
   if (x>=obj->x && y>=obj->y && x<(obj->x+max(16,obj->w)) &&
        y < (obj->y+max(40,obj->h))) {
      if ( (!(obj->flags&STAMPED)) || pointer_mode==PTR_STAMPED) {
         return (obj);
         }
      }
   }
return (0);
}
/**********************************/
/** update all objects in range ***/
/**********************************/
                                     
       /*
    view == view to update
    list == list to search for an object that x,y covers
    thisobj  == get x,y,h,w from this object. Update all other objects
             in "list" in this range.
    */

update(view,list,thisobj)

struct List *list;
struct pageview *view;
struct object *thisobj;
{
struct Node *node;
struct object *obj;
COORD x,y,h,w;
struct Window *Window;

Window=view->window;
x=thisobj->x;
y=thisobj->y;
h=thisobj->h;
w=thisobj->w;

newpointer(Window->WScreen,BUSY);
for (node=list->lh_Head;node->ln_Succ;node=node->ln_Succ) {
  obj=(struct object *)node;
   if (!(obj->flags&INV) && !(obj->flags&TOP)) {
      if (overlap(x,y,h,w,obj->x,obj->y,obj->h,obj->w) && obj!=thisobj) {
        if (obj->mag != view->mag || obj->screendata == 0) {
           freedata(obj);
           obj->mag=view->mag;
           makedisplay(obj);   /* recalculate bitmap for new magnification */
           }
        displayobject(view,obj);
        }
      }
   }
newpointer(Window->WScreen,RESTORE);
update_top(thisobj);         /** get rest of objects **/
}

/*******************************************************************/
/** Update all objects with INV or TOP flags set (objects on top) **/
/** redraws any object on same page as "thisobj" with flags set   **/
/*******************************************************************/

update_top(thisobj)

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
x=page[pg].view->x/view->mag;
y=page[pg].view->y/view->mag;
h=page[pg].view->h/view->mag;
w=page[pg].view->w/view->mag;

newpointer(Window->WScreen,BUSY);
for (node=page[pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
  obj=(struct object *)node;
   if ((obj->flags&TOP) || (obj->flags&INV)) {
      if (overlap(x,y,h,w,obj->x,obj->y,obj->h,obj->w) && (obj!=thisobj)) {
        if (obj->mag != view->mag || obj->screendata == 0) {
           freedata(obj);
           obj->mag=view->mag;
           makedisplay(obj);   /* recalculate bitmap for new magnification */
           }
        displayobject(view,obj);
        }
      }
   }
draw_columns(pg);
newpointer(Window->WScreen,RESTORE);
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
WORD mag,density;
int i;
UBYTE minterm;
int pixwidth, pixheight, planesize;


if (obj->type==RULE) {
   drawrule(view,obj,BLACK);
   return(0);
   }

if (obj->type==BLOCK) {
   density = (WORD)obj->density;
   i = GRAY2;
   if (density > 49)
      i = GRAY1;
   if (density > 98)
      i = BLACK;
   SetDrMd(view->window->RPort, JAM1);
   SetAPen(view->window->RPort, i);
   RectFill(view->window->RPort, 
          ctomousex(view, obj->x),
          ctomousey(view, obj->y),
          ctomousex(view, obj->x+obj->w)-1,
          ctomousey(view, obj->y+obj->h)-1);
   draw_outline(view, obj);
   return(0);
   }

if (obj->flags & SHADOWRIGHT) {
/*    i=obj->width + BOXGAP - 8; */
   i= BOXGAP - 8;
   drawhline(view,obj->x + ((obj->w*15)/100), /* shadow  80 % of box **/
      obj->y + obj->h + i,
      (obj->w*85)/100 + i + SHADOWSIZE,
      SHADOWSIZE,(UWORD)0xFFFF,BLACK);

   drawvline(view,obj->x + obj->w +i,
      obj->y + ((obj->h*15)/100),
      (obj->h*85)/100 + i,
      SHADOWSIZE,0xFFFF,BLACK);
   }

if (obj->flags & SHADOWLEFT) {
/*   i=obj->width + BOXGAP - 8; */
   i= BOXGAP - 8;
   drawhline(view,obj->x - i - SHADOWSIZE,
      obj->y + obj->h + i,
      (obj->w*85)/100,
      SHADOWSIZE,(UWORD)0xFFFF,BLACK);

   drawvline(view,obj->x - i - SHADOWSIZE,
      obj->y + ((obj->h*15)/100),
      (obj->h*85)/100 + SHADOWSIZE,
      SHADOWSIZE,0xFFFF,BLACK);
   }

if (obj->screendata==0) return(0);

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
if (obj->flags&TOP)
   minterm=0xC0;        /** D = B    **/

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
int i;

if (obj->flags & OUTLINEOBJ) {
   i=8;
   if (obj->flags & LINEWIDTH2)
      i = 16;
   else if (obj->flags & LINEWIDTH3)
      i = 24;

   drawthickbox(view,obj->x , obj->y , obj->h ,
         obj->w, i, 0xFFFF, BLACK);
   }
}


/****************************************/
/** clear display  of object's size    **/
/****************************************/
clearobject(view,obj)

struct pageview *view;
struct object *obj;
{
struct BitMap bitmap;
struct RastPort rp;
struct Window *Window;
WORD mag,color;
int i;

if (view->window->Flags&BACKDROP)
   color = GRAY2;
else
   color = WHITE;

if (obj->type==RULE) {
   drawrule(view,obj,color);
   return(0);
   }

#if 0
if (obj->flags & OUTLINEOBJ) {
   i=obj->width + BOXGAP;
   drawthickbox(view,obj->x - i, obj->y - i, obj->h + (i<<1),
         obj->w + (i<<1),obj->width,obj->pattern,color);
   }
#endif

if (obj->flags & SHADOWRIGHT) {
/*   i=obj->width + BOXGAP - 8; */
   i=BOXGAP - 8;
   drawhline(view,obj->x + ((obj->w*15)/100), /* shadow  80 % of box **/
      obj->y + obj->h + i,
      (obj->w*85)/100 + i + SHADOWSIZE,
      SHADOWSIZE,(UWORD)0xFFFF,color);

   drawvline(view,obj->x + obj->w +i,
      obj->y + ((obj->h*15)/100),
      (obj->h*85)/100 + i,
      SHADOWSIZE,0xFFFF,color);
   }

if (obj->flags & SHADOWLEFT) {
/*   i=obj->width + BOXGAP - 8; */
   i = BOXGAP - 8;
   drawhline(view,obj->x - i - SHADOWSIZE,
      obj->y + obj->h + i,
      (obj->w*85)/100,
      SHADOWSIZE,(UWORD)0xFFFF,color);

   drawvline(view,obj->x - i - SHADOWSIZE,
      obj->y + ((obj->h*15)/100),
      (obj->h*85)/100 + SHADOWSIZE,
      SHADOWSIZE,0xFFFF,color);
   }

/* if (obj->screendata==0) return(0); */

Window=view->window;
SetDrMd(Window->RPort, JAM1);
SetAPen(Window->RPort, color);

RectFill(Window->RPort,
         ctomousex(view, obj->x),
         ctomousey(view, obj->y),
         ctomousex(view, obj->x + obj->w)-1,
         ctomousey(view, obj->y + obj->h)-1);
return(0);

#if 0
InitRastPort(&rp);
mag=obj->mag;

bitmap.BytesPerRow=RASSIZE(ctosx(obj->w),1);
bitmap.Rows=ctosy(obj->h);
bitmap.Flags=0;
bitmap.Depth=2;
bitmap.Planes[0]=obj->screendata;
bitmap.Planes[1]=obj->screendata;

rp.BitMap=&bitmap;
ClipBlit(&rp,0,0,Window->RPort,
          XBORDER+ctosx(obj->x*mag) - ctosx(view->x),
          ctosy(obj->y*mag) - ctosy(view->y),
          ctosx(obj->w*mag),ctosy(obj->h*mag),0x00A,0x03,0);
#endif

}

/********************/
/** createobject   **/
/********************/

struct object *createobject(list)

struct List *list;

{
struct object *obj;

obj=(struct object *)AllocMem(sizeof(struct object),MEMF_CLEAR);
if (obj==0) return(0);
obj->node.ln_Name="";
AddHead(list,&obj->node);
obj->dpi=150;
/* obj->width=4; */
/* obj->pattern=0xFFFF; */
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
   text = ((struct text_object *)obj->data)->macro->macrotext->start;
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
   Remove(obj);
   next=((struct text_object *)obj->data)->next;
   free_text_object(obj);   /** free any text_line structures **/
   free_macro_chain(((struct text_object *)obj->data)->macro);
  ((struct text_object *)obj->data)->macro=NULL;
   FreeMem(obj->data,sizeof(struct text_object));  
   freedata(obj);  /** free screendata if needed */
   FreeMem(obj,sizeof(struct object));
   obj=next;
   } while (next!=NULL);
}

