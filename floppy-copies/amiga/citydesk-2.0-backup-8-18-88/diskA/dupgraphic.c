#include "exec/types.h"
#include "graphics/gfx.h"
#include "desk.h"

struct object *dupgraphic(page,obj)

struct pageinfo *page;
struct object *obj;
{
struct object *objcopy,*createobject();
struct graphic *graphic,*clone_graphic();

graphic = clone_graphic(obj->data);
if (graphic==0)
   return(0);

objcopy=createobject(&page->objects);

if (objcopy==0) {
   free_graphic_data(graphic);
   FreeMem(graphic,sizeof(struct graphic));
   return(0);
   }
          
objcopy->numplanes=obj->numplanes;
objcopy->pn=obj->pn;
objcopy->type=obj->type;
objcopy->x=obj->x+100/obj->mag;      /* bump copy so you can see it */
objcopy->y=obj->y+100/obj->mag;
objcopy->h=obj->h;
objcopy->w=obj->w;
objcopy->angle=obj->angle;
objcopy->mag=obj->mag;
objcopy->data=(APTR)graphic;
objcopy->dpi=obj->dpi;
objcopy->flags=obj->flags;
objcopy->screendata=NULL;

return(objcopy);
}

/****************************************************/
/** Given a graphic structure, return a copy of it **/
/****************************************************/

struct graphic *clone_graphic (oldgraphic)

struct graphic *oldgraphic;

{
struct graphic *graphic;
short i,j;

graphic=(struct graphic *)AllocMem(sizeof(struct graphic),0);
if (graphic==0) {
   return(0);
    }

movmem(oldgraphic,graphic,sizeof(struct graphic)); /* copy struct */

for (i=0; i<6; i++)
   graphic->planes[i] = 0;

for (i=0; i<graphic->numplanes; i++) {
   graphic->planes[i]=(UBYTE *)
       AllocMem(RASSIZE(graphic->w,graphic->h),0);
   if (graphic->planes[i]==NULL) {
      for (j=0; j<6; j++)
         if (graphic->planes[j])
            FreeMem(graphic->planes[j],RASSIZE(graphic->w,graphic->h));
      FreeMem(graphic,sizeof(struct graphic));
      return(0);
      }
   movmem(oldgraphic->planes[i],   /* copy graphic data */
          graphic->planes[i],
          RASSIZE(graphic->w,graphic->h));
   }
return(graphic);
}

