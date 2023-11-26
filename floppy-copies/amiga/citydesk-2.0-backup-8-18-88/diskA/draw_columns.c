#include "exec/types.h"
#include "desk.h"

void draw_columns(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];
extern SHORT displayguides;
struct Window *Window;
COORD columnwidth,x;
short boxheight, tm_offset;
short pw,ph;
short mag,i;

if (pg==CB || !displayguides)
   return;

Window = page[pg].view->window;
mag = page[pg].view->mag;
pw=ctosx(page[pg].pw);
ph=ctosy(page[pg].ph);

/*** draw margins and column markers ***/

columnwidth=(page[pg].pw-page[pg].rm-page[pg].lm-(page[pg].nc-1)*
              page[pg].cg)/page[pg].nc;

SetDrMd(Window->RPort,JAM1);
SetAPen(Window->RPort,GRAY1);

x=page[pg].lm;
boxheight=ctosy(mag*(page[pg].ph - page[pg].bm - page[pg].tm));
tm_offset=ctomousey(page[pg].view, page[pg].tm);
for (i=0;i<page[pg].nc;i++)  {
   drawbox(Window->RPort,ctomousex(page[pg].view, x),
           tm_offset, boxheight, ctosx(columnwidth*mag));
   x = x + columnwidth + page[pg].cg;
   }
}


