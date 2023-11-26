#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct studioBase *studioBase;

void main()

{
static struct NewModule mod_data ={"SimpleModule",0};
struct Module *thismod;
struct StudioEvent *event;
short se_kill_module;

studioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (studioBase==NULL) {
	exit(10);
	}

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	CloseLibrary(studioBase);
   exit(10);
   }

se_kill_module=GetEventID("SE_KILL_MODULE");
NotifyMeOnEvent(se_kill_module, thismod);

Wait(1<<thismod->notifyme->mp_SigBit);
while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
	if (event->type==se_kill_module && (struct Module *)event->arg1==thismod) {
		ReplyMsg(event);
		DeleteModule(thismod);
		CloseLibrary(studioBase);
		exit(0);
		}
	}
}
