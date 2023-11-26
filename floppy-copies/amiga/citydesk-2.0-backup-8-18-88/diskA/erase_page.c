#include "exec/types.h"
#include "desk.h"

erase_page(pg)

int pg;
{
struct object *obj;
struct Node *node;
extern struct pageinfo page[MAXPAGES];

if ((page[pg].flags&PAGEINUSE)==0)
   return(AOK);

node=page[pg].objects.lh_Head;

while (node->ln_Succ) {
   obj=(struct object *)node;
   if (!deleteobject(obj)) {
      printf("PGM_ERROR03\n");
      node=node->ln_Succ;
      }
   else {
      node=page[pg].objects.lh_Head;
      }
   }
draw_page(pg);
}


