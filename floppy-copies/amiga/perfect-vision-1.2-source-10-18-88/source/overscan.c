#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "vid.h"

extern struct Screen *menu_screen;

static short pref_x,pref_y;

static struct NewScreen os32 = {
   0, 0, 352, 240,      /* size */     /** overscan **/
   5,                   /* depth */
   0, 0,                /* pens */
   0,                 /* mode */
   SCREENQUIET, 0, 0, 0, 0
};


left_align()
{
struct Preferences p;

GetPrefs(&p,sizeof(p));

/* printf("(%d) %d %d %d\n",p.ViewXOffset, p.ViewYOffset, p.ViewInitX,
      p.ViewInitY);*/

pref_x = p.ViewXOffset;
pref_y = p.ViewYOffset;

p.ViewXOffset = -16;
p.ViewYOffset = -20;

SetPrefs(&p,sizeof(p),FALSE);
}

normal_align()
{
struct Preferences p;

GetPrefs(&p,sizeof(p));

p.ViewXOffset = pref_x;
p.ViewYOffset = pref_y;

SetPrefs(&p,sizeof(p),FALSE);
}

do_save_overscan(screen)
struct Screen *screen;
{
extern short lace;
struct Screen *os_screen;
short nlines,i,num_colors;
short dheight, *cmap;
struct BitMap *dbm,*sbm;

nlines = screen_height();
os32.ViewModes = 0;
sbm = screen->RastPort.BitMap;

if (lace)
   os32.Height = nlines+80;
else
   os32.Height = nlines+40;

if (nlines==400)
   os32.ViewModes |= LACE;

if (sbm->Depth == 6)
   os32.ViewModes |= HAM;

os32.Depth = sbm->Depth;
dheight = os32.Height;

os_screen = (struct Screen *)OpenScreen(&os32);
if (!os_screen) {
   show_error(OUTOFMEM);
   return(0);
   }

newpointer(menu_screen,BUSY);
left_align();
show_overscan_menu();
ScreenToFront(menu_screen);

cmap = screen->ViewPort.ColorMap->ColorTable;
num_colors = screen->ViewPort.ColorMap->Count;
LoadRGB4(&os_screen->ViewPort, cmap, num_colors);

dbm = os_screen->RastPort.BitMap;

for (i=0; i < sbm->Depth; i++) {   /** size all planes **/

fit(sbm->Planes[i],0,nlines,320,40, /* end src parms, start dest */
    dbm->Planes[i],0,dheight,352,RASSIZE(352,1));

   }

newpointer(menu_screen,NORMAL);

while (TRUE) {
   switch(wait_menu()) {
      case 0:
         break;
      case 1:
         CloseScreen(os_screen);
         normal_align();
         return(0);
      case 2:
         save_iff(os_screen);   /** Save IFF **/
         break;
      }
   }
}


show_overscan_menu()
{
char *t[5];

t[0] = "Exit";
t[1] = "Save IFF Overscan";
t[2] = 0;

create_menu(t);
}
