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

extern struct StudioBase *StudioBase;
extern struct ExecBase *SysBase;

void handle_studioevents();
struct FileFilter *AddFileFilter();
void InitUtility();
void CreatePlaySampProc();
void PlaySampProc();
void StopAllPlayback();
struct ChanKey *AllocChan();

/** events I'm interested in **/

short se_cmd_kill_module;    
short se_cmd_play_sample;
short se_cmd_stop_playing;
short se_cmd_stop_all_playback;
short se_info_module_closed;
short se_info_module_opened;

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
static struct NewModule utility_data ={"Utility",MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct StandardModState),MOD_SINGLE_RESIDENT};

if ((thismod=(struct Module *)AddModule(&utility_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	closelibraries();
   exit(10);
   }

se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_playing		=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_play_sample, thismod);
NotifyMeOnEvent(se_cmd_stop_all_playback, thismod);

/** Add File filter's to system **/

InitKwik();
InitNewKwik();
InitRaw();
InitSVX();

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/

		/** Remove File Filters **/
			UnInitKwik();
			UnInitNewKwik();
			UnInitRaw();
			UnInitSVX();

			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

		else	if (event->type==se_cmd_play_sample) {
			CreatePlaySampProc(event->arg1, event->arg2, event->arg3, event->arg4, event->arg5);
			}

		else	if (event->type==se_cmd_stop_all_playback) {
			StopAllPlayback();
			}
	ReplyMsg(event); 
	}
}

/***********************************************************/

void CreatePlaySampProc(sampname,start,len,loop,callback)
char *sampname[];
int len,start;
long loop;
void (*callback)();
{
struct Region region;
struct ChanKey *key;
struct ActiveSample *as[16];
struct ActiveSample *ASOpen();
short count;

count=0;
while (sampname[count]) {
	as[count]=NULL;
	key=AllocChan(NULL,-1,CK_CHAN_PLAY);
	if (key==0) {
		while (count--)
			ASClose(as[count]);	
		telluser("Utility:","Can't Allocate a play channel!",0);
		return;
		}
	setmem(&region,sizeof(struct Region),0);
	strcpy(region.samp_name, sampname[count]);
	region.effect=callback;
	region.loop=loop;
	if (len==0) {
		region.start=0;	/** 0,0 tells ASOpen() to use defaults **/
		region.end=0;
		}
	else {
		region.start=start;
		region.end=start+len-1;
		}

	/** Since region.tag is all NULL (we never initilized it) **/
	/** ASOpen() will pull the default parameters from the sample **/

	if ((as[count]=ASOpen(&region, AS_PLAY|AS_AUTO_DATA|AS_AUTO_CLOSE|AS_AUTO_FREECHAN))==0) {
		FreeChan(key);
		while (count--)
			ASClose(as[count]);	
		telluser("Utility:","Can't Play Sample. Sample may be in use, or Amiga low on RAM.",NULL);
		return;
		}

	as[count]->key=key;
	count++;
	}
as[count]=0;	/** indicate last entry **/
ASTriggerList(as,0);
}

/***********************************************************/

void StopAllPlayback()
{
int cn,x=0;
struct Node *node;
struct AudioHandler *ah;
struct ChanKey *key;

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->handlers.lh_Head; node->ln_Succ; node = node->ln_Succ) {
	ah=(struct AudioHandler *)node;
	x=0;
	for (cn=0; cn < ah->number_of_channels; cn++) {
		if (ah->channel[cn].flags&CK_CHAN_PLAY) {
			key=AllocChan(ah, x++, CK_ID_ONLY|CK_CHAN_PLAY);
			BroadcastEventParms(se_cmd_stop_playing,key,0,0,0,0,0);
			}
		}
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
}

/***********************************************************/
