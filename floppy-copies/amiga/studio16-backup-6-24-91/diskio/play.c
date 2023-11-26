#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include <intuition/intuition.h>
#include "/include/psound.h"

extern struct studioBase *studioBase;
struct Module *thismod;

short se_info_block_played;
short se_info_channel_done;
short se_cmd_kill_module;
short se_cmd_record_sample;
short se_cmd_stop_recording;
short se_cmd_begin_recording;
short se_cmd_stop_recording;
short se_info_record_data;
short se_cmd_preload_sample;
short se_info_preload_done;

void preload_sample();
void FreeChan();
void InitModDiskIO();
void handle_studioevents();
void telluser();
void msg_chan_done();
void stop_record_sound();
struct ChanKey *AllocChan();
void read_data();

/***********************************************************/

void main()
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
static struct NewModule mod_data ={"DiskIO",0};

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
   telluser("Can't add mod_diskio","");
	closelibraries();
   exit(10);
   }

/** Turn on the events we want to hear about **/

se_info_block_played			=GetEventID("SE_INFO_BLOCK_PLAYED");
se_info_channel_done			=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_kill_module			=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_record_sample			=GetEventID("SE_CMD_RECORD_SAMPLE");
se_cmd_stop_recording		=GetEventID("SE_CMD_STOP_RECORDING");
se_cmd_begin_recording		=GetEventID("SE_CMD_BEGIN_RECORDING");
se_info_record_data			=GetEventID("SE_INFO_RECORD_DATA");		
se_cmd_preload_sample		=GetEventID("SE_CMD_PRELOAD_SAMPLE");
se_info_preload_done			=GetEventID("SE_INFO_PRELOAD_DONE");

NotifyMeOnEvent(se_info_block_played, thismod);
NotifyMeOnEvent(se_info_channel_done, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_info_record_data, thismod);
NotifyMeOnEvent(se_cmd_record_sample, thismod);
NotifyMeOnEvent(se_cmd_stop_recording, thismod);
NotifyMeOnEvent(se_info_preload_done, thismod);
NotifyMeOnEvent(se_cmd_preload_sample, thismod);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

/*printf("DISKIO event %d\n",event->type);*/

		if (event->type==se_cmd_preload_sample) {
			preload_sample(event->arg1, event->arg2, event->arg3, event->arg4, event->arg5);
			}

/***!!!!! WHAT IF I GET KILLED DURING PLAYBACK???? ****/

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);			/** need because of exit **/
			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

		else if (event->type==se_info_block_played) {
			read_data(event->arg1);	/* ps */
			}

		else if (event->type==se_info_channel_done) {
			msg_chan_done(event);
			}
		
		else if (event->type==se_info_record_data) {
			msg_record_data(event);
			}

		else if (event->type==se_cmd_record_sample) {
			record_sound(event->arg1, event->arg2);	/* name, rate */
			}

		else if (event->type==se_cmd_stop_recording) {
			stop_record_sound();
			}

	ReplyMsg(event);
	}
}

/*********************************************************/

void preload_sample(mod,samp,start,len,loop)
int len,start;
struct Disk_Samp *samp;
struct Module *mod;
{
struct MsgPort *CreatePort();
struct PlayingSample *ps;

ps = (struct PlayingSample *)AllocMem(sizeof(struct PlayingSample),MEMF_CLEAR);
if (ps==0)
	return;

ps->big_block[0] = (short *)AllocMem(BIG_BLOCK_SIZE*2, 0);
if (ps->big_block[0]==0) {
	FreeMem(ps, sizeof(struct PlayingSample));
	return;
	}

ps->big_block[1] = (short *)AllocMem(BIG_BLOCK_SIZE*2, 0);
if (ps->big_block[1]==0) {
	FreeMem(ps->big_block[0], BIG_BLOCK_SIZE*2);
	FreeMem(ps, sizeof(struct PlayingSample));
	return;
	}

ps->bb_len=BIG_BLOCK_SIZE;			/** in samples (words) ! **/
ps->load_count = 0;
ps->load_start = start;
ps->load_len = len;
ps->playing_samp = samp;
ps->owner=mod;
ps->loop_count = loop;
ps->fp = ReadLockSamp(samp);

if (ps->fp==-1) {
	FreeMem(ps->big_block[0], BIG_BLOCK_SIZE*2);
	FreeMem(ps->big_block[1], BIG_BLOCK_SIZE*2);
	FreeMem(ps, sizeof(struct PlayingSample));
	return;
	}

Seek(ps->fp,start,OFFSET_BEGINNING);
ps->bb_diskio_index=1;			/** So we start at zero! */
ps->bb_handler_index=0;
read_data(ps);
read_data(ps);
BroadcastEventParms(se_info_preload_done, ps,0,0,0,0,0);
}


/*********************************************************/

void read_data(ps)
struct PlayingSample *ps;
{
int blklen;
short flags;

/*printf("read_data: ps %x flags %x load_count %d\n",
	ps->playing_samp,ps->flags,ps->load_count);*/

if (ps->playing_samp==0 || ps->flags&LAST_BLOCK)
	return;

ps->bb_diskio_index=(ps->bb_diskio_index+1)&1;

if (ps->load_count+ps->bb_len >= ps->load_len) {
	flags = LAST_BLOCK;
	blklen = ps->load_len - ps->load_count;
   }
else {
   flags = 0;
   blklen = ps->bb_len;
   }

/*printf("blklen=%d\n",blklen);*/
QuickRead(ps->fp,ps->big_block[ps->bb_diskio_index],blklen);
ps->load_count=ps->load_count+blklen;
ps->flags = flags;
}

/**************************************************************/

void msg_chan_done(msg)
struct StudioEvent *msg;
{
struct PlayingSample *ps;

/*printf("play: msg chan done!\n");*/

ps = (struct PlayingSample *)msg->arg1;
ReadUnLockSamp(ps->playing_samp,ps->fp);
FreeMem(ps->big_block[0], BIG_BLOCK_SIZE*2);
FreeMem(ps->big_block[1], BIG_BLOCK_SIZE*2);
FreeMem(ps, sizeof(struct PlayingSample));
}

/**************************************************************/

void debug_samp_print(s)
struct Disk_Samp *s;
{
printf("Debug Print: samp %x '%s'\n",s,s->name);
printf("wfp %x rfp0 %x\n",s->wfp,s->rfp[0]);
printf("len %d rate %d type %d flags %x\n",s->length,s->rate,s->type,s->flags);
printf("version %d\n",s->version);
}
/**************************************************************/

/***!!!!!! CHANGE THIS!!! **/

void telluser(l1,l2)
char *l1,*l2;
{
if (l1)
	printf(l1);
if (l2)
	printf(l2);
}

