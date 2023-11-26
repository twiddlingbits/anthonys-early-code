#include "exec/types.h"
#include "/include/studio16.h"

/**************************/
/*  12 bit save filter 	 **/
/**************************/

Save12Bit(wfp,rsp)

struct Disk_Samp *samp;
long wfp,rfp;
{
return(Quick12BitCopy(wfp,rfp,samp->length));
}

/***************************************************/

#define QC_BLOCKSIZE 4096			/** in bytes **/

int Quick12BitCopy(tofp, fromfp, len)
int tofp;
int fromfp;
int len;
{
char *buf;
char *bytebuf;
short *wordbuf;
int tomove;
short i;

buf = (char *)AllocMem(QC_BLOCKSIZE, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

bytebuf = buf;
wordbuf = (short *)buf;
do {
	tomove = min(QC_BLOCKSIZE/2, len);	/* len is in words! */
	if (Read(fromfp, buf, tomove*2)!=tomove*2)
		return(READ_ERROR);
	for (i=0; i < tomove; i++)
		bytebuf[i]=(wordbuf[i]>>8)&0xFF;
	if (Write(tofp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	len = len - tomove;
	} while (len);

FreeMem(buf, QC_BLOCKSIZE);
return(NO_ERROR);
}
