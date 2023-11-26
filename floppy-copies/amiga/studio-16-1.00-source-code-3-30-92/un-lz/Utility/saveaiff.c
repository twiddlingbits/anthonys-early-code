#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include <stdio.h>
#include "/include/psound.h"
#include "/include/iff8svx.h"
#include "/include/studio16.h"

/*-----------------------------------------------------------------------*/
struct  Extended
	{
	short	Exponent;
	long	Mantissa[2];
	};
/*-----------------------------------------------------------------------*/
struct AIFFstruct
	{
	char	form[4];
	ULONG	Size;
	char	type[4];

	char	comm[4];
	ULONG	Csize;
	short	Channels;
	ULONG	Samples;
	short	Resolution;
/*	Extended	Rate;*/
	short	Rate;
	long	x1;
	long	x2;

	char	anno[4];
	ULONG	Asize;
	char	Atxt[68];

	char	ssnd[4];
	ULONG	Ssize;
	ULONG	unused[2];
	};
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
SaveAIFF8(fp,rsp)

long fp;
struct SampleFilePointer *rsp;
{
struct Disk_Samp *samp=rsp->disksamp;
double rate=samp->parms.rate;
static struct AIFFstruct AIFFhead =
	{'F','O','R','M',0,
	'A','I','F','F',
	'C','O','M','M',18,1,0,8,0,0,0,
	'A','N','N','O',68,
	"Recorded with STUDIO 16     from SunRize Industries. (408) 374-4962",
	'S','S','N','D',0,0,0};

AIFFhead.Channels=1;
AIFFhead.Size=4+26+76+16+(samp->length);
AIFFhead.Samples=samp->length;
AIFFhead.Resolution=16;
DoubleToExtend(&rate,&AIFFhead.Rate);
AIFFhead.Ssize=8+(samp->length);

if(sizeof(AIFFhead)!=Write(fp,&AIFFhead,sizeof(AIFFhead))) {telluser("ERROR SAVING FILE","could not write AIFF header",0);return(WRITE_ERROR);}
return(QuickSplitCopyFromSP(fp,rsp,samp->length));
}                              
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
SaveAIFF16(fp,rsp)

long fp;
struct SampleFilePointer *rsp;
{
struct Disk_Samp *samp=rsp->disksamp;
double rate=samp->parms.rate;
static struct AIFFstruct AIFFhead =
	{'F','O','R','M',0,
	'A','I','F','F',
	'C','O','M','M',18,1,0,8,0,0,0,
	'A','N','N','O',68,
	"Recorded with STUDIO 16     from SunRize Industries. (408) 374-4962",
	'S','S','N','D',0,0,0};

AIFFhead.Channels=1;
AIFFhead.Size=4+26+76+16+(samp->length<<1);
AIFFhead.Samples=samp->length;
AIFFhead.Resolution=16;
DoubleToExtend(&rate,&AIFFhead.Rate);
AIFFhead.Ssize=8+(samp->length<<1);

if(sizeof(AIFFhead)!=Write(fp,&AIFFhead,sizeof(AIFFhead))) {telluser("ERROR SAVING FILE","could not write AIFF header",0);return(WRITE_ERROR);}
return(QuickCopyFromSP(fp,rsp,samp->length));
}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
LoadAIFF(rfp,wsp)

long rfp;
struct SampleFilePointer *wsp;
{
struct Disk_Samp *samp;
ULONG rez=0,x,k,len,tl=0,cont=0;
USHORT z;
struct Extended rat;
double r;
char line[80];
int samplen;
int err;

samp=wsp->disksamp;

samp->type=LEFT;

Read(rfp,&k,4);
if (k!=FORM) 
	{
	Seek(rfp,-4L,OFFSET_CURRENT);
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}
Read(rfp,&tl,4);	/* total Size */
Read(rfp,&k,4); tl -= 4;
if (k!=AIFF) 
	{
	Seek(rfp,-12L,OFFSET_CURRENT);
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}

while (tl>0)
	{
	Read(rfp,&k,4);		tl -= 4;
	Read(rfp,&len,4);	tl -= 4;

	switch (k)
		{
		case COMM:
		if (len!=18) {telluser("Corrupt AIFF file","Load Failed #1",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
/*CHANNELS*/	Read(rfp,&z,2);		tl -= 2;
		if (z>1) {telluser("AIFF file contains multiple channels","I am not intelligent enough to load it",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
/*SAMPLES*/	Read(rfp,&x,4);		tl -= 4;
		samplen=x;
/*REZOLUT*/	Read(rfp,&z,2);		tl -= 2;
		rez=z;
/*RATE*/	Read(rfp,&rat,sizeof(struct Extended)); tl -= sizeof(struct Extended);
		ExtendToDouble(&r,&rat);
		samp->parms.rate=r;
		samp->parms.filter3db=r/2;
		samp->parms.volume=ZERO_DB;
		break;

		case SSND:

		if (rez==0) {telluser("Load Failed","I require the COMM to come before SSND",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
		sprintf(line,"AIFF %d bits at %d samples per second",rez,samp->parms.rate);
		telluser(line,0,0);
		if (rez<9)
			{
			if (samplen!=len-8) {telluser("Corrupt AIFF file","Load Failed #2",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
			Read(rfp,&x,4);
			Read(rfp,&x,4);
			tl -= len;
			telluser("Sorry 8bit AIFF currently disabled","Call SunRize.",0);
			Seek(rfp,len-8,OFFSET_CURRENT);
			}
		else if (rez<17)
			{
			if (samplen!=((len-8)>>1)) {telluser("Corrupt AIFF file","Load Failed #3",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
			Read(rfp,&x,4);
			Read(rfp,&x,4);

		err=QuickCopyToSP(wsp,rfp,samplen); tl -= len;

		if (err) {sprintf(line,"QuickCopy ERROR %d",err);telluser(line,0,0);}

			}
		else {telluser("Sorry I am too stupid to load in AIFF Files","of greater resolution than 16 bits, LOAD ABORTED",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
		break;

		default: 
		cont++;
		if (cont==32) {telluser("Corrupt AIFF file","Load Failed #4",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
		if (k&0x80808080) {telluser("Corrupt AIFF file","Load Failed #5",0);Seek(rfp,0,OFFSET_BEGINING);return(STDERR_UNKNOWN_FILE_FORMAT);}
		Seek(rfp,len,OFFSET_CURRENT); tl -= len;
		break;
		}
	}
return(NO_ERROR);
}
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/
/*-----------------------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
