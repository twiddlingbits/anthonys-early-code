#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"


outlineobjects(page)

struct pageinfo *page;

{
struct Node *node;
struct object *obj;
struct RastPort *rp;

rp=page->view->window->RPort;

SetDrMd(rp,COMPLEMENT);

for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
   obj=(struct object *)node;
   drawbox(rp,ctomousex(page->view,obj->x),ctomousey(page->view,obj->y),
      ctosy(obj->h*obj->mag),ctosx(obj->w*obj->mag));
   }
}


