#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "include/devices/printer.h"
#include "include/devices/prtbase.h"
#include "cd_driver.h"

#define DUMPSIZE sizeof(struct IODRPReq)

struct IntuitionBase *IntuitionBase;
struct MsgPort *printerPort;

main()
{
struct RastPort *rp;
struct cd_port cd_port;
struct IODRPReq *request;
struct IODRPReq *OpenPrinter();
struct Screen *s;

static USHORT ct[] = {0xFFF,0x000};  /* white and black */
static struct ColorMap cm = {
   0, /*flags */
   0, /* Type */
   2, /* count */
   (APTR)&ct[0]  /* color table UWORD xRGB */
   };

static struct NewScreen NewScreenStructure = {
   0,0,   /* screen XY origin relative to View */
   640,400,   /* screen width and height */
   1,   /* screen depth (number of bitplanes) */
   0,1,   /* detail and block pens */
   LACE+HIRES,   /* display modes for this screen */
   CUSTOMSCREEN,   /* screen type */
   NULL,   /* pointer to default screen font */
   "City Desk Driver Test",   /* screen title */
   NULL,   /* first in list of custom screen gadgets */
   NULL   /* pointer to custom BitMap structure */
};

IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);
if (IntuitionBase==0)
   exit(10);

rp = &IntuitionBase->ActiveScreen->RastPort;

cd_port.real_port.mp_Node.ln_Name = "city_desk_port";
cd_port.real_port.mp_Node.ln_Pri = 0;
cd_port.real_port.mp_Node.ln_Type = NT_MSGPORT;

cd_port.real_port.mp_Flags = 0;
cd_port.real_port.mp_SigTask = FindTask(0);

AddPort(&cd_port);

s = (struct Screen *)OpenScreen(&NewScreenStructure);
if (s==0)
   exit(10);

SetRGB4(&s->ViewPort, 0, 0, 0, 0);        /** Gray **/
SetRGB4(&s->ViewPort, 1, 15, 15, 15);        /** white **/

cd_port.x_off = 0;
cd_port.modulo = 80;
cd_port.buf_start = s->BitMap.Planes[0];
cd_port.MaxXDots = 640;
cd_port.MaxYDots = 400;
cd_port.XDotsInch = 75;
cd_port.YDotsInch = 36;

request=OpenPrinter();
if (request==NULL)  {
   CloseScreen(s);
   CloseLibrary(IntuitionBase);
   exit(10);
   }
     
DumpRPort(request,rp,&cm,0,0,0,320,200,320,200,0);

ClosePrinter(request);
CloseScreen(s);
CloseLibrary(IntuitionBase);
}


/*********************************/
/*** send rastprot to printer ****/
/*********************************/
     
DumpRPort(rq,rastPort,colorMap,modes,sx,sy,sw,sh,dc,dr,s)
     
struct RastPort *rastPort;
struct ColorMap *colorMap;
ULONG modes;
UWORD sx,sy,sw,sh;
LONG dc,dr;
UWORD s;
struct IODRPReq *rq;
     
{
rq->io_Command=PRD_DUMPRPORT;
rq->io_RastPort=rastPort;
rq->io_ColorMap=colorMap;
rq->io_Modes=modes;
rq->io_SrcX=sx;
rq->io_SrcY=sy;
rq->io_SrcWidth=sw;
rq->io_SrcHeight=sh;
rq->io_DestCols=dc;
rq->io_DestRows=dr;
rq->io_Special=s;

 printf("srx = %x src y = %x sw = %x sh = %x dc = %x dr = %x\n",
      rq->io_SrcX, rq->io_SrcY, rq->io_SrcWidth, rq->io_SrcHeight,
      rq->io_DestCols, rq->io_DestRows);
     
DoIO(rq);
}


struct IODRPReq *OpenPrinter()
{
struct IODRPReq *req;
int error;
     
printerPort=CreatePort(NULL,0);
req=CreateExtIO(printerPort, DUMPSIZE);
if (req == NULL)
   return(NULL);
     
error=OpenDevice("printer.device",0,req,0);
if (error!=NULL) return(NULL);
return(req);
}
     
ClosePrinter(req)
struct IODRPReq *req;
{
     
CloseDevice(req);
DeleteExtIO(req);
DeletePort(printerPort);
     
return(0);
}

