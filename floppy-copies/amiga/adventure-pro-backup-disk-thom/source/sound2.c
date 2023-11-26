
#include <exec/types.h>
#include <stdio.h>
#include <exec/exec.h>
#include <fcntl.h>
#include <intuition/intuition.h>
#include "libraries/dos.h"
#include "df0:sed1.h"


#define SHOW 8      /* no. of samples to display in load requester */
#define IHI 50      /* height of getint requester */
#define SHI 30

WORD delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};



/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,current,samp,fname)

int *current;
struct Samp  *samp;
struct Window *Window;
char *fname;

{
int in,i,len;
ULONG k,x;
UWORD e1,e2,z;
long pos;
static char line[80];
UBYTE comp=0;

i = 0;
if ((in=(int)open(fname,O_RDONLY))==-1) {
   printf("Can't open that file.");
   return(-1);
   }

read(in,&k,4);
if (k!=FORM) {         /* not in standard form */
   samp->rate=350;           /* default length */
   samp->type=LEFT;          /* use left (ch 0) as default */
   pos=lseek(in,0L,2);
   samp->length=(int)pos;
   printf("No playback rate specified -- 350 used");
   lseek(in,0L,0);    /* rewind */
   }

else {
   read(in,&x,4);    /* skip length */
   read(in,&x,4);    /* get form type */
   if (x!=SVX) {
      printf("File is IFF, but not form 8SVX !!");
      close(in);
      return(-1);
      }

   samp->type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   read(in,&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      skip(in);
      read(in,&x,4);
      if (x==-1 || x==0) {
         printf("Bad IFF file--Can't load.");
         close(in);
         return(-1);
         }
      }

   if (x==CHAN) {
      read(in,&x,4);
      if (x!=4) {
         printf("Bad CHAN length.");
         close(in);
         return(-1);
         }
      read(in,&samp->type,4);
      if (samp->type!=STEREO && samp->type!=RIGHT
          && samp->type!=LEFT) samp->type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      read(in,&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         printf("Format error--invalid length for VHDR");
         close (in);
         return(-1);
         }
      read(in,&samp->oneshot,4); /* should be length of sample */
      read(in,&samp->repeat,4);
      read(in,&samp->cycles,4);
      read(in,&z,2);   /* get samples per second */
      read(in,&samp->ctoctave,1);
      samp->rate=(int)(1.0/(float)z*3579546.5);
      read(in,&comp,1);
      lseek(in,4L,1);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   read(in,&samp->length,4);
   if (samp->type==STEREO) samp->length=samp->length>>1;
   }

/* read in data--either BODY or dump */

len=samp->length;
if (comp==1) len=(len-2)*2;
samp->lmem=AllocMem(len,MEMF_CHIP);
if (samp->lmem==0) {
   printf("File to large for available memory.");
   close(in);
   return(-1);
    }

   if (comp==0)  
      e1=read(in,samp->lmem,samp->length);     /* read in sample */
   if (comp==1)
      e1=cread(in,samp->lmem,samp->length);     /* read in sample */

close(in);

if (e1==-1 || e2==-1) {
   printf("ERROR loading file!");
   return(-1);
   }

return(0);
}

/****************************/
/* compresed data read    ***/
/****************************/
cread(fp,data,len)
int fp;
register int len;
register char *data;
{
BYTE x,d;
register LONG i,j;
BYTE buf[512];

read(fp,&d,1);   /* skip padding */
read(fp,&x,1);   /* get inital value */

len=len-2;
if (read(fp,buf,min(512,len))==-1) return(-1);
j=0;
for (i=0;i<len;i++) {
   *data=x+delta[15&(buf[j]>>4)];
   x=*data++;
   *data=x+delta[buf[j++]&15];
   x=*data++;
   if (j==512) {
      j=0;
      if (read(fp,buf,min(512,(len-i-1)))==-1) return(-1);
      }
   }
return(0);
}


wipe_it(samp)
struct Samp *samp;
{

if (samp->lmem)
   FreeMem(samp->lmem,samp->length);

samp->name = 0;
samp->lmem = 0;

}



