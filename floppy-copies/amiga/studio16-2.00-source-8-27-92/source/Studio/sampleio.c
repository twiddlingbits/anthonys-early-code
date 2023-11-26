#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include "/include/psound.h"
#include <stdio.h>

/** This file contains "SampleX()" functions.  These functions **/
/** are the prefered way of manipulating sample files' **/
/** data.  These functions all make call's to FileFilter functions **/
/** FFOpen(), FFSeek(), etc.  these are lower level calls that hide **/
/** The actual file structure for particular type of file (AIFF, etc.) **/

/**!!!!! ADD A SampleDelete() menu! **/

extern struct StudioBase *StudioBase;
extern short se_info_sample_created;

BPTR Lock(char *, LONG);
void MapEditedToData2();

/*############################################################
NAME	
	SampleGetOne -
FUNCTION
	Given a SampleFilePointer (which should be obtained by calling
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
	with sample files.  SampleRead understands file formats and
	DataClips inside a sample and deals with them.   You can
	use SampleSeek() to find the beginning of a sample's data.
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
struct FileFilter *ff;
struct SampleDataClip *dclips;
int readlen=0;
int actread=0;
int totalread;

ObtainSemaphore(&sp->locksfp);
dclips=&sp->tag.dclips[0];
totalread=0;
ff=sp->ff;

while (length >0 && actread==readlen && dclips[sp->dclipindex].end &&	sp->dclipindex < DS_NUM_CLIPS) {
	readlen = min(length, dclips[sp->dclipindex].end-sp->fileindex+1);
	if (readlen <=0)
		break;
	actread=(*ff->FFRead)(sp->ffp,buffer,readlen);
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
			(*ff->FFSeek)(sp->ffp,sp->fileindex,OFFSET_BEGINNING);
			}
		else {		/** init a new clip if at end of file **/
			if (sp->fileindex!=sp->tag.org_length) {
				sp->fileindex=sp->tag.org_length;
				(*ff->FFSeek)(sp->ffp,sp->fileindex,OFFSET_BEGINNING);
				}
			dclips[sp->dclipindex].start=sp->tag.org_length;
			dclips[sp->dclipindex].end=sp->tag.org_length-1;
			}
		}
	}
ReleaseSemaphore(&sp->locksfp);
return(totalread);
}

/*############################################################
NAME	
	SampleWrite -
FUNCTION
	This is similar to a dos Write() function, except it deals
	with sample files.  SampleWrite understands file formats and 
	DataClips inside a sample and deals with them. You can
	use SampleSeek() to find a position inside sample's data.
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
struct FileFilter *ff;
struct SampleDataClip *dclips;
int writelen;
int totalwritten;
int count;

ObtainSemaphore(&sp->locksfp);
dclips=&sp->tag.dclips[0];
totalwritten=0;
ff=sp->ff;

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
		totalwritten+=(*ff->FFWrite)(sp->ffp,buffer,writelen);
		buffer+=writelen;	/** buffer points to short so this adds 2X */
		length-=writelen;
		sp->fileindex+=writelen;
		if (sp->fileindex > dclips[sp->dclipindex].end) {
			sp->dclipindex++;
			if (dclips[sp->dclipindex].end) {
				/** check for empty clips hanging on the end **/
				if (dclips[sp->dclipindex].start==sp->tag.org_length) {
					count=totalwritten+SampleAppend(sp, buffer, length);
					ReleaseSemaphore(&sp->locksfp);
					return(count);
					}
				else {	/** must be new clip **/
					sp->fileindex=dclips[sp->dclipindex].start;
					(*ff->FFSeek)(sp->ffp,sp->fileindex,OFFSET_BEGINNING);
					}
				}
			else {
				dclips[sp->dclipindex].start=sp->tag.org_length;
				dclips[sp->dclipindex].end=sp->tag.org_length-1;
				count=totalwritten+SampleAppend(sp, buffer, length);
				ReleaseSemaphore(&sp->locksfp);
				return(count);
				}
			}
		}
	}
ReleaseSemaphore(&sp->locksfp);
return(totalwritten);
}


SampleAppend(sp, buffer, length)
struct SampleFilePointer *sp;
short *buffer;
long length;
{
int totalwritten;

totalwritten=(*sp->ff->FFWrite)(sp->ffp,buffer,length);
sp->tag.length += totalwritten;
sp->tag.org_length += totalwritten;
sp->tag.dclips[sp->dclipindex].end = sp->tag.org_length-1;
sp->fileindex+=totalwritten;
return(totalwritten);
}

/*############################################################
NAME	
	SampleSeek -
FUNCTION
	This is similar to a dos Seek() function, except it deals
	with sample files.  Seeks to the specified position
	within a sample. Understands DataClips inside a sample 
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
	rpos += (sp->tag.length);
else if (mode==SS_OFFSET_CURRENT)
	rpos += iamat;

if (rpos < 0)
	rpos=0;	/** Saftey check **/
MapEditedToData2(sp,rpos,&sp->fileindex,&sp->dclipindex);
if (sp->fileindex < 0)	/** saftey check **/
	sp->fileindex=0;
(*sp->ff->FFSeek)(sp->ffp,sp->fileindex,OFFSET_BEGINNING);	/* seekpos is in samps */
ReleaseSemaphore(&sp->locksfp);
return(iamat);
}

/************************************************************/
/** Given a corrdinate in the "edited" sample, return      **/
/** the equivilant position in the actual data on disk 	  **/
/************************************************************/

int MapEditedToData(sp,editval)
struct SampleFilePointer *sp;
int editval;
{
short dclipindex;
int fileindex;

MapEditedToData2(sp,editval,&fileindex,&dclipindex);
return(fileindex);
}

/************************************************************/

void MapEditedToData2(sp,editval,fileindex,dclipindex)
struct SampleFilePointer *sp;
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

while (sp->tag.dclips[i].end && i < DS_NUM_CLIPS) {
	clip_len=DataClipLength(&sp->tag.dclips[i]);
	if (editval >= edit_pos && editval < (edit_pos+clip_len)) {
		*fileindex=sp->tag.dclips[i].start+editval-edit_pos;
		*dclipindex=i;
		return;
		}
	edit_pos += clip_len;
	i++;
	}

/** Here if edit_val is out of range. **/

/** Seeking to EOF & there is a null clip present? **/
if (sp->tag.dclips[i-1].end==sp->tag.org_length-1 &&
	editval==sp->tag.length) {
	*fileindex=sp->tag.org_length;
	*dclipindex=i-1;
	}
else if (i>=DS_NUM_CLIPS) {	/** clip overflow? **/
	*fileindex=sp->tag.dclips[DS_NUM_CLIPS-1].start;
	*dclipindex=DS_NUM_CLIPS-1;
	}
else {	/** Initilize a new clip to zero length **/
	sp->tag.dclips[i].start=sp->tag.org_length;
	sp->tag.dclips[i].end=sp->tag.org_length-1;
	*fileindex=sp->tag.org_length;
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
	edit_pos += DataClipLength(&sp->tag.dclips[i]);
	i++;
	}

edit_pos += (sp->fileindex-sp->tag.dclips[i].start);
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
INPUTS
	name	A complete Ascii path pointer to a file.  For
			Example, "DH0:Project1/Bang"
	flags	way to access sample (SP_MODE_OLDFILE,
			SP_MODE_NEWFILE, SP_MODE_READWRITE)
	ff		FileFilter to use when accessing file, or NULL for
			default.file_filter_name on writes and autodetect on reads.
RESULTS
	SampleFilePointer or NULL if can't open sample
##############################################################*/

struct SampleFilePointer *SampleOpen(name, flags, dff)
char *name;
long flags;
struct FileFilter *dff;
{
long mode;
struct SampleFilePointer *sp;
long lock;

/*kprintf("SAMPLEOPEN '%s'\n",name);*/

/*** Allocate SampleFilePointer strucutre ***/

sp = (struct SampleFilePointer *)AllocMem(sizeof(struct SampleFilePointer), MEMF_CLEAR);
if (sp==0)
	{
	return(NULL);
	}

/*** Determine Access mode ***/

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

/** Determine if we are creating a new file **/

switch(mode) {
	case MODE_READWRITE:
		if (lock=Lock(name,ACCESS_READ)) {
			UnLock(lock);
			break;
			}
	case MODE_NEWFILE:
			flags |= SP_NEW_SAMPLE; /** so SampleClose sends out a msg **/
			break;
	case MODE_OLDFILE:
			break;
		}

/*** Determine FileFilter to use when accessing sample ***/

if (dff) {
	sp->ff=dff;
	}
else {
	sp->ff=(struct FileFilter *)GetFileType(name,mode,&sp->ffp);
	if (sp->ff==NULL) {
		FreeMem(sp, sizeof(struct SampleFilePointer));
		return(NULL);
		}
	}

/*kprintf("FileFormat: %s\n",sp->ff->name);*/

/** At this point, ffp MAY have been filled in by GetFileType() **/

if (sp->ffp==0) 
	sp->ffp = (*sp->ff->FFOpen)(name,  mode);	/** returns 0 if error **/
if (sp->ffp==0) {
	FreeMem(sp, sizeof(struct SampleFilePointer));
	return(NULL);
	}


sp->flags = flags;
strcpy(sp->samp_name,name);
InitSemaphore(&sp->locksfp);
if (!(sp->flags&SP_NEW_SAMPLE))
	SampleReadTag(sp);	/** Fill in tag structure **/
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
char *str;

ObtainSemaphore(&sp->locksfp);
if (!(sp->flags&SP_MODE_OLDFILE)) {
	SampleWriteTag(sp);
	}
reterr=(*sp->ff->FFClose)(sp->ffp);
if (sp->flags&SP_NEW_SAMPLE) {
	str=(char *)AllocCompletePath(sp->samp_name,NULL);
	BroadcastAndLock(se_info_sample_created,str,0,0,0,0,0);
	UnlockedFreeMem(str, strlen(str)+1);
	}
ReleaseSemaphore(&sp->locksfp);
FreeMem(sp, sizeof(struct SampleFilePointer));
return(reterr);
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
	Calls SampleOpen(name, SP_MODE_NEWFILE, NULL)
INPUTS
	name	A pointer to the name of the sample to create.
RESULTS
	Sample pointer or NULL if can't create sample.
##############################################################*/

struct SampleFilePointer *SampleCreate(name)
char *name;
{
struct SampleFilePointer *sp;

sp=SampleOpen(name, SP_MODE_NEWFILE, 0);
return(sp);
}

/*############################################################
NAME	
	SampleWriteTag -
FUNCTION
	Causes a open sample's tag structure to be written out
	to disk via the FileFilter.  Not all file formats support
	all tag entries.  For example, RAW doesn't support any.

	If you change a sample's paramaters (such as playback speed)
	you should call SampleWriteTag() to update the disk file.

	SampleClose() will automaticlly call SampleWriteTag() if
	the file is MODE_NEWFILE or MODE_READWRITE.	
INPUTS
	sp		result from SampleOpen()
RESULTS
	std error code
##############################################################*/

int SampleWriteTag(sp)
struct SampleFilePointer *sp;
{
int error;

ObtainSemaphore(&sp->locksfp);

if ((*sp->ff->FFWriteTag)(sp->ffp, &sp->tag)==sizeof(struct SampleFileTag))
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
	Causes the FileFilter to fill in the SampleFileTag structure
	by reading the appropriate fields (as known by the file filter)
	from the sample's disk file.
	This is normally only called by system functions.
INPUTS
	sp		result from SampleOpen()
RESULTS
	standard error code
##############################################################*/

int SampleReadTag(sp)
struct SampleFilePointer *sp;
{
int error;

ObtainSemaphore(&sp->locksfp);

if ((*sp->ff->FFReadTag)(sp->ffp, &sp->tag)==sizeof(struct SampleFileTag))
	error=NO_ERROR;
else
	error=WRITE_ERROR;

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
retval=(*sp->ff->FFQueryAlign)(sp->ffp);
ReleaseSemaphore(&sp->locksfp);
return(retval);
}
