#include "exec/types.h"
#include "desk.h"

int count_pages()

{
short i,total;
extern struct pageinfo page[MAXPAGES];

total = 0;
for (i=1; i <= LASTPAGE; i++) {
   if (page[i].flags&PAGEINUSE) {
      total++;
      }
   }
return((int)total);
}

int last_page()

{
short i;
extern struct pageinfo page[MAXPAGES];

for (i=LASTPAGE; i > 0; i--) {
   if (page[i].flags&PAGEINUSE) {
      return((int)i);
      }
   }
}

