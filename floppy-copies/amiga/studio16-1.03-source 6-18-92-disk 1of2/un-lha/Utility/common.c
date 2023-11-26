#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/iff8svx.h"
#include "clib/macros.h"

char *AllocMem();
/**********************************************************/

fix_len(i)
int i;
{
if (i==0xFFFFFFFE&i) return(i);
else return(i+1);
}

/**********************************************************/

void SkipChunk(in)
int in;
{
ULONG x;

Read(in,&x,4);  /* get lenth */
Seek(in,x,OFFSET_CURRENT);  /* seek past it */
if ((x&1)==1) Seek(in,1L,OFFSET_CURRENT);  /* skip padding if odd */
}

/**********************************************************/

void QuickSplit(tmem,lmem,len)
short tmem[];
char lmem[];
int len;
{
int i;
for (i=0; i < len; i++) tmem[i] = lmem[i]<<8;
}

/**********************************************************/

/****************************/
/* compresed data read    ***/
/****************************/

CRead(fp,data,len,last)
int fp;
register int len;
register char *data;
BYTE *last;
{
BYTE x=*last;
register LONG i,j;
BYTE buf[512];
static short delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

if (Read(fp,buf,min(512,len))!=min(512,len)) return(0);
j=0;
for (i=0;i<len;i++) 
	{
	*data=x+delta[15&(buf[j]>>4)];
	x=*data++;
	*data=x+delta[buf[j++]&15];
	x=*data++;
	if (j==512) 
		{
		j=0;
		if ( Read(fp,buf,min(512,(len-i-1))) != min(512,(len-i-1)) ) return(0);
		}
	}
*last=x;
return(len);
}


/**********************************************************/
extern struct StudioBase *StudioBase;
/**********************************************************/
int Transfer8BitBody(in,sp,len,comp)
long in;
struct SampleFilePointer *sp;
int len;
UBYTE comp;
{
int tlen,rlen;
UBYTE *lmem;
UBYTE *tmem;
BYTE last;
if (comp==1) len=(len-2)*2;

if (0==(lmem=AllocMem(StudioBase->defaults.copy_buf_size/2,0))) return(OUT_OF_MEMORY);
if(0==(tmem=AllocMem(StudioBase->defaults.copy_buf_size, 0))) {FreeMem(lmem,len);return(OUT_OF_MEMORY);}

tlen=0;
if (comp==1)
	{
	if(1!=Read(in,&last,1))	return(READ_ERROR);/* skip padding */
	if(1!=Read(in,&last,1))	return(READ_ERROR);/* get inital value */
	}

while (len!=tlen)
	{
	rlen=MIN(StudioBase->defaults.copy_buf_size/2,len-tlen);
	tlen+=rlen;

	if (comp==1)
		{
		 if(CRead(in,lmem,rlen/2,&last)!=rlen/2)
			{
			FreeMem(tmem,StudioBase->defaults.copy_buf_size);
			FreeMem(lmem,StudioBase->defaults.copy_buf_size/2);
			return(READ_ERROR);
			}
		}
	else 
		{
		if(rlen!=Read(in,lmem,rlen))
			{
			FreeMem(tmem,StudioBase->defaults.copy_buf_size);
			FreeMem(lmem,StudioBase->defaults.copy_buf_size/2);
			return(READ_ERROR);
			}
		}

	QuickSplit(tmem,lmem,rlen);

	if (rlen!=SampleWrite(sp, tmem, rlen))
		{
		FreeMem(tmem,StudioBase->defaults.copy_buf_size);
		FreeMem(lmem,StudioBase->defaults.copy_buf_size/2);
		return(WRITE_ERROR);
		}
	}

FreeMem(tmem,StudioBase->defaults.copy_buf_size);
FreeMem(lmem,StudioBase->defaults.copy_buf_size/2);
return(NO_ERROR);
}
/**********************************************************/
