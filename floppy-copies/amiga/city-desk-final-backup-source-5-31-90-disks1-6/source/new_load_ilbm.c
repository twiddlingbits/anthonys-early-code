#include "exec/types.h"
#include "intuition/intuition.h"
#include "ilbm.h"
#include "readpict.h"
#include "desk.h"

static USHORT hist[48];
/* static USHORT ihist[48]; */
static UBYTE min_pix;
static UBYTE max_pix;

int graphicload(g,Window)

struct Window *Window;
struct graphic **g;

{
static char fname[34]={0};
static char drawer[35]={0};
struct Screen *screen;
char path[80];
int cancel;


#if GERMAN
cancel=get_fname(Window,Window->WScreen,"Graphik einlesen",fname,drawer);
#else
cancel=get_fname(Window,Window->WScreen,"Load A Graphic",fname,drawer);
#endif

if (cancel==NULL) return(CANCEL);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if ((*g=(struct graphic *)AllocMem(sizeof(struct graphic),MEMF_CLEAR))==0)
      return(OUTOFMEM);

screen = Window->WScreen;
newpointer(screen, BUSY);
cancel = load_ilbm (path, *g, screen, FALSE);
newpointer(screen, NORMAL);

if (cancel!=AOK) {
   FreeMem(*g,sizeof(struct graphic));
   return(cancel);
   }
return(AOK);
}


/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (path, g, screen, alloc_chip)

struct graphic *g;
char *path;
struct Screen *screen;
int alloc_chip;

{
int i,f;
struct BitMap bitmap0;
ILBMFrame iFrame;
long xasp, yasp;
extern ExtAlloc(), ChipAlloc();


if ((f=Open(path,MODE_OLDFILE))==0) {
   return(CANTOPENFILE);
   }

/** The following assumes a grayscale picture. Change so that **/
/** color pictures are converted to true grayscales at this stage **/

#if 0
if (alloc_chip)
   i=ReadPicture(f, &bitmap0, &iFrame, ChipAlloc);
else
   i=ReadPicture(f, &bitmap0, &iFrame, ExtAlloc);
#endif

i=ReadPicture(f, &bitmap0, &iFrame, ExtAlloc);

Close(f);

if (i!=IFF_DONE) {
   return(BADIFF);
   }

i=iff_to_gray (screen,g, &bitmap0, &iFrame, alloc_chip); /** move iff data to graphic struct **/

RemFree(bitmap0.Planes[0]);
if (i!=AOK)
   return(i);
xasp = iFrame.bmHdr.xAspect;
yasp = iFrame.bmHdr.yAspect;

g->aspect=((xasp*(ULONG)g->w)<<15)/(yasp*(ULONG)g->h);

if (g->w==320 && g->h==200)
   g->aspect=(ASPLOW*(ULONG)g->w)/((ULONG)g->h);

if (g->w==640 && g->h==200)
   g->aspect=(ASPMED*(ULONG)g->w)/((ULONG)g->h);

if (g->w==320 && g->h==400)
   g->aspect=(ASPLOWLACE*(ULONG)g->w)/((ULONG)g->h);

if (g->w==320 && g->h==200)
   g->aspect=(ASPHI*(ULONG)g->w)/((ULONG)g->h);

return(AOK);
}


/*****************************
typedef WORD Color4;
typedef struct {
   ClientFrame clientFrame;
   UBYTE foundBMHD;
   UBYTE nColorRegs;
   BitMapHeader bmHdr;
   Color4 colorMap[32];
   } ILBMFrame;
*****************************/

iff_to_gray (screen,g, bitmap, iFrame, alloc_chip)

struct Screen *screen;
struct graphic *g;
struct BitMap *bitmap;
ILBMFrame *iFrame;

{
int i,j;
USHORT x,y,val,six_percent;
USHORT convert_flags;
static UBYTE translate[48];

for (i=0; i < 6; i++)
   g->planes[i] = 0;

if (bitmap->Depth==1)
   g->numplanes = 1;
else
   g->numplanes = 4;
g->h = iFrame->bmHdr.h;
g->w = iFrame->bmHdr.w;
g->useh = g->h;
g->usew = g->w;
g->usex = 0;
g->usey = 0;

/** Allocate memory for new bitplanes **/

for (i=0; i<g->numplanes; i++) {
   if (alloc_chip)
      g->planes[i]=(UBYTE *)AllocMem(RASSIZE(g->w,g->h),MEMF_CHIP|MEMF_CLEAR);
   else
      g->planes[i]=(UBYTE *)AllocMem(RASSIZE(g->w,g->h),MEMF_CLEAR);
   if (g->planes[i]==NULL) {
      for (j=0; j<6; j++)
         if (g->planes[j])
            FreeMem(g->planes[j],RASSIZE(g->w,g->h));
      return(OUTOFMEM);
      }
   }

/** Copy bitplane data so that it is alloced with AllocMem() **/

if (perfect_iff(iFrame)) {
   for (i=0; i<g->numplanes; i++) {
      movmem(bitmap->Planes[i], g->planes[i], RASSIZE(g->w,g->h));
      }
   if (iFrame->colorMap[0]==0)
      invert(g);
   }

else {
   convert_flags = gray_convert_options(screen);
   init_hist();

   if ((convert_flags&(GRAY_SAT|GRAY_OVERSAT))==0) {
      for (i=0; i < 48; i++)
         translate[i] = i/3;
      if (convert_flags&GRAY_NOWHITE) {
         translate[45] = 14;
         translate[46] = 14;
         translate[47] = 14;
         }
      }
   else {
      for (y=0; y < g->h; y++) {
         for (x=0; x < g->w; x++) {
            get_bitmap_info (bitmap, iFrame, x , y);
            }
         }
      }

   if (convert_flags & GRAY_OVERSAT) {
      six_percent = (g->h*g->w*6)/100;
      x = 0;i=0;
      while (x < six_percent)
         x = x + hist[i++];
      min_pix = (i-1);

      x = 0;i=47;
      while (x < six_percent)
         x = x + hist[i--];
      max_pix = (i+1);
      }

   if (convert_flags & (GRAY_SAT|GRAY_OVERSAT)) {
      for (i=0; i < min_pix; i++)
         translate[i] = 0;
      for (i=min_pix; i < 48; i++) {
         translate[i] = ((i-min_pix)*15)/(max_pix - min_pix);
         if (convert_flags & GRAY_NOWHITE) {
            if (translate[i] >= 14)
               translate[i] = 14;
            }
         else {
            if (translate[i] >= 15)
               translate[i] = 15;
            }
         }
      }

   for (y=0; y < g->h; y++) {
      for (x=0; x < g->w; x++) {
         val = get_bitmap_val (bitmap, iFrame, x , y);
         set_bitmap_val (g, x , y, translate[val]);
         }
      }
   invert(g);
   }
return(AOK);
}

get_bitmap_val (bitmap, iFrame, x , y)

struct BitMap *bitmap;
ILBMFrame *iFrame;
int x, y;

{

int off;
USHORT val,i,gray;
USHORT red, green, blue;
UBYTE byte,bit;
extern UBYTE bit_table[];     /** defined in dither.c **/
static UBYTE or_val_table[] = {1,2,4,8,16,32};
UBYTE or_val;

val = 0;
or_val = or_val_table[bitmap->Depth-1];

off = RASSIZE(iFrame->bmHdr.w, y) + (x>>3);
bit = bit_table[x&7];

/* printf("off bit depth %d %d %d\n",off,bit,bitmap->Depth); */

for (i=0; i < bitmap->Depth; i++) {
   byte = *(bitmap->Planes[i]+off);
   val = val>>1;
   if (byte&bit) {
      val = val|or_val;
      }
   }
/* printf("index = %d ",val);   */
val = iFrame->colorMap[val];
/* printf("cr = %x ",val); */

red   = val&15;
green = (val>>4)&15;
blue  = (val>>8)&15;
gray = (red + green + blue);

/* printf("red %x green %x blue %x gray %x\n",red,green,blue,gray); */

return(gray);
}

get_bitmap_info (bitmap, iFrame, x , y)

struct BitMap *bitmap;
ILBMFrame *iFrame;
int x, y;

{

int off;
USHORT val,i,gray;
USHORT red, green, blue;
UBYTE byte,bit;
extern UBYTE bit_table[];     /** defined in dither.c **/
static UBYTE or_val_table[] = {1,2,4,8,16,32};
UBYTE or_val;

val = 0;
or_val = or_val_table[bitmap->Depth-1];

off = RASSIZE(iFrame->bmHdr.w, y) + (x>>3);
bit = bit_table[x&7];

for (i=0; i < bitmap->Depth; i++) {
   byte = *(bitmap->Planes[i]+off);
   val = val>>1;
   if (byte&bit) {
      val = val|or_val;
      }
   }
val = iFrame->colorMap[val];

red   = val&15;
green = (val>>4)&15;
blue  = (val>>8)&15;
gray = (red + green + blue);

hist[gray]++;
if (max_pix < gray)
   max_pix = gray;

if (min_pix > gray)
   min_pix = gray;
}

init_hist()
{
short i;
for (i=0; i < 48; i++) {
   hist[i] = 0;
   /* ihist[i] = i; */
   }

min_pix = 999;
max_pix = 0;
}

#if 0
sort_hist()
{
short t,i,did_swap,last;

last = 48;
do {
   did_swap = FALSE;
   for (i=0; i < last; i++) {
      if (hist[ihist[i]] > hist[ihist[i+1]]) {
         t = ihist[i];
         ihist[i] = ihist[i+1];
         ihist[i+1] = t;
         did_swap = TRUE;
         }
      }
      last--;
   } while (did_swap);
}
#endif

set_bitmap_val (g, x , y, val)

struct graphic *g;
int x,y;

{

int off;
USHORT i;
register UBYTE byte , shift;

off = RASSIZE(g->w, y) + (x>>3);
shift = 7 - (x&7);

for (i=0; i < g->numplanes; i++) {
   byte = *(g->planes[i]+off) | ((val&1)<<shift);
   *(g->planes[i]+off) = byte;
   val = val>>1;
   }
}

perfect_iff(iFrame)

ILBMFrame *iFrame;

{
USHORT i;

/*
printf("nc %d\n",iFrame->nColorRegs);
printf("np %d\n",iFrame->bmHdr.nPlanes);
for (i=0; i<16; i++)
   printf("i=%d %x\n",i,iFrame->colorMap[i]); */

if (iFrame->bmHdr.nPlanes==1)
   return(TRUE);

if (iFrame->bmHdr.nPlanes!=4)
   return(FALSE);

for (i=0; i<16; i++)
   if (iFrame->colorMap[i] != ((i<<8)|(i<<4)|i) &&  /** normal **/
       iFrame->colorMap[15-i] != ((i<<8)|(i<<4)|i)) /** inverted? **/
      return(FALSE);

return(TRUE);
}

