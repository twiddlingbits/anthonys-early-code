#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/rastport.h"
#include "desk.h"
#include "stdio.h"

drawhline(view,x,y,w,thick,pat,color)

struct pageview *view;
COORD x,y,w,thick;
int color;
UWORD pat;

{
struct object obj;
struct rule r;

obj.data=&r;

obj.x=x;
obj.y=y;
obj.w=w;
obj.mag=view->mag;

r.pattern=pat;
r.width=thick;
r.flags=HORIZ;

drawrule(view,&obj,color);
}


drawvline(view,x,y,h,thick,pat,color)

struct pageview *view;
COORD x,y,h,thick;
int color;
UWORD pat;

{
struct object obj;
struct rule r;

obj.data=&r;

obj.x=x;
obj.y=y;
obj.h=h;
obj.mag=view->mag;

r.pattern=pat;
r.width=thick;
r.flags=VERT;

drawrule(view,&obj,color);
}


drawrule(view,obj,color)

struct pageview *view;
struct object *obj;
int color;

{
struct rule *r;
UWORD *oldpat;
BYTE oldsize;
UWORD p[8];
static UBYTE pow2[]={128,64,32,16,8,4,2,1};
SHORT i;
SHORT x,y,h,w;

r=(struct rule *)obj->data;
SetAPen(view->window->RPort,color);
SetDrMd(view->window->RPort,JAM1);
/* view->window->RPort->Mask=0xFD; */
oldsize=view->window->RPort->AreaPtSz;
oldpat=view->window->RPort->AreaPtrn;

#if 0
if (r->flags==HORIZ) {
   view->window->RPort->AreaPtSz=0;
   view->window->RPort->AreaPtrn=&r->pattern;
   RectFill(view->window->RPort,ctomousex(view,obj->x),ctomousey(view,obj->y),
      max(1,ctomousex(view,obj->x)+ctosx((LCOORD)obj->w*obj->mag)),
      max(1,ctomousey(view,obj->y)+ctosy((LCOORD)r->width*obj->mag)));
   }
#endif

x = ctomousex(view, obj->x);
y = ctomousey(view, obj->y);
w = ctomousex(view, obj->x+obj->w)-1;
h = ctomousey(view, obj->y+r->width)-1;

if (r->flags==HORIZ) {
   view->window->RPort->AreaPtSz=0;
   view->window->RPort->AreaPtrn=&r->pattern;
   RectFill(view->window->RPort,x, y, max(x, w), max(y, h));
   }

else {

   /** flip pattern */

   for (i=0; i<8; i++) {
      if (r->pattern & pow2[i])
         p[i]=0xFFFF;
      else
         p[i]=0;
      }

   w = ctomousex(view, obj->x + r->width)-1;
   h = ctomousey(view, obj->y + obj->h)-1;

   view->window->RPort->AreaPtSz=3;
   view->window->RPort->AreaPtrn=&p[0];
   RectFill(view->window->RPort, x, y, max(x, w), max(y, h));
   }

view->window->RPort->AreaPtSz=oldsize;
view->window->RPort->AreaPtrn=oldpat;
view->window->RPort->Mask=0xFF;
}

#if 0

/***************************************/
/** draw a rule into printer rastport **/
/***************************************/

printrule(rp,obj)

struct RastPort *rp;
struct object *obj;

{
rp->LinePtrn=((struct rule *)(obj->data))->pattern;
Move(rp,ctopx(obj->x),ctopy(obj->y));

if ( ((struct rule *)(obj->data))->flags==HORIZ) {
Draw(rp,ctopx(obj->x + obj->w), ctopy(obj->y));
   }
  
else {
Draw(rp,ctopx(obj->x), ctopy(obj->y + obj->h) );
   }

rp->LinePtrn=0xFFFF;
}
#endif


/*****************************************/
/*** draw a box using width and pattern **/
/*****************************************/

drawthickbox(view,x,y,h,w,width,pat,color)

struct pageview *view;
COORD x,y,h,w,width;
UWORD pat;
int color;

{
struct object obj;
struct rule r;
COORD round_width, val;

obj.x=x;
obj.y=y;
obj.w=w;
obj.h=h;
obj.mag=view->mag;

val = stocy(1)/view->mag;

round_width = (width + val-1) / val;
round_width *= val;

printf("width %d round %d\n",width, round_width);

r.pattern=pat;
r.width = round_width;
obj.data=(APTR)&r;

r.flags=HORIZ;
drawrule(view,&obj,color);

obj.y=y+h-round_width;
drawrule(view,&obj,color);

val = stocx(1)/view->mag;
round_width = (width + val-1) / val;
round_width *= val;

r.flags=VERT;
obj.y=y;
r.width = round_width;
drawrule(view,&obj,color);

obj.x=x+w-round_width;
drawrule(view,&obj,color);
}

