#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct StudioBase *StudioBase;
struct Library *GadLibBase;

void main()

{
static struct NewModule mod_data ={"Quit",0,sizeof(struct StandardModState)};
struct Module *thismod;
struct StudioEvent *event;
short se_cmd_kill_module;
short se_cmd_select_module;
struct MinNode *node;
struct Module *mod;

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	exit(10);
	}

GadLibBase=(struct StudioBase *)OpenLibrary("gadlib.library",0);
if (GadLibBase==NULL) {
	CloseLibrary(StudioBase);
	exit(10);
	}

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	CloseLibrary(StudioBase);
	CloseLibrary(GadLibBase);
   exit(10);
   }

se_cmd_kill_module	=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module	=GetEventID("SE_CMD_SELECT_MODULE");
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);

while (TRUE) {
	Wait(1<<thismod->notifyme->mp_SigBit);
	while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);
			DeleteModule(thismod);
			CloseLibrary(StudioBase);
			CloseLibrary(GadLibBase);
			exit(0);
			}
		}
	}
}
