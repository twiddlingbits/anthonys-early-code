#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include "/include/studio16.h"

extern short se_info_file_filter_added;
extern short se_info_file_filter_deleted;

extern struct StudioBase *StudioBase;

struct FileFilter *GetFileFilter();


/*############################################################
NAME	
	AddFileFilter -
FUNCTION
	Adds a new file filter to studio 16.  File filters are used
	when accessing a samples data.  Usually via SampleOpen().
INPUTS
	NewFileFilter struct *
RESULTS
	NULL or pointer to FileFilter if everything went ok
##############################################################*/

struct FileFilter *AddFileFilter(nff)
struct NewFileFilter *nff;
{
extern struct StudioBase *StudioBase;
char *name;
struct FileFilter *new;

if (nff->name) {
	name = (char *)AllocMem(strlen(nff->name)+1,0);
	if (name==0) return(0);
	strcpy(name,nff->name);
	}
else {
	name = 0;
	}

new = (struct FileFilter *)AllocMem(sizeof(struct FileFilter), MEMF_CLEAR);
if (new==0) {
	if (name)
		FreeMem(name, strlen(name)+1);
	return(0);
	}

new->flags = nff->flags;
new->name = name;
new->FFOpen=			nff->FFOpen;
new->FFRead=			nff->FFRead;
new->FFWrite=			nff->FFWrite;
new->FFSeek=			nff->FFSeek;
new->FFClose=			nff->FFClose;
new->FFReadTag=		nff->FFReadTag;
new->FFWriteTag=		nff->FFWriteTag;
new->FFQueryAlign=	nff->FFQueryAlign;
new->FFQueryOptions=	nff->FFQueryOptions;
new->node.ln_Pri=nff->priority;
ObtainExclusiveSemaphore(&StudioBase->lockstudio);
Enqueue(&StudioBase->file_filters, new);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
BroadcastEventParms(se_info_file_filter_added,new,0,0,0,0,0);

return(new);
}

/*############################################################
NAME	
	DeleteFileFilter -
FUNCTION
	Remove a previously added file filter from StudioBase.
	File filters are use to translate Sample files upon load
	to/from studio 16's internal format.
INPUTS
	pointer to a FileFilter that was returned from AddFileFilter()
RESULTS
	NONE
##############################################################*/

void DeleteFileFilter(ff)
struct FileFilter *ff;
{
extern struct StudioBase *StudioBase;

ObtainExclusiveSemaphore(&StudioBase->lockstudio);
Remove(ff);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
if (ff->name)
	FreeMem(ff->name, strlen(ff->name)+1);
FreeMem(ff, sizeof(struct FileFilter));
BroadcastEventParms(se_info_file_filter_deleted,ff,0,0,0,0,0);
}


/*############################################################
NAME	
	GetFileType
FUNCTION
	Determine FileFilter that can decode a given sample with
	the indicated access mode.
INPUTS
	buf complete path ponting to file
	mode MODE_OLDFILE, MODE_NEWFILE, MODE_READWRITE
	ptr   An optional pointer to a long.  If this is non null,
			GetFileType(), MAY fill it in with a non null
			handle from FFOpen().  This is used by internal
			Studio 16 routines to speed up cases that must
			immediately recall FFOpen().
RESULTS
	NULL if no qualified FileFilter available, or FileFilter pointer.
##############################################################*/

struct FileFilter *GetFileType(buf,fmode,ptr)
char *buf;
long fmode;
long *ptr;
{
struct Node *node;
struct FileFilter *ff;
long ffp;
long lock;

/** Use default write fileformat, if creating new file **/

if (fmode==MODE_NEWFILE)
	return(GetFileFilter(StudioBase->defaults.file_filter_name));
if (fmode==MODE_READWRITE) 
	if (lock=Lock(buf,ACCESS_READ))
		UnLock(lock);
	else
		return(GetFileFilter(StudioBase->defaults.file_filter_name));

fmode=MODE_OLDFILE;
ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
	ff=(struct FileFilter *)node;
	if (ffp=(*ff->FFOpen)(buf, fmode)) {
		if (ptr)
			*ptr=ffp;
		else
			(*ff->FFClose)(ffp);
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
		return(ff);
		}
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(0);
}

/*############################################################
NAME	
	GetFileFilter
FUNCTION
	Given the name of a file filter, determine its pointer.
INPUTS
	name	name of the file filter
RESULTS
	NONE
##############################################################*/

struct FileFilter *GetFileFilter(name)
char *name;
{
struct FileFilter *ff;
struct Node *node;

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
	ff=(struct FileFilter *)node;
	if (stricmp(ff->name, name)==0) { 
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
		return(ff);
		break;
		}
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(NULL);
}

