/*********************/
/*** pref_driver.c ***/
/*********************/

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
#include "VirtualPage.h"

#define debug 0

/*****************************************/
/** Preferences printer driver routines **/
/*****************************************/

/*
union {
struct IOStdReq ios;
struct IODRPReq iodrp;
struct IOPrtCmdReq iopc;
} printerIO;
*/
     
#define DUMPSIZE sizeof(struct IODRPReq)
     
extern ULONG xpix,ypix;    /*** Actual no. of printer dots on a line ***/
extern UWORD xinch,yinch;
extern short prt_passes;
extern struct printerprefs printerprefs;

extern struct Window *clipboard;  /** KILL THIS!! **/

struct MsgPort *printerPort;
struct RastPort rp;
ULONG xoffset,yoffset;
ULONG pixwidth, pixheight;
ULONG current_line;           /** line current chunk starts on **/
ULONG chunk_size;             /** height of rast port **/
struct IODRPReq *request;
UBYTE *virtual_page_start;
SHORT numrows;                /** height of print head in pixels **/
     
SHORT inversetext = FALSE;    /*** Tell draw text whether or not to **/
                              /*** use inverting fits              ***/
SHORT clipping = FALSE;       /*** use a fit that clips ?? **/
UBYTE *fitp_cliptop;   /*** bytes that define the clipping zone **/
UBYTE *fitp_clipbot;   /*** used by fits ***/
     
static COORD x_black_dash_size;
static COORD x_white_dash_size;
static COORD y_black_dash_size;
static COORD y_white_dash_size;

extern struct VPLib *VPLibBase;     /** VP Library pointer **/
static struct VirtualPage *vp;      /** virtual page printing to **/
     
/*****************************************************/
/*** Preferences init function --sets up rastport ***/
/*****************************************************/
     
pref_init(page)
     
struct pageinfo *page;
     
{
static struct BitMap bitmap;
struct IODRPReq *OpenPrinter();
int remainder;

/* printf("pref_init called\n"); */
     
/** first open printer device to get resolution **/

request=OpenPrinter();
if (request==NULL)  {
   return (CANTOPENPRINTER);
   }
     
/** now reset printer, just as a precationary cleanup **/
     
request->io_Command=CMD_RESET;
DoIO(request);
request->io_Error=0;
request->io_Flags=0;
     
/** set up width/height/offsets **/

pixwidth = page->pw;
pixheight= page->ph;

/* printf("\npixwidth %d pixheight %d\n",pixwidth, pixheight); */
if (printerprefs.flags & STRIP_PRINT) {
   if (pixheight*pixwidth > 7000000)          /** 300 X 300 dpi **/
      chunk_size = pixheight/20+1;  /** 20? **/
   else  if (pixheight*pixwidth > 3000000)    /** 24 pin printer **/
      chunk_size = pixheight/12+1; /** 10? **/
   else 
      chunk_size = pixheight/5+1;             /** lower density printers **/
   remainder = chunk_size - ((chunk_size/numrows)*numrows);
   chunk_size = chunk_size + numrows - remainder;
   clipping = TRUE;
   }
else {
   chunk_size = pixheight;
   clipping = FALSE;
   }

/* printf("numrows %d chuncksize %d \n",numrows,chunk_size); */
prt_passes = pixheight/chunk_size;
if ( (prt_passes*chunk_size) != pixheight)
   prt_passes++;

current_line = 0;

/* printf("xpix %d ypix %d\n",xpix,ypix); */
if (ypix==0) ypix=yinch*11;   /* if room for full page, use it */
/* printf("xpix %d ypix %d\n",xpix,ypix);
printf("xinch %d yinch %d\n",xinch,yinch); */
     
/* xoffset=(((ULONG)page->pw*(ULONG)xinch/(ULONG)576)-xpix)>>1;
yoffset=(((ULONG)page->ph*(ULONG)yinch/(ULONG)576)-ypix)>>1; */

xoffset=(((ULONG)page->pw)-xpix)>>1; /** these center area to print when **/
yoffset=(((ULONG)page->ph)-ypix)>>1; /** printer cant print entire page */
     
/**** Next setup a rasport to use in printing ****/
     
InitRastPort(&rp);
InitBitMap(&bitmap,1,pixwidth,chunk_size);

/* printf("bm: %d %d %d %d\n",bitmap.BytesPerRow,bitmap.Rows,bitmap.Flags,bitmap.Depth); */

#if debug
bitmap.Planes[0] = clipboard->RPort->BitMap->Planes[0]+1600;
#else
bitmap.Planes[0]=(PLANEPTR)AllocMem(RASSIZE(pixwidth,chunk_size),MEMF_CLEAR|MEMF_CHIP);
#endif

/* printf("alloc %d ptr %x\n",RASSIZE(pixwidth,chunk_size),bitmap.Planes[0]); */

if (bitmap.Planes[0]==NULL) {
   ClosePrinter(request);
   return(OUTOFMEM);
   }
     
rp.BitMap=&bitmap;
virtual_page_start = bitmap.Planes[0];
fitp_cliptop = virtual_page_start;
fitp_clipbot = virtual_page_start + RASSIZE(pixwidth,chunk_size);

x_black_dash_size = xinch/5;             /** size in coord's of dashes **/
x_white_dash_size = xinch*7/100;         /** to use when drawing dashed **/
y_black_dash_size = yinch/5;             /** boxes **/
y_white_dash_size = yinch*7/100;

/* BLACK_DASH_SIZE 114  old coords
#define WHITE_DASH_SIZE 40 */

#if debug
BltClear(rp.BitMap->Planes[0], RASSIZE(pixwidth,chunk_size), 1);
pref_box(0,0,chunk_size,pixwidth,1,FALSE);
#endif

return(AOK);
}
     
/************************************/
/*** Preferences "endpage" code *****/
/************************************/
     
     
pref_endpage()
     
{
#if debug
#else
FreeMem(rp.BitMap->Planes[0],RASSIZE(pixwidth,(rp.BitMap->Rows)));
/* printf("prt: %x freeeing %d\n",rp.BitMap->Planes[0],RASSIZE(pixwidth,(rp.BitMap->Rows))); */
#endif
/* printf("pref_endpage called\n"); */
ClosePrinter(request);
allocflush();
clipping = FALSE;
return(AOK);
}
     
pref_dumpchunk()
     
{
short done;
long val, ysize;
static USHORT ct[] = {0xFFF,0x000};  /* white and black */
static struct ColorMap cm = {
   0, /*flags */
   0, /* Type */
   2, /* count */
   (APTR)&ct[0]  /* color table UWORD xRGB */
   };

/* printf("pref_dumpchunk called\n"); */

if (current_line+chunk_size >= ypix) {
   done = TRUE;
   chunk_size = ypix - current_line;
   }
else {
   done = FALSE;
   }

if (yoffset) {                   /** yoffset does not work when there **/
   ysize = chunk_size-yoffset;   /** is more than one strip of yoffset **/
   }
else {
   ysize = chunk_size;
   }

if (ysize >1)
   my_DumpRPort(request,&rp,&cm,0,xoffset,yoffset,xpix,ysize,xpix,ysize,SPECIAL_TRUSTME|SPECIAL_NOFORMFEED);

val = RASSIZE(pixwidth,chunk_size);
BltClear(rp.BitMap->Planes[0],val,1);


#if debug
pref_box(0,0,chunk_size,pixwidth,1,FALSE); /** KILL THIS!! **/
#endif

virtual_page_start = virtual_page_start - val;
current_line += chunk_size;

yoffset = 0;

return(done);
}
     
/***************************/
/** draw a gray rectangle **/
/***************************/
     
pref_doblock(obj)
struct object *obj;
{
static UBYTE bit_table[] = {128,64,32,16,8,4,2,1};

register USHORT x,y;
USHORT val;
static UBYTE matrix[] = {0,8,2,10,12,4,14,6,3,11,1,9,14,7,13,5};
UBYTE *mem;
int x_off, mod;
int dest_w, dest_h;

mod    = rp.BitMap->BytesPerRow;
mem    = virtual_page_start+mod*ctopy(obj->y);
x_off    = ctopx(obj->x);
dest_w = ctopx(obj->w);
dest_h = ctopy(obj->h);
val    = (15*obj->density)/100;

for (y=0; y < dest_h; y++) {
   if (mem >= fitp_cliptop && mem < fitp_clipbot) {
      for (x=0; x < dest_w; x++) {
         if ( matrix[ (x&3) + ((y&3)<<2)] <= val) {
            (*(mem + ((x+x_off) >> 3))) |= bit_table[(x+x_off)&7];
            }
         }
      }
   mem += mod;
   }
return(AOK);
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
     
bpr = rp.BitMap->BytesPerRow;
     
if (obj->flags & TOP) {
   ClipBlit(&rp,
       ctopx(obj->x),
       ctopy(obj->y)-current_line,
       &rp,
       ctopx(obj->x),
       ctopy(obj->y)-current_line,
       ctopx(obj->w),ctopy(obj->h),0x20,0x01,0);
      }

if (obj->type==GRAPHIC) {
   g=(struct graphic *)obj->data;

   if (obj->flags&INV)  {
      inv_clip_dither(g, virtual_page_start+bpr*ctopy(obj->y),
          ctopx(obj->x), ctopx(obj->w), ctopy(obj->h), bpr, printerprefs.flags);
      }
   else  {
      clip_dither(g, virtual_page_start+bpr*ctopy(obj->y),
          ctopx(obj->x), ctopx(obj->w), ctopy(obj->h), bpr, printerprefs.flags);
      }
#if 0
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
#endif
     
   }
     
if (obj->type==TEXT) {
   if (obj->flags&INV)  {
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
     
int pref_hline(x,y,w,thick, dashed)
     
COORD x,y,w,thick;
int dashed;
     
{
SHORT i,limit, ypos,j;
COORD first_dash_length, dash_size;
int num_dashes;
     
limit = max (1, thick);
     
SetAPen(&rp,1);
SetDrMd(&rp,JAM1);

w--;     /** adjust because Draw draws to and including final point **/
if (x < 0) x=0;
if (y < 0) y=0;
if (w < 0) w=0;

if (!dashed) {
   for (i=0; i < limit; i++) {
      ypos = ctopy(y)+i-current_line;
      if (ypos >=0 && ypos < chunk_size) {
         Move(&rp, x, ypos);
         for(j=0; j<w; j=j+1000) {     /** blitter can only draw 1024 dots */
            Draw(&rp, x+j+min(1000,w-j), ypos);
            }
         }
      }
   return(AOK);
   }

/** BLACK_DASH_SIZE is the exact length of the black line,         **/
/** WHITE_DASH_SIZE is a nominal size that is adjusted to make the **/
/**                 dashes look evenly spaced **/

num_dashes = w/(x_white_dash_size+x_black_dash_size)-1;  /** pick reasonable **/
dash_size  = (w - x_black_dash_size)/num_dashes;         /** get w+b to use **/

first_dash_length = w - num_dashes*dash_size;

for (i=0; i < limit; i++) {
   ypos = ctopy(y)+i-current_line;
   if (ypos >=0 && ypos < chunk_size) {
      Move(&rp, ctopx(x), ypos);
      Draw(&rp, ctopx(x+first_dash_length), ypos);
      }
   }

x += first_dash_length+dash_size-x_black_dash_size;

while (num_dashes) {
   for (i=0; i < limit; i++) {
      ypos = ctopy(y)+i-current_line;
      if (ypos >0 && ypos < chunk_size) {
         Move(&rp, ctopx(x), ypos);
         Draw(&rp, ctopx(x+x_black_dash_size), ypos);
         }
      }
   x += dash_size;
   num_dashes--;
   }
return(AOK);
}
     
/*********************************************/
/*** draw a line to a prefererence printer ***/
/*********************************************/
     
int pref_vline(x,y,w,thick,dashed)
     
COORD x,y,w,thick;
int dashed;
     
{
SHORT limit;
COORD first_dash_length, dash_size;
int num_dashes;
     
limit = max (1,thick);
     
SetAPen(&rp,1);
SetDrMd(&rp,JAM1);

w--;              /** I'm using Draw which draws final point **/
if (x < 0) x=0;
if (y < 0) y=0;
if (w < 0) w=0;
     
if (!dashed) {
   clip_vline(x,y,w,limit);
   return(AOK);
   }

/** BLACK_DASH_SIZE is the exact length of the black line,         **/
/** WHITE_DASH_SIZE is a nominal size that is adjusted to make the **/
/**                 dashes look evenly spaced **/

num_dashes = w/(y_white_dash_size+y_black_dash_size)-1; /** pick reasonable **/
dash_size  = (w - y_black_dash_size)/num_dashes;        /** get w+b to use **/

first_dash_length = w - num_dashes*dash_size;

clip_vline(x,y,first_dash_length,limit);
y += first_dash_length+dash_size-y_black_dash_size;

while (num_dashes) {
   clip_vline(x,y,y_black_dash_size,limit);
   y += dash_size;
   num_dashes--;
   }
return(AOK);
}

clip_vline(x,y,w,limit)

COORD x,y,w;

{
SHORT i,top,bot,height;

top = ctopy(y)-current_line;
bot = ctopy(y+w)-current_line;
height = rp.BitMap->Rows;

/* printf("clip_vline top %d bot %d height %d\n",top,bot,height); */

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
     
return(AOK);
}
     
/********************************************/
/*** draw a box to a preferences printer ****/
/********************************************/
     
int pref_box(x,y,h,w,thick,dashed)
     
COORD x,y,h,w,thick;
int dashed;
     
{
COORD xthick, ythick;

/** thick is in old coords (1/576") so convert to new coords **/

xthick = max(1,thick*xinch/576);
ythick = max(1,thick*yinch/576);

/* printf("x %d y %d h %d w %d thick %d dashed %d\n",x,y,h,w,thick,dashed);
printf("xthick %d thick %d\n",xthick,ythick); */

pref_hline(x,           y,          w-xthick, ythick, dashed);
pref_hline(x,           y+h-ythick, w-xthick, ythick, dashed);
pref_vline(x,           y,          h,        xthick, dashed);
pref_vline(x+w-xthick,  y,          h,        xthick, dashed);
return(AOK);
}
     
/*********************************/
/*** send rastprot to printer ****/
/*********************************/
     
/* my_DumpRPort(request,&rp,&cm,modes,xoffset,yoffset,
xpix,ysize,xpix,ysize,SPECIAL_TRUSTME|SPECIAL_NOFORMFEED); */

my_DumpRPort(rq,rastPort,colorMap,modes,sx,sy,sw,sh,dc,dr,s)
     
struct RastPort *rastPort;
struct ColorMap *colorMap;
ULONG modes;
UWORD sx,sy,sw,sh;
LONG dc,dr;
UWORD s;
struct IODRPReq *rq;
     
{
extern struct Window *canwin;
struct MsgPort *port;
extern struct MsgPort *printerPort;
struct Message *myio;

/*printf("rq %x,rastPort %x,colorMap %x,modes %x,sx %x,sy %x,sw %x,sh %x,dc %x,dr %x,s %x\n",
       rq,rastPort,colorMap,modes,sx,sy,sw,sh,dc,dr,s); */

rq->io_Command=PRD_DUMPRPORT;
rq->io_RastPort=rastPort; /*clipboard->RPort; */
rq->io_ColorMap=colorMap;
rq->io_Modes=modes;
rq->io_Flags=0;
rq->io_SrcX=(sx&0xFFF0); /*sx; on some drivers this was causing a crash! */
rq->io_SrcY=sy; /* sy; */
rq->io_SrcWidth=sw;
rq->io_SrcHeight=sh;
rq->io_DestCols=dc;
rq->io_DestRows=dr;
rq->io_Special=s;

  /* printf("rq->srx = %x src y = %x sw = %x sh = %x dc = %x dr = %x\n",
      rq->io_SrcX, rq->io_SrcY, rq->io_SrcWidth, rq->io_SrcHeight,
      rq->io_DestCols, rq->io_DestRows); */

SendIO(rq);
     
while (TRUE) {

   port = rq->io_Message.mn_ReplyPort;
   Wait( (1<<canwin->UserPort->mp_SigBit) | (1<<port->mp_SigBit) );
/* printf("got sig\n"); */
   if (checkcan()) {    /** we were canceled ! **/
/* printf("can\n"); */
/*      if (!(CheckIO(rq))) { */
      rq->io_Error=0;
      rq->io_Flags=0;
      AbortIO(rq);
/* printf("a_can\n"); */
      WaitIO(rq);
/* printf("b_can\n"); */
      rq->io_Error=0;
      rq->io_Flags=0;
      while ((myio=GetMsg(port))!=0) {/*printf("xx\n");*/}
      return(CANCEL);
      }

/*
#define  PDERR_CANCEL         1
#define  PDERR_NOTGRAPHICS    2
#define  PDERR_INVERTHAM      3
#define  PDERR_BADDIMENSION   4
#define  PDERR_DIMENSIONOVFLOW   5
#define  PDERR_INTERNALMEMORY 6
#define  PDERR_BUFFERMEMORY   7
*/

   while ((myio=GetMsg(port))!=0) { /** finished ! **/
      /*printf("extra?\n"); */
      }
   if (rq->io_Error) {
      telluser2(0,"Unknown error while printing.","",FALSE);
      /* printf("PREF_DRIVER!!!: Error %d\n",rq->io_Error); */
      }
   rq->io_Error=0;
   rq->io_Flags=0;
   return(AOK);
   }
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
     

/*printf("opening printer, request %d\n",DUMPSIZE); */
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
numrows=PED->ped_NumRows;   /** Height of print head **/
     
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
     
/* printf("closing printer, request %x\n",req); */
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
return((int)x);
}
     
ctopy(y)
COORD y;
{
ULONG pix;
return((int)y);
}
     
/***************************************/
/** convert LCOORDs to printer pixels **/
/***************************************/
/* Must maintain in parallel with ctopx, ctopy! */
     
ctopxlong(x)
LCOORD x;
{
return(x);
}
     
ctopylong(y)
LCOORD y;
{
return(y);
}
     
allocflush()
{
UBYTE *yeow;
     
if (yeow = AllocMem(16000000,0L)) {
   FreeMem(yeow,16000000);
   }
}
     

#if 0
show_cksum(rp)
struct RastPort *rp;
{
UBYTE *start;
UBYTE sum;
int i;

start = ((UBYTE *)(rp->BitMap->Planes[0])) +
          RASSIZE(pixwidth,(rp->BitMap->Rows));

sum = 0;

for (i=0; i<1000;i++)
   sum = sum + *(start+i);

printf("end sum=%d\n",sum);


start = (UBYTE *)(rp->BitMap->Planes[0]);

start = start - 800;

sum = 0;

for (i=0; i<800;i++)
   sum = sum + *(start+i);

printf("start sum=%d\n",sum);
}

#endif

/*** VIRTUAL PAGE DRIVERS (SIMILAR TO PREF DRIVERS) **/

/*******************************************/
/*** VP init function --sets up rastport ***/
/*******************************************/
     
vp_init(page)
     
struct pageinfo *page;
     
{
static struct BitMap bitmap;
struct IODRPReq *OpenPrinter();
int remainder;
struct NewVirPage nvp;
struct BitmapList *vpbl;     /** virtual page bitmap list **/

/** Get VP info **/

if (VPLibBase==0)
   return(VPLIBERR);

vpbl = InquireVP(0);
if (vpbl) {
   xpix = vpbl->vpbl_VirPageDesc->vpd_Width;
   ypix = vpbl->vpbl_VirPageDesc->vpd_Height;
   }

/* printf("vpbl=%x name=%s x %d y %d\n",vpbl,vpbl->vpbl_Name,xpix,ypix); */

setmem(&nvp, sizeof(nvp), 0);
nvp.nvp_Reference = vpbl;
if (vpbl==0) {
   nvp.nvp_Width =  2448;
   nvp.nvp_Height = 3204;
   nvp.nvp_Depth  = 1;
   nvp.nvp_DPI_X  = 300;
   nvp.nvp_DPI_Y  = 300;
   nvp.nvp_VirFlags = VPRQF_NEWPAGE;
   }

vp = OpenVP(&nvp);
if (vp==0)
   return(OUTOFMEM);

if (vpbl==0) {
   NameVP(vp, "VP_CityDesk2.0");
   vpbl = vp->vp_BitmapList;
   xpix = vpbl->vpbl_VirPageDesc->vpd_Width;
   ypix = vpbl->vpbl_VirPageDesc->vpd_Height;

  /* printf("2) vpbl=%x name=%s x %d y %d\n",vpbl,vpbl->vpbl_Name,xpix,ypix); */

   }

/* printf("access %d\n",AccessVP(vp, 1, 0)); */
AccessVP(vp, 1, 0);

/*     0 = Successfull - Access Granted
    -1 = Locked - Page Is Currently Locked By Another User
     # = Number Of Users - Exlclusive Access Denied */

/** set up width/height/offsets **/

pixwidth = page->pw;
pixheight= page->ph;

if (printerprefs.flags & STRIP_PRINT) {
   if (pixheight*pixwidth > 7000000)          /** 300 X 300 dpi **/
      chunk_size = pixheight/20+1;  /** 20? **/
   else  if (pixheight*pixwidth > 3000000)    /** 24 pin printer **/
      chunk_size = pixheight/12+1; /** 10? **/
   else 
      chunk_size = pixheight/5+1;             /** lower density printers **/
   remainder = chunk_size - ((chunk_size/numrows)*numrows);
   chunk_size = chunk_size + numrows - remainder;
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
     
xoffset=(((ULONG)page->pw)-xpix)>>1;
yoffset=(((ULONG)page->ph)-ypix)>>1;
     
/**** Next setup a rasport to use in printing ****/
     
InitRastPort(&rp);
InitBitMap(&bitmap,1,pixwidth,chunk_size);
     
bitmap.Planes[0]=(PLANEPTR)AllocMem(RASSIZE(pixwidth,chunk_size),MEMF_CLEAR|MEMF_CHIP);

/* printf("alloc %d\n",RASSIZE(pixwidth,chunk_size)); */

if (bitmap.Planes[0]==NULL) {
   CloseVP(vp);
   return(OUTOFMEM);
   }
     
rp.BitMap=&bitmap;
virtual_page_start = bitmap.Planes[0];
fitp_cliptop = virtual_page_start;
fitp_clipbot = virtual_page_start + RASSIZE(pixwidth,chunk_size);

x_black_dash_size = xinch/5;           /** size in coord's of dashes **/
x_white_dash_size = xinch*7/100;       /** to use when drawing dashed **/
y_black_dash_size = yinch/5;           /** boxes **/
y_white_dash_size = yinch*7/100;

/* BLACK_DASH_SIZE 114  old coords
#define WHITE_DASH_SIZE 40 */

return(AOK);
}
     
/************************************/
/*** Preferences "endpage" code *****/
/************************************/
     
     
vp_endpage()
     
{
struct PrinterInfo *vppi;          /* Printer Info Structure Pointer   */

FreeMem(rp.BitMap->Planes[0],RASSIZE(pixwidth,(rp.BitMap->Rows)));
clipping = FALSE;
FreeAccessVP(vp);
if (telluser2(0,"Send Virtual Page to","the laser printer?",TRUE)) {
   if ((vppi = InquirePrt(-1))) {            /* Get Def Printer  */
/* printf("vppi %x\n",vppi); */
/*      if (OpenPrt(vp, vppi, VPPRF_INIT|VPPRF_AUTOF|VPPRF_AUTOX|VPPRF_AUTOY|VPPRF_CENTRX|VPPRF_CENTRY)) { */
      if (OpenPrt(vp, vppi, VPPRF_INIT)) {
 /* printf("doing print\n"); */
         PrintVP(vp);    /** zero if succesfull **/
         ClosePrt(vp);
         }
      }
   }
CloseVP(vp);
return(AOK);
}
     
vp_dumpchunk()
     
{
short done;
long val;


if (current_line+chunk_size >= ypix) {
   done = TRUE;
   chunk_size = ypix - current_line;
   }
else {
   done = FALSE;
   }

val = rp.BitMap->BytesPerRow*8;
MoveToVP(&rp.BitMap->Planes[0],0,0, vp, 0, current_line, val, val, chunk_size, 0, 1);

/*
     MoveToVP(bmla, sx, sy, vp, dx, dy,
              BitmapWidth, Width, Height, Mode, Mask)

     char   **bmla;
     ULONG  sx, sy;

     struct VirtualPage *vp;
     ULONG  dx, dy;

     ULONG  BitmapWidth, Width, Height, Mode, Mask;

     bmla = A pointer to a null terminated array of bitmap
            pointers.
     vp = Destination Pointer, VirtualPage Structure.

     sx = Source X offset
     sy = Source Y offset
     dx = Destination X offset
     dy = Destination Y offset

     BitmapWidth = Actual width of user's bitmap
     Width = Requested width of move
     Height = Number of lines to move
     Mode = 0 - Copy, 1 - OR, 2 = AND, 3 = EOR
     Mask = Bit Field Indicating which layers to affect.  A one
     indicates that layer should be affected.

     Restrictions:

         User's Bitmaps MUST start on even word boundries.
         Offsets (sx, sy, dx, dy) MUST be an even multiple of 16.
         BitmapWidth MUST be an even multiple of 16.
         Width MUST be an even multiple of 8.
         (Height is not restricted to any boundry). */


val = RASSIZE(pixwidth,chunk_size);
BltClear(rp.BitMap->Planes[0],val,1);

virtual_page_start = virtual_page_start - val;
current_line += chunk_size;

yoffset = 0;


return(done);
}
