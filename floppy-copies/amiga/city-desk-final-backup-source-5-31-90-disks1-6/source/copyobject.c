
#include "exec/types.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "text.h"

copyobject(s,d,obj)

struct pageinfo *s,*d;
struct object *obj;
{
struct object *objcopy, *firstobj, *createobject();
struct graphic *graphic;
struct rule *rule;
struct text_object *tobj,*prevtobj;
short first_pass;
int i;

if (obj->type==GRAPHIC) {
   graphic=clone_graphic(obj->data);
   if (graphic==0) {
      return(OUTOFMEM);
      }

   objcopy=createobject(&d->objects);

   if (objcopy==0) {
      free_graphic_data(graphic);
      FreeMem(graphic,sizeof(struct graphic));
      return(OUTOFMEM);
      }

   objcopy->data=(APTR)graphic;
   }

else if (obj->type == RULE) {

   rule=(struct rule *)AllocMem(sizeof(struct rule),0);
   if (rule==0)
      return(OUTOFMEM);

   objcopy=createobject(&d->objects);
   if (objcopy==0) {
      FreeMem(rule,sizeof(struct rule),0);
      return(OUTOFMEM);
      }
   movmem(obj->data, rule, sizeof(struct rule)); /* copy struct */
   objcopy->data=(APTR)rule;
   }

else if (obj->type == BLOCK) {

   objcopy=createobject(&d->objects);
   if (objcopy==0) {
      return(OUTOFMEM);
      }
   }

else if (obj->type == TEXT && obj == obj->first) {

   first_pass = TRUE;

   while (obj) {
      objcopy = createtextobj(d);
      tobj = objcopy->data;
      if (first_pass) {
         i = attach_text(objcopy, newstr(tobj_text(obj)));
         if (i!=AOK) return(i);
         firstobj    = objcopy;
         tobj->state = ((struct text_object *)obj->data)->state;
         tobj->state.objptr = objcopy;
         prevtobj    = tobj;
         first_pass  = FALSE;
         }
      else {
         tobj->macro = copy_macro_chain (prevtobj->endmacro);
         tobj->state=prevtobj->endstate;
         tobj->state.objptr = objcopy;
         prevtobj->next=objcopy;
         prevtobj = tobj;
         }
       
      objcopy->first=firstobj;  /** set pointer to 1st obj in chain **/
      objcopy->numplanes=obj->numplanes;
      objcopy->pn=d->pn;
      objcopy->type=obj->type;
      objcopy->x=obj->x;      /* bump copy so you can see it */
      objcopy->y=obj->y;
      objcopy->h=obj->h;
      objcopy->w=obj->w;
      objcopy->angle=obj->angle;
      objcopy->mag=obj->mag;
      objcopy->dpi=obj->dpi;
      objcopy->flags=obj->flags;
      objcopy->screendata=NULL;
      objcopy->density = obj->density;

      format(objcopy,FALSE);

      obj=((struct text_object *)obj->data)->next;
      }
   return(AOK);
   }

else {
   return(AOK);
   }

   objcopy->numplanes=obj->numplanes;
   objcopy->pn=d->pn;
   objcopy->type=obj->type;
   objcopy->x=obj->x;      /* bump copy so you can see it */
   objcopy->y=obj->y;
   objcopy->h=obj->h;
   objcopy->w=obj->w;
   objcopy->angle=obj->angle;
   objcopy->mag=obj->mag;
   objcopy->dpi=obj->dpi;
   objcopy->flags=obj->flags;
   objcopy->screendata=NULL;
   objcopy->density = obj->density;

return(AOK);
}

