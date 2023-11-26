#include "exec/types.h"
#include "stdio.h"
#include "desk.h"

correct_sliders(pg)

int pg;

{
extern struct pageinfo page[MAXPAGES];
struct pageview *view;
LONG hpot, vpot;
struct PropInfo *pi;
int i;

view = page[pg].view;

view->w=stocx(view->window->Width - XBORDER_AREA);
view->h=stocy(view->window->Height - YBORDER_AREA);


pi=&view->prl;
if (view->w >= (page[pg].pw*view->mag))
   pi->HorizBody=0xFFFF;
else
   pi->HorizBody=0xFFFFL*(LONG)view->w/((LONG)page[pg].pw*(LONG)view->mag);

hpot=((LONG)view->x)<<16;

i = page[pg].pw*view->mag - view->w;
if (i)
   hpot=min(0xFFFF,hpot/i);
else
   hpot = 0;

pi->HorizPot=hpot;

pi=&view->pud;
if (view->h >= (page[pg].ph*view->mag))
   pi->VertBody=0xFFFF;
else
   pi->VertBody=
   0xFFFFL*(LONG)view->h/((LONG)page[pg].ph*(LONG)view->mag);

vpot=((LONG)view->y)<<16;
i = page[pg].ph*view->mag - view->h;
if (i)
   vpot=min(0xFFFF,vpot/i);
else
   vpot = 0;

pi->VertPot=vpot;

RefreshGadgets(&view->ud,view->window,NULL);
}


