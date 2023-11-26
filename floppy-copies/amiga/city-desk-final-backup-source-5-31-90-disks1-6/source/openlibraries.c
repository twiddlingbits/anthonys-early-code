#include "exec/types.h"
#include "exec/execbase.h"
#include "virtualpage.h"


#define NULL 0
#define FALSE 0

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
struct Library *DiskfontBase;
struct LayersBase *LayersBase;
struct ArpBase *ArpBase;
struct VPLib *VPLibBase;

openlibraries()

{
struct ExecBase **eb;

IntuitionBase = 0;
GfxBase = 0;
DiskfontBase=0;
LayersBase = 0;
ArpBase = 0;
eb = 4;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   exit(10);
   }

/* telluser2(0,"This is an NTSC BETA version of City Desk 2.0",
"NOT FOR RESALE. Release 04/10/89",FALSE); */

/*
if ((*eb)->PowerSupplyFrequency != 60) {
   telluser2(0,"This version of City Desk is",
                "for North America only.",FALSE);
   exit(10);
   }  */

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   telluser(0,"Can't open graphics library!","",FALSE);
   closelibraries();
   exit(10);
   }

/** open font library **/

DiskfontBase = (struct Library *)OpenLibrary("diskfont.library", 0);
if (DiskfontBase==NULL) {
   telluser(0,"Can't open diskfonts library!","",FALSE);
   closelibraries();
   exit(10);
   }

/** Open Layers library for InstallClipRegion **/

LayersBase = OpenLibrary("layers.library",0);
if (LayersBase==0) {
   telluser(0,"Can't open layers library!","",FALSE);
   closelibraries();
   exit(10);
   }

/** Open arpbase for the file requester **/

ArpBase = OpenLibrary("arp.library",0);
if (ArpBase==0) {
   telluser(0,"Can't open ARP library!","",FALSE);
   closelibraries();
   exit(10);
   }

VPLibBase = OpenLibrary(VPLIBNAME, VPLIBVERSION);
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

if (ArpBase)
   CloseLibrary(ArpBase);

if (VPLibBase)
   CloseLibrary(VPLibBase);
}

