#include <exec/types.h>
#include <stdio.h>
#include "/include/psound.h"

/********************************/
/*  8SVX 8 bit save filter 	 **/
/********************************/

Save8SVX(fp,rsp)

long fp;
struct SampleFilePointer *rsp;
{
ULONG i,len,evenlen;
UWORD j;
UBYTE k;
short comp;
int error;
struct Disk_Samp *samp;

samp=rsp->disksamp;

comp = FALSE;

evenlen=samp->length&(~1);   /*make length an even number */
len=evenlen;
if (samp->type==STEREO)
	len=len*2;

j=(UWORD)samp->parms.rate;

Write(fp,"FORM",4);
if (comp==FALSE) i=4+28+76+8+len+12;
else {
   i=4+28+76+8+(len>>1)+2+12;
   if (samp->type==STEREO) i=i+2;
   }
Write(fp,&i,4);
Write(fp,"8SVXVHDR",8);
i=20; Write(fp,&i,4);

Write(fp,&len,4);    /* # of samples field */
i=0;
Write(fp,&i,4);
Write(fp,&i,4);
Write(fp,&j,2);

k=1;
Write(fp,&k,1);

k=0;if (comp==TRUE) k=1;
Write(fp,&k,1);   /* compression */
i=0x10000;   /*max volume */
Write(fp,&i,4);

Write(fp,"ANNO",4);
i=68; Write(fp,&i,4);
Write(fp,"Recorded with STUDIO 16     from SunRize Industries.  (408) 374-4962",
          68);

/* The following new chuck CHAN is 4 bytes long and is used to store
  channel information (RIGHT, LEFT, STEREO --see sed1.h) */

Write(fp,"CHAN",4);  /* non standard channel chunck */
i=4;Write(fp,&i,4);  /* length */
Write(fp,&samp->type,4);

Write(fp,"BODY",4);
len=evenlen;
if (comp==TRUE) len=(len>>1)+2;
if (samp->type==STEREO) len=len*2;
Write(fp,&len,4);

error=QuickSplitCopyFromSP(fp, rsp, evenlen);
return(error);
}

#if 0
if (samp->type==LEFT || samp->type==STEREO) {
   if (comp==FALSE)*/
		error=QuickSplitCopy(fp, rfp, evenlen);
   if (comp==TRUE)  error=cWrite(fp,samp->lmem,evenlen);
   if (error!=NO_ERROR) {
      return(error);
      }
   }

if (samp->type==RIGHT || samp->type==STEREO) {
   if (comp==FALSE) i=Write(fp,samp->rmem,evenlen);
   if (comp==TRUE)  i=cWrite(fp,samp->rmem,evenlen);
   if (i==-1) {
      show_msg(Window,186);
      close(fp);
      return(-1);
      }
   }
return(NO_ERROR);
}                              

/*******************************/
/** compressed Write          **/
/*******************************/

CWrite(fp,data,len)
int fp,len;
BYTE *data;
{
WORD prev=0;
register int pererr=0;
register int i;
register int k,j;
int bpos;
char val;
BYTE buf[512];
static short delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

if (Write(fp,&prev,1)==-1) return(-1);   /* padding */
prev=*data;
if (Write(fp,data,1)==-1) return(-1);    /* 1st value */
i=len/2;
bpos=0;

for (k=0;k<i;k++) {
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=(val<<4)|j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;

   buf[bpos++]=val;
   if (bpos==512) {
      bpos=0;
      if (Write(fp,buf,512)==-1) return(-1);
      }
   }
   if (bpos!=0)
      if (Write(fp,buf,bpos)==-1) return(-1);
return(pererr*100/len);
}

#endif
 