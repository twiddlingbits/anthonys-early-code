/********************/
/*** move_page.c  ***/
/********************/

#include "exec/types.h"
#include "desk.h"


/***  copies a source page to a destination page. Pages cannot
      have text chains that flow off the page.  ***/

move_page(spg,dpg)

int spg,dpg;

{
struct object *obj;
struct Node *node;
int error=AOK;
struct pageinfo *s,*d;
extern struct pageinfo page[MAXPAGES];

s = &page[spg];
d = &page[dpg];
node=s->objects.lh_Head;

while (node->ln_Succ) {
   obj=(struct object *)node;
   if ((error=copyobject(s,d,obj))!=AOK)
      break;
   node=node->ln_Succ;
   }

d->rm=s->rm;
d->lm=s->lm;
d->tm=s->tm;
d->bm=s->bm;
d->nc=s->nc;
d->cg=s->cg;
d->pw=s->pw;
d->ph=s->ph;
d->flags=s->flags;
return (error);
}
