#include "exec/types.h"
#include "/include/studio16.h"

/********************************/
/*  RAW 16 bit save filter 	 **/
/********************************/

Save16BitRaw(wfp,rsp)

long wfp;
struct SampleFilePointer *rsp;
{
return(QuickCopyFromSP(wfp,rsp,rsp->disksamp->length));
}
