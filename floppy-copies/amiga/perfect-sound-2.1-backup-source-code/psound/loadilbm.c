#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "fcntl.h"
#include "df1:sed.h"

/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (fname,iff,cmap)

struct Image *iff;
char *fname;
UBYTE *cmap;

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
   close(in);
   return(-1);
   }

nextchunk:

read(in,&x,4);     /* get chunk type */
while (x!=BODY && x!=CMAP && x!=BMHD) {    /* skip if not what we want */
   skip(in);
   read(in,&x,4);
   if (x==-1 || x==0) {
      close(in);
      return(-1);
      }
   }

if (x==CMAP) {
   read(in,&len,4);
   read(in,cmap,len);
   if ((len&1)==1) lseek(in,1L,1);  /* skip padding bytes */
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
   goto nextchunk;
   }

if (x==BODY) {
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
         if (z!=0) {
            printf("ERROR! compression fault.\n");
            exit(20);
            }
         }

   FreeMem(comp,templen);
   close (in);
   return(0);
   }
printf("ERROR: How did i get here?\n\n");
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
