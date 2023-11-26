#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"

extern struct StudioBase *StudioBase;
extern short se_info_sample_deleted;
extern short se_info_sample_created;

/** This file contains functions for creating/deleteing/manipulating **/
/** Disk_Sample structures.  File I/O with Disk_Samps is in sio.c  **/
/** and sampleio.c **/


/*############################################################
NAME	
	DeleteSample -

FUNCTION
	Call's CloseSample() and broadcasts the "SE_INFO_SAMPLE_DELETED"
	event.
INPUTS
	Disk_Samp pointer
RESULTS
	NONE
##############################################################*/

void DeleteSample(s)

struct Disk_Samp *s;

{
if (s==NULL)
   return;

CloseSample(s);
BroadcastEventParms(se_info_sample_deleted,s,0,0,0,0,0);
}

/*############################################################
NAME	
	DeleteWorkingFile -

FUNCTION
	Delete's a sample's working file from disk. 
	After making this call you must call DeleteSample() before
	anyone tries to get a read or write lock on the sample.
INPUTS
	disk samp pointer
RESULTS
	NONE
##############################################################*/

int DeleteWorkingFile(samp)
struct Disk_Samp *samp;
{
struct Disk_Samp *FindSample();
char buf[150];
int version;
int success;

/** first delete requested sample **/

version = samp->version;
WorkingFilePath(samp->name,version,buf,sizeof(buf));
success=DeleteFile(buf);
if(!success)
	return(success);

/** now delete lower versions **/

while (--version >= 0) {
	if (FindSample(samp->name, version)==NULL) {	/** unused? **/
		WorkingFilePath(samp->name,version,buf,sizeof(buf));
		success=DeleteFile(buf);
		}
	}
return(TRUE);
}


/*############################################################
NAME	
	GetSamplePtr -

FUNCTION
	return pointer to a sample given its position in the chain 
INPUTS
	index num (0 -> max_samples-1)
RESULTS
	struct Disk_Samp *
##############################################################*/

struct Disk_Samp *GetSamplePtr(i)
int i;
{
struct MinNode *node;

if (i < 0)        /** -1 means no entrys in list->current **/
   return(0);

ObtainSharedSemaphore(&StudioBase->lockstudio);

for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
   if (i-- <= 0) {
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
      return((struct Disk_Samp *)node);
		}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(NULL);
}

/*############################################################
NAME	
	GetSampleNum -

FUNCTION
	return position number of a sample 
INPUTS
	sample pointer
RESULTS
	index number (0 -> max_samples-1)
##############################################################*/

int GetSampleNum(s)
struct Disk_Sample *s;
{
struct MinNode *node;
int i;

i=0;
ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (s==(struct Disk_Samp *)node) {
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
      return(i);
		}
   i++;
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(-1);
}

/*############################################################
NAME	
	FindSample -
FUNCTION
	Searchs the sample list for the passed sample name
	Treats upper and lower case the same.
INPUTS
	pointer to name to search for
	version version number to match, or -1 for any version
RESULTS
	pointer to struct Disk_Samp or NULL
##############################################################*/

struct Disk_Samp *FindSample(name,version)
char *name;
int version;   
{
extern struct StudioBase *StudioBase;
struct Disk_Samp *s;
struct MinNode *node;

if (name==0)
   return(0);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   s=(struct Disk_Samp *)node;
   if (s->name) {
      if (stricmp(s->name,name)==0) {
			if (s->version==version || version==-1) {
				ReleaseSharedSemaphore(&StudioBase->lockstudio);
   	      return(s);
				}
			}
		}
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(0);
}

/***********************************************************/

int WorkingFilePath(name,version,buf,size)
char *name;
int version;
char *buf;
int size;
{
char temp[40];

sprintf(temp,"%s.%03dL",name,version);

if (strlen(temp)+strlen(StudioBase->defaults.temp_files_path) > (size-3)) {
	return(OUT_OF_MEMORY);
	}

strcpy(buf,StudioBase->defaults.temp_files_path);
add(buf,temp);    /** append filename to path **/
return(NO_ERROR);
}

/*############################################################
NAME
	CreateSample -
FUNCTION
	OBSOLETE! Use CreateSampleStruct()
	allocates a Disk_Samp structure, links it to StudioBase,
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
char *name;
int version;
struct Disk_Samp **ptrptr;
{
return(CreateSampleStruct(name, version, ptrptr, 0));
}


/*############################################################
NAME
	CreateSampleStruct -
FUNCTION
	Allocates a Disk_Samp structure, links it to StudioBase.
   The sample name is copied so that the passed name's memory
	can be re-used by the calling program.  Any working files
	that exist whos name/version matches the working file of this
	new sample are deleted (unless you set CSS_KEEP_WORKING_FILE).
	Also, working files with the same name but previous version
	numbers are deleted based upon "undo_levels."
	
INPUTS
	name     pointer to sample name
	version  version number of this name
	ptrptr	pointer to a Disk_Samp * that CreateSampleStruct fills in
	flags		CSS_KEEP_WORKING_FILE tells createsample that we are
				going to use an existing working file, so don't
				delete it.
RESULTS
	returns an error code as described in studio16.h
##############################################################*/

int CreateSampleStruct(name, version, ptrptr, flags)
char *name;
int version;
struct Disk_Samp **ptrptr;
unsigned long flags;
{
struct Disk_Samp *ptr;
int i;
int delete_me;
struct MinNode *node;
char buf[120];

ObtainExclusiveSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	if (strcmp(name,((struct Disk_Samp *)node)->name)==0)
		if (version<=((struct Disk_Samp *)node)->version) {
			ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
			return(DUPLICATE_NAME);
			}
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);

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
ptr->wfp = -1;	/** UNUSED--DELETE!!! **/
for (i=0; i < MAX_READ_LOCKS; i++)	/** UNUSED--DELETE!!! **/
	ptr->rfp[i] = -1;

/** If working file exists, erase it! **/

if (!(flags&CSS_KEEP_WORKING_FILE)) {
	WorkingFilePath(ptr->name,ptr->version,buf,sizeof(buf));
	DeleteFile(buf);
	}

if (version > StudioBase->defaults.undo_levels) {
	delete_me=ptr->version-StudioBase->defaults.undo_levels-1;
	if (!FindSample(name,delete_me)) {	/* In use? */
		WorkingFilePath(ptr->name,delete_me,buf,sizeof(buf));
		DeleteFile(buf);
		}
	}

ObtainExclusiveSemaphore(&StudioBase->lockstudio);
AddTail(&StudioBase->samps, ptr);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
ptr->flags |= DS_NEW_SAMPLE;
*ptrptr = ptr;
return(NO_ERROR);
}


/*############################################################
NAME
	CloseSample -

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
   
ObtainExclusiveSemaphore(&StudioBase->lockstudio);
Remove(ptr);   /** This is the same as Remove(ptr->all_node); **/
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
if (!(ptr->flags&KEEP_NAME))  /** ????!!!!!!!!! *******/
   FreeMem(ptr->name,strlen(ptr->name)+1);
FreeMem(ptr, sizeof(struct Disk_Samp));
return(error);
}

