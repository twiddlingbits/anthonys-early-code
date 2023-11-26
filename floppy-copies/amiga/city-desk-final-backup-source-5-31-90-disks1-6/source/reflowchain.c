#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"
#include "stdio.h"
     
/**********************************************************************/
/*** given an object (which better be a text object) reformat every ***/
/*** object in the the chain after this object.                     ***/
/**********************************************************************/
     
     
struct object *reflowchain(firstobj, prevobj, start_text)
     
struct object *firstobj;   /** obj to start flowing to **/
struct object *prevobj;    /** 0 if first obj else previous object **/
char *start_text;          /** if prevobj==0 this is the text to flow **/
     
{
struct viewmsg msg;
struct text_object *tobj,*oldtobj;
struct object *oldobj,*obj;
extern struct pageinfo page[MAXPAGES];
     
/*** note: format returns TRUE if box overflowed ***/

obj=firstobj;

if (prevobj==0) {
   if (attach_text(obj, start_text)!=AOK) {
      showstat(0,OUTOFMEM);
      return(NULL);
      }
   }
else {
   oldtobj = prevobj->data;
   tobj = obj->data;
   tobj->macro = copy_macro_chain (oldtobj->endmacro);
   tobj->state = oldtobj->endstate;
   }

while(expand_format(obj)) {

   if (page[obj->pn].view!=0) {  /** this object being displayed? **/
      msg.obj=obj;
      msg.page=&page[obj->pn];
      msg.command=DRAWIT;
      viewmonitor(&msg);
      }
     
   oldobj=obj;          /** remember current obj and text obj **/
   oldtobj=obj->data;
   obj=oldtobj->next;   /** get next obj and text obj in list **/
   tobj=obj->data;
   if (obj==NULL) {
      oldobj->flags |= MORETEXT;
      return(oldobj);
      }
   tobj->macro = copy_macro_chain (oldtobj->endmacro);
   tobj->state = oldtobj->endstate;
   }

obj->flags &= (~MORETEXT);       /** don't need flowin' no more **/
if (obj->screendata!=0 && page[obj->pn].view!=0) {  /** this object being displayed? **/
   msg.page=&page[obj->pn];
   msg.obj=obj;
   msg.command=CLEAR;
   viewmonitor(&msg);
   }
freedata(obj);
if (page[obj->pn].view!=0) {  /** this object being displayed? **/
   msg.page=&page[obj->pn];
   msg.obj=obj;
   msg.command=DRAWIT;
   viewmonitor(&msg);
   }
tobj=(struct text_object *)obj->data;
if (tobj->next != NULL) {
   deletetextobjlist(tobj->next);
   tobj->next=NULL;
   }
return(NULL);
}
     
     
/*******************/
/** expand_format **/
/*******************/
     
/*** This routine keeps formatting a box 'till at least one line fits **/
/*** Or intil the box gets "real big" **/
/*** note: format returns TRUE if box overflowed ***/
     
#define INCVAL 10     /** add 7.2 pts each try **/
#define MAXVAL 4   /** 4 inches **/
     
expand_format(obj)
     
struct object *obj;
     
{
extern SHORT texterrors;
extern UWORD xinch, yinch;
SHORT did,again,saveerrors;
struct text_object *tobj;
extern struct pageinfo page[MAXPAGES];
struct viewmsg msg;
SHORT first_pass;
     
saveerrors = texterrors;
     
if (obj->screendata != 0 && page[obj->pn].view!=0) {
   msg.obj=obj;
   msg.page=&page[obj->pn];
   msg.command=CLEAR;         /** clear if currently on screen **/
   viewmonitor(&msg);
   }
     
freedata(obj);
again=TRUE;
first_pass=TRUE;
tobj= (struct text_object *)obj->data;
     
while (again) {
   did=format(obj,FALSE);
   texterrors = FALSE;
   again=FALSE;

/* printf("did %x h %d w %d\n",did,tobj->height,tobj->width); */
     
   if (!did) {   /** quit trying if we have filled box! **/
      if (!first_pass) {    /** If box was forced to expanded, shrink **/
         obj->h=max(8,tobj->height);
         obj->w=max(8,tobj->width);
         }
      break;
      }

   first_pass = FALSE;

   if (tobj->height == 0)  {
      if (obj->h < (yinch*MAXVAL)) {
         obj->h += (yinch/INCVAL);
         again = TRUE;
         }
      }
     
   if (tobj->width == 0) {
      if (obj->w < (xinch*MAXVAL)) {
         obj->w += (xinch/INCVAL);
         again = TRUE;
         }
      }
   }
texterrors = saveerrors;
return((int)did);
}
     

