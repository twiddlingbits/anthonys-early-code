#include "exec/types.h"
#include "desk.h"

/** intidefaultpage.c  should change this to better way latter **/

initdefaultpage()
{

extern struct pageinfo defaultpage;

defaultpage.rm=288;   /* 0.75 inch */
defaultpage.lm=288;
defaultpage.tm=288;   /* 0.50 inch */
defaultpage.bm=288;
defaultpage.nc=2;
defaultpage.cg=288;   /* .25 inch */
defaultpage.flags=PORT|PAGEINUSE;
defaultpage.pw=4896;    /* 8.5 inches */
defaultpage.ph=6336;    /* 11 inches  */
}


copypage(s,d)

struct pageinfo *s,*d;
{

d->rm=s->rm;
d->lm=s->lm;
d->tm=s->tm;
d->bm=s->bm;
d->nc=s->nc;
d->cg=s->cg;
d->pw=s->pw;
d->ph=s->ph;
d->flags=s->flags;
}
