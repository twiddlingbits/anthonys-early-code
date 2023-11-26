#define NULL 0

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *DiskfontBase;
struct LayersBase *LayersBase;

openlibraries()

{
IntuitionBase = 0;
GfxBase = 0;
DiskfontBase;
LayersBase = 0;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   printf("Can't open Intuition!\n");
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!\n");
   closelibraries();
   exit(10);
   }

/** open font library **/

DiskfontBase = (struct Library *)OpenLibrary("diskfont.library", 0);
if (DiskfontBase==NULL) {
   printf("Can't open diskfonts library!\n");
   closelibraries();
   exit(10);
   }

/** Open Layers library for InstallClipRegion **/

LayersBase = OpenLibrary("layers.library",0);
if (LayersBase==0) {
   printf("can't open layers library\n");
   closelibraries();
   exit(10);
   }
}

closelibraries()
{
if (DiskfontBase)
   CloseLibrary(DiskfontBase);

if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);

if (LayersBase)
   CloseLibrary(LayersBase);
}

