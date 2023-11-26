#include <exec/types.h>
#include <stdio.h>
#include <exec/exec.h>
#include <fcntl.h>
#include <intuition/intuition.h>
#include "libraries/dos.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define FORM MakeID('F','O','R','M')
#define BODY MakeID('B','O','D','Y')
#define BMHD MakeID('B','M','H','D')
#define CMAP MakeID('C','M','A','P')
#define ILBM MakeID('I','L','B','M')

UBYTE cmap[32*3];
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

main()
{
int i;
char *fname;
struct Image image;
struct Screen *screen;

fname="dragon";

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",29);
if (IntuitionBase==0) {
   printf("Cant Open intution!\n");
   exit (10);
   }

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",29);
if (GfxBase==0) {
   printf("Cant open graphics library!\n");
   exit(10);
   }

printf("Trying to load dragon...\n");
load_ilbm(fname,&image);

printf("Opening Screen...\n");
screen=openscr(image.Width,image.Depth);
if (screen==0) {
   printf("ERROR... screen not opened\n");
   exit(0);
   }
printf("Screen opened...\n");

printf("Setting color registers\n");

for (i=0;i<16*3;i=i+3) {
   printf("colors %d %d %d",cmap[i],cmap[i+1],cmap[i+2]);
   SetRGB4(screen->ViewPort,i/3,cmap[i]>>4,cmap[i+1]>>4,cmap[i+2]>>4);
   }

printf("drawing Image...\n");
DrawImage(&screen->RastPort,&image,0,0);
printf("OK... DONE\n");
xx: goto xx;
}


/***************************/
/** Open a screen         **/
/***************************/

openscr(width,depth)
SHORT width,depth;
{
struct NewScreen cust;
static struct TextAttr thefont=
   {
   "topaz.font",
   TOPAZ_SIXTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

cust.LeftEdge=0;
cust.TopEdge=0;
cust.Width=width;
cust.Height=200;
cust.Depth=depth;
cust.DetailPen=2;
cust.BlockPen=3;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
cust.Type=CUSTOMSCREEN;
cust.Font=&thefont;
cust.DefaultTitle="My dumb title";
cust.Gadgets=0;
cust.CustomBitMap=0;

return((int)OpenScreen(&cust));
}



/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (fname,iff)

struct Image *iff;
char *fname;

{
ULONG x;
UBYTE planes;
int plen,rowlen,len;
int templen;
register int i,z;
int offset,in,k;
BYTE *comp;
static int p_tab[]={1,3,7,15,31,63};

iff->LeftEdge=0;
iff->TopEdge=0;
iff->NextImage=0;
iff->PlaneOnOff=0;

if ((in=(int)open(fname,O_RDONLY))==-1)
   return(-1);

read(in,&k,4);

if (k!=FORM) return (-1);

read(in,&x,4);    /* skip length */
read(in,&x,4);    /* get form type */
if (x!=ILBM) {
   printf("File is IFF, but not form ILBM !!\n");
   close(in);
   return(-1);
   }

nextchunk:

read(in,&x,4);     /* get chunk type */
while (x!=BODY && x!=CMAP && x!=BMHD) {    /* skip if not what we want */
   skip(in);
   read(in,&x,4);
   if (x==-1 || x==0) {
      printf("Bad IFF file--Can't load.");
      close(in);
      return(-1);
      }
   }

if (x==CMAP) {
   read(in,&len,4);
   read(in,&cmap,len);
   if ((len&1)==1) lseek(in,1L,1);  /* skip padding bytes */
   printf("Read CMAP...\n");
   goto nextchunk;
   }
   
if (x==BMHD) {
   lseek(in,4L,1);   /* skip length */
   read(in,&iff->Width,2);
   read(in,&iff->Height,2);
   lseek(in,4L,1);
   read(in,&planes,1);
   iff->Depth=(SHORT)planes;
   iff->PlanePick=p_tab[planes];
   lseek(in,11L,1);
   printf("Read BMHDR...\n");
   goto nextchunk;
   }

if (x==BODY) {
   printf("Starting BODY...\n");
   printf("w,h,d %d %d %d\n",iff->Width,iff->Height,iff->Depth);
   read(in,&templen,4);
   comp=AllocMem(templen,0);
   if (comp==0) {
      printf("ERROR! not enough memory to load iff file!\n");
      exit(0);
      }
   read(in,comp,templen);

   rowlen=iff->Width/8;
   if (rowlen*8!=iff->Width) rowlen++;
   plen=rowlen*iff->Height;
   iff->ImageData=AllocMem(plen*iff->Depth,MEMF_CHIP);   /* change this? */
   if (iff->ImageData==0) {
      printf("ERROR! Out of memory\n");
      exit(10);
      }
   offset=0;
   for (i=0;i<iff->Height;i++)
      for(k=0;k<iff->Depth;k++) {
         z=uncomp(&offset,comp+offset,(BYTE *)iff->ImageData+rowlen*i+k*plen,rowlen);
         if (z!=0) printf("ERROR! compression fault.\n");
         }

   FreeMem(comp,templen);
   close (in);
   return(0);
   }
printf("ERROR: How did i get here?\n\n");
}

/**********************/
/**    skip a form    */
/**********************/

skip(in)
int in;

{
ULONG x;
read(in,&x,4);  /* get lenth */
lseek(in,x,1);  /* seek past it */
if ((x&1)==1)
   lseek(in,1L,1);  /* skip padding if odd */
}

/*********************************************************************/
/*       Uncompress one row                                          */
/* Control Bytes:                                                    */
/*    [0..127]   : followed by n+1 bytes of data.                      */
/*    [-1..-127] : followed by byte to be repeated (-n)+1 times.       */
/*    -128       : NOOP.                                               */
/*********************************************************************/

uncomp(offset,src,dest,width)

register char *src,*dest;
int *offset,width;
{
register char n,val;


while (width>0) {
   if ((n=*src)>0) {
   src++;
   (*offset)++;
   
      while (n-->=0) {
         *dest=*src;
         dest++; src++; (*offset)++; width--;
         }
      }
   else {
      src++;
      (*offset)++;
      val=*src;
      src++;
      (*offset)++;
      while (n++<=0) {
         *dest=val; width--;
         dest++;
         }
      }     /* end else */
   }        /* end 1st while */

return (width);
}

   


