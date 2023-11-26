#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/studio16.h"

#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define ID_AIFF MakeID('A','I','F','F')
#define ID_FORM MakeID('F','O','R','M')
#define ID_COMM MakeID('C','O','M','M')
#define ID_SSND MakeID('S','S','N','D')

void __asm ByteToWord(register __a0 short *,register __a1 char *, 
					  register __d0 long len);
void __asm WordToByte(register __a0 char *,register __a1 short *,
					  register __d0 long len);

/** FileFilter for Studio 16 1.1 internal format **/

struct FileFilter *InitAIFF8();
void UnInitAIFF8();

static long AIFF8FFOpen();
static long AIFF8FFRead();
static long AIFF8FFWrite();
static long AIFF8FFSeek();
static long AIFF8FFClose();
static long AIFF8FFReadTag();
static long AIFF8FFWriteTag();
static long AIFF8FFQueryAlign();

static struct FileFilter *ff;

/*-----------------------------------------------------------------------*/
struct  Extended
	{
	short	Exponent;
	long	Mantissa[2];
	};
/*-----------------------------------------------------------------------*/
struct AIFFstruc
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
	char	Atxt[48];

	char	ssnd[4];
	ULONG	Ssize;
	ULONG	unused[2];
	};

struct COMM
	{
	short	Channels;
	ULONG	Samples;
	short	Resolution;
/*	Extended	Rate;*/
	short	Rate;
	long	x1;
	long	x2;
	};

/*-----------------------------------------------------------------------*/

struct AIFF8Handle {
	long fp;		/** SOpen pointer **/
	long SSND;
	long COMM;
	};

/**********************************************************************/

struct FileFilter *InitAIFF8()
{
static struct NewFileFilter nff = 
	{"AIFF 8Bit",
	FF_READ | FF_WRITE,16,-5,
	AIFF8FFOpen,AIFF8FFRead,AIFF8FFWrite,AIFF8FFSeek,
	AIFF8FFClose,AIFF8FFReadTag,AIFF8FFWriteTag,
	AIFF8FFQueryAlign,NULL}; /** No QueryOptions() **/

ff = (struct FileFilter *)AddFileFilter(&nff);
return(ff);
}

/**********************************************************************/

void UnInitAIFF8()
{
DeleteFileFilter(ff);
}

/**********************************************************************/
long AIFF8FFOpen(char *path, long mode)
{
struct AIFF8Handle *kh;
long len,dummy;
int succ;

kh=(struct AIFF8Handle *)AllocMem(sizeof(struct AIFF8Handle), MEMF_CLEAR);
if (kh==NULL) return(NULL);

kh->fp=SOpen(path, mode);
if (kh->fp==NULL) {
	FreeMem(kh, sizeof(struct AIFF8Handle));
	return(NULL);
	}

if (mode==MODE_OLDFILE || mode==MODE_READWRITE) 
	{
	len=SRead(kh->fp,&dummy,sizeof(dummy));
	if (mode==MODE_READWRITE && len==0) 	/** new file mode READ/WRITE **/
		{
		succ=AIFF8WriteNewForm(kh,StrFileName(path));
		}
	else	/** Make sure this file is the kind of format I understand **/
		{
		succ=AIFF8ReadNewForm(kh);
		}
	}
else	/** MODE_NEWFILE **/
	{
	succ=AIFF8WriteNewForm(kh,StrFileName(path));
	}

if (!succ)
	{
	SClose(kh->fp);
	FreeMem(kh, sizeof(struct AIFF8Handle));
	kh=NULL;
	}

return((long)kh);
}
/**********************************************************************/
AIFF8ReadNewForm(kh)
struct AIFF8Handle *kh;
{
long k,x,pos=0,len;
struct COMM comm;

SSeek(kh->fp, 0, OFFSET_BEGINNING);
if (4!=SRead(kh->fp,&k,4)) return(FALSE);
if (k!=ID_FORM) return(FALSE);
if(4!=SRead(kh->fp,(char *)&x,4)) return(FALSE);	 /* skip length */
if(4!=SRead(kh->fp,(char *)&x,4)) return(FALSE);	 /* get form type */
if (x!=ID_AIFF) return(FALSE);

pos=pos+12;

do	{
	if(4!=SRead(kh->fp,(char *)&k,4)) return(FALSE);	 /* get chunk type */
	pos=pos+8;
	switch(k)
		{
		case ID_COMM:
			kh->COMM=pos;
			if(4!=SRead(kh->fp,(char *)&x,4)) return(FALSE);
			len=SRead(kh->fp, &comm, sizeof(struct COMM));
			if (comm.Resolution!=8) return(FALSE);
			if (comm.Channels!=1) return(FALSE);
			pos=pos+sizeof(struct COMM);
			break;

		case ID_SSND:
			kh->SSND=pos;
			break;

		default:
			if(4!=SRead(kh->fp,(char *)&x,4)) return(FALSE);
			SkipRest(kh->fp,x,&pos);
			break;
		}
	} while(k!=ID_SSND);

return(TRUE);
}
/**********************************************************************/

AIFF8WriteNewForm(kh,sampname)
struct AIFF8Handle *kh;
char *sampname;
{
static struct AIFFstruc AIFFhead =
	{'F','O','R','M',0,
	'A','I','F','F',
	'C','O','M','M',18,1,0,8,0,0,0,
	'A','N','N','O',48,
	"Recorded with Studio 16 from SunRize Industries",
	'S','S','N','D',0,0,0};
double rate=0;


kh->COMM=20;
kh->SSND=110;

AIFFhead.Channels=1;
AIFFhead.Size=102 +(0);
AIFFhead.Samples=(0);
AIFFhead.Resolution=8;
DoubleToExtend(&rate,&AIFFhead.Rate);
AIFFhead.Ssize=8+(0);

if(sizeof(AIFFhead)!=SWrite(kh->fp,&AIFFhead,sizeof(AIFFhead))) return(FALSE);

return(TRUE);
}
/**********************************************************************/

long AIFF8FFRead(struct AIFF8Handle *kh, short *buffer, long len)
{
long x=0;
char *rb;

if (len) {
	rb=(char *)AllocDmaMem(len);
	if (rb) {
		x=SRead(kh->fp, rb, len);
		ByteToWord(buffer,rb,len);
		FreeMem(rb, len);
		}
	}

return(x);
}

/**********************************************************************/

long AIFF8FFWrite(struct AIFF8Handle *kh, short *buffer, long len)
{
long x=0;
char *rb;

if (len) {
	rb=(char *)AllocDmaMem(len);
	if (rb) {
		WordToByte(rb,buffer,len);
		x=SWrite(kh->fp, rb, len);
		FreeMem(rb, len);
		}
	}

return(x);
}

/**********************************************************************/

long AIFF8FFSeek(struct AIFF8Handle *kh, long pos, long mode)
{
long oldpos;

if (mode==OFFSET_BEGINNING) pos+=kh->SSND;
oldpos=SSeek(kh->fp, pos, mode);
oldpos=oldpos-kh->SSND;
if (oldpos < 0)
	oldpos=0;

return(oldpos);
}

/**********************************************************************/

long AIFF8FFClose(struct AIFF8Handle *kh)
{
long fp;
fp=kh->fp;
FreeMem(kh, sizeof(struct AIFF8Handle));
return(SClose(fp));
}

/**********************************************************************/

long AIFF8FFReadTag(struct AIFF8Handle *kh, struct SampleFileTag *filltag)
{
struct COMM comm;
long len,formlen;
double rate;

setmem(filltag, sizeof(struct SampleFileTag), 0);

/*** Get length of FORM ***/

SSeek(kh->fp, 4, OFFSET_BEGINNING);
len=SRead(kh->fp, &formlen, sizeof(formlen));
if (len!=sizeof(formlen)) return(NULL);

/*** Determine Pan info ***/

/****** Read COMM for sampling rate, etc. *****/

SSeek(kh->fp, kh->COMM, OFFSET_BEGINNING);
len=SRead(kh->fp, &comm, sizeof(struct COMM));
if (len!=sizeof(struct COMM)) return(NULL);

ExtendToDouble(&rate,&comm.Rate);
filltag->parms.rate=rate;
filltag->parms.filter3db=filltag->parms.rate/2;
filltag->parms.volume=ZERO_DB;
filltag->parms.pan=ZERO_DB;

/***** Calculate length ******/

filltag->length=comm.Samples;
filltag->org_length=filltag->length;

/** Initilized one clip containing entire sample **/

filltag->dclips[0].end=filltag->length-1;
return(sizeof(struct SampleFileTag));
}
/**********************************************************************/
/**********************************************************************/
long AIFF8FFWriteTag(struct AIFF8Handle *kh, struct SampleFileTag *filltag)
{
long fp=kh->fp;
struct COMM comm;
double rate=filltag->parms.rate;
long x,len;

/*----- COMM PARMS -----*/
DoubleToExtend(&rate,&comm.Rate);
comm.Channels=1;
comm.Samples=filltag->length;
comm.Resolution=8;
SSeek(kh->fp, kh->COMM, OFFSET_BEGINNING);
len=SWrite(kh->fp, &comm, sizeof(struct COMM));
if (len!=sizeof(struct COMM)) return(NULL);

/*----- SSND PARMS -----*/
x=8+(filltag->length);

SSeek(kh->fp, kh->SSND-12, OFFSET_BEGINNING);
if(4!=SWrite(fp,&x,4)) return(NULL);

/** Update FORM length **/

SSeek(kh->fp, 4, OFFSET_BEGINNING);
x=(filltag->length)+kh->SSND-8;
if (SWrite(kh->fp, &x, 4)!=4) return(NULL);

return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

static long AIFF8FFQueryAlign(struct AIFF8Handle *kh)
{
return(SQueryAlign(kh->fp));
}

/**********************************************************************/

