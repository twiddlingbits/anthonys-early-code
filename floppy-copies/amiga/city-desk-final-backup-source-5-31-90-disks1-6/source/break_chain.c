#include "exec/types.h"
#include "desk.h"
#include "text.h"

break_chain (obj)

struct object *obj;

{
struct object *op;
struct object *prevobj();
struct object *first_obj;
char *obj_one;
char *obj_two;
int len1, len2;

/** make sure this is a valid place to brake a text chain **/

if (((struct text_object *)obj->data)->macro->parent)
   return(CANTBREAK);

if (obj->first==obj)
   return(CANTBREAK);

/** now create two pieces of text out of the old longer one **/

len1 = ((struct text_object *)obj->data)->macro->current - tobj_text(obj->first)+1;
obj_one = AllocMem(len1, 0);
if (obj_one==0)
   return(OUTOFMEM);
movmem(tobj_text(obj->first), obj_one, len1);
obj_one[len1-1] = 0;

len2 = strlen(tobj_text(obj->first)) - len1 + 2;
obj_two = AllocMem(len2, 0);
if (obj_two==0) {
   FreeMem(obj_one,len1);
   return(OUTOFMEM);
   }
movmem(tobj_text(obj), obj_two, len2);
movmem( ((struct text_object *)obj->data)->macro->current, obj_two, len2);

/** now free all of bobbys text line & macro info **/

op = obj->first;
while (op) {
   free_text_object(op);   /** free any text_line structures **/
   free_macro_chain(((struct text_object *)op->data)->macro);
   ((struct text_object *)op->data)->macro=NULL;
   free_macro_chain(((struct text_object *)op->data)->endmacro);
   ((struct text_object *)op->data)->endmacro=NULL;
   op = ((struct text_object *)op->data)->next;
   }

/** tell previous object its the first last in that chain now **/

((struct text_object *)((prevobj(obj))->data))->next=NULL;

/** now change new chain's start obj to the correct value **/

first_obj = obj->first;
op = obj;
while (op) {
   op->first = obj;
   op = ((struct text_object *)op->data)->next;
   }

/** now reformat the two new chains and were done **/

reflowchain(first_obj, 0, obj_one);
reflowchain(obj, 0, obj_two);

return(AOK);
}

/* flowobj=reflowchain(obj->first, 0, text);
         if (flowobj!=NULL) {
            mode=FLOW;
            } */

