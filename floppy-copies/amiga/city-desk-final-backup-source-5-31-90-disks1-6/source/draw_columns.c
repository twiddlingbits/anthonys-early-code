#include "exec/types.h"
#include "desk.h"

void draw_columns(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];
extern SHORT displayguides;
extern COORD xoutlinespc;
extern COORD youtlinespc;
struct Window *Window;
COORD columnwidth,x,y;
short boxheight, tm_offset;
short pw,ph;
short mag,i,j;

if (pg==CB)
   return;

Window = page[pg].view->window;
mag = page[pg].view->mag;
pw=ctosx(page[pg].pw);
ph=ctosy(page[pg].ph);

SetAPen(Window->RPort,BLACK);
columnwidth=colwid(pg);

if ((page[pg].flags & DRAWRULES) && (page[pg].nc > 1)) {
   j=page[pg].lm+columnwidth-(page[pg].cg>>1);
   for (i=1; i < page[pg].nc; i++) {
      x=ctomousex(page[pg].view,j);  /** COORD -> screen pixel **/
      Move(Window->RPort,x,ctomousey(page[pg].view,page[pg].tm));
      Draw(Window->RPort,x,ctomousey(page[pg].view,page[pg].ph - page[pg].bm));
      j=j+columnwidth;
      }
   }

if (page[pg].flags & OUTLINEPAGE) {
   x=ctomousex(page[pg].view,page[pg].lm-xoutlinespc);
   y=ctomousey(page[pg].view,page[pg].tm-youtlinespc);
   ph=ctosy(page[pg].ph-page[pg].tm-page[pg].bm+(youtlinespc<<1))*mag;
   pw=ctosx(page[pg].pw-page[pg].lm-page[pg].rm+(xoutlinespc<<1))*mag;
/* printf("x %d y %d h %d w %d\n",x,y,ph,pw); */
   drawbox(Window->RPort,x,y,ph+1,pw+1);
   }

if (!displayguides)
   return;

/*** draw margins and column markers ***/

columnwidth=colwid(pg)-page[pg].cg;

SetDrMd(Window->RPort,JAM1);
SetAPen(Window->RPort,GRAY1);

x=page[pg].lm;
boxheight=ctosy(mag*(page[pg].ph - page[pg].bm - page[pg].tm))+1;
tm_offset=ctomousey(page[pg].view, page[pg].tm);
for (i=0;i<page[pg].nc;i++)  {
   drawbox(Window->RPort,ctomousex(page[pg].view, x),
           tm_offset, boxheight, ctosx(columnwidth*mag)+1);
   x = x + columnwidth + page[pg].cg;
   }
}

