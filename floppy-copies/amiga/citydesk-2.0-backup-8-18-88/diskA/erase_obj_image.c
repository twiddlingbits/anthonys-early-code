#include "exec/types.h"
#include "desk.h"

void erase_obj_image(obj)

struct object *obj;

{
extern struct pageinfo page[MAXPAGES];
short pg;

pg = obj->pn;

clearobject(page[pg].view,obj);
update(page[pg].view,&page[pg].objects,obj);
}

