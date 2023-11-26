#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
int itc,*ptc;
struct TimeCode tc;
int we_are_initing;

if (argc!=1) {
	printf("usage: QueryTime\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

/** If this is the first time studio.library is opened, we **/
/** need to tell studio.library to go find all the working **/
/** samples in the audio: directory **/

if (StudioBase->LibNode.lib_OpenCnt==1) {
	we_are_initing=TRUE; 
	InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);
	}
else {
	we_are_initing=FALSE;
	}

ptc=(int *)&tc;
itc=QuerySmpteTime();
*ptc=itc;
printf("%02d:%02d:%02d:%02d\n",tc.hours,tc.minutes,tc.seconds,tc.frames);
if (we_are_initing)
	CloseAllModules(FALSE);
CloseLibrary(StudioBase);
}
