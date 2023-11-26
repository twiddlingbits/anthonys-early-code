#include "exec/types.h"
#include "exec/exec.h"
#include "desk.h"
#include "text.h"

attach_text(obj,text)

char *text;
struct object *obj;

{
struct macro_text *mt;
struct text_object *tobj;

if (text==0)
   return(OUTOFMEM);

mt = AllocMem(sizeof(struct macro_text), MEMF_CLEAR);
if (mt==0) {
   return(OUTOFMEM);
   }

mt->start = text;
mt->end = text + strlen(text)+1;
mt->flags = MTEXT_FREEMTEXT|MTEXT_FREETEXT;  /** auto free on **/
tobj = (struct text_object *)obj->data;
tobj->macro = (struct macro *)start_macro(0, mt, 0);
return(AOK);
}

