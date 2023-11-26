#include "exec/types.h"
#include "exec/exec.h"

struct GfxBase *GfxBase;
struct ArpBase *ArpBase;
struct IntuitionBase *IntuitionBase;

void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
ArpBase = 0;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   telluser("Use Workbench 1.2 or greater.","");
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   telluser("Can't open graphics library!","");
   closelibraries();
   exit(10);
   }

/** Open arpbase for the file requester **/

ArpBase = (struct ArpBase *)OpenLibrary("arp.library",0);
if (ArpBase==0) {
   telluser("Can't open ARP library!","");
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
}

