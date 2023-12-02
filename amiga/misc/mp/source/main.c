#include "exec/types.h"
#include "intuition/intuitionbase.h"
#include "mp.h"

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

main()

{
struct anim_pointer ap;
/* extern struct IntuitionBase *IntuitionBase; */
int i;

IntuitionBase = OpenLibrary("intuition.library",0);
if (IntuitionBase==0) {
   printf("can't open intuiton\n");
   exit(0);
   }

GfxBase = OpenLibrary("graphics.library",0);
if (GfxBase==0) {
   CloseLibrary(IntuitionBase);
   printf("can't open graphics library\n");
   exit (0);
   }
printf("%d\n",load_cell("cell0",&ap,0));
printf("%d\n",load_cell("cell1",&ap,1));
printf("%d\n",load_cell("cell2",&ap,2));
printf("%d\n",load_cell("cell3",&ap,3));

ap.cell_tick[0] = 10;
ap.cell_tick[1] = 8;
ap.cell_tick[2] = 6;
ap.cell_tick[3] = 6;

ap.cell_order[0] = 0;
ap.cell_order[1] = 1;
ap.cell_order[2] = 2;
ap.cell_order[3] = 3;
ap.cell_order[4] = 200;

cycle_pointer(&ap);
CloseLibrary(GfxBase);
CloseLibrary(IntuitionBase);
}

cycle_pointer(ap)

struct anim_pointer *ap;

{
int i,x;
/* extern struct IntuitionBase *IntuitionBase; */
struct Window *w;

w = IntuitionBase->ActiveWindow;

for (i=0; i < 4; i++) {
   SetRGB4(&w->WScreen->ViewPort, i+16,ap->cmap[i*3],ap->cmap[i*3+1],ap->cmap[i*3+2]);
   }

for (x=0; x<100; x++) {
i=0;
while (ap->cell_order[i] < 99) {
   SetPointer(w,ap->cell_data[ap->cell_order[i]],ap->h,ap->w, 0, 0);
   Delay(ap->cell_tick[i]);
   i++;
   }
}
}


