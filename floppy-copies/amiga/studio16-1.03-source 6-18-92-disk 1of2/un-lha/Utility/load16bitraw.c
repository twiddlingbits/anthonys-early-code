#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include "/include/studio16.h"

/*****************************************/
/** 16 bit raw data load filter			 **/
/*****************************************/

Load16BitRaw(in,sp)

long in;		/** in fp, out sp **/
struct SampleFilePointer *sp;
{
int len;
struct Disk_Samp *s;

s = sp->disksamp;

telluser("Loading RAW data",0,0);

s->parms.rate=44100;	         		/* default length */
s->parms.filter3db=20000;
s->parms.volume=ZERO_DB;
s->type=LEFT;     			   		/* use left (ch 0) as default */
len=Seek(in,0L,OFFSET_END);			/** returns previous pos **/
len=Seek(in,0L,OFFSET_BEGINNING);   /* rewind */
len = len/2;								/* bytes to samples */
if (len<2) {telluser("Can not load file","Sample is too short"); return(BAD_IFF);}
return(QuickCopyToSP(sp, in, len));
}
