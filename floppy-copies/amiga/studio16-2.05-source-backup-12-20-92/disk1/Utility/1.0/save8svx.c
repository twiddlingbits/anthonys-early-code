#include <exec/types.h>
#include <stdio.h>
#include "/include/psound.h"

/********************************/
/*  8SVX 8 bit save filter     **/
/********************************/

Save8SVX(fp,rsp)

long fp;
struct SampleFilePointer *rsp;
{
ULONG i,len,namelen;
UWORD SampRate;
UBYTE k,pad;
short comp= FALSE;
int error;
struct Disk_Samp *samp;

samp=rsp->disksamp;

len=samp->length&(~1);
if (samp->type==STEREO) len=len*2;

SampRate=(UWORD)samp->parms.rate;

/**************************/
if(4!=Write(fp,"FORM",4)) 	return(WRITE_ERROR);

namelen=strlen(samp->name);
if (namelen&1) {namelen++;pad=1;}	/** pad odd length with zero **/
else {pad=0;}

if (comp==FALSE) i=4+28+76+8+len+12+namelen+8;
else
	{
	i=4+28+76+8+(len>>1)+2+12+namelen;
	if (samp->type==STEREO) i=i+2;
	}

if(4!=Write(fp,&i,4)) 		return(WRITE_ERROR);
/**************************/
if(8!=Write(fp,"8SVXVHDR",8)) 	return(WRITE_ERROR);
i=20; 
if(4!=Write(fp,&i,4)) 		return(WRITE_ERROR);

if(4!=Write(fp,&len,4))		return(WRITE_ERROR);	/* # of samples field */
i=0;
if(4!=Write(fp,&i,4)) 		return(WRITE_ERROR);	/* repeat */
if(4!=Write(fp,&i,4)) 		return(WRITE_ERROR);	/* cycles */
if(2!=Write(fp,&SampRate,2))	return(WRITE_ERROR);	/* samples per second */
k=1;
if(1!=Write(fp,&k,1))		return(WRITE_ERROR);	/* one octave = oneshot */
k=0; if (comp==TRUE) k=1;
if(1!=Write(fp,&k,1))		return(WRITE_ERROR);	/* compression */
i=0x10000;
if(4!=Write(fp,&i,4))		return(WRITE_ERROR);	/*max volume */
/**************************/
if(4!=Write(fp,"NAME",4))	return(WRITE_ERROR);
if(4!=Write(fp,&namelen,4))		return(WRITE_ERROR);
Write(fp,samp->name,namelen-pad);
if (pad==1) {
	i=0;
	Write(fp,&i,1);
	}
/**************************/
if(4!=Write(fp,"ANNO",4))	return(WRITE_ERROR);
i=68; 
if(4!=Write(fp,&i,4))		return(WRITE_ERROR);
if(68!=Write(fp,"Recorded with STUDIO 16     from SunRize Industries.  (408) 374-4962",68)) return(WRITE_ERROR);
/**************************/
if(4!=Write(fp,"CHAN",4)) 	return(WRITE_ERROR);
i=4;
if(4!=Write(fp,&i,4))		return(WRITE_ERROR);		/* Chunk length */
if(4!=Write(fp,&samp->type,4))	return(WRITE_ERROR);	/* L/R Stereo */
/**************************/
if(4!=Write(fp,"BODY",4))	return(WRITE_ERROR);
if (comp==TRUE) len=(len>>1)+2;
if (samp->type==STEREO) len=len*2;
if(4!=Write(fp,&len,4)) 	return(WRITE_ERROR);

error=QuickSplitCopyFromSP(fp, rsp, len);
return(error);
}
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/
/**********************************************************/

#if 0
if (samp->type==LEFT || samp->type==STEREO) {
   if (comp==FALSE)*/
		error=QuickSplitCopy(fp, rfp, len);
   if (comp==TRUE)  error=cWrite(fp,samp->lmem,len);
   if (error!=NO_ERROR) {
      return(error);
      }
   }

if (samp->type==RIGHT || samp->type==STEREO) {
   if (comp==FALSE) i=Write(fp,samp->rmem,len);
   if (comp==TRUE)  i=cWrite(fp,samp->rmem,len);
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
/**********************************************************/

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

for (k=0;k<i;k++) 
	{
	for (j=1;j<16;j++) if ((prev+delta[j]) > (WORD)*data) break;
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
	if (bpos==512) 
		{
		bpos=0;
		if (Write(fp,buf,512)==-1) return(-1);
		}
	}
	if (bpos!=0)
		if (Write(fp,buf,bpos)==-1) return(-1);
return(pererr*100/len);
}
/**********************************************************/

#endif
