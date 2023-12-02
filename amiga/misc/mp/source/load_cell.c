#include <exec/types.h>
#include "stdio.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "libraries/dos.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "mp.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define FORM MakeID('F','O','R','M')
#define BODY MakeID('B','O','D','Y')
#define BMHD MakeID('B','M','H','D')
#define CMAP MakeID('C','M','A','P')
#define ILBM MakeID('I','L','B','M')


/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_cell (fname, ap, cell_num)

char *fname;
struct anim_pointer *ap;
int cell_num;

{
ULONG x,xasp,yasp;
UBYTE mask,compmethd;
UWORD c1,tc;   /** transparant color **/
UBYTE temp;
int plen,rowlen,len;
int templen;
register int i,z;
int offset,in,k;
BYTE *comp,*bitmap;
UBYTE cmap[32*3];
USHORT s1,s2,d;
static USHORT final[100];

if ((in=(int)open(fname,O_RDONLY))==-1)
   return(FILENOTFOUND);

read(in,&k,4);

if (k!=FORM) return (NOTIFF);

read(in,&x,4);    /* skip length */
read(in,&x,4);    /* get form type */
if (x!=ILBM) {    /* file is iff but not ILBM */
   close(in);
   return(BADIFF);
   }

nextchunk:

read(in,&x,4);     /* get chunk type */
while (x!=BODY && x!=CMAP && x!=BMHD) {    /* skip if not what we want */
   skip(in);
   read(in,&x,4);
   if (x==-1 || x==0) {
      close(in);
      return(BADIFF);
      }
   }

if (x==CMAP) {
   read(in,&len,4);
   read(in,&cmap,len);
   if ((len&1)==1) lseek(in,1L,1);  /* skip padding bytes */
   goto nextchunk;
   }
   
if (x==BMHD) {
   lseek(in,4L,1);   /* skip length */
   read(in,&ap->w,2);
   read(in,&ap->h,2);
   lseek(in,4L,1);
   read(in,&ap->planes,1);
   read(in,&mask,1);
printf("mask=%d\n",mask);
   read(in,&compmethd,1);
   lseek(in,1L,1);
   read(in,&tc,2);    /* transparent color for mask */
   read(in,&temp,1);  /* x aspect */
   xasp=temp;
   read(in,&temp,1);  /* y aspect */
   lseek(in,4L,1);
   goto nextchunk;
   }

if (x==BODY) {
   read(in,&templen,4);
   comp=(BYTE *)AllocMem(templen,0);
   if (comp==0) {
      return(OUTOFMEM);
      }
   read(in,comp,templen);
   rowlen=RASSIZE(ap->w,1);
   plen=rowlen*ap->h;            /** 12 bytes extra for pointer **/
   bitmap=(UBYTE *)AllocMem(12 + plen*ap->planes,MEMF_CHIP);
   if (bitmap==0) {
      FreeMem(comp,templen);
      return(OUTOFMEM);
      }
   ap->cell_data[cell_num] = bitmap;
   offset=0;
   for (i=0;i<ap->h;i++)
      for(k=0;k < ap->planes;k++) {
         if (compmethd==1) {
            z=uncomp(&offset,comp+offset,
                     bitmap+rowlen*i+k*plen,rowlen);
            if (z!=0) printf("Comp fault!\n");
            }
         else {
            movmem(comp+offset,bitmap+plen*k+rowlen*i,rowlen);
            offset=offset+rowlen;
            }
         }
   FreeMem(comp,templen);
   close (in);
   final[0] = 0;
   final[1] = 0;
   final[ap->h] = 0;
   final[ap->h+1] = 0;
   final[ap->h+2] = 0;
   final[ap->h+3] = 0;
   for (i=0; i<ap->h; i++) {
      s1 = ((USHORT *)bitmap)[i];
      s2 = ((USHORT *)bitmap)[i+ap->h];
printf("s1 s2 %x %x\n",s1,s2);
      final[i*2+2] = s1;
      final[i*2+3] = s2;
      }
   movmem(final, bitmap, plen*2+12);
   if (ap->planes!=2 || ap->w != 16)
      printf("Sprite should be 2 planes deep with a width of 16\n");
   for (i=0; i < 12; i++)
      ap->cmap[i] = cmap[i]>>4;
   return(AOK);
   }
printf("PRG_ERROR\n");
return(CANCEL);
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

