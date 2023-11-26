#include "exec/types.h"
#include "desk.h"

/*************************************/
/*** find page number of this window */
/*************************************/

whatpage(thiswindow)

struct Window *thiswindow;

{
extern struct pageinfo page[MAXPAGES];
int i;

for (i=0; i<MAXPAGES; i++)
   if (page[i].view!=0 && page[i].view->window==thiswindow)
      return(i);
return(-1);
}

