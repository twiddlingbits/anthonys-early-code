#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/studio16.h"

/** FileFilter for Studio 16 1.0 internal format **/

struct FileFilter *InitKwik();
void UnInitKwik();

static long FFOpen();
static long FFRead();
static long FFWrite();
static long FFSeek();
static long FFClose();
static long FFReadTag();
static long FFWriteTag();
static long FFQueryAlign();

static struct FileFilter *ff;

/*** Structures ****/

#define KWIK1 (('K'<<24)|('W'<<16)|('I'<<8)|('K'))

struct SampleDataClip1 {
	long start;			/** inclusive **/
	long end;			/** inclusive **/
	};

#define DS_NUM_CLIPS1 128

struct SampleInfo1 {
	int rate;				/** sampling rate **/
	int filter3db;			/** Anti-aliasing filter value **/
	unsigned short volume;			/** prefered volume level **/
	int starttimecode;
	float smpte_sampling_rate;
	long reserved[3];
	};

struct OriginalTag {
	unsigned long id;			/** set to TAG_KWIK **/
	struct SampleInfo1 parms;
	int org_length;			/** length of samp with no data clips **/
	int length;					/** length of samp with using data clips **/
	struct SampleDataClip1 dclips[DS_NUM_CLIPS1];
	};

struct KwikHandle {
	long fp;		/** SOpen pointer **/
	};

/**********************************************************************/

struct FileFilter *InitKwik()
{
static struct NewFileFilter nff = {"Studio16_1.0",
											FF_READ | FF_WRITE | FF_DCLIPS,16,-1,
											FFOpen,FFRead,FFWrite,FFSeek,
											FFClose,FFReadTag,FFWriteTag,
											FFQueryAlign,NULL}; /** No QueryOptions() **/

ff = (struct FileFilter *)AddFileFilter(&nff);
return(ff);
}

/**********************************************************************/

void UnInitKwik()
{
DeleteFileFilter(ff);
}

/**********************************************************************/
long FFOpen(char *path, long mode)
{
struct KwikHandle *kh;
struct OriginalTag header;
long len;

/*kprintf("Kwik OPen '%s'\n",path);*/
kh=(struct KwikHandle *)AllocMem(sizeof(struct KwikHandle), MEMF_CLEAR);
if (kh==NULL)
	return(NULL);

kh->fp=SOpen(path, mode);
if (kh->fp==NULL) {
	FreeMem(kh, sizeof(struct KwikHandle));
	return(NULL);
	}

if (mode==MODE_OLDFILE || mode==MODE_READWRITE) {
	len=SRead(kh->fp,&header,sizeof(header));
	if (mode==MODE_READWRITE && len==0) {	/** new file mode READ/WRITE **/
		setmem(&header, sizeof(header), 0);
		FFWriteTag(kh, &header);
		}
	else { /** Make sure this file is the kind of format I understand **/
		if (header.id!=KWIK1 || len!=sizeof(header)) {
			SClose(kh->fp);
			FreeMem(kh, sizeof(struct KwikHandle));
			kh=NULL;
			}
		}
	}
else {	/** MODE_NEWFILE **/
	setmem(&header, sizeof(header), 0);
	FFWriteTag(kh, &header);
	}

/*kprintf("Open returns kh=%04x%04x\n",kh);*/
return((long)kh);
}

/**********************************************************************/

long FFRead(struct KwikHandle *kh, short *buffer, long len)
{
long x;

x=SRead(kh->fp, buffer, len*2)/2;
/*kprintf("KWIK: try to read %04x%04x, read %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long FFWrite(struct KwikHandle *kh, short *buffer, long len)
{
long x;

x=SWrite(kh->fp, buffer, len*2)/2;
/*kprintf("KWIK: try to write %04x%04x, wrote %04x%04x\n",len,x);*/
return(x);
}

/**********************************************************************/

long FFSeek(struct KwikHandle *kh, long pos, long mode)
{
long oldpos;

pos=pos*2;	/** samples to bytes **/
if (mode==OFFSET_BEGINNING)
	pos+=sizeof(struct OriginalTag);
oldpos=SSeek(kh->fp, pos, mode);
oldpos=(oldpos-(long)sizeof(struct OriginalTag))/2;
if (oldpos < 0)
	oldpos=0;
return(oldpos);
}

/**********************************************************************/

long FFClose(struct KwikHandle *kh)
{
long fp;

fp=kh->fp;
FreeMem(kh, sizeof(struct KwikHandle));
return(SClose(fp));
}

/**********************************************************************/

long FFReadTag(struct KwikHandle *kh, struct SampleFileTag *filltag)
{
struct OriginalTag orgtag;
long len;
int i;

SSeek(kh->fp, 0, OFFSET_BEGINNING);
len=SRead(kh->fp, &orgtag, sizeof(orgtag));
if (len!=sizeof(orgtag))
	return(NULL);
filltag->parms.rate=orgtag.parms.rate;
filltag->parms.filter3db=orgtag.parms.filter3db;
filltag->parms.volume=orgtag.parms.volume;
filltag->parms.starttimecode=orgtag.parms.starttimecode;
filltag->parms.smpte_sampling_rate=orgtag.parms.smpte_sampling_rate;
filltag->length=orgtag.length;
filltag->org_length=orgtag.org_length;
for (i=0; i < DS_NUM_CLIPS1; i++) {
	filltag->dclips[i].start=orgtag.dclips[i].start;
	filltag->dclips[i].end=orgtag.dclips[i].end;
	}

/** parameters not defined in KWIK are given defaults **/

filltag->parms.pan=ZERO_DB;

return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

long FFWriteTag(struct KwikHandle *kh, struct SampleFileTag *filltag)
{
struct OriginalTag orgtag;
long len;
int i;

setmem(&orgtag, sizeof(orgtag),0);
orgtag.id=KWIK1;
orgtag.parms.rate=filltag->parms.rate;
orgtag.parms.filter3db=filltag->parms.filter3db;
orgtag.parms.volume=filltag->parms.volume;
orgtag.parms.starttimecode=filltag->parms.starttimecode;
orgtag.parms.smpte_sampling_rate=filltag->parms.smpte_sampling_rate;
orgtag.length=filltag->length;
orgtag.org_length=filltag->org_length;
for (i=0; i < DS_NUM_CLIPS1; i++) {
	orgtag.dclips[i].start=filltag->dclips[i].start;
	orgtag.dclips[i].end=filltag->dclips[i].end;
	}
SSeek(kh->fp, 0, OFFSET_BEGINNING);
len=SWrite(kh->fp, &orgtag, sizeof(orgtag));
if (len!=sizeof(orgtag))
	return(NULL);
else
	return(sizeof(struct SampleFileTag));
}

/**********************************************************************/

static long FFQueryAlign(struct KwikHandle *kh)
{
return(SQueryAlign(kh->fp));
}

/**********************************************************************/
