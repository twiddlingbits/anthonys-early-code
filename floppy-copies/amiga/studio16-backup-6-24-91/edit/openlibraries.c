#include "exec/types.h"
#include "exec/exec.h"
#include <fcntl.h>

/** do not use -b0 when compiling this module **/

struct GfxBase *GfxBase;
struct ArpBase *ArpBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;
struct Library *GadLibBase;
struct studioBase *studioBase;
void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
ArpBase = 0;
GadLibBase=0;
studioBase=0;

studioBase=(struct Library *)OpenLibrary("studio.library",0);
if (studioBase==NULL) {
	exit(10);
	}

/** open gadget library **/

GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0);
if (GadLibBase==NULL) {
   closelibraries();
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

/** Open arpbase for the file requester **/

ArpBase = (struct ArpBase *)OpenLibrary("arp.library",0);
if (ArpBase==0) {
   telluser("Can't open ARP library!","",0);
   closelibraries();
   exit(10);
   }

LayersBase = (struct Library *)OpenLibrary("layers.library",0);
if (LayersBase==0) {
   telluser("Can't open layers library!","",0);
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

if (ArpBase)
   CloseLibrary(ArpBase);

if (LayersBase)
   CloseLibrary(LayersBase);

if (GadLibBase)
	CloseLibrary(GadLibBase);

if (studioBase)
	CloseLibrary(studioBase);
}

