#include "exec/types.h"
#include "desk.h"


add_link(obj)

struct object *obj;

{
extern struct object *last_picked;
struct object *end_of_link();

if (verifypick(obj)!=AOK)
   return(CANCEL);

if (last_picked==0 && obj->top_link==0) {
   last_picked = obj;
   obj->top_link = obj;
   return(AOK);
   }

if (last_picked==0) {
   last_picked = end_of_link(obj);
   return(AOK);
   }

if (obj->top_link) {
   return(CANTLINK);
   }

if (obj->pn != last_picked->pn)
   return(CANTLINK);

last_picked->next = obj;
obj->top_link = last_picked->top_link;
last_picked = obj;
return(AOK);
}

struct object *end_of_link(obj)

struct object *obj;

{
while (obj->next)
   obj = obj->next;
return(obj);
}

void remove_link(obj)

struct object *obj;

{
struct object *p, *new_top;

if (obj->top_link==0)
   return;

if (obj == obj->top_link) {
   p = obj->next;
   new_top = p;
   obj->top_link = 0;
   obj->next = 0;
   while (p) {
      p->top_link = new_top;
      p = p->next;
      }
   return;
   }

p = obj->top_link;

while(p->next != obj)
   p = p->next;

p->next = obj->next;
obj->next = 0;
obj->top_link = 0;
}


unlink_all(obj)

struct object *obj;

{
struct object *p,*newp;

if (verifypick(obj)!=AOK)
   return(CANCEL);

p = obj->top_link;
if (!p)
   return;

do {
   p->top_link = 0;
   newp = p->next;
   p->next = 0;
   p = newp;
   } while (p);
}

