#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/studio16.h"

/** FileFilter for Studio 16 1.1 internal format **/

struct FileFilter *InitNewKwik();
void UnInitNewKwik();

static long NewKwikFFOpen();
static long NewKwikFFRead();
static long NewKwikFFWrite();
static long NewKwikFFSeek();
static long NewKwikFFClose();
static long NewKwikFFReadTag();
static long NewKwikFFWriteTag();
static long NewKwikFFQueryAlign();

static struct FileFilter *ff;

/*** Structures ****/

#define NEWKWIK (('K'<<24)|('W'<<16)|('K'<<8)|('3'))

struct NewKwikHandle {
	long fp;		/** SOpen pointer **/
	};

struct NewKwikHeader {
	long id;
	struct SampleFileTag tag;
	};

/**********************************************************************/

struct FileFilter *InitNewKwik()
{
static struct NewFileFilter nff = {"Studio16_2.0",
											FF_READ|FF_WRITE|FF_DCLIPS|FF_REGIONS|FF_PAN,16,10,
											NewKwikFFOpen,NewKwikFFRead,NewKwikFFWrite,NewKwikFFSeek,
											NewKwikFFClose,NewKwikFFReadTag,NewKwikFFWriteTag,
											NewKwikFFQueryAlign,NULL}; /** No QueryOptions() **/

ff = (struct FileFilter *)AddFileFilter(&nff);
return(ff);
}

/**********************************************************************/

void UnInitNewKwik()
{
DeleteFileFilter(ff);
}

/**********************************************************************/
long NewKwikFFOpen(char *path, long mode)
{
struct NewKwikHandle *kh;
struct NewKwikHeader *header;
long len;

/*kprintf("NewKwik Open '%s'\n",path);*/
kh=(struct NewKwikHandle *)AllocMem(sizeof(struct NewKwikHandle), MEMF_CLEAR);
if (kh==NULL)
	return(NULL);

header=(struct NewKwikHeader *)AllocMem(sizeof(struct NewKwikHeader), MEMF_CLEAR);
if (header==NULL) {
	FreeMem(kh, sizeof(struct NewKwikHandle));
	return(NULL);
	}

kh->fp=SOpen(path, mode);
if (kh->fp==NULL) {
	FreeMem(kh, sizeof(struct NewKwikHandle));
	FreeMem(header, sizeof(struct NewKwikHeader));
	return(NULL);
	}

if (mode==MODE_OLDFILE || mode==MODE_READWRITE) {
	len=SRead(kh->fp,header,sizeof(struct NewKwikHeader));
	if (mode==MODE_READWRITE && len==0) {	/** new file mode READ/WRITE **/
		NewKwikFFWriteTag(kh, header);
		}
	else { /** Make sure this file is the kind of format I understand **/
		if (header->id!=NEWKWIK || len!=sizeof(struct NewKwikHeader)) {
			SClose(kh->fp);
			FreeMem(kh, sizeof(struct NewKwikHandle));
			kh=NULL;
			}
		}
	}
else {	/** MODE_NEWFILE **/
	NewKwikFFWriteTag(kh, header);
	}

FreeMem(header, sizeof(struct NewKwikHeader));
/*kprintf("Open returns kh=%04x%04x\n",kh);*/
return((long)kh);
}

/**********************************************************************/

long NewKwikFFRead(struct NewKwikHandle *kh, short *buffer, long len)
{
long x;

x=SRead(kh->fp, buffer, len*2)/2;
/*kprintf("KWIK: try to read %04x%04x, read %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long NewKwikFFWrite(struct NewKwikHandle *kh, short *buffer, long len)
{
long x;

x=SWrite(kh->fp, buffer, len*2)/2;
/*kprintf("KWIK: try to write %04x%04x, wrote %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long NewKwikFFSeek(struct NewKwikHandle *kh, long pos, long mode)
{
long oldpos;

pos=pos*2;	/** samples to bytes **/
if (mode==OFFSET_BEGINNING)
	pos+=sizeof(struct NewKwikHeader);
oldpos=SSeek(kh->fp, pos, mode);
oldpos=(oldpos-(long)sizeof(struct NewKwikHeader))/2;
if (oldpos < 0)
	oldpos=0;
return(oldpos);
}

/**********************************************************************/

long NewKwikFFClose(struct NewKwikHandle *kh)
{
long fp;

fp=kh->fp;
FreeMem(kh, sizeof(struct NewKwikHandle));
return(SClose(fp));
}

/**********************************************************************/

long NewKwikFFReadTag(struct NewKwikHandle *kh, struct SampleFileTag *filltag)
{
struct NewKwikHeader *orgtag;
long len;

orgtag=(struct NewKwikHeader *)AllocMem(sizeof(struct NewKwikHeader), MEMF_CLEAR);
if (orgtag==NULL)
	return(0);

SSeek(kh->fp, 0, OFFSET_BEGINNING);
len=SRead(kh->fp, orgtag, sizeof(struct NewKwikHeader));
if (len!=sizeof(struct NewKwikHeader)) {
	FreeMem(orgtag,sizeof(struct NewKwikHeader));
	return(NULL);
	}
*filltag=orgtag->tag;
FreeMem(orgtag,sizeof(struct NewKwikHeader));
return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

long NewKwikFFWriteTag(struct NewKwikHandle *kh, struct SampleFileTag *filltag)
{
struct NewKwikHeader *orgtag;
long len;

orgtag=(struct NewKwikHeader *)AllocMem(sizeof(struct NewKwikHeader), MEMF_CLEAR);
if (orgtag==NULL)
	return(0);

orgtag->id=NEWKWIK;
orgtag->tag=*filltag;
SSeek(kh->fp, 0, OFFSET_BEGINNING);
len=SWrite(kh->fp, orgtag, sizeof(struct NewKwikHeader));
FreeMem(orgtag,sizeof(struct NewKwikHeader));
if (len!=sizeof(struct NewKwikHeader))
	return(NULL);
else
	return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

static long NewKwikFFQueryAlign(struct NewKwikHandle *kh)
{
return(SQueryAlign(kh->fp));
}

/**********************************************************************/
