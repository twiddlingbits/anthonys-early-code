#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"

/***************************/
/** Create a text object ***/
/***************************/

createtextobj(page)
struct pageinfo *page;
{
extern struct text_state defaultstate;
struct object *obj;
struct text_object *textobj;

textobj=(struct text_object *)
         AllocMem(sizeof(struct text_object),MEMF_CLEAR);

if (textobj==0) {
   return(NULL);
   }

obj=(struct object *)createobject(&page->objects);
if (obj==0) {
   FreeMem(textobj,sizeof(struct text_object));
   return(NULL);
   }

obj->pn=page->pn;
obj->data=(APTR)textobj;
textobj->state=defaultstate;  /* set inital text parameters from user's */
textobj->state.objptr = obj;  /* back pointer to object */
return(obj);                  /* default **/
}
