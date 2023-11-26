
/*******************/
/*** printpage.c ***/
/*******************/

#include "exec/types.h"
#include "exec/exec.h"
#include "graphics/gfx.h"
#include "graphics/text.h"
#include "graphics/rastport.h"
#include "graphics/view.h"
#include "devices/printer.h"
#include "devices/prtbase.h"
#include "stdio.h"
#include "desk.h"
#include "text.h"
     
/*****************************************/
/** Preferences printer driver routines **/
/*****************************************/
     
     
union {
struct IOStdReq ios;
struct IODRPReq iodrp;
struct IOPrtCmdReq iopc;
} printerIO;
     
#define DUMPSIZE sizeof(struct IODRPReq)
     
struct MsgPort *printerPort;
struct RastPort rp;
extern ULONG xpix,ypix;    /*** Actual no. of printer dots on a line ***/
extern UWORD xinch,yinch;
extern FIXED cpertextpixx; /** Number of printer dots on a line if you **/
extern FIXED cpertextpixy; /** assume the printer can printer ph X pw **/
extern short prt_passes;
ULONG xoffset,yoffset;
ULONG pixwidth, pixheight;
ULONG current_line;           /** line current chunk starts on **/
ULONG chunk_size;             /** height of rast port **/
struct IODRPReq *request;
UBYTE *virtual_page_start;
     
SHORT inversetext = FALSE;    /*** Tell draw text whether or not to **/
                              /*** use inverting fits              ***/
SHORT clipping = FALSE;       /*** use a fit that clips ?? **/
short *fitp_cliptop;   /*** bytes that define the clipping zone **/
short *fitp_clipbot;   /*** used by fits ***/
     
     
/*****************************************************/
/*** Preferences init function --sets up rastport ***/
/*****************************************************/
     
pref_init(page)
     
struct pageinfo *page;
     
{
static struct BitMap bitmap;
struct IODRPReq *OpenPrinter();
     
/** first open printer device to get resolution **/
request=OpenPrinter();
if (request==NULL)  {
   return (CANTOPENPRINTER);
   }
     
/** now reset printer, just as a precationary cleanup **/
     
request->io_Command=CMD_RESET;
DoIO(request);
request->io_Error=0;
     
/** set up width/height/offsets **/
     
pixwidth=(ULONG)page->pw<<16;
pixwidth=(pixwidth/cpertextpixx)+1;
     
pixheight=(ULONG)page->ph<<16;
pixheight=(pixheight/cpertextpixy)+1;

/* if (pixheight*pixwidth > 1000000L)*/ /** Using bigger that 125K chunks?**/
if (pixheight*pixwidth > 100000L) {
   chunk_size = pixheight/4+1;
   clipping = TRUE;
   }
else {
   chunk_size = pixheight;
   clipping = FALSE;
   }

prt_passes = pixheight/chunk_size;
if ( (prt_passes*chunk_size) != pixheight)
   prt_passes++;
current_line = 0;     
if (ypix==0) ypix=yinch*11;   /* if room for full page, use it */
     
xoffset=(((ULONG)page->pw*(ULONG)xinch/(ULONG)576)-xpix)>>1;
yoffset=(((ULONG)page->ph*(ULONG)yinch/(ULONG)576)-ypix)>>1;
     
/**** Next setup a rasport to use in printing ****/
     
     
InitRastPort(&rp);
     
bitmap.BytesPerRow=RASSIZE(pixwidth,1);
bitmap.Rows=chunk_size;
bitmap.Flags=0;
bitmap.Depth=1;
bitmap.Planes[0]=(PLANEPTR)AllocMem(RASSIZE(pixwidth,chunk_size),
   MEMF_CLEAR|MEMF_CHIP);
     
printf("pixwidth=%d, height=%d\n",pixwidth,pixheight);
printf("chunksize=%d rassize=%d alloc=%d\n",
  chunk_size,RASSIZE(pixwidth,pixheight),bitmap.Planes[0]);     
if (bitmap.Planes[0]==NULL) {
   ClosePrinter(request);
   return(OUTOFMEM);
   }
     
rp.BitMap=&bitmap;
virtual_page_start = bitmap.Planes[0];
fitp_cliptop = virtual_page_start;
fitp_clipbot = virtual_page_start + RASSIZE(pixwidth,chunk_size);
     
return(AOK);
}
     
/************************************/
/*** Preferences "endpage" code *****/
/************************************/
     
     
pref_endpage()
     
{
FreeMem(rp.BitMap->Planes[0],RASSIZE(pixwidth,(rp.BitMap->Rows)));
ClosePrinter(request);
allocflush();
clipping = FALSE;
return(AOK);
}
     
pref_dumpchunk()
     
{
short error,done;
long val;
static USHORT ct[] = {0xFFF,0x000};  /* white and black */
static struct ColorMap cm = {
   0, /*flags */
   0, /* Type */
   2, /* count */
   (APTR)&ct[0]  /* color table UWORD xRGB */
   };

if (current_line+chunk_size >= ypix) {
   done = TRUE;
   chunk_size = ypix - current_line;
   }
else {
   done = FALSE;
   }

DumpRPort(request,&rp,&cm,0,xoffset,yoffset,xpix,chunk_size,xpix,chunk_size,0);

val = RASSIZE(pixwidth,chunk_size);
BltClear(rp.BitMap->Planes[0],val,1);

virtual_page_start = virtual_page_start - val;
current_line += chunk_size;

yoffset = 0;


return(done);
}
     
/********************************************/
/*** render graphics and text objects     ***/
/********************************************/
     
pref_doobj(obj)
     
struct object *obj;
     
{
     
struct graphic *g;
void fit(),invfit(),cfit(),cinvfit();
void (*fitptr)();
USHORT bpr;
     
printf("trying to print object %x of type %x\n",obj,obj->type);

bpr = rp.BitMap->BytesPerRow;
     
if (obj->flags & TOP) {
   ClipBlit(&rp,0,0,&rp,   /** clear any bits where object is going **/
       ctopx(obj->x),
       ctopy(obj->y),
       ctopx(obj->w),ctopy(obj->h),0x00A,0x01,0);
      }
     
     
if (obj->type==GRAPHIC) {
/* printf("obj is graph\n"); */
   g=(struct graphic *)obj->data;
   if (clipping)
      fitptr = cfit;
   else
      fitptr=fit;

   if (obj->flags & INV) {
      /* printf("inv graph fit\n"); */
      if (clipping)
         fitptr = cinvfit;
      else
         fitptr=invfit;
      }

printf("calling fit %x\n", virtual_page_start+bpr*ctopy(obj->y));

   (*fitptr)(g->planes[0]+RASSIZE(g->w,g->usey),
        g->usex,             /* srcoff */
        g->useh,             /* srch */
        g->usew,             /* srcw */
        RASSIZE(g->w,1),     /* srcmod */
        virtual_page_start+bpr*ctopy(obj->y), /* destsrce */
        ctopx(obj->x),       /* destoff */
        ctopy(obj->h),       /* desth */
        ctopx(obj->w),       /* destw */
        bpr);    /* destmod == len in bytes of dest area */
     
   }
     
if (obj->type==TEXT) {
printf("obj is text\n");
   if (obj->flags&INV)  {
       printf("inv text\n");
      inversetext=TRUE;
      }
     
   draw_text(obj,virtual_page_start + bpr*ctopy(obj->y),
             ctopx(obj->w),
             ctopy(obj->h),
             ctopx(obj->x),    /* destxoff */
             bpr,              /* destmod */
             ctopxlong(1<<16),
             ctopylong(1<<16));
      inversetext=FALSE;
   }
return(AOK);
}
     
/*********************************************/
/*** draw a line to a prefererence printer ***/
/*********************************************/
     
int pref_hline(x,y,w,thick,pat)
     
COORD x,y,w,thick;
UWORD pat;
     
{
SHORT i,limit;
     
limit = max (1,(thick*yinch+288)/576);
     
SetAPen(&rp,1);
SetDrMd(&rp,JAM1);
rp.LinePtrn=0;
fit (&pat, 0, 1, 8, 1, &rp.LinePtrn,0,1,16,2);  /** stretch pattern **/
     
for (i=0; i < limit; i++) {
   Move(&rp,ctopx(x),ctopy(y)+i);
   Draw(&rp,ctopx(x+w),ctopy(y)+i);
   }
     
rp.LinePtrn=0xFFFF;
return(AOK);
}
     
/*********************************************/
/*** draw a line to a prefererence printer ***/
/*********************************************/
     
int pref_vline(x,y,w,thick,pat)
     
COORD x,y,w,thick;
UWORD pat;
     
{
SHORT i,limit,top,bot,height;
     
limit = max (1,(thick*xinch+288)/576);
     
SetAPen(&rp,1);
SetDrMd(&rp,JAM1);
     
rp.LinePtrn=0;    /** fits "ORs" this bits in **/
fit (&pat, 0, 1, 8, 1, &rp.LinePtrn,0,1,16,2);  /** stretch pattern **/

top = ctopy(y)-current_line;
bot = ctopy(y+w)-current_line;
height = rp.BitMap->Rows;


if (top >= height) {
   return(AOK);
   }

if (bot < 0)  {
   return(AOK);
   }

if (top < 0)
   top = 0;
if (bot >= height)
   bot = height-1;


for (i=0; i < limit; i++) {
   Move(&rp,ctopx(x)+i,top);
   Draw(&rp,ctopx(x)+i, bot);
   }
     
rp.LinePtrn=0xFFFF;
return(AOK);
}
     
/********************************************/
/*** draw a box to a preferences printer ****/
/********************************************/
     
int pref_box(x,y,h,w,thick,pat)
     
COORD x,y,h,w,thick;
UWORD pat;
     
{
pref_hline(x,y,w-thick + 1 , thick,pat);
pref_hline(x,y+h-thick + 1 , w-thick,thick,pat);
pref_vline(x,y,h,thick,pat);
pref_vline(x+w-thick,y,h,thick,pat);
return(AOK);
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
extern struct Window *canwin;
extern struct MsgPort *printerPort;
struct Message *myio;

printf("rq %x,rastPort %x,colorMap %x,modes %x,sx %x,sy %x,sw %x,sh %x,dc %x,dr %x,s %x\n",
       rq,rastPort,colorMap,modes,sx&0xFFFE,sy&0xFFFE,sw,sh,dc,dr,s);
     
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
     
SendIO(rq);
     
while (TRUE) {
     
   Wait( (1<<canwin->UserPort->mp_SigBit) | (1<<printerPort->mp_SigBit) );
printf("got sig\n");
   if (checkcan()) {    /** we were canceled ! **/
printf("can\n");
      if (!(CheckIO(rq))) {
printf("abort\n");
         AbortIO(rq);
         WaitIO(rq);
         }
      return(CANCEL);
      }
     
   if ((myio=GetMsg(printerPort))!=0) { /** finished ! **/
      printf("Error %d\n",rq->io_Error);
      if ((myio=GetMsg(printerPort))!=0) { /** finished ! **/
         printf("extra?\n");
         }
      return(AOK);
      }
printf("empty wait??\n");
   }        /** wait for next msg **/
}
     
     
struct IODRPReq *OpenPrinter()
{
extern struct MsgPort *printerPort;
extern ULONG xpix,ypix;
extern UWORD xinch,yinch;
extern char *pname;
struct IODRPReq *req;
struct PrinterExtendedData *PED;
int error;
     
     
printerPort=CreatePort(NULL,0);
req=CreateExtIO(printerPort, DUMPSIZE);
if (req == NULL)
   return(NULL);
     
error=OpenDevice("printer.device",0,req,0);
if (error!=NULL) return(NULL);
req->io_Command=CMD_RESET;
req->io_Flags=0;
DoIO(req);
req->io_Flags=0;
req->io_Error=0;
     
PED=&((struct PrinterData *)(req->io_Device))->pd_SegmentData->ps_PED;
xpix=PED->ped_MaxXDots;   /** number of dots printer has on a horz. line */
ypix=PED->ped_MaxYDots;   /** number of printer dots on vertical line */
xinch=PED->ped_XDotsInch;
yinch=PED->ped_YDotsInch;
pname=PED->ped_PrinterName;
     
#if 0
Forbid();   /** put our render in front of the graphic driver's **/
real_render = PED->ped_Render;
PED->ped_Render = my_render;
Permit();
printf("real r = %x dev r = %x\n",real_render, PED->ped_Render);
#endif
return(req);
}
     
ClosePrinter(req)
struct IODRPReq *req;
{
extern struct MsgPort *printerPort;
     
CloseDevice(req);
DeleteExtIO(req);
DeletePort(printerPort);
     
return(0);
}
     
#if 0
/*****************************************/
/** Opens and Closes the printer device **/
/*****************************************/
     
openclose()
{
APTR r;
struct MsgPort *p;
int error;
     
p=CreatePort(NULL,0);
if (p==0) return(FALSE);
r=CreateExtIO(p, DUMPSIZE);
if (r==NULL) return(FALSE);
error=OpenDevice("printer.device",0,r,0);
if (error!=NULL) return(FALSE);
CloseDevice(r);
DeleteExtIO(r);
DeletePort(p);
return(TRUE);
}
#endif
     
/**************************************/
/** convert COORDs to printer pixels **/
/**************************************/
/* Must maintain in parallel with ctopxlong, ctopylong! */
     
ctopx(x)
COORD x;
{
ULONG pix;
extern FIXED cpertextpixx;
     
pix= (x<<16)/cpertextpixx;
return((int)pix);
}
     
ctopy(y)
COORD y;
{
ULONG pix;
extern FIXED cpertextpixy;
     
pix= (y<<16)/cpertextpixy;
return((int)pix);
}
     
/***************************************/
/** convert LCOORDs to printer pixels **/
/***************************************/
/* Must maintain in parallel with ctopx, ctopy! */
     
ctopxlong(x)
LCOORD x;
{
ULONG pix;
extern FIXED cpertextpixx;
     
pix= (x<<14)/cpertextpixx;
return(pix<<2);
}
     
ctopylong(y)
LCOORD y;
{
ULONG pix;
extern FIXED cpertextpixy;
     
pix= (y<<14)/cpertextpixy;
return(pix<<2);
}
     
allocflush()
{
UBYTE *yeow;
     
if (yeow = AllocMem(16000000,0L)) {
   FreeMem(yeow,16000000);
   }
}
     



