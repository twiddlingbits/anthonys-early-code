#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct Library *GadLibBase=NULL;
struct studioBase *studioBase=NULL;
struct ExpansionBase *ExpansionBase=NULL;

void openlibraries()

{
void closelibraries();

if ((studioBase=(struct Library *)OpenLibrary("studio.library",0))==NULL)
	{exit(10);}

if ((GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0))==NULL)
	{closelibraries();exit(10);}

if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{telluser("Use Workbench 1.2 or greater.","",0);closelibraries();exit(10);}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{telluser("Can't open graphics library!","",0);closelibraries();exit(10);}

}

void closelibraries()
{
if (IntuitionBase) CloseLibrary(IntuitionBase);

if (GfxBase)	CloseLibrary(GfxBase);

if (GadLibBase)	CloseLibrary(GadLibBase);

if (studioBase)	CloseLibrary(studioBase);
}

