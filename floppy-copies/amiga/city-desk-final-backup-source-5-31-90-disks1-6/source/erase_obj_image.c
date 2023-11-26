#include "exec/types.h"
#include "desk.h"

void erase_obj_image(obj)

struct object *obj;

{
extern struct pageinfo page[MAXPAGES];
short pg;

pg = obj->pn;

clearobject(page[pg].view,obj);
obj->flags |= DO_NOT_DRAW;
refresh_all (obj);
obj->flags &= (~DO_NOT_DRAW);
}

