#include "exec/types.h"
#include "desk.h"

cut_page(pg)

int pg;

{
short i;
extern struct pageinfo page[MAXPAGES];

erase_page(CUTPAGE);
move_page(pg,CUTPAGE);
removepage(pg);
for (i=pg; i<LASTPAGE; i++) {
   if (page[i].view) {
      check_page(i);
      draw_page(i);
      }
   }
}

