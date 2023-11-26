#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/iff8svx.h"

void QuickSplit();
void Skip();

/************************************************/
/** 8SVX filter.  Called by studio/LoadSamp() ***/
/************************************************/

Load8SVX(in,sp)

long in;		/** in fp, out sp **/
struct SampleFilePointer *sp;

{
char *AllocMem();
int len,oldlen;
ULONG k,x;
UWORD e1,z;
static char line[80];
UBYTE comp=0;
UBYTE *lmem, *rmem;
char *tmem;
struct Disk_Samp *s;

s=sp->disksamp;

Read(in,&k,4);
if (k!=FORM) { 			        /** not in standard form */
	Seek(in,-4L,OFFSET_CURRENT); /** backup to call point **/
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}

else {
   Read(in,(char *)&x,4);    /* skip length */
   Read(in,(char *)&x,4);    /* get form type */
   if (x!=SVX) {
		Seek(in,-12L,OFFSET_CURRENT); /** backup to call point **/
      return(STDERR_UNKNOWN_FILE_FORMAT);
      }

   s->type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   Read(in,(char *)&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      Skip(in);
      Read(in,(char *)&x,4);
      if (x==-1 || x==0) {
         return(BAD_IFF);
         }
      }

   if (x==CHAN) {
      Read(in,(char *)&x,4);
      if (x!=4) {
         return(BAD_IFF);
         }
      Read(in,(char *)&s->type,4);
      if (s->type!=STEREO && s->type!=RIGHT
          && s->type!=LEFT) s->type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      Read(in,(char *)&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         return(BAD_IFF);
         }
      Read(in,(char *)&x,4);   /* oneshot:should be length of sample */
      Read(in,(char *)&x,4);   /* repeat */
      Read(in,(char *)&x,4);   /* cycles */
      Read(in,(char *)&z,2);   /* get samples per second */
      Read(in,(char *)&x,1);
      sprintf(line,"IFF at %d samples per second",z);
      telluser(line,0,0);
      s->parms.rate=z;
		s->parms.filter3db=z/2;
		s->parms.volume=ZERO_DB;
      Read(in,(char *)&comp,1);
      Seek(in,4L,OFFSET_CURRENT);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   Read(in,(char *)&len,4);
   if (s->type==STEREO)
      len=len/2;
   }

/* read in data--either BODY or dump */

if (comp==1) {oldlen=len;len=(len-2)*2;}
lmem=AllocMem(fix_len(len),0);
if (lmem==0) {
   return(OUT_OF_MEMORY);
   }
rmem=lmem;
if (comp==0)  
	e1=Read(in,lmem,len);     /* read in sample */
if (comp==1)
	e1=CRead(in,lmem,oldlen);     /* read in sample */
if (e1==-1) {
	return(READ_ERROR);
   }

/* get sample name */

len=fix_len(len);

tmem = AllocMem(len*2, 0);
if (tmem) {
	QuickSplit(tmem,lmem,len);
	SampleWrite(sp, tmem, len);
	FreeMem(tmem, len*2);
	}
FreeMem(lmem,len);

if (comp==1) telluser("compressed IFF file",0,0);
return(NO_ERROR);
}

/** force a length to be even by increasing the length one if needed **/

fix_len(i)
int i;
{
int even;

even=0xFFFFFFFE&i;
if (i==even)
   return(i);
else
   return(i+1);
}

/**********************************************************/

void QuickSplit(tmem,lmem,len)
short tmem[];
char lmem[];
int len;
{
int i;

for (i=0; i < len; i++) {
	tmem[i] = lmem[i]<<8;
/*	tmem[i] = tmem[i]&0xFF00;*/
	}
}

/**********************************************************/

/****************************/
/* compresed data read    ***/
/****************************/

CRead(fp,data,len)
int fp;
register int len;
register char *data;
{
BYTE x,d;
register LONG i,j;
BYTE buf[512];
static short delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

Read(fp,&d,1);   /* skip padding */
Read(fp,&x,1);   /* get inital value */

len=len-2;
if (Read(fp,buf,min(512,len))==-1) return(-1);
j=0;
for (i=0;i<len;i++) {
   *data=x+delta[15&(buf[j]>>4)];
   x=*data++;
   *data=x+delta[buf[j++]&15];
   x=*data++;
   if (j==512) {
      j=0;
      if (Read(fp,buf,min(512,(len-i-1)))==-1) return(-1);
      }
   }
return(0);
}

/**********************************************************/

/**********************/
/**    skip a form    */
/**********************/

void Skip(in)
int in;

{
ULONG x;
Read(in,&x,4);  /* get lenth */
Seek(in,x,OFFSET_CURRENT);  /* seek past it */
if ((x&1)==1)
   Seek(in,1L,OFFSET_CURRENT);  /* skip padding if odd */
}

