#include "exec/types.h"
#include "desk.h"
#include "text.h"

/** append text chain 'bot' to the end of text chain 'top' **/

connect_chain (obj_top, obj_bot)

struct object *obj_top;
struct object *obj_bot;

{
struct object *op;
char *obj_one;
char *obj_two;
char *obj_big;
int len1, len2;

/** check to see if this is a valid place to connect **/

if (((struct text_object *)(obj_top->data))->next)
   return(CANTCONNECT);

if (obj_bot->first!=obj_bot)
   return(CANTCONNECT);

if (obj_bot==obj_top)
   return(CANTCONNECT);

/** now create one pice of text, the sum of two shorter pieces **/

obj_one = tobj_text(obj_top);
obj_two = tobj_text(obj_bot);
len1 = strlen(obj_one);
len2 = strlen(obj_two)+1;  /** This length includes the trailing zero **/

obj_big = AllocMem(len1+len2,0);
if (obj_big==0)
   return(OUTOFMEM);

movmem(obj_one, obj_big, len1);
movmem(obj_two, obj_big+len1, len2);

/** now free all of bobbys text line & macro info **/

op = obj_top->first;
while (op) {
   free_text_object(op);   /** free any text_line structures **/
   free_macro_chain(((struct text_object *)op->data)->macro);
   ((struct text_object *)op->data)->macro=NULL;
   free_macro_chain(((struct text_object *)op->data)->endmacro);
   ((struct text_object *)op->data)->endmacro=NULL;
   op = ((struct text_object *)op->data)->next;
   }

op = obj_bot->first;
while (op) {
   free_text_object(op);   /** free any text_line structures **/
   free_macro_chain(((struct text_object *)op->data)->macro);
   ((struct text_object *)op->data)->macro=NULL;
   free_macro_chain(((struct text_object *)op->data)->endmacro);
   ((struct text_object *)op->data)->endmacro=NULL;
   op = ((struct text_object *)op->data)->next;
   }

/** tell top object it is now connected to second object **/

((struct text_object *)(obj_top->data))->next=obj_bot;

/** now change new chain's start obj to the correct value **/

op = obj_bot;
while (op) {
   op->first = obj_top->first;
   op = ((struct text_object *)op->data)->next;
   }

/** now reformat the new chain and were done **/

/* printf("conect chain: about to reflow\n'%s'",obj_big); */
reflowchain(obj_top->first, 0, obj_big);

return(AOK);
}

/* flowobj=reflowchain(obj->first, 0, text);
         if (flowobj!=NULL) {
            mode=FLOW;
            } */

