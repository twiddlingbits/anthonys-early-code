
#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"
     
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
struct macro_text *mt;
extern struct pageinfo page[MAXPAGES];
     
/*** note: format returns TRUE if box overflowed ***/

obj=firstobj;

if (prevobj==0) {
   mt = AllocMem(sizeof(struct macro_text), MEMF_CLEAR);
   if (mt==0) {
      printf("REFLOWCHAIN:  OUT of Memory!!\n");  /**!!!!!! **/
      return(NULL);
      }

   mt->start = start_text;
   mt->end = start_text + strlen(start_text)+1;
   mt->flags = MTEXT_FREEMTEXT|MTEXT_FREETEXT;  /** causes the macro text structure to */
                                 /** go away when ref = 0 */
   ((struct text_object *)(obj->data))->macro = (struct macro *)start_macro(0, mt, 0);
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
     
#define INCVAL 64     /** add 8 pts each try **/
#define MAXVAL 2880   /** 5 inches **/
     
expand_format(obj)
     
struct object *obj;
     
{
extern SHORT texterrors;
SHORT did,again,saveerrors;
struct text_object *tobj;
extern struct pageinfo page[MAXPAGES];
struct viewmsg msg;
     
saveerrors = texterrors;
     
if (obj->screendata != 0 && page[obj->pn].view!=0) {
   msg.obj=obj;
   msg.page=&page[obj->pn];
   msg.command=CLEAR;         /** clear if currently on screen **/
   viewmonitor(&msg);
   }
     
freedata(obj);
again=TRUE;
tobj= (struct text_object *)obj->data;
     
while (again) {
   did=format(obj,FALSE);
   again=FALSE;
   texterrors = FALSE;
     
   if (tobj->height == 0)  {
      if (obj->h < MAXVAL) {
         obj->h += INCVAL;
         again = TRUE;
         }
      }
     
   if (tobj->width == 0) {
      if (obj->w < MAXVAL) {
         obj->w += INCVAL;
         again = TRUE;
         }
      }
     
   if (!did) {   /** quit trying if we have filled box! **/
      break;
      }
   }
texterrors = saveerrors;
return((int)did);
}
     

