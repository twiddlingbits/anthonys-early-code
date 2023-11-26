#include "exec/types.h"
#include "desk.h"

/** intidefaultpage.c  should change this to better way latter **/

initdefaultpage()
{

extern struct pageinfo defaultpage;
extern UWORD xinch, yinch;

defaultpage.rm=xinch/2;
defaultpage.lm=xinch/2;
defaultpage.tm=yinch/2;   /* 0.50 inch */
defaultpage.bm=yinch/2;
defaultpage.nc=2;
defaultpage.cg=xinch/2;
defaultpage.flags=PORT|PAGEINUSE;
defaultpage.pw=xinch*8+xinch/2;    /* 8.5 inches */
defaultpage.ph=yinch*11;    /* 11 inches  */
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
