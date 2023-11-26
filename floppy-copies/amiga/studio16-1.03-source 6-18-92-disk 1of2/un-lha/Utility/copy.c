#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <limits.h>
#include <stdio.h>
#include "/include/studio16.h"

extern struct StudioBase *StudioBase;

/*** !!!!! CHANGE NAMES TO BETTER CONVENTION. IE, FileSetLength,
				FileCopy  ****/

#if 0
/******************************************************/

void SetSampLength(samp, fp)
struct Disk_Samp *samp;
long fp;
{
Seek(fp, 0, OFFSET_END);
samp->length = Seek(fp, 0, OFFSET_CURRENT)/2;
Seek(fp, 0, OFFSET_BEGINNING);
}

/******************************************************/

int QuickCopy(tofp, fromfp, length)
long tofp, fromfp;
int length;
{
char *buf;
int tomove;

if (length==0)
	return(NO_ERROR);

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(StudioBase->defaults.copy_buf_size/2, length);
	if (Read(fromfp, buf, tomove*2)!=tomove*2)
		return(READ_ERROR);
	if (Write(tofp, buf, tomove*2)!=tomove*2)
		return(WRITE_ERROR);
	length = length - tomove;
	} while (length);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}
#endif 

/******************************************************/

int QuickCopyToSP(tosp, fromfp, length)
long fromfp;
struct SampleFilePointer *tosp;
int length;
{
char *buf;
int tomove;

if (length==0)
	return(NO_ERROR);

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(StudioBase->defaults.copy_buf_size/2, length);
	if (Read(fromfp, buf, tomove*2)!=tomove*2)
		return(READ_ERROR);
	if (SampleWrite(tosp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	length = length - tomove;
	} while (length);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}

/******************************************************/

int QuickCopyFromSP(tofp, fromsp, length)
long tofp;
struct SampleFilePointer *fromsp;
int length;
{
char *buf;
int tomove;

if (length==0)
	return(NO_ERROR);

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(StudioBase->defaults.copy_buf_size/2, length);
	if (SampleRead(fromsp, buf, tomove)!=tomove)
		return(READ_ERROR);
	if (Write(tofp, buf, tomove*2)!=tomove*2)
		return(WRITE_ERROR);
	length = length - tomove;
	} while (length);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}

/***************************************************/

int QuickSplitCopyFromSP(tofp, fromsp, len)
int tofp;
struct SampleFilePointer *fromsp;
int len;
{
char *buf;
char *bytebuf;
short *wordbuf;
int tomove;
short i;
long x;

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

bytebuf = buf;
wordbuf = (short *)buf;
do {
	tomove = min((StudioBase->defaults.copy_buf_size/2), len);	/* len is in words! */
	if (SampleRead(fromsp, buf, tomove)!=tomove)
		return(READ_ERROR);
	for (i=0; i < tomove; i++) {
		if (wordbuf[i] > 0) {
			bytebuf[i]=(wordbuf[i]>>8)&0xFF;
			}
		else {
			x=wordbuf[i];
			if (x < -SHRT_MAX) x++;	/** limits.h **/
			x=((-x)>>8)&0xFF;
			bytebuf[i]=-x;
			}
#if 0
	x=wordbuf[i];
	x=x/257;
	bytebuf[i]=x;
#endif
/*if (bytebuf[i]==1 || bytebuf[i]==-1)
	kprintf("%04x%04x\n",wordbuf[i]);*/
		}
	if (Write(tofp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	len = len - tomove;
	} while (len);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}
