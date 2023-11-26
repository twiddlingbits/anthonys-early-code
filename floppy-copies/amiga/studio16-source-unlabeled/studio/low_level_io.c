#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include "/include/psound.h"
#include <stdio.h>

extern struct studioBase *studioBase;
extern short se_info_sample_created;

/*############################################################
NAME	
	GetOneSample -
FUNCTION
	Given a file pointer (which should be a read or write lock
	returned from WriteLockSamp() or ReadLockSamp()) this will
	return one sample.
INPUTS
	fp file pointer to a read or write lock
	i index (0 to sample_len - 1) of sample to return
RESULTS
	a short sample value
##############################################################*/

short GetOneSample(fp, i)
int i;
int fp;
{
short val;

Seek(fp,i<<1,OFFSET_BEGINNING);
Read(fp, &val, 2);
return(val);
}

/******************************************************************/
/* quick_write - writes a block of data to a file pointed to		*/
/* by a sample ptr.  After calling this alot, you need to call		*/
/* update_block1(ptr, 0); to reset internal stuff						*/
/******************************************************************/

int QuickWrite(ptr,data,len)
char *data;
struct Disk_Samp *ptr;
int len;
{
ptr->length += len;
if (Write(ptr->wfp, data, len*2)!=len*2)
   return(WRITE_ERROR);
else
   return(NO_ERROR);
}

/******************************************************************/
/* quick_read - reads a block of data to a file pointed to			*/
/* by a sample ptr.  After calling this alot, you need to call		*/
/* update_block1(ptr, 0); to reset internal stuff						*/
/******************************************************************/

int QuickRead(fp,data,len)
int fp;
char *data;
int len;
{
if (Read(fp, data, len*2)!= len*2)
   return(READ_ERROR);
else
   return(NO_ERROR);
}

/*############################################################
NAME	
	WriteLockSamp -
FUNCTION
	returns a file pointer which you can use to write to a
	sample.  Since only one task can write to a sample, you
	must call WriteLockSamp() before writing.
INPUTS
	ptr pointer to Disk_Samp that you want to write to
RESULTS
	file pointer (lock) or -1 if no lock available
##############################################################*/

int WriteLockSamp(ptr)
struct Disk_Samp *ptr;
{
char buf[120];
char temp[40];

ObtainSemaphore(&studioBase->lock);
if (ptr->wfp!=-1) {
	ReleaseSemaphore(&studioBase->lock);
	return(-1);
	}

sprintf(temp,"%s.%03dL",ptr->name,++ptr->version);

if (strlen(temp)+strlen(studioBase->defaults.temp_files_path) > 117) {
	ReleaseSemaphore(&studioBase->lock);
	return(-1);
	}

strcpy(buf,studioBase->defaults.temp_files_path);
add(buf,temp);    /** append filename to path **/
ptr->wfp = Open(buf,  MODE_NEWFILE);	/** returns 0 if error **/
if (ptr->wfp==0)
	ptr->wfp=-1;
ReleaseSemaphore(&studioBase->lock);
return(ptr->wfp);
}

/*############################################################
NAME	
	WriteUnLockSamp -
FUNCTION
	Release a write lock that you gained with the WriteLockSamp()
	function.
INPUTS
	ptr - sample on which to free the write lock.
RESULTS
	NONE
##############################################################*/

void WriteUnLockSamp(ptr)
struct Disk_Samp *ptr;
{
Close(ptr->wfp);
ptr->wfp=-1;
}

/*############################################################
NAME	
	ReadLockSamp -
FUNCTION
	returns a file pointer which is a read lock on a specified 
	sample.  Up to MAX_READ_LOCKS read locks can be placed
	one a sample.
INPUTS
	ptr pointer to the Disk_Samp to read lock
RESULTS
	file pointer to read lock or -1 if no read lock could
	be allocated.
##############################################################*/

int ReadLockSamp(ptr)
struct Disk_Samp *ptr;
{
char buf[120];
char temp[40];
short i;

ObtainSemaphore(&studioBase->lock);

for (i=0; i < MAX_READ_LOCKS; i++) {
	if (ptr->rfp[i]==-1) {
		sprintf(temp,"%s.%03dL",ptr->name,ptr->version);
		if (strlen(temp)+strlen(studioBase->defaults.temp_files_path) > 117)
			return(-1);
		strcpy(buf,studioBase->defaults.temp_files_path);
		add(buf,temp);    /** append filename to path **/
		ptr->rfp[i] = Open(buf, MODE_OLDFILE);
		if (ptr->rfp[i]==0)
			ptr->rfp[i]=-1;
		ReleaseSemaphore(&studioBase->lock);
		return(ptr->rfp[i]);
		}
	}
ReleaseSemaphore(&studioBase->lock);
return(-1);
}
/******************************************************************/
/*  ReadUnLockSamp - 															*/
/******************************************************************/

void ReadUnLockSamp(ptr, fp)
struct Disk_Samp *ptr;
int fp;
{
int i;

for (i=0; i < MAX_READ_LOCKS; i++) {
	if (ptr->rfp[i]==fp) {
		Close(fp);
		ptr->rfp[i]=-1;
		}
	}
}
/****************************************/
void SetSampLength(ptr,fp)
struct Disk_Samp *ptr;
{
Seek(fp, 0, OFFSET_END);
ptr->length = Seek(fp, 0, OFFSET_CURRENT)/2;
Seek(fp, 0, OFFSET_BEGINNING);
}
/****************************************/

#define QC_BLOCKSIZE 4096			/** in bytes **/

int QuickCopy(tofp, fromfp, len)
int tofp;
int fromfp;
int len;
{
char *buf;
int tomove;

len = len*2;		/** convert samples to bytes **/

buf = (char *)AllocMem(QC_BLOCKSIZE, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(QC_BLOCKSIZE, len);
	if (Read(fromfp, buf, tomove)!=tomove)
		return(READ_ERROR);
	if (Write(tofp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	len = len - tomove;
	} while (len);

FreeMem(buf, QC_BLOCKSIZE);
return(NO_ERROR);
}
/****************************************/


/*############################################################
NAME
	create_sample -
FUNCTION
	allocates a Disk_Samp structure, links it to studioBase,
   and open's the required DOS files.  The sample name is copied
   so that the passed name's memory can be re-used by the calling
   program.
INPUTS
	name     pointer to sample name
	version  version number of this name
RESULTS
	returns an error code as described in studio.h
##############################################################*/

int CreateSample(name, version, ptrptr)
struct Disk_Samp **ptrptr;
char *name;
int version;
{
struct Disk_Samp *ptr;
int i;
struct MinNode *node;

for (node=studioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	if (strcmp(name,((struct Disk_Samp *)node)->name)==0)
		return(DUPLICATE_NAME);

ptr = (struct Disk_Samp *)AllocMem(sizeof(struct Disk_Samp), MEMF_CLEAR);
if (ptr==NULL)
   return(OUT_OF_MEMORY);

ptr->name=(char *)AllocMem(strlen(name)+1,0);
if (ptr->name==0) {
   FreeMem(ptr, sizeof(struct Disk_Samp));
   return(OUT_OF_MEMORY);
   }
strcpy(ptr->name, name);
ptr->version = version;
ptr->type=RIGHT;     /**!!!!! need to support stereo!!!!!! **/
ptr->wfp = -1;
for (i=0; i < MAX_READ_LOCKS; i++)
	ptr->rfp[i] = -1;

AddTail(&studioBase->samps, ptr);
*ptrptr = ptr;
BroadcastEventParms(se_info_sample_created,ptr,0,0,0,0,0);
return(NO_ERROR);
}


/*############################################################
NAME CloseSample -

FUNCTION
   closes all files and de-allocates all memory associated with
   a file sample.         
INPUTS
   pointer to Disk_Samp struct
RESULTS
   standard error code
##############################################################*/

int CloseSample(ptr)
struct Disk_Samp *ptr;
{
int error;

error=NO_ERROR;
   
Remove(ptr);   /** This is the same as Remove(ptr->all_node); **/
if (!(ptr->flags&KEEP_NAME))  /** ????!!!!!!!!! *******/
   FreeMem(ptr->name,strlen(ptr->name)+1);
FreeMem(ptr, sizeof(struct Disk_Samp));
return(error);
}

