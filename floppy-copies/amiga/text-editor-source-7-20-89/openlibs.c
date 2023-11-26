#include <exec/types.h>
#include <graphics/gfxbase.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>

extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;

openlibs()
{

IntuitionBase = (struct IntuitionBase *) OpenLibrary(
                "intuition.library",0);
if (IntuitionBase == NULL) {
   printf("intuition won't open!\n");
   exit(10);
   }

GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",0);
if (GfxBase == NULL)  {
   printf("graph.lib. won't open!\n");
   exit(10);
   }


}


