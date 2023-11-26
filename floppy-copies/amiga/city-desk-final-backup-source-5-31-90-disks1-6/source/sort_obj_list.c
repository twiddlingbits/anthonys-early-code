#include "exec/types.h"
#include "exec/exec.h"
#include "desk.h"

sort_obj_list(page)

struct pageinfo *page;

{
struct Node *node;
struct object *obj;

node=page->objects.lh_Head;
while (node->ln_Succ) {
   obj=(struct object *)node;
   node=node->ln_Succ;
   if (obj->type==GRAPHIC) {     /** move graphic objects to start */
      Remove(obj);
      AddHead(&page->objects, obj);
      }
   }
}

