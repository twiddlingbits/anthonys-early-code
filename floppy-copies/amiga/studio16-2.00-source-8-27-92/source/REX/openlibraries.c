#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct Library *GadLibBase=NULL;
struct StudioBase *StudioBase=NULL;
struct Library *InterfaceBase=NULL;

void openlibraries()

{
void closelibraries();

if ((StudioBase=(struct Library *)OpenLibrary("studio.library",0))==NULL)
	{exit(10);}

if ((GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0))==NULL)
	{closelibraries();exit(10);}

if ((InterfaceBase=(struct Library *)OpenLibrary("Interface.library",0))==NULL)
	{telluser("Rexx:","Can not open interface lib",NULL);closelibraries();exit(10);}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{telluser("Rexx:","Can't open graphics library!",0);closelibraries();exit(10);}

}

void closelibraries()
{
if (IntuitionBase) CloseLibrary(IntuitionBase);
if (GfxBase)	CloseLibrary(GfxBase);
if (GadLibBase)	CloseLibrary(GadLibBase);
if (StudioBase)	CloseLibrary(StudioBase);
if(InterfaceBase) CloseLibrary(InterfaceBase);
}

