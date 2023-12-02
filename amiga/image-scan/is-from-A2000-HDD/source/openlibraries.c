#include "exec/types.h"
#include "exec/exec.h"
#include "scan.h"

struct GfxBase *GfxBase;
struct ArpBase *ArpBase;
struct IntuitionBase *IntuitionBase;

extern struct config config;

void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
ArpBase = 0;

/** Is there enough memory for interlaced ? **/


if (AvailMem(MEMF_FAST) < 100000)
   config.flags |= IMG_NOLACE;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   telluser(0,"Can't open graphics library!","",FALSE);
   closelibraries();
   exit(10);
   }

/** Open arpbase for the file requester **/

ArpBase = (struct ArpBase *)OpenLibrary("arp.library",0);
if (ArpBase==0) {
   telluser(0,"Can't open ARP library!","",FALSE);
   closelibraries();
   exit(10);
   }

if (config.flags & IMG_NOLACE)
   config.raw_data_size = RAW_DATA_SIZE/2;
else
   config.raw_data_size = RAW_DATA_SIZE;

config.rawdata = (char *)AllocMem(config.raw_data_size,MEMF_CLEAR);

if (config.rawdata==0) {
   telluser2(0,"Not Enough Memory Available","",FALSE);
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

if (config.rawdata)
   FreeMem(config.rawdata, config.raw_data_size);
}

