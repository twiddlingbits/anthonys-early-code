#include "exec/types.h"
#include "exec/exec.h"
#include "graphics/gfx.h"
#include "desk.h"


freedata(obj)
struct object *obj;
{
if (obj->screendata==NULL) return;
FreeMem(obj->screendata,DATASIZE(obj));
obj->screendata=NULL;
}

