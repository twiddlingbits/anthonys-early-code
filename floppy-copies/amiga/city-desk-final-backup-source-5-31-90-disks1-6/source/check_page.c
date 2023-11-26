#include "exec/types.h"
#include "desk.h"

void check_page(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];

if (page[pg].flags&PAGEINUSE)
   return;

if (pg&1)
   move_page(DEFAULTODD,pg);
else
   move_page(DEFAULTEVEN,pg);
}

