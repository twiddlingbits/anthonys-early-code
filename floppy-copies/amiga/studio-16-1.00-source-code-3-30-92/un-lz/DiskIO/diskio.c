#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include <intuition/intuition.h>
#include "/include/psound.h"


/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_DISKIO";	/* The name of the task to create         */
long _priority = 3;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

short TEMP;

extern struct StudioBase *StudioBase;
struct Module *thismod;

short se_info_block_played;
short se_info_channel_done;
short se_cmd_kill_module;
short se_info_block_recorded;
short se_cmd_preload_sample;
short se_cmd_presave_sample;
short se_cmd_unload_sample;
short se_info_unload_done;
short se_info_playback_done;
short se_info_record_done;
short se_cmd_stop_playing;
short se_cmd_stop_recording;

/*short se_info_presave_done;*/
/*short se_info_preload_done;*/

void InitModDiskIO();
void preload_sample();
void presave_sample();
void handle_studioevents();
void telluser();

/***********************************************************/

void _main()
{
openlibraries();
InitModDiskIO();

while (TRUE) {
   Wait(1<<thismod->notifyme->mp_SigBit);
	handle_studioevents();
	}
}

/***********************************************************/

void InitModDiskIO()
{
static struct NewModule mod_data ={"DiskIO",MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct StandardModState),MOD_SINGLE_RESIDENT};

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	closelibraries();
   exit(10);
   }

/** Turn on the events we want to hear about **/

se_info_block_played			=GetEventID("SE_INFO_BLOCK_PLAYED");
se_info_block_recorded		=GetEventID("SE_INFO_BLOCK_RECORDED");
se_info_channel_done			=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_kill_module			=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_preload_sample		=GetEventID("SE_CMD_PRELOAD_SAMPLE");
se_cmd_presave_sample		=GetEventID("SE_CMD_PRESAVE_SAMPLE");
se_info_unload_done			=GetEventID("SE_INFO_UNLOAD_DONE");
se_cmd_unload_sample			=GetEventID("SE_CMD_UNLOAD_SAMPLE");
se_info_playback_done		=GetEventID("SE_INFO_PLAYBACK_DONE");
se_info_record_done			=GetEventID("SE_INFO_RECORD_DONE");
se_cmd_stop_playing			=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_recording		=GetEventID("SE_CMD_STOP_RECORDING");

/*se_info_preload_done			=GetEventID("SE_INFO_PRELOAD_DONE");*/
/*se_info_presave_done			=GetEventID("SE_INFO_PRESAVE_DONE");*/

NotifyMeOnEvent(se_info_block_played, thismod);
NotifyMeOnEvent(se_info_block_recorded, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_preload_sample, thismod);
NotifyMeOnEvent(se_cmd_presave_sample, thismod);
NotifyMeOnEvent(se_cmd_unload_sample, thismod);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct ActiveSample *as;
long flags;
struct ChanKey *key;
int errorcode;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_preload_sample) {
			preload_sample(event->arg1, event->arg2, event->arg3, event->arg4, event->arg5); 
			}

		else if (event->type==se_cmd_presave_sample) {
			presave_sample(event->arg1, event->arg2, event->arg3); 
			}


		else if (event->type==se_info_block_played) {
			as = (struct ActiveSample *)event->arg1;
			if (as->flags&AS_AUTO_DATA) {
				errorcode=ASRead(as);
				if (errorcode!=NO_ERROR && errorcode!=STDERR_DATA_EOF) {	/* ActiveSample */
					BroadcastEventParms(se_cmd_stop_playing,event->arg1,0,0,0,0,0);
					telluser("DISKIO:","Error trying to read data for playback",NOTICE_RETURN_NOW);
					}
				}
			}

		else if (event->type==se_info_block_recorded) {
			as = (struct ActiveSample *)event->arg1;
			if (as->flags&AS_AUTO_DATA) {
				if (ASWrite(as)!=NO_ERROR)	{	/* ActiveSample */
/*kprintf("DISKIO: ASWrite error!\n");*/
					if (!(as->flags&AS_ERROR_DISPLAYED)) {
						BroadcastEventParms(se_cmd_stop_recording, ((struct ActiveSample *)event->arg1)->key,0,0,0,0,0);
						telluser("DISKIO:","Error trying to write data for record.",NOTICE_RETURN_NOW);
						as->flags |= AS_ERROR_DISPLAYED;
						}
					}
				}
			}

		else if (event->type==se_info_channel_done) {
			as = (struct ActiveSample *)event->arg1;
			/*** THE FOLLOWING LINE IS REQUIRES THAT DISKIO BE PRESENT WHEN USING ASIO.c Funcs!! **/
			as->flags &= (~AS_IS_ACTIVE);	/** clear flag.  Not active anymore **/

			key=as->key;
			flags=as->flags;

			if (flags&AS_AUTO_CLOSE) {
				ASClose(as);
				}

			if (flags&AS_AUTO_FREECHAN) 
				{
				if (key) FreeChan(key);
				}
			}
		
		else if (event->type==se_cmd_unload_sample) {
			as = (struct ActiveSample *)event->arg1;
			key=as->key;
			flags=as->flags;

			if (flags&AS_AUTO_CLOSE) {
				as = (struct ActiveSample *)event->arg1;
				BroadcastEventParms(se_info_unload_done,as->region.owner,as->key,as->region.user_data,0,0);
				ASClose(as);
				}

			if (flags&AS_AUTO_FREECHAN) 
				{
				if (key) FreeChan(key);
				}
			}
		
		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			/***!!!!! WHAT IF I GET KILLED DURING PLAYBACK???? ****/
			ReplyMsg(event);			/** need because of exit **/
			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

	ReplyMsg(event);
	}
}

/*********************************************************/

void preload_sample(mod,samp,start,len,user_data)
int len,start,user_data;
struct Disk_Samp *samp;
struct Module *mod;
{
struct Region myregion;

myregion.samp=samp;
myregion.start=start;
myregion.end=start+len-1;
myregion.owner=mod;
myregion.user_data=user_data;
myregion.parms=samp->parms;
if (ASOpen(&myregion, AS_PLAY|AS_AUTO_CLOSE|AS_AUTO_DATA)==0)
	telluser("DISKIO:","ASOpen() error! (low memory or sample in use)",0);

/**BroadcastEventParms(se_info_preload_done, ps,0,0,0,user_data,0); **/
}

/*********************************************************/

void presave_sample(mod,name,rate)
struct Module *mod;
char *name;
int rate;
{
struct Region myregion;
struct Disk_Samp *samp;
int i;
struct ActiveSample *as;

i=CreateSample(name, 0, &samp);
if (i!=NO_ERROR)  {
	telluser("DISKIO:","Can't create record sample!",0);
	return;
	}

myregion.samp=samp;
myregion.start=0;
myregion.end=0;
myregion.owner=mod;
myregion.user_data=0;
myregion.parms.rate=rate;
myregion.parms.filter3db=rate/2;
myregion.parms.volume=3200;
samp->parms = myregion.parms;
as=(struct ActiveSample *)ASOpen(&myregion, AS_RECORD|AS_AUTO_CLOSE|AS_AUTO_DATA);
if (as==0)  {
	telluser("DISKIO:","Can't open record sample! (low memory?)",0);
	CloseSample(samp);	/** Delete with no msg broadcast **/
	return;
	}

/**BroadcastEventParms(se_info_presave_done, ps,0,0,0,0,0);**/

return;
}
