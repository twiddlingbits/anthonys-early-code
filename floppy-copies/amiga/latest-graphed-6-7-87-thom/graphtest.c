/*      this creates a false main for graphed      */
#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <desk.h>

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

main () {

   static SHORT testimage[] =
   {
   0x00FF, 0xFF00,
   0x00F8, 0x1F00,
   0x00FF, 0xFF00,
   0x00F0, 0x0F00,
   0x00F5, 0x5F00,
   0x00FF, 0xFF00,
   };

   static struct graphic gra1 = {
   &testimage[0],
   6,32,
   0,0,
   6,32,
   0,
   1,
   };

   IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",0);
   GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0);
   if (IntuitionBase == NULL)
      printf("intbase kaput!");

   gra1.image = AllocMem(24,MEMF_CHIP);
   movmem(&testimage[0],gra1.image,24);


   graphic_ed(&gra1);

   FreeMem(gra1.image,RASSIZE(gra1.w,gra1.h));


}

