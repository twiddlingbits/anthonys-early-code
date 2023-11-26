#include "exec/types.h"
#include "desk.h"

struct object *dup_block(obj)

struct object *obj;
{
extern struct pageinfo page[MAXPAGES];
struct object *objcopy,*createobject();
short mag;

mag = page[obj->pn].view->mag;
objcopy=createobject(page[obj->pn].objects);

if (objcopy==0) {
   return(0);
   }
          
objcopy->numplanes=obj->numplanes;
objcopy->pn=obj->pn;
objcopy->type=obj->type;
objcopy->x=obj->x+100/mag;      /* bump copy so you can see it */
objcopy->y=obj->y+100/mag;
objcopy->h=obj->h;
objcopy->w=obj->w;
objcopy->angle=obj->angle;
objcopy->mag=obj->mag;
objcopy->data=0;
objcopy->dpi=obj->dpi;
objcopy->flags=obj->flags;
objcopy->screendata=NULL;
objcopy->density = obj->density;

return(objcopy);
}

