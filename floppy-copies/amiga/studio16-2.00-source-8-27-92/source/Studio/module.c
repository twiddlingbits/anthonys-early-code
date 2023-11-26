#include <exec/types.h>
#include <exec/memory.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include "/include/studio16.h"

extern short se_info_state_added;
extern short se_info_state_deleted;
extern short se_info_module_added;
extern short se_info_module_deleted;

struct StandardModState *AddModState();
struct StandardModState *FindModState();
struct Module *FindModule();
void DeleteModState();

struct SampleFilePointer *SampleOpen();


/*############################################################
NAME	
	AddModule -
FUNCTION
	Allocate memory for a new module struct and links it into the
	StudioBase->modules chain.	 All data from NewModule is
	copied, including the name, so that NewModule may be
	discarded. A port is also allocated to recieve broadcasted
	messsages (module->notifyme).	 The Module's ModState struct
	is also allocated and zeroed based on the passed size.
INPUTS
	pointer to module struct
RESULTS
	NONE
###############################################################*/

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
MoveRequester(FindTask(NULL));
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
/** required it.	It's set here based on the assumption that **/
/** You are trying to find a state that is already initilized. **/
/** This is fairly safe, since recently created states (thus **/
/** un-initilized) generally have the MS_NEED_MODULE flags set **/
			s->flags |= MS_STATE_INITILIZED; /* set flag */
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
	Allocates a new Mod State structure & links it into StudioBase
	
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
	Unlinks and deallocates a ModState structure.  Will also
	broadcast the se_info_state_deleted message.
INPUTS
	s	Pointer to the ModState to delete
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
	SlowFreeMem(s->module_name,strlen(s->module_name)+1);
if (s->instance_name)
	SlowFreeMem(s->instance_name,strlen(s->instance_name)+1);
SlowFreeMem(s,s->state_size);
BroadcastEventParms(se_info_state_deleted,s,0,0,0,0,0);
}

/*############################################################
NAME	
	DeleteModule -
FUNCTION
	Deallocates a module's memory and removes it from the module
	list.	 The passed module must be linked to StudioBase->Modules.
	The module's notifyme port is also closed and all references
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

Remove(m);		/** this is the same as Remove(&m->node) **/
m->state->attached_module=NULL;
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);

if (m->name)
	SlowFreeMem(m->name, strlen(m->name)+1);

while ((e=(struct StudioEvent *)GetMsg(m->notifyme)))
	ReplyMsg(e);

DeletePort(m->notifyme);
SlowFreeMem(m,sizeof(struct Module));
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
char *name;		/* module name to search for **/
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
char *name;		/* module name to search for **/
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

/************************************************************/
