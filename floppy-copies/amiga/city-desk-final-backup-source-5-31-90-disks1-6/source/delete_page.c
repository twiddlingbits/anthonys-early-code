#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

/******************************************/
/*** Delete all objects on page and view **/
/******************************************/

delete_page(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];

deleteview(&page[pg]);
erase_page(pg);
page[pg].flags = 0;
}

