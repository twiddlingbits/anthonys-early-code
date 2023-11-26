#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct studioBase *studioBase;
void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
studioBase=0;

studioBase=(struct Library *)OpenLibrary("studio.library",0);
if (studioBase==NULL) {
	exit(10);
	}

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   telluser("Use Workbench 1.2 or greater.","",0);
   closelibraries();
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   telluser("Can't open graphics library!","",0);
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

if (studioBase)
	CloseLibrary(studioBase);
}

