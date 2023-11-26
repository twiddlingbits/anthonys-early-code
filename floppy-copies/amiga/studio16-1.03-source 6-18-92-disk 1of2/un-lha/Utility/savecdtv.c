#include <exec/types.h>
#include <stdio.h>
#include "/include/studio16.h"

/********************************/
/*  CDTV 16 bit save filter 	 **/
/********************************/

SaveCDTV(wfp,rsp)

long wfp;
struct SampleFilePointer *rsp;
{
return(QuickTransFromSP(wfp,rsp,rsp->disksamp->length));
}

/************************************************/
/* transpose high order & low order bytes,**/
/*  plus duplicate for stereo **/
/************************************************/

VOID __asm transfast( register __a0 UBYTE *, register __a1 UBYTE *, 
					  register __d0 LONG size );

extern struct StudioBase *StudioBase;

int QuickTransFromSP(tofp, fromsp, len)
int tofp;
struct SampleFilePointer *fromsp;
int len;
{
char *bufin, *bufout;
int tomove;

len = len*2;		/** convert samples to bytes **/

bufin = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (bufin==0)
	return(OUT_OF_MEMORY);

bufout = (char *)AllocMem(StudioBase->defaults.copy_buf_size*2, 0);
if (bufout==0) {
	FreeMem(bufin, StudioBase->defaults.copy_buf_size);
	return(OUT_OF_MEMORY);
	}

do {
	tomove = min(StudioBase->defaults.copy_buf_size, len);
	if (SampleRead(fromsp, bufin, tomove/2)!=tomove/2)
		return(READ_ERROR);
	transfast(bufin, bufout, tomove);
	if (Write(tofp, bufout, tomove*2)!=tomove*2)
		return(WRITE_ERROR);
	len = len - tomove;
	} while (len);

FreeMem(bufin, StudioBase->defaults.copy_buf_size);
FreeMem(bufout, StudioBase->defaults.copy_buf_size*2);
return(NO_ERROR);
}
