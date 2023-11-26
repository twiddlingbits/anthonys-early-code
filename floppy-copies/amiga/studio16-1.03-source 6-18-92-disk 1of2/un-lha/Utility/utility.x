#include "exec/types.h"
#include "exec/exec.h"
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
struct ActiveSample *all_as[MAX_PLAYING_SAMPLES];
struct ChanKey *my_keys[32];
struct FileFilter *ff1;
struct FileFilter *ff2;
struct FileFilter *ff3;
struct FileFilter *ff4;
struct FileFilter *ff5;
struct FileFilter *ff6;
struct FileFilter *ff7;
struct FileFilter *ff8;

extern struct StudioBase *StudioBase;

void handle_studioevents();
void playsamp();
void FreeMyChans();
void stop_all_playback();
struct FileFilter *AddFileFilter();
void InitUtility();

/** events I'm interested in **/

short se_cmd_kill_module;    
short se_cmd_edit_sample;
short se_cmd_play_sample;
short se_cmd_stop_playing;
short se_cmd_stop_all_playback;
short se_cmd_trigger_playback;
short se_info_channel_done;
short se_info_preload_done;
short se_cmd_preload_sample;
short se_info_module_closed;
short se_info_module_opened;
short se_cmd_unload_sample;

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
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_playing		=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_cmd_trigger_playback	=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_channel_done		=GetEventID("SE_INFO_CHANNEL_DONE");
se_info_preload_done		=GetEventID("SE_INFO_PRELOAD_DONE");
se_cmd_preload_sample	=GetEventID("SE_CMD_PRELOAD_SAMPLE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");
se_cmd_unload_sample		=GetEventID("SE_CMD_UNLOAD_SAMPLE");

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_play_sample, thismod);
NotifyMeOnEvent(se_cmd_stop_all_playback, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_info_preload_done, thismod);
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
struct ActiveSample *as;
short i;

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

		else if (event->type==se_info_preload_done) {
			as = (struct ActiveSample *)event->arg1;
			if (as->region.owner==thismod) {
				as->key = (struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
				for (i=0; i < 32; i++) {
					if (my_keys[i]==NULL) {
						my_keys[i]=as->key;
						break;
						}
					}

				if (as->key) {
					BroadcastEventParms(se_cmd_trigger_playback,as,as->key,0,0,0,0);
					}
				else {
					telluser("Utility:","Can't Alloc Channel",0);
					BroadcastEventParms(se_cmd_unload_sample,as,0,0,0,0,0);
					/*	kprintf("Utility: AllocChan: FAIL\n"); */
					}
				}
			}

		else	if (event->type==se_cmd_play_sample) {
			playsamp(event->arg1, event->arg2, event->arg3, event->arg4);
			}

		else	if (event->type==se_info_channel_done) {
			FreeMyChans(event->arg1,event->arg2);
			}

		else	if (event->type==se_cmd_stop_all_playback) {
			stop_all_playback();
			}

		else if (event->type==se_cmd_trigger_playback) {
			for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
				if (all_as[i]==NULL) {
					all_as[i]=(struct ActiveSample *)event->arg1;
					break;
					}
				}
			}

	ReplyMsg(event); 
	}
}

/***********************************/
/* Play a digitized sample         */
/***********************************/

void playsamp(samp,start,len,key)
int len,start;
struct Disk_Samp *samp;
struct ChanKey *key;
{
BroadcastEventParms(se_cmd_preload_sample,thismod, samp, start, len, 1,0);
}

/*******************************************************/
/*** cancel a playing channel 								**/
/*******************************************************/

void stop_all_playback()
{
short i;


for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_as[i]) {
		BroadcastEventParms(se_cmd_stop_playing,all_as[i],0,0,0,0,0);
		}
	}
}

/***********************************************************/

void FreeMyChans(as,key)
struct ActiveSample *as;
struct ChanKey *key;
{
short i;

for (i=0; i < MAX_PLAYING_SAMPLES; i++) {
	if (all_as[i]==as) {
		all_as[i]=NULL;
		break;
		}
	}

for (i=0; i < 32; i++) {
	if (my_keys[i]==key) {
		FreeChan(key);
		my_keys[i]=NULL;
		break;
		}
	}
}
