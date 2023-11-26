/**** removepage.c     Delete's an entire page and moves everything
                       down to fill its place. **/

#include "exec/types.h"
#include "desk.h"


removepage(pg)

int pg;

{
int i;
struct object *obj;
struct Node *node;
extern struct pageinfo page[MAXPAGES];

if (pg==CB || pg > LASTPAGE)
   return(CANCEL);

erase_page(pg);
for (i=pg; i< LASTPAGE; i++) {
   copypage(&page[i+1],&page[i]);
   node=page[i+1].objects.lh_Head;
   while (node->ln_Succ) {
      obj=(struct object *)node;
      Remove(obj);
      AddHead(&page[i].objects,obj);
      obj->pn = i;
      node=page[i+1].objects.lh_Head;
      }
   }

erase_page(LASTPAGE);
return(AOK);
}

