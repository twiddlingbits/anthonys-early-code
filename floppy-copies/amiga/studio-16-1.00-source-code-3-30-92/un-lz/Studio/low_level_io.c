#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include "/include/psound.h"
#include <stdio.h>

extern struct StudioBase *StudioBase;
extern short se_info_sample_created;

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
	OBSLLETE! Use CreateSampleStruct()
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
	allocates a Disk_Samp structure, links it to StudioBase.
   The sample name is copied so that the passed name's memory
	can be re-used by the calling program.  Any working files
	that exist whos name matches the working file of this
	new sample are deleted (unless you set CSS_KEEP_WORKING_FILE)
	
INPUTS
	name     pointer to sample name
	version  version number of this name
	ptrptr	pointer to a Disk_Samp * that CreateSampleStruct fills in
	flags		CSS_KEEP_WORKING_FILE tells createsample that we are
				going to use an existing temporary file, so don't
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
ptr->wfp = -1;
for (i=0; i < MAX_READ_LOCKS; i++)
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
*ptrptr = ptr;
BroadcastEventParms(se_info_sample_created,ptr,0,0,0,0,0);
return(NO_ERROR);
}


/*############################################################
NAME
	CloseSample -

FUNCTION
   Closes all files and de-allocates all memory associated with
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
