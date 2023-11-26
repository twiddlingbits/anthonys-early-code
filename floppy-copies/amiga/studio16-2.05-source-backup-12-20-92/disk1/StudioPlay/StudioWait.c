#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/** use -b0 when compiling this file **/

struct StudioBase *StudioBase;
short GetEventID();

struct TimeCode HitTime;
struct Task *task;
long sink_sigmask;
long sink_sigbit;

void SinkFunc();		/** in Signal.asm **/
struct NewSmpteSink NewSink = {0,NULL,SinkFunc,NULL};
struct SmpteSink *sink;


void main(argc,argv)	
int argc;
char *argv[];
{
int a,b,c,d;
int we_are_initing;

if (argc!=2) {
	printf("usage: WaitTime <HH:MM:SS:FF>\n");
	exit(10);
	}

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	printf("Can't open studio.library\n");
	exit(10);
	}

/** If this is the first time studio.library is opened, we **/
/** need to tell studio.library assign Studio16: etc.**/

if (StudioBase->LibNode.lib_OpenCnt==1) {
	we_are_initing=TRUE; 
	InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);
	}
else {
	we_are_initing=FALSE;
	}

task=(struct Task *)FindTask(0);
sink_sigbit=AllocSignal(-1);
sink_sigmask=1<<sink_sigbit;
sscanf(argv[1],"%d:%d:%d:%d",&a,&b,&c,&d);
HitTime.hours=a;
HitTime.minutes=b;
HitTime.seconds=c;
HitTime.frames=d;

sink=(struct SmpteSink *)AddSmpteSink(&NewSink);

Wait(sink_sigmask|SIGBREAKF_CTRL_C);
FreeSignal(sink_sigbit);

DeleteSmpteSink(sink);
if (we_are_initing)
	CloseAllModules(FALSE);
CloseLibrary(StudioBase);
}
