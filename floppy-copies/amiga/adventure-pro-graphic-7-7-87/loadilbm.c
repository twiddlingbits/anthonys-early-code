#include <exec/types.h>
#include "stdio.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "libraries/dos.h"
#include "intuition/intuition.h"
#include "adventure.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define FORM MakeID('F','O','R','M')
#define BODY MakeID('B','O','D','Y')
#define BMHD MakeID('B','M','H','D')
#define CMAP MakeID('C','M','A','P')
#define ILBM MakeID('I','L','B','M')


struct graphic *graphicload(Window)

struct Window *Window;

{
static char fname[34]={0};
static char drawer[35]={0};
char path[80];
char *cancel;
struct graphic *graphic;



cancel=get_fname(Window,Window->WScreen,"Load A Graphic",fname,drawer);

if (cancel==NULL) return(0);

path[0]=0;
add(path,drawer);  /* connect drawer and filename into a path */
add(path,fname);

if ((graphic=(struct graphic *)AllocMem(sizeof(struct graphic),0))==0)
      return(0);

if (load_ilbm(path,graphic)==-1) {
   FreeMem(graphic,sizeof(struct graphic));
   return(0);
   }
return(graphic);
}


/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (fname,iff)

struct graphic *iff;
char *fname;

{
ULONG x;
UBYTE planes,mask,compmethd;
int plen,rowlen,len;
int templen;
register int i,z;
int offset,in,k;
BYTE *comp;
WORD cmap[16*3];

printf("%s\n",fname);
printf("Boo1.");
if ((in=(int)open(fname,O_RDONLY))==-1)
   return(-1);
printf("Boo2.");

read(in,&k,4);

if (k!=FORM) return (-1);
printf("Boo3.");

read(in,&x,4);    /* skip length */
read(in,&x,4);    /* get form type */
if (x!=ILBM) { /* file is iff but not ILBM */
   close(in);
   return(-1);
   }
printf("Boo4.");

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
   goto nextchunk;
   }
   
if (x==BMHD) {
   lseek(in,4L,1);   /* skip length */
   read(in,&iff->w,2);
   read(in,&iff->h,2);
   lseek(in,4L,1);
   read(in,&planes,1);
   read(in,&mask,1);
   read(in,&compmethd,1);
   iff->planenum=(int)planes;
   lseek(in,9L,1);
   goto nextchunk;
   }

if (x==BODY) {
   read(in,&templen,4);
   comp=(BYTE *)AllocMem(templen,0);
   if (comp==0) {
      printf("ERROR! not enough memory to load iff file!\n");
      return(-1);
      }
   read(in,comp,templen);
   rowlen=((iff->w+15)>>4) << 1;
   printf("w%d,h%d,planes%d,ptr%d\n",iff->w,iff->h,iff->planenum,iff->bitmap);
   plen=rowlen*iff->h;
   iff->bitmap=(UBYTE *)AllocMem(plen*iff->planenum,0);
   if (iff->bitmap==0) {
      printf("ERROR! Out of memory\n");
      FreeMem(comp,templen);
      return(-1);
      }
   offset=0;
   for (i=0;i<iff->h;i++)
      for(k=0;k<iff->planenum;k++) {
         if (compmethd==1) {
            z=uncomp(&offset,comp+offset,
                     (BYTE *)iff->bitmap+rowlen*i+k*plen,rowlen);
            if (z!=0) printf("ERROR! compression fault.\n");
            }
         else {
            movmem(comp+offset,(BYTE *)iff->bitmap+plen*k+rowlen*i,rowlen);
            offset=offset+rowlen;
            }
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

