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
studioBase=0;

studioBase=(struct Library *)OpenLibrary("studio.library",0);
if (studioBase==NULL) {
	exit(10);
	}
}

void closelibraries()
{
if (studioBase)
	CloseLibrary(studioBase);
}

