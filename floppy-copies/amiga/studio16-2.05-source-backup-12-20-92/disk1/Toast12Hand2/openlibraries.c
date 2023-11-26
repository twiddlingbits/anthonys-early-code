#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct Library *GadLibBase=NULL;
struct StudioBase *StudioBase=NULL;
struct ExpansionBase *ExpansionBase=NULL;
struct Library *InterfaceBase=NULL;

void openlibraries()
{
void Fail();

if ((GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0))==NULL)
	exit(10);
if ((StudioBase=(struct Library *)OpenLibrary("studio.library",0))==NULL)
	Fail("Can't open studio Library",1);
if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	Fail("Use Workbench 1.2 or greater.",1);
if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	Fail("Can't open graphics library!","",1);
if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0)
	Fail("Cant open Expansion Lib!",1);
if ((InterfaceBase=(struct Library *)OpenLibrary("Interface.library",0))==NULL)
	{Fail("Cant open Interface Lib!",1);}
}

void closelibraries()
{
if (IntuitionBase)	CloseLibrary(IntuitionBase);
if (GfxBase)		CloseLibrary(GfxBase);
if (GadLibBase)		CloseLibrary(GadLibBase);
if (StudioBase)		CloseLibrary(StudioBase);
if (ExpansionBase)	CloseLibrary(ExpansionBase);
if(InterfaceBase) CloseLibrary(InterfaceBase);
}

