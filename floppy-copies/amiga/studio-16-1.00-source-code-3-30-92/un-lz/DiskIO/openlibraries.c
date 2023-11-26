#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct StudioBase *StudioBase;
struct Library *GadLibBase;


void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
StudioBase=0;

StudioBase=(struct Library *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	exit(10);
	}

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   closelibraries();
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   closelibraries();
   exit(10);
   }


GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0);
if (GadLibBase==NULL) {
   closelibraries();
   exit(10);
   }
}

void closelibraries()
{
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);

if (StudioBase)
	CloseLibrary(StudioBase);

if (GadLibBase)
	CloseLibrary(GadLibBase);
}

