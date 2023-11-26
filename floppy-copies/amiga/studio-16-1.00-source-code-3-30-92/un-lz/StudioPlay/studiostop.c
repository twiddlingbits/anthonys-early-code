#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
if (argc!=1) {
	printf("usage: StudioStop\n");
	printf("       For StudioStop to work, 'Utility' must be resident\n");
	printf("       before you use StudioPlay. I.E. use 'StudioOpen Utility'\n");
	printf("       before StudioPlay.\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

BroadcastEventParms(GetEventID("SE_CMD_STOP_ALL_PLAYBACK"),0,0,0,0,0,0);

CloseLibrary(StudioBase);
}
