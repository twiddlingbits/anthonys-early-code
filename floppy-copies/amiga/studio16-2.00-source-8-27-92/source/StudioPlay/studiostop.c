#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
int we_are_initing;

if (argc!=1) {
	printf("usage: StudioStop\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

if (StudioBase->LibNode.lib_OpenCnt==1) {
	we_are_initing=TRUE; 
	InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);
	}
else {
	we_are_initing=FALSE;
	}

BroadcastEventParms(GetEventID("SE_CMD_STOP_ALL_PLAYBACK"),0,0,0,0,0,0);

if (we_are_initing)
	CloseAllModules(FALSE);

CloseLibrary(StudioBase);
}
