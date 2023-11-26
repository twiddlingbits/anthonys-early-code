#include <exec/types.h>
#include <exec/memory.h>
#include "/include/psound.h"

extern short se_info_module_added;
extern short se_info_module_deleted;

/*############################################################
NAME	
	AddModule -
FUNCTION
	Allocate memory for a new module struct and links it into the
	studioBase->modules chain.  All data from NewModule is
	copied, including the name, so that NewModule may be
	discarded. A port is also allocated to recieve broadcasted
	messsages (module->notifyme)
INPUTS
	pointer to module struct
RESULTS
	NONE
##############################################################*/

struct Module *AddModule(nm)
struct NewModule *nm;
{
extern struct studioBase *studioBase;
char *name;
struct Module *create_module();
struct Module *new;
struct MsgPort *port;

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
   FreeMem(name, strlen(name)+1);
	return(0);
	}

new = (struct Module *)AllocMem(sizeof(struct Module), MEMF_CLEAR);
if (new) {
   new->flags = nm->flags;
   new->name = name;
	new->notifyme = port;
	ObtainSemaphore(&studioBase->lock);
   AddTail(&studioBase->modules, new);
	ReleaseSemaphore(&studioBase->lock);
	BroadcastEventParms(se_info_module_added,new,0,0,0,0,0);
   }
return(new);
}


/*############################################################
NAME	
	DeleteModule -
FUNCTION
	Deallocates a module's memory and removes it from the module
	list.  The passed module must be linked to studioBase->Modules.
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

ObtainSemaphore(&studioBase->lock);
for (i=0; i < studioBase->max_registered_ids; i++) {
	IgnoreEvent(i, m);
	IgnoreCount(i, m);
	}

Remove(m);     /** this is the same as Remove(&m->node) **/
ReleaseSemaphore(&studioBase->lock);

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
extern struct studioBase *studioBase;
struct Module *m;
struct MinNode *node;

if (name==0)
   return(0);

ObtainSemaphore(&studioBase->lock);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   m=(struct Module *)node;
   if (m->name)
      if (stricmp(m->name,name)==0) {
			ReleaseSemaphore(&studioBase->lock);
         return(m);
			}
   }
ReleaseSemaphore(&studioBase->lock);
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
ObtainSemaphore(&studioBase->lock);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (m==(struct Module *)node) {
		ReleaseSemaphore(&studioBase->lock);
      return(i);
		}
   i++;
   }
ReleaseSemaphore(&studioBase->lock);
return(-1);
}


