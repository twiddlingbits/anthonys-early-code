#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include "/include/psound.h"
#include <stdio.h>

/** This file contains "SampleX()" functions.  These functions **/
/** are the prefered way of manipulating Disk_Samp's "SampleFile" **/
/** data.  These functions all make call's to SX() functions **/
/** SOpen(), SSeek(), etc.  these are lower level calls that hide **/
/** such things as direct AmigaDos calls & the SampleFileTag struct **/
/** located at the start of all SampleFile's.  **/

/**!!!!! ADD A SampleDelete() menu! **/

extern struct StudioBase *StudioBase;
extern short se_info_sample_created;

BPTR Lock(char *, LONG);
void MapEditedToData2();
void CopySampData();

/*############################################################
NAME	
	SampleGetOne -
FUNCTION
	Given a sample pointer (which should be obtained by calling
	SampleOpen(), this funciton will return one sample.
INPUTS
	sp sample pointer to opened sample
	i index (0 to sample_len - 1) of sample to return
RESULTS
	a short sample value
##############################################################*/

short SampleGetOne(sp, i)
struct SampleFilePointer *sp;
long i;
{
short val;

SampleSeek(sp,i,SS_OFFSET_BEGINNING);
SampleRead(sp, &val, 1);
return(val);
}


/*############################################################
NAME	
	SampleRead -
FUNCTION
	This is similars to a dos Read() function, except it deals
	with Disk_Samp's, not files.  SampleRead understands 
	DataClips inside a sample and deals with them.   You can
	use SampleSeek() to find the beginning of a DiskSamp's data.
INPUTS
	sp sample pointer to opened sample
	buffer pointer to memory to fill with data
	length number of samples to read (not bytes!) Make sure
			your buffer points to length*2 bytes!
RESULTS
	the number of samples actually read
##############################################################*/

int SampleRead(sp, buffer, length)
struct SampleFilePointer *sp;
short *buffer;
long length;
{
struct SampleDataClip *dclips;
int readlen=0;
int actread=0;
int totalread;

ObtainSemaphore(&sp->locksfp);
dclips=&sp->disksamp->dclips[0];
totalread=0;

while (length >0 && actread==readlen*2 && dclips[sp->dclipindex].end &&	sp->dclipindex < DS_NUM_CLIPS) {
	readlen = min(length, dclips[sp->dclipindex].end-sp->fileindex+1);
	if (readlen <=0)
		break;
	actread=SRead(sp->fp,buffer,readlen*2);
	totalread+=actread;
	buffer+=readlen;	/** buffer points to short so this adds 2X */
	length-=readlen;
	sp->fileindex+=readlen;
	if (sp->fileindex > dclips[sp->dclipindex].end) {
		sp->dclipindex++;
		if (dclips[sp->dclipindex].end) {
			sp->fileindex=dclips[sp->dclipindex].start;
/** These seek's cause "problems" when there is a large difference **/
/** between the last clip and the actual end of the file.  **/
/** There will be a large seek to the null clip pointing to **/
/** the end of the file.  This can cause buffers to run out. **/
			SSeek(sp->fp,sp->fileindex*2,OFFSET_BEGINNING);
			}
		else {		/** init a new clip if at end of file **/
			if (sp->fileindex!=sp->disksamp->org_length) {
				sp->fileindex=sp->disksamp->org_length;
				SSeek(sp->fp,sp->fileindex*2,OFFSET_BEGINNING);
				}
			dclips[sp->dclipindex].start=sp->disksamp->org_length;
			dclips[sp->dclipindex].end=sp->disksamp->org_length-1;
			}
		}
	}
ReleaseSemaphore(&sp->locksfp);
return(totalread/2);
}

/*############################################################
NAME	
	SampleWrite -
FUNCTION
	This is similar to a dos Write() function, except it deals
	with Disk_Samp's, not files.  SampleWrite understands 
	DataClips inside a sample and deals with them. You can
	use SampleSeek() to find a position inside DiskSamp's data.
	Seeking to the middle of a sample, and calling SampleWrite()
	will cause the current data to be overwritten with the new
	data.  A sample will not be made shorter by writing to the
	middle of a sample.
INPUTS
	sp sample pointer to opened sample
	buffer pointer to memory to fill with data
	length number of samples to write (not bytes!) Make sure
			your buffer points to length*2 bytes!
RESULTS
	the number of samples actually written.  If this is shorter
	than length, an error (such as disk full) occured during the
	write.
##############################################################*/

int SampleWrite(sp, buffer, length)
struct SampleFilePointer *sp;
short *buffer;
long length;
{
struct SampleDataClip *dclips;
int writelen;
int totalwritten;
int count;

ObtainSemaphore(&sp->locksfp);
dclips=&sp->disksamp->dclips[0];
totalwritten=0;


/** Special case to fix .end==0 bug **/

if (sp->dclipindex==0 && dclips[0].end==0) {
	count=SampleAppend(sp, buffer, length);
	ReleaseSemaphore(&sp->locksfp);
	return(count);
	}

/** Are we appending to the end of an existing clip? **/

if (dclips[sp->dclipindex].end==(sp->fileindex-1)) {
	count=SampleAppend(sp, buffer, length);
	ReleaseSemaphore(&sp->locksfp);
	return(count);
	}

/** Are we recording into the middle of existing samples? **/

while (length >0 && dclips[sp->dclipindex].end && sp->dclipindex < DS_NUM_CLIPS) {
	writelen = min(length, dclips[sp->dclipindex].end-sp->fileindex+1);
	if (writelen>0) {
		totalwritten+=SWrite(sp->fp,buffer,writelen*2);
		buffer+=writelen;	/** buffer points to short so this adds 2X */
		length-=writelen;
		sp->fileindex+=writelen;
		if (sp->fileindex > dclips[sp->dclipindex].end) {
			sp->dclipindex++;
			if (dclips[sp->dclipindex].end) {
				/** check for empty clips hanging on the end **/
				if (dclips[sp->dclipindex].start==sp->disksamp->org_length) {
					count=totalwritten/2+SampleAppend(sp, buffer, length);
					ReleaseSemaphore(&sp->locksfp);
					return(count);
					}
				else {	/** must be new clip **/
					sp->fileindex=dclips[sp->dclipindex].start;
					SSeek(sp->fp,sp->fileindex*2,OFFSET_BEGINNING);
					}
				}
			else {
				dclips[sp->dclipindex].start=sp->disksamp->org_length;
				dclips[sp->dclipindex].end=sp->disksamp->org_length-1;
				count=totalwritten/2+SampleAppend(sp, buffer, length);
				ReleaseSemaphore(&sp->locksfp);
				return(count);
				}
			}
		}
	}
ReleaseSemaphore(&sp->locksfp);
return(totalwritten/2);
}


SampleAppend(sp, buffer, length)
struct SampleFilePointer *sp;
short *buffer;
long length;
{
int totalwritten;

totalwritten=SWrite(sp->fp,buffer,length*2);
sp->disksamp->length += totalwritten/2;
sp->disksamp->org_length += totalwritten/2;
sp->disksamp->dclips[sp->dclipindex].end = sp->disksamp->org_length-1;
sp->fileindex+=totalwritten/2;
return(totalwritten/2);
}

/*############################################################
NAME	
	SampleSeek -
FUNCTION
	This is similar to a dos Seek() function, except it deals
	with Disk_Samp's, not files.  Seeks to the specified position
	within a Disk_Samp. Understands DataClips inside a sample 
	and deals with them. Thus, if the user has performed some
	non-destructive edits on the sample, the actual file position
	SampleSeek() seeks to probably won't match the position you
	pass since you are passing an edited position.

INPUTS
	sp sample pointer to opened sample
	rpos	Edit sample position to seek to
	mode  SS_OFFSET_END, SS_OFFSET_CURRENT, SS_OFFSET_BEGINNING
RESULTS
	Sample position upon call to SampleSeek()
##############################################################*/

int SampleSeek(sp, rpos, mode)
struct SampleFilePointer *sp;
long rpos;
int mode;
{
long iamat;

ObtainSemaphore(&sp->locksfp);
iamat=SSPos(sp);

if (rpos==0 && mode==SS_OFFSET_CURRENT)	{/** make speedy **/
	ReleaseSemaphore(&sp->locksfp);
	return(iamat);
	}

if (mode==SS_OFFSET_END)
	rpos += (sp->disksamp->length);
else if (mode==SS_OFFSET_CURRENT)
	rpos += iamat;

if (rpos < 0)
	rpos=0;	/** Saftey check **/
MapEditedToData2(sp->disksamp,rpos,&sp->fileindex,&sp->dclipindex);
if (sp->fileindex < 0)	/** saftey check **/
	sp->fileindex=0;
SSeek(sp->fp,sp->fileindex*2,OFFSET_BEGINNING);	/* seekpos is in samps */
ReleaseSemaphore(&sp->locksfp);
return(iamat);
}

/************************************************************/
/** Given a corrdinate in the "edited" sample, return      **/
/** the equivilant position in the actual data on disk 	  **/
/************************************************************/

int MapEditedToData(samp,editval)
struct Disk_Samp *samp;
int editval;
{
short dclipindex;
int fileindex;

MapEditedToData2(samp,editval,&fileindex,&dclipindex);
return(fileindex);
}

/************************************************************/

void MapEditedToData2(samp,editval,fileindex,dclipindex)
struct Disk_Samp *samp;
int editval;
int *fileindex;
short *dclipindex;
{
short i;
int edit_pos;
int clip_len;

i=0;
edit_pos=0;

/** There's a small bug below.  Since we don't allow .end==0 **/
/** a clip from 0 to 0 will cause problems! **/

while (samp->dclips[i].end && i < DS_NUM_CLIPS) {
	clip_len=DataClipLength(&samp->dclips[i]);
	if (editval >= edit_pos && editval < (edit_pos+clip_len)) {
		*fileindex=samp->dclips[i].start+editval-edit_pos;
		*dclipindex=i;
		return;
		}
	edit_pos += clip_len;
	i++;
	}

/** Here if edit_val is out of range. **/

/** Seeking to EOF & there is a null clip present? **/
if (samp->dclips[i-1].end==samp->org_length-1 &&
	editval==samp->length) {
	*fileindex=samp->org_length;
	*dclipindex=i-1;
	}
else if (i>=DS_NUM_CLIPS) {	/** clip overflow? **/
	*fileindex=samp->dclips[DS_NUM_CLIPS-1].start;
	*dclipindex=DS_NUM_CLIPS-1;
	}
else {	/** Initilize a new clip to zero length **/
	samp->dclips[i].start=samp->org_length;
	samp->dclips[i].end=samp->org_length-1;
	*fileindex=samp->org_length;
	*dclipindex=i;
	}
}

/************************************************************/

long SSPos(sp)
struct SampleFilePointer *sp;
{
short i;
int edit_pos;

i=0;
edit_pos=0;
while (i<sp->dclipindex) {
	edit_pos += DataClipLength(&sp->disksamp->dclips[i]);
	i++;
	}

edit_pos += (sp->fileindex-sp->disksamp->dclips[i].start);
return(edit_pos);
}

/************************************************************/

DataClipLength(dclip)
struct SampleDataClip *dclip;
{
return(dclip->end-dclip->start+1);
}

/*############################################################
NAME	
	SampleOpen -
FUNCTION
	returns sample pointer which you can use in calls to
	SampleRead(), SampleWrite(), etc.  SampleOpen() requires
	that a Disk_Samp already exists (use CreateSampleStruct())
INPUTS
	ptr pointer to Disk_Samp that you want to access
	flags way to access sample (SP_MODE_OLDFILE,
		SP_MODE_NEWFILE, SP_MODE_READWRITE)
RESULTS
	Sample pointer or NULL if can't open sample
##############################################################*/

struct SampleFilePointer *SampleOpen(ptr, flags)
struct Disk_Samp *ptr;
long flags;
{
char buf[120];
long mode;
struct SampleFilePointer *sp;

sp = (struct SampleFilePointer *)AllocMem(sizeof(struct SampleFilePointer), MEMF_CLEAR);
if (sp==0)
	{
	return(NULL);
	}

if (WorkingFilePath(ptr->name,ptr->version, buf, sizeof(buf))!=NO_ERROR) {
	FreeMem(sp, sizeof(struct SampleFilePointer));
	return(NULL);
	}

if (flags&SP_MODE_OLDFILE)
	mode=MODE_OLDFILE;
else if (flags&SP_MODE_NEWFILE)
	mode=MODE_NEWFILE;
else if (flags&SP_MODE_READWRITE)
	mode=MODE_READWRITE;
else {
	FreeMem(sp, sizeof(struct SampleFilePointer));
	return(NULL);
	}

sp->fp = SOpen(buf,  mode);	/** returns 0 if error **/
if (sp->fp==0) {
	FreeMem(sp, sizeof(struct SampleFilePointer));
	return(NULL);
	}

sp->flags = flags;
sp->disksamp = ptr;

InitSemaphore(&sp->locksfp);
SampleSeek(sp, 0L, SS_OFFSET_BEGINNING); /** Not necessarily the beginning of the file! **/

return(sp);
}

/*############################################################
NAME	
	SampleClose
FUNCTION
	Release a SampleFilePointer obtained with SampleOpen().
	This also calls SampleWriteTag() if the file being closed
	is SP_MODE_READWRITE or SP_MODE_NEWFILE
INPUTS
	sp	SampleFilePointer returned by SampleOpen()
RESULTS
	Returns Zero if successful
##############################################################*/

int SampleClose(sp)
struct SampleFilePointer *sp;
{
int reterr;
struct Disk_Samp *ptr;

ObtainSemaphore(&sp->locksfp);
if (!(sp->flags&SP_MODE_OLDFILE)) {
	SampleWriteTag(sp);
	}
ptr=sp->disksamp;
reterr=SClose(sp->fp);
ReleaseSemaphore(&sp->locksfp);
FreeMem(sp, sizeof(struct SampleFilePointer));
if (ptr->flags&DS_NEW_SAMPLE) {
	ptr->flags &= (~DS_NEW_SAMPLE);
	BroadcastEventParms(se_info_sample_created,ptr,0,0,0,0,0);
	}
return(reterr);
}

/*############################################################
NAME	
	SampleSetLength
FUNCTION
	Refreshes a Disk_Samp.length based on actual file length.
	This function also creates a dclips[0].  This function
	should not really be used, as SampleRead()/SampleWrite()
	will keep track of lengths for you.
INPUTS
	sp	SampleFilePointer returned by SampleOpen()
RESULTS
	NONE
##############################################################*/


void SampleSetLength(sp)
struct SampleFilePointer *sp;
{
ObtainSemaphore(&sp->locksfp);
SSeek(sp->fp, 0, OFFSET_END);
sp->disksamp->length = SSeek(sp->fp, 0, OFFSET_CURRENT)/2;
sp->disksamp->org_length = sp->disksamp->length;
sp->disksamp->dclips[0].end = sp->disksamp->length-1;
SSeek(sp->fp, 0, OFFSET_BEGINNING);
ReleaseSemaphore(&sp->locksfp);
}

/*############################################################
NAME	
	SampleCopy
FUNCTION
	Copy data from one SampleFilePointer to another using
	multiple SampleRead()/SampleWrite()s.
	StudioBase->defaults.copy_buf_size is used to determine
	how big a temporary buffer to allocate.
INPUTS
	tosp		SampleFilePointer returned by SampleOpen()
	fromsp	SampleFilePointer returned by SampleOpen()
	length	Number of samples to copy
RESULTS
	Returns standard error code (NO_ERROR if ok)
##############################################################*/

int SampleCopy(tosp, fromsp, length)
struct SampleFilePointer *tosp;
struct SampleFilePointer *fromsp;
int length;
{
char *buf;
int tomove;

if (length==0)
	return(NO_ERROR);

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(StudioBase->defaults.copy_buf_size/2, length);
	if (SampleRead(fromsp, buf, tomove)!=tomove)
		return(READ_ERROR);
	if (SampleWrite(tosp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	length = length - tomove;
	} while (length);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}

/*############################################################
NAME	
	SampleCreate -
FUNCTION
	Calls CreateSampleStruct() & SampleOpen() to create a newsample.
INPUTS
	name	A pointer to the name of the sample to create.  Version
			number will be zero.
RESULTS
	Sample pointer or NULL if can't create sample.  To access
	the created Disk_Samp, use sp->disksamp
##############################################################*/

struct SampleFilePointer *SampleCreate(name)
char *name;
{
struct SampleFilePointer *sp;
struct Disk_Samp *ptr;

if (CreateSampleStruct(name, 0, &ptr, 0)!=NO_ERROR)
	return(NULL);

sp=SampleOpen(ptr, SP_MODE_NEWFILE);
if (sp==NULL) {
	CloseSample(ptr);		/** Removes the sample struct from StudioBase */
	}
return(sp);
}

/*############################################################
NAME	
	SampleCreateRevision -
FUNCTION
	Calls CreateSampleStruct() & SampleOpen() to create a newsample
	with the same names as the passed disksamp but with version+1.
	Also copies samp->parms from the original to new samp.

	For example, suppose an editor wants to delete a range from a 
	sample.  One way is to create a new sample with the same name
	(but higher version number), copy the data from the original
	sample to the new sample that you want to keep (via SampleCopy),
	then delete the original sample.
INPUTS
	sampptr	a pointer to the disksamp that is used for a name and
				version number.
RESULTS
	Sample pointer or NULL if can't create sample.  To access
	the created Disk_Samp, use sp->disksamp
##############################################################*/

struct SampleFilePointer *SampleCreateRevision(sampptr)
struct Disk_Samp *sampptr;
{
struct SampleFilePointer *sp;
struct Disk_Samp *ptr;

if (CreateSampleStruct(sampptr->name, sampptr->version+1, &ptr, 0)!=NO_ERROR)
	return(NULL);

sp=SampleOpen(ptr, SP_MODE_NEWFILE);
if (sp==NULL) {
	CloseSample(ptr);		/** Removes the sample struct from StudioBase */
	}
CopySampData(sampptr,ptr);
return(sp);
}

/************************************************************/

void CopySampData(rs,ws)
struct Disk_Samp *rs,*ws;
{
ws->parms=rs->parms;
ws->type=rs->type;
}


/*############################################################
NAME	
	SampleWriteTag -
FUNCTION
	Update's the "SampleFileTag" structure which is located
	at the beginning of every "Sample File"  A Sample File is
	the working file, on disk, associated with a particular
	Disk_Samp structure.

	If you change a sample's paramaters (such as playback speed)
	you should call SampleWriteTag() to update the disk file.
INPUTS
	sp		result from SampleOpen()

RESULTS
	std error code
##############################################################*/

int SampleWriteTag(sp)
struct SampleFilePointer *sp;
{
struct SampleFileTag tag;
int error;

ObtainSemaphore(&sp->locksfp);
setmem(&tag, sizeof(struct SampleFileTag),0L);

tag.parms=sp->disksamp->parms;
tag.org_length=sp->disksamp->org_length;
tag.length=sp->disksamp->length;
movmem(sp->disksamp->dclips, tag.dclips, DS_NUM_CLIPS*sizeof(struct SampleDataClip));
tag.id=TAG_KWIK;
if (SWriteTag(sp->fp, &tag)==sizeof(struct SampleFileTag))
	error=NO_ERROR;
else
	error=WRITE_ERROR;
SampleSeek(sp, 0L, SS_OFFSET_CURRENT);
ReleaseSemaphore(&sp->locksfp);
return(error);
}

/*############################################################
NAME	
	SampleReadTag -
FUNCTION
	Reads a file's "SampleFileTag" structure which is located
	at the beginning of every "Sample File" and updates the 
	appropriate entries in the Disk_Samp structure. (playbackspeed,
	dclips, etc.)

	This is normally only called by system functions.
INPUTS
	sp		result from SampleOpen()

RESULTS
	standard error code
##############################################################*/

int SampleReadTag(sp)
struct SampleFilePointer *sp;
{
struct SampleFileTag tag;
int error;

ObtainSemaphore(&sp->locksfp);
setmem(&tag, sizeof(struct SampleFileTag),0L);

if (SReadTag(sp->fp, &tag)==sizeof(struct SampleFileTag))
	error=NO_ERROR;
else
	error=WRITE_ERROR;

if (tag.id!=TAG_KWIK)
	error=STDERR_NO_TAG;
if (error==NO_ERROR) {
	if (tag.parms.volume==0x8000)	/** Fix to convert Beta 0.52 vol's **/
		tag.parms.volume=ZERO_DB;	/** to new dB format. **/
	sp->disksamp->parms=tag.parms;
	sp->disksamp->org_length=tag.org_length;
	sp->disksamp->length=tag.length;
	movmem(tag.dclips, sp->disksamp->dclips, DS_NUM_CLIPS*sizeof(struct SampleDataClip));
	}
SampleSeek(sp, 0L, SS_OFFSET_CURRENT);
ReleaseSemaphore(&sp->locksfp);
return(error);
}

/************************************************************/

int SampleQueryAlign(sp)
struct SampleFilePointer *sp;
{
int retval;

ObtainSemaphore(&sp->locksfp);
retval=SQueryAlign(sp->fp);
ReleaseSemaphore(&sp->locksfp);
return(retval);
}
