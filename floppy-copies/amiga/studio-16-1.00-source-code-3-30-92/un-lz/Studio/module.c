#include <exec/types.h>
#include <exec/memory.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include "/include/studio16.h"

extern short se_info_state_added;
extern short se_info_state_deleted;
extern short se_info_module_added;
extern short se_info_module_deleted;
extern short se_info_file_filter_added;
extern short se_info_file_filter_deleted;

struct StandardModState *AddModState();
struct StandardModState *FindModState();
struct Module *FindModule();
void DeleteModState();

struct SampleFilePointer *SampleOpen();

int errno, _OSERR;	/** rename() puts stuff here! **/


/*############################################################
NAME	
	AddModule -
FUNCTION
	Allocate memory for a new module struct and links it into the
	StudioBase->modules chain.  All data from NewModule is
	copied, including the name, so that NewModule may be
	discarded. A port is also allocated to recieve broadcasted
	messsages (module->notifyme).  The Modules __ModState struct
	is also allocated and zereoed based on the passed size.
INPUTS
	pointer to module struct
RESULTS
	NONE
##############################################################*/

struct Module *AddModule(nm)
struct NewModule *nm;
{
extern struct StudioBase *StudioBase;
char *name;
struct Module *create_module();
struct Module *new;
struct MsgPort *port;
struct StandardModState *state;

if (nm->mod_flags&MOD_SINGLE_RESIDENT) 
	if (FindModule(nm->name))		/** Already loaded?? **/
		return(NULL);

if (nm->name) {
   name = (char *)AllocMem(strlen(nm->name)+1,0);
   if (name==0) return(0);
   strcpy(name,nm->name);
   }
else {
   name = 0;
   }

port=(struct MsgPort *)CreatePort(0,0);
if (port==0) {
	if (name)
	   FreeMem(name, strlen(name)+1);
	return(0);
	}

state = FindModState(name);
if (state==NULL) {
	Forbid();
	state = AddModState(nm->state_size,name,NULL);
	state->flags |= nm->state_flags;
	Permit();
	}
else {	/** state_size's should match! **/
	if (state->state_size!=nm->state_size) {
		if (port) 
			DeletePort(port);
		if (name)
	   	FreeMem(name, strlen(name)+1);
		return(0);
		}
	}
if (state==NULL) {
	DeletePort(port);
	if (name)
	   FreeMem(name, strlen(name)+1);
	return(0);
	}

new = (struct Module *)AllocMem(sizeof(struct Module), MEMF_CLEAR);
if (new==0) {
	DeleteModState(state);
	DeletePort(port);
	if (name)
	   FreeMem(name, strlen(name)+1);
	return(0);
	}

new->name = name;
new->notifyme = port;
new->state = state;
new->flags = nm->mod_flags|MOD_JUST_ADDED;
state->attached_module=new;
ObtainExclusiveSemaphore(&StudioBase->lockstudio);
AddTail(&StudioBase->modules, new);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
BroadcastEventParms(se_info_module_added,new,0,0,0,0,0);

return(new);
}

/*############################################################
NAME	
	FindModState -

FUNCTION
	Searchs for a prexisting mod state. Search order is:
	1) any module with same name & MS_NEED_MODULE flag set
	2) any module with same name
INPUTS
	module name (should match file name of module)
RESULTS
	struct StandardModState *
##############################################################*/


struct StandardModState *FindModState(name)
char *name;
{
struct StandardModState *s;
struct Node *node;

/** First search for any ModState's that have MS_NEED_MODULE set **/

ObtainExclusiveSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   s=(struct StandardModState *)node;
   if (s->flags&MS_NEED_MODULE) {
      if (stricmp(s->module_name,name)==0) {
			s->flags &= (~MS_NEED_MODULE);	/* clear flag */
			ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
         return(s);
			}
		}
	}

/** No MS_NEED_MODULE's so take first guy in list who has
	the same module name as the name passed to us **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   s=(struct StandardModState *)node;
	if (stricmp(s->module_name,name)==0) {
		if (s->attached_module==NULL) {
/** The following flags should probably be set by each module **/
/** after it has initilized it's modstate.  However, we never **/
/** required it.  It's set here based on the assumption that **/
/** You are trying to find a state that is already initilized. **/
/** This is fairly safe, since recently created states (thus **/
/** un-initilized) generally have the MS_NEED_MODULE flags set **/
			s->flags |= MS_STATE_INITILIZED;	/* set flag */
			ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
   	   return(s);
			}
		}
	}
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
return(NULL);
}

/*############################################################
NAME	
	AddModState -

FUNCTION
	Allocate's a new Mod State structure & links it into StudioBase
	
INPUTS
	state_size	size of state struct to alloc
	name			name of module or NULL for no name
	instname		name of this instance or NULL to generate default
RESULTS
	NONE
##############################################################*/

struct StandardModState *AddModState(state_size,name,instname)
char *name;
char *instname;
int state_size;
{
struct StandardModState *s;
struct Node *node;
short instance_count;
char line[50];

ObtainExclusiveSemaphore(&StudioBase->lockstudio);

/** count # of module's with same name **/

instance_count = 0;			
for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   s=(struct StandardModState *)node;
	if (stricmp(s->module_name,name)==0) {
		instance_count++;
		}
	}

/** Create Mod Struct **/

s=(struct StandardModState *)AllocMem(state_size, MEMF_CLEAR);
s->state_size = state_size;

if (name) {
	s->module_name = (char *)AllocMem(strlen(name)+1,0);
	if (s->module_name==0) {
		FreeMem(s,state_size);
		ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
		return(NULL);
		}

	strcpy(s->module_name, name);
	}

if (instname) {
	sprintf(line,"%s",instname);
	}
else {
	if (instance_count==0)	/** don't put #1, just #2,#3, etc. **/
		sprintf(line,"%s",name);
	else
		sprintf(line,"%s #%d",name,instance_count+1);
	}
s->instance_name = (char *)AllocMem(strlen(line)+1,0);
if (s->instance_name==0) {
	FreeMem(s->module_name,strlen(s->module_name)+1);
	FreeMem(s,state_size);
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(NULL);
	}

strcpy(s->instance_name, line);

AddTail(&StudioBase->module_states, s);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
BroadcastEventParms(se_info_state_added,s,0,0,0,0,0);
return(s);
}

/*############################################################
NAME	
	DeleteModState -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

void DeleteModState(s)

struct StandardModState *s;
{
if (s->attached_module!=NULL)
	return;
ObtainExclusiveSemaphore(&StudioBase->lockstudio);
Remove(s);
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
if (s->module_name)
	FreeMem(s->module_name,strlen(s->module_name)+1);
if (s->instance_name)
	FreeMem(s->instance_name,strlen(s->instance_name)+1);
FreeMem(s,s->state_size);
BroadcastEventParms(se_info_state_deleted,s,0,0,0,0,0);
}



/*############################################################
NAME	
	DeleteModule -
FUNCTION
	Deallocates a module's memory and removes it from the module
	list.  The passed module must be linked to StudioBase->Modules.
	The module's notifyme port is also closed and all any references
	that will cause events to be sent to the specified module
	are removed.
INPUTS
	pointer to struct Module
RESULTS
	NONE
##############################################################*/

void DeleteModule(m)
struct Module *m;			/** Module to delete **/
{
struct StudioEvent *e;
int i;

while(m->state->lock) WaitTOF();

ObtainExclusiveSemaphore(&StudioBase->lockstudio);

for (i=0; i < StudioBase->max_registered_ids; i++) {
	IgnoreEvent(i, m);
	IgnoreCount(i, m);
	}

Remove(m);     /** this is the same as Remove(&m->node) **/
m->state->attached_module=NULL;
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);

if (m->name)
   FreeMem(m->name, strlen(m->name)+1);

while ((e=(struct StudioEvent *)GetMsg(m->notifyme)))
	ReplyMsg(e);

DeletePort(m->notifyme);
FreeMem(m,sizeof(struct Module));
BroadcastEventParms(se_info_module_deleted,m,0,0,0,0,0);
}

/*############################################################
NAME	
	FindModule -
FUNCTION
	Searchs the module list for a module with the passed name.
	Treats upper and lower case the same.
INPUTS
	pointer to name to search for
RESULTS
	pointer to struct Module or NULL
##############################################################*/

struct Module *FindModule(name)
char *name;    /* module name to search for **/
{
extern struct StudioBase *StudioBase;
struct Module *m;
struct MinNode *node;

if (name==0)
   return(0);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   m=(struct Module *)node;
   if (m->name)
      if (stricmp(m->name,name)==0) {
			ReleaseSharedSemaphore(&StudioBase->lockstudio);
         return(m);
			}
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(0);
}

/**********************************************************/

struct Module *FindLatestModule(name)
char *name;    /* module name to search for **/
{
extern struct StudioBase *StudioBase;
struct Module *m;
struct MinNode *node;

if (name==0)
   return(0);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   m=(struct Module *)node;
   if (m->name) {
		if (m->flags&MOD_JUST_ADDED) {
	      if (stricmp(m->name,name)==0) {
				m->flags &= (~MOD_JUST_ADDED);
				ReleaseSharedSemaphore(&StudioBase->lockstudio);
      	   return(m);
				}
			}
		}
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(0);
}

/*############################################################
NAME	
	GetModuleNum -
FUNCTION
	Given a pointer to a module, returns the index of that module
	in the linked list of modules.
INPUTS
	module pointer
RESULTS
	Module index
##############################################################*/

int GetModuleNum(m)
struct Module *m;
{
struct MinNode *node;
int i;

i=0;
ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (m==(struct Module *)node) {
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
	AddFileFilter -
FUNCTION
	Add's a new file filter to studio 16.  File filters are used
	when loading or saveing sample files.  You pass a pointer
	to a FileFilter struct when calling SaveSamp() or
	LoadSamp()
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
new->func	= nff->func;
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
	Remove a previously added filter filter from StudioBase.
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
	LoadSamp -
FUNCTION
	Create's a new studio16 "Disk_Samp" from an AmigaDos file.
	Each load filter in StudioBase->file_filters is called
	to determine if it understands the file type.  These filters
	are linked by priority (as specified in NewFileFilter).  
	Usually the "RAW" filter is last (pri -128.)
INPUTS
	path		device path.  Eg. "dh0:samples" or NULL
	fname		file name. Eg. "door_bell".
	sampname	name to use when creating Disk_Samp. Often the same
				as fname.
RESULTS
	NONE
##############################################################*/

int LoadSamp(path, fname, sampname)
char *path, *fname, *sampname;
{
struct Node *node;
struct FileFilter *ff;
char *inname;
unsigned long infp;
short namelen;
int error;
struct SampleFilePointer *wsp;
struct Disk_Samp *samp;

namelen=strlen(path)+strlen(fname)+2;
inname = (char *)AllocMem(namelen,0);
strcpy(inname, path);
add(inname, fname);

infp = Open(inname, MODE_OLDFILE);
FreeMem(inname, namelen);

if (infp==NULL)
	return(CANT_OPEN_FILE);

wsp=(struct SampleFilePointer *)SampleCreate(sampname);
if (wsp==NULL) {
	Close(infp);
	return(STDERR_CANT_GET_WRITE_LOCK);
	}

ObtainSharedSemaphore(&StudioBase->lockstudio);
error=STDERR_UNKNOWN_FILE_FORMAT;
for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   ff=(struct FileFilter *)node;
	if (ff->flags&FF_LOAD_FILTER) {
	   if ((error=((*ff->func)(infp, wsp)))==NO_ERROR) {
			break;
			}
	   }
	}

ReleaseSharedSemaphore(&StudioBase->lockstudio);
samp=wsp->disksamp;
SampleClose(wsp);
Close(infp);
if (error!=NO_ERROR)
	CloseSample(samp);	/** Like DeleteSample but no msg broadcasted **/
return(error);
}

/*############################################################
NAME	
	SaveSamp -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

int SaveSamp(path, fname, samp, ff)
char *path, *fname;
struct Disk_Samp *samp;
struct FileFilter *ff;
{
struct Node *node;
char *outname;
unsigned long outfp;
short namelen;
int error;
struct SampleFilePointer *rsp;

namelen=strlen(path)+strlen(fname)+2;
outname = (char *)AllocMem(namelen,0);
strcpy(outname, path);
add(outname, fname);

outfp = Open(outname, MODE_NEWFILE);
FreeMem(outname, namelen);

if (outfp==NULL)
	return(CANT_OPEN_FILE);

if ((rsp=SampleOpen(samp,SP_MODE_OLDFILE))==0) {
	Close(outfp);
	return(STDERR_CANT_GET_READ_LOCK);
	}

if (ff==NULL) {
	ObtainSharedSemaphore(&StudioBase->lockstudio);
	for (node=StudioBase->file_filters.lh_Head; node->ln_Succ; node = node->ln_Succ) {
	   ff=(struct FileFilter *)node;
		if (ff->flags&FF_SAVE_FILTER)
			break;
		}
	ReleaseSharedSemaphore(&StudioBase->lockstudio);
	}

if (ff==NULL || !(ff->flags&FF_SAVE_FILTER)) {
	Close(outfp);
	SampleClose(rsp);
	return(STDERR_UNKNOWN_FILE_FORMAT);
	}

error=(*ff->func)(outfp, rsp);
Close(outfp);
SampleClose(rsp);
return(error);
}

/*############################################################
NAME
	RenameTempFile -

FUNCTION
	Given a complete path (path+name), this function renames
	the specified file to/from audio:samp->name.001l for use
	as a working file.
INPUTS
	path,name source path if "!saveing", destination if saveing
	samp 	Disk_Samp to get name & version from
	saveing TRUE if renaming from audio:,
			  FALSE if renameing into audio:
RESULTS
	Standard error code
##############################################################*/

int RenameTempFile(path,name,samp,saveing)
char *path;
char *name;
struct Disk_Samp *samp;
int saveing;
{
int error;
int namelen;
char *outname;
char temppath[150];

/** First build complete temporary file path eg. audio:samp.000L **/

	WorkingFilePath(samp->name,samp->version,temppath,sizeof(temppath));

/** Now build complete destination name path ***/

	namelen=strlen(path)+strlen(name)+2;
	outname = (char *)AllocMem(namelen,0);
	if (outname==0)
		return(OUT_OF_MEMORY);
	strcpy(outname, path);
	add(outname, name);

/** Now try and perform rename **/

	if (saveing)
		error = rename(temppath,outname);
	else
		error = rename(outname,temppath);
	FreeMem(outname, namelen);
	if (error) {
		if (saveing)
			return(STDERR_CANT_DO_RENAME_FROM_TEMP);
		else
			return(STDERR_CANT_DO_RENAME_TO_TEMP);
		}
	else {
		return(NO_ERROR);
	}
}

/*############################################################
NAME	
	IsRenamePossible -

FUNCTION
	Checks to see if a call to "RenameTempFile()" with the
	same parameters would succeed.  RenameTempFile uses
	a samp struct to get samp->name & samp->version.  
	IsRenamePossible assumes that 1) version is 0, and
	2) samp->name == name (as passed below).
INPUTS
	see RenameTempFile
RESULTS
	TRUE if the rename would work, else FALSE
##############################################################*/

int IsRenamePossible(path,name,saveing)
char *path;
char *name;
int saveing;
{
struct Disk_Samp samp;

samp.version=0;	/** our dummy sample **/
samp.name=name;

if (RenameTempFile(path,name,&samp,saveing)!=NO_ERROR)
	return(FALSE);

/** rename worked ok, so lets put it back the way we found it */

RenameTempFile(path,name,&samp,!saveing);
return(TRUE);
}

/*############################################################
NAME	
	QuickSaveSamp -
FUNCTION
	"Saves" a sample by simply renaming it to the passed complete
	path (path+name).
INPUTS
	path 	directory to put sample in.  Must be on same device as AUDIO:
	name	name to rename sample as
	samp  the Disk_Samp to "save"
RESULTS
	Standard error code (NO_ERROR, etc.  see studio16.h)
##############################################################*/

int QuickSaveSamp(path,name,samp)
char *path, *name;
struct Disk_Samp *samp;
{
int cancel;

if ((cancel=RenameTempFile(path,name,samp,TRUE))==NO_ERROR) {
   DeleteSample(samp);
	}

return(cancel);
}

/*############################################################
NAME	
	QuickLoadSamp(path,fname,sampname) -

FUNCTION
	"Loads" a sample by renaming it to the working directory
	and creating the appropriate Disk_Samp structure.  The
	path+fname combo is renamed into the working dir with the name
	"sampname.000L"   This file is then used as the working
	file for studio16 edits, etc.
	
	If path==StudioBase->defaults.temp_files_path, then the
	file will be "attached" to a disk samp struct.  In this case,
	fname should equal sampname+".???L"
INPUTS
	path	directory fname is in
	fname	file name
	sampname	name to call this sample
RESULTS
	standard error code
##############################################################*/

int QuickLoadSamp(path,fname,sampname)
char *path, *fname, *sampname;
{
struct Disk_Samp *samp;
int error;
struct SampleFilePointer *sp;
int version;

if (stricmp(path,StudioBase->defaults.temp_files_path)==0) {
	version=0;
	sscanf(fname+strlen(fname)-4,"%d",&version);
	if ((error=CreateSampleStruct(sampname,version,&samp,CSS_KEEP_WORKING_FILE))!=NO_ERROR) {
		return(error);
		}
	}
else {
	if ((error=CreateSampleStruct(sampname,0,&samp,0))!=NO_ERROR) {
		return(error);
		}
	}

if ((error=RenameTempFile(path,fname,samp,FALSE))!=NO_ERROR) {
	DeleteSample(samp); 
	return(error);
	}

if ((sp=SampleOpen(samp, SP_MODE_OLDFILE))==0) {
	error=STDERR_CANT_GET_READ_LOCK;
	}
else {	/** error==NO_ERROR from RenameTempFile **/
	if (SampleReadTag(sp)==STDERR_NO_TAG) {
		SampleSetLength(sp);			/** Assume RAW data and set length **/
		samp->parms.rate=44100; 	/* default rate */
		samp->parms.filter3db=samp->parms.rate/2;
		samp->parms.volume=ZERO_DB;
		}
	SampleClose(sp);
	}

return(error);
}

/************************************************************/
