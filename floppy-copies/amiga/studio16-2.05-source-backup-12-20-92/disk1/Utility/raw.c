#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/studio16.h"

/** FileFilter for Studio 16 1.1 internal format **/

struct FileFilter *InitRaw();
void UnInitRaw();

static long RawFFOpen();
static long RawFFRead();
static long RawFFWrite();
static long RawFFSeek();
static long RawFFClose();
static long RawFFReadTag();
static long RawFFWriteTag();
static long RawFFQueryAlign();

static struct FileFilter *ff;

struct RawHandle {
	long fp;		/** SOpen pointer **/
	};

/**********************************************************************/

struct FileFilter *InitRaw()
{
static struct NewFileFilter nff = {"RAW",
											FF_READ | FF_WRITE,16,-50,
											RawFFOpen,RawFFRead,RawFFWrite,RawFFSeek,
											RawFFClose,RawFFReadTag,RawFFWriteTag,
											RawFFQueryAlign,NULL}; /** No QueryOptions() **/

ff = (struct FileFilter *)AddFileFilter(&nff);
return(ff);
}

/**********************************************************************/

void UnInitRaw()
{
DeleteFileFilter(ff);
}

/**********************************************************************/
long RawFFOpen(char *path, long mode)
{
struct RawHandle *kh;

kh=(struct RawHandle *)AllocMem(sizeof(struct RawHandle), MEMF_CLEAR);
if (kh==NULL)
	return(NULL);

kh->fp=SOpen(path, mode);
if (kh->fp==NULL) {
	FreeMem(kh, sizeof(struct RawHandle));
	return(NULL);
	}

return((long)kh);
}

/**********************************************************************/

long RawFFRead(struct RawHandle *kh, short *buffer, long len)
{
long x;

x=SRead(kh->fp, buffer, len*2)/2;
return(x);
}

/**********************************************************************/

long RawFFWrite(struct RawHandle *kh, short *buffer, long len)
{
long x;

x=SWrite(kh->fp, buffer, len*2)/2;
return(x);
}

/**********************************************************************/

long RawFFSeek(struct RawHandle *kh, long pos, long mode)
{
long oldpos;

pos=pos*2;	/** samples to bytes **/
oldpos=SSeek(kh->fp, pos, mode)/2;
return(oldpos);
}

/**********************************************************************/

long RawFFClose(struct RawHandle *kh)
{
long fp;

fp=kh->fp;
FreeMem(kh, sizeof(struct RawHandle));
return(SClose(fp));
}

/**********************************************************************/

long RawFFReadTag(struct RawHandle *kh, struct SampleFileTag *filltag)
{
long now;

setmem(filltag, sizeof(struct SampleFileTag), 0);
filltag->parms.rate=44100;
filltag->parms.filter3db=22000;
filltag->parms.volume=ZERO_DB;
filltag->parms.smpte_sampling_rate=44100.0;
filltag->parms.pan=ZERO_DB;
now=SSeek(kh->fp, 0, OFFSET_END);
filltag->length=SSeek(kh->fp, now, OFFSET_BEGINNING)/2;
filltag->org_length=filltag->length;

/** Initilized one clip containing entire sample **/
filltag->dclips[0].end=filltag->length-1;

return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

long RawFFWriteTag(struct RawHandle *kh, struct SampleFileTag *filltag)
{
return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

static long RawFFQueryAlign(struct RawHandle *kh)
{
return(SQueryAlign(kh->fp));
}

/**********************************************************************/
