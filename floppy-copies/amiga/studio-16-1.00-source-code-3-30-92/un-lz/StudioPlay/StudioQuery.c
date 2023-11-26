#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

struct StudioBase *StudioBase;
short GetEventID();

void main(argc,argv)	
int argc;
char *argv[];
{
struct Module *han;
int itc,*ptc;
struct TimeCode tc;

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

InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE,0,0);

if (FindModule("AD1012Handler")==NULL) {
	han=(struct Module *)LoadModule("AD1012Handler");
	if (han==NULL) {
		printf("Error loading AD1012Handler\n");
		CloseLibrary(StudioBase);
		exit(10);
		}
	}
else {
	han=NULL;
	}

ptc=(int *)&tc;
if (han) {	/** Wait for handler to generate load and generate a tick **/
	Delay(20);
	}
itc=QuerySmpteTime();
*ptc=itc;
printf("%02d:%02d:%02d:%02d\n",tc.hours,tc.minutes,tc.seconds,tc.frames);
if (han) {
	SendMsg(han, GetEventID("SE_CMD_KILL_MODULE"),han,0,0,0,0,0);
	}
CloseLibrary(StudioBase);
}
