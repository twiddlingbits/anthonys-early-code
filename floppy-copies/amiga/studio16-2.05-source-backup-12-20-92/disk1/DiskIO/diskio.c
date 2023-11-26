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
short se_info_unload_done;
short se_info_record_done;
short se_cmd_stop_playing;
short se_cmd_stop_recording;

void InitModDiskIO();
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
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	closelibraries();
   exit(10);
   }

/** Turn on the events we want to hear about **/

se_info_block_played			=GetEventID("SE_INFO_BLOCK_PLAYED");
se_info_block_recorded		=GetEventID("SE_INFO_BLOCK_RECORDED");
se_info_channel_done			=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_kill_module			=GetEventID("SE_CMD_KILL_MODULE");
se_info_unload_done			=GetEventID("SE_INFO_UNLOAD_DONE");
se_info_record_done			=GetEventID("SE_INFO_RECORD_DONE");
se_cmd_stop_playing			=GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_recording		=GetEventID("SE_CMD_STOP_RECORDING");

NotifyMeOnEvent(se_info_block_played, thismod);
NotifyMeOnEvent(se_info_block_recorded, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct ActiveSample *as;
int errorcode;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_info_block_played) {
			as = (struct ActiveSample *)event->arg1;
			if (as->flags&AS_AUTO_DATA) {
				errorcode=ASRead(as);
				if (errorcode!=NO_ERROR && errorcode!=STDERR_DATA_EOF) {	/* ActiveSample */
					BroadcastEventParms(se_cmd_stop_playing,as->key,0,0,0,0,0);
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
			if (as->flags&AS_AUTO_CLOSE) {
				ASClose(as);		/** Also handles AS_AUTO_FREECHAN **/
				}
			else if (as->flags&AS_AUTO_FREECHAN) {
				if (as->key) {
					FreeChan(as->key);
					as->key=NULL;
					}
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
