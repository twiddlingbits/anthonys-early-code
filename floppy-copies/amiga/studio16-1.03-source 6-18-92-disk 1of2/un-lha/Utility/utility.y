#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_UTILITY";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

#define MAX_PLAYING_SAMPLES 32

struct Module *thismod;
struct FileFilter *ff1;
struct FileFilter *ff2;
struct FileFilter *ff3;
struct FileFilter *ff4;
struct FileFilter *ff5;
struct FileFilter *ff6;
struct FileFilter *ff7;
struct FileFilter *ff8;

extern struct StudioBase *StudioBase;
extern struct ExecBase *SysBase;

void handle_studioevents();
struct FileFilter *AddFileFilter();
void InitUtility();
void CreatePlaySampProc();
void PlaySampProc();
void StopAllPlayback();
void AddAS();
void UnAddAS();

/** events I'm interested in **/

short se_cmd_kill_module;    
short se_cmd_play_sample;
short se_cmd_stop_playing;
short se_cmd_stop_all_playback;
short se_info_module_closed;
short se_info_module_opened;
short se_cmd_trigger_playback;
short se_info_channel_done;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
InitUtility();					/** initalize module **/

while (TRUE) {
	waitmask=(1<<thismod->notifyme->mp_SigBit);
   mask=Wait(waitmask);
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	}
}

/***********************************************************/

void InitUtility()
{
int Load8SVX();
int Load16BitRaw();
int Save16BitRaw();
int Save8SVX();
int SaveCDTV();
int SaveAIFF16();
int SaveAIFF8();
int LoadAIFF();
static struct NewModule utility_data ={"Utility",MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct StandardModState),MOD_SINGLE_RESIDENT};
static struct NewFileFilter olf1 = {"IFF 8SVX",FF_LOAD_FILTER,Load8SVX,8,-10};
static struct NewFileFilter olf2 = {"RAW",FF_LOAD_FILTER,Load16BitRaw,16,-100};
static struct NewFileFilter olf3 = {"RAW",FF_SAVE_FILTER,Save16BitRaw,16,-100};
static struct NewFileFilter olf4 = {"IFF 8SVX",FF_SAVE_FILTER,Save8SVX,8,-10};
static struct NewFileFilter olf5 = {"CDTV RAW",FF_SAVE_FILTER,SaveCDTV,16,-101};
static struct NewFileFilter olf6 = {"AIFF 16 bit",FF_SAVE_FILTER,SaveAIFF16,16,0};
static struct NewFileFilter olf7 = {"AIFF 8 bit",FF_SAVE_FILTER,SaveAIFF8,8,-11};
static struct NewFileFilter olf8 = {"AIFF ANY bit",FF_LOAD_FILTER,LoadAIFF,16,0};

if ((thismod=(struct Module *)AddModule(&utility_data))==0) {
	closelibraries();
   exit(10);
   }

se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_playing		=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");
se_cmd_trigger_playback	=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_channel_done		=GetEventID("SE_INFO_CHANNEL_DONE");

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_play_sample, thismod);
NotifyMeOnEvent(se_cmd_stop_all_playback, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);

/** Add File filter's to system **/

ff1 = AddFileFilter(&olf1);
ff2 = AddFileFilter(&olf2);
ff3 = AddFileFilter(&olf3);
ff4 = AddFileFilter(&olf4);
ff5 = AddFileFilter(&olf5);
ff6 = AddFileFilter(&olf6);
ff7 = AddFileFilter(&olf7);
ff8 = AddFileFilter(&olf8);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/

			DeleteFileFilter(ff1);
			DeleteFileFilter(ff2);
			DeleteFileFilter(ff3);
			DeleteFileFilter(ff4);
			DeleteFileFilter(ff5);
			DeleteFileFilter(ff6);
			DeleteFileFilter(ff7);
			DeleteFileFilter(ff8);

			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

		else	if (event->type==se_info_channel_done) {
			UnAddAS(event->arg1);
			}

		else if (event->type==se_cmd_trigger_playback) {
			AddAS(event->arg1);
			}

		else	if (event->type==se_cmd_play_sample) {
			CreatePlaySampProc(event->arg1, event->arg2, event->arg3, event->arg4);
			}

		else	if (event->type==se_cmd_stop_all_playback) {
			StopAllPlayback();
			}
	ReplyMsg(event); 
	}
}

/***********************************************************/

ULONG __aligned my_seg[4];

void CreatePlaySampProc(samp,start,len,key)
int len,start;
struct Disk_Samp *samp;
struct ChanKey *key;
{
struct Region region;
struct MsgPort *proc;
struct MsgPort *CreatePort(),*CreateProc();
struct Task *pptask;
char procname[100];
long code=(ULONG)PlaySampProc;

my_seg[0]=16;
my_seg[1]=0;
my_seg[2]=0x4EF90000|((code>>16)&0xFFFF);
my_seg[3]=0x00000000|((code&0xFFFF)<<16);

sprintf(procname,"UTILPLAY:%s",samp->name);
if (SysBase->LibNode.lib_Version>=37) CacheClearU();

proc=CreateProc(procname,4,((ULONG)(&my_seg[1]))>>2,6000);
if (proc==NULL) {
	return;
	}

pptask=(struct Task *)((int)proc - sizeof(struct Task));
if (pptask->tc_UserData!=0)
	return;

region.samp=samp;
region.start=start;
region.end=len;
region.owner=NULL;
region.user_data=NULL;
region.parms=samp->parms;

pptask->tc_UserData=(APTR)&region;
while((ULONG)pptask->tc_UserData) WaitTOF();
return;
}

/***********************************************************/

void PlaySampProc()
{
struct Region region;
struct Task *pptask;
struct Task *FindTask();
struct ActiveSample *as;
struct ChanKey *key;

pptask=FindTask(0);

while(pptask->tc_UserData==NULL)
	WaitTOF();

region=*((struct Region *)pptask->tc_UserData);
pptask->tc_UserData=NULL;

if ((as=(struct ActiveSample *)ASOpen(&region, AS_PLAY|AS_AUTO_DATA))==0) {
	telluser("Utility:","Sample in use or low memory.",NULL);
	return;
	}

as->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
if (as->key==0)
	{
	ASClose(as);
	telluser("Utility:","Can't Allocate a play channel!",0);
	return;
	}

ASTrigger(as);
ASWaitStop(as);
UnAddAS(as);
key=as->key;
ASClose(as);
FreeChan(key);
}

/***********************************************************/

#define MAX_PLAYING_SAMPLES 32

struct ActiveSample *all_as[MAX_PLAYING_SAMPLES];

void AddAS(as)
struct ActiveSample *as;
{
short i;


Forbid();
for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_as[i]==NULL) {
kprintf("add\n");
		all_as[i]=as;
		break;
		}
	}
Permit();
}

/***********************************************************/

void UnAddAS(as)
struct ActiveSample *as;
{
short i;

Forbid();
for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_as[i]==as) {
		all_as[i]=NULL;
kprintf("unadd\n");
		break;
		}
	}
Permit();
}

/***********************************************************/

void StopAllPlayback()
{
short i;

for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_as[i]) {
		BroadcastEventParms(se_cmd_stop_playing,all_as[i],0,0,0,0,0);
		}
	}
}

/***********************************************************/
