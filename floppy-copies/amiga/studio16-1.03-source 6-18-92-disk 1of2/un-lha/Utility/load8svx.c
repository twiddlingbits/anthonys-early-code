#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/iff8svx.h"
#include "clib/macros.h"

void QuickSplit();
void SkipChunk();

/************************************************/
/** 8SVX filter.  Called by studio/LoadSamp() ***/
/************************************************/

Load8SVX(in,sp)
long in;
struct SampleFilePointer *sp;
{
int len;
ULONG k,x,tm;
UWORD z;
static char line[80];
UBYTE comp=0;
struct Disk_Samp *s;
ULONG flag=0;
int Transfer8BitBody();

s=sp->disksamp;

if(4!=Read(in,&k,4)) return(BAD_IFF);

if (k!=FORM) 
	{
	Seek(in,-4L,OFFSET_CURRENT);			 /** backup to call point **/
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}

if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	 /* skip length */

if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	 /* get form type */

if (x!=SVX)
	{
	Seek(in,-12L,OFFSET_CURRENT);		 /** backup to call point **/
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}

s->type=RIGHT;
tm=0;

while(tm!=BODY)
	{
	if(4!=Read(in,(char *)&tm,4)) return(BAD_IFF);	 /* get chunk type */

	switch(tm)
		{
		case CHAN:
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);
			if (x!=4) return(BAD_IFF);  /* Bad Size */
			if(4!=Read(in,(char *)&s->type,4)) return(BAD_IFF);
			if (s->type!=STEREO && s->type!=RIGHT && s->type!=LEFT) s->type=RIGHT;
		break;

		case VHDR:
			flag=1;
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);  /* should be 20, length of VHDR */
			if (x!=20) return(BAD_IFF);
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	/* oneshot:should be length of sample */
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	/* repeat */
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	/* cycles */
			if(2!=Read(in,(char *)&z,2)) return(BAD_IFF);	/* get samples per second */
			if(1!=Read(in,(char *)&x,1)) return(BAD_IFF);	
			if(1!=Read(in,(char *)&comp,1)) return(BAD_IFF);/* Compression */
			if(4!=Read(in,(char *)&x,4)) return(BAD_IFF);	/* volume */

			s->parms.rate=z;
			s->parms.filter3db=z/2;
			s->parms.volume=ZERO_DB;

			if (comp==1) sprintf(line,"Compressed 8SVX IFF at %d samples per second",z);
			else sprintf(line,"8SVX IFF at %d samples per second",z);
			telluser(line,0,0);
		break;

		case BODY:
			if (!flag) return(BAD_IFF);
			if(4!=Read(in,(char *)&len,4)) return(BAD_IFF);
			if (s->type==STEREO) len=len/2;

			return(Transfer8BitBody(in,sp,len,comp));
		break;

		default:
			SkipChunk(in);
		break;
		}
	}

return(NO_ERROR);
}


/**********************************************************/
