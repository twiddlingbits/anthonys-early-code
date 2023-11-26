#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "/include/studio16.h"
#include "/include/cardcmds.h"
#include "handler.data"
#include "messages.data"
#include "clib/macros.h"

/**********************************/
#define BLOCKWORDS 1024
/**********************************/
extern USHORT Pw2[];
extern USHORT NegPw2[];
extern struct Module *mod;
extern struct Window *wind;
extern void SendW();
extern USHORT GetW();
extern void DisplayWind();
extern void HideWind();
extern void prnt();
/**********************************/
/**********************************/
int OldGainVal;
/**********************************/
short SMPTERRORcnt=0,HILOcnt=0;
extern struct StudioBase *StudioBase;

extern struct AudioHandler *hand;
extern struct Handler12 *state;
/**********************************/
/**********************************/
/**********************************/
struct ActiveSample *PS;

struct ChanData CD[4];
/**********************************/
/**********************************/
/**********************************/

struct IntData
	{
	USHORT *CardAddr;
	USHORT CmdBuffPtr;
	USHORT *DspCmd;
	USHORT SoundOn;
	SHORT PlayCount[4];
	};

extern struct IntData ID;

extern USHORT *Rbuff,*RbuffEND;
/****************************************************************/
struct ChanKey *key[4],*tkey;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern USHORT *port;

extern USHORT CardRev,CardMemory,CardPal,CardFlags;
extern ULONG  CardSerialNum,CardXtra;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_trigger_record;
short se_info_channel_done;
short se_cmd_stop_playing;
short se_cmd_stop_recording;
short se_cmd_kill_module;
short se_cmd_select_module;
short se_cmd_set_channel_volume;
short se_cmd_set_cutoff_freq;
short se_cmd_set_input_gain;
short se_cmd_set_sampling_rate;
short se_info_event_count;
short se_info_hilo;
short se_cmd_pause_playback;

short se_cmd_download_code;
short se_info_block_played;
short se_cmd_trigger_playback;

short se_cmd_set_delay;
short se_cmd_set_feedback;
short se_cmd_set_timer_rate;
short se_cmd_set_delay2;
short se_cmd_set_mix;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitStudioMessages()
{
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");

se_info_hilo			= GetEventID("SE_INFO_HILO");
se_cmd_trigger_record		= GetEventID("SE_CMD_TRIGGER_RECORD");
se_info_channel_done		= GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_stop_playing		= GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_recording		= GetEventID("SE_CMD_STOP_RECORDING");
se_cmd_kill_module		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_info_event_count		= GetEventID("SE_INFO_EVENT_COUNT");
se_cmd_pause_playback		= GetEventID("SE_CMD_PAUSE_PLAYBACK");

se_cmd_trigger_playback		= GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_block_played		= GetEventID("SE_INFO_BLOCK_PLAYED");

se_cmd_set_delay		=GetEventID("SE_CMD_SET_DELAY");
se_cmd_set_feedback		=GetEventID("SE_CMD_SET_FEEDBACK");
se_cmd_set_timer_rate		= GetEventID("SE_CMD_SET_TIMER_RATE");
se_cmd_set_delay2		= GetEventID("SE_CMD_SET_DELAY2");
se_cmd_set_mix			= GetEventID("SE_CMD_SET_MIX");
se_cmd_download_code		= GetEventID("SE_CMD_DOWNLOAD_CODE");

NotifyMeOnEvent(se_cmd_download_code,mod);
NotifyMeOnEvent(se_cmd_set_delay2,mod);
NotifyMeOnEvent(se_cmd_set_mix,mod);
NotifyMeOnEvent(se_cmd_set_timer_rate,mod);
NotifyMeOnEvent(se_cmd_set_delay,mod);
NotifyMeOnEvent(se_cmd_set_feedback,mod);

NotifyMeOnEvent(se_cmd_stop_playing,mod);
NotifyMeOnEvent(se_cmd_trigger_record,mod);
NotifyMeOnEvent(se_cmd_stop_recording,mod);
NotifyMeOnEvent(se_cmd_set_channel_volume,mod);
NotifyMeOnEvent(se_cmd_set_input_gain,mod);
NotifyMeOnEvent(se_cmd_set_cutoff_freq,mod);
NotifyMeOnEvent(se_cmd_set_sampling_rate,mod);
NotifyMeOnEvent(se_cmd_select_module,mod);
NotifyMeOnEvent(se_cmd_kill_module,mod);
NotifyMeOnEvent(se_info_event_count, mod);
NotifyMeOnEvent(se_cmd_pause_playback,mod);

NotifyMeOnEvent(se_cmd_trigger_playback,mod);

NotifyMeOnCount(se_info_hilo, mod);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
ULONG TC2Frames(x)
int x;
{
ULONG y;
y=((x&0xFF000000)>>24)*60;
y=y+((x&0x00FF0000)>>16)*60;
y=y+((x&0x0000FF00)>>8)*30;/* FIX */
y=y+((x&0x000000FF)>>8);
return(y);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
ULONG RecSamps;
ULONG RecFrames;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
void SetSRate();
struct ChanKey *key;
struct StudioEvent *event;
int unit_num,rate,x,z;
ULONG last,new;
double fr,sa;
void PlayFirst();
void InitCardSware();
void StartTimer();
void StopTimer();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
/*||PLAY||*/
	if(event->type == se_cmd_trigger_playback)
	/***************************************/
		{
		PS=(struct ActiveSample *)event->arg1;
		PS->key=(struct ChanKey *)event->arg2;
		if (PS->key->handler==hand)
			{
			x=PS->key->chan_num;
			CD[x].PS=PS;

			unit_num=PS->key->chan_mask;

/* V0.52 V1.0 Compatabilty fix */ if (PS->region.parms.volume==0x8000) PS->region.parms.volume=3200;

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				state->vol_play[x]=PS->region.parms.volume;
				SendW(ChanVol+((x+1)<<8));
				SendW(DBs[MIN(PS->region.parms.volume,3393)]);
				}

			z=PS->region.parms.filter3db;
			state->filter[0]=z;
			SendW(SetFreq+FindFreq(z));

			SetSRate(PS->region.parms.rate);

			PlayFirst(x);

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,PS->key,PS->region.parms.volume,0,0,0,0);

			BroadcastEventParmsLessOne(se_cmd_set_cutoff_freq,mod,hand,PS->region.parms.filter3db,0,0,0,0);
			}
		}

	if(event->type == se_cmd_stop_playing)
	/***************************************/
		{
		tkey = ((struct ActiveSample *)event->arg1)->key; 
		unit_num=tkey->chan_mask;
		x=tkey->chan_num;

		if (ID.SoundOn & unit_num)
			{
			ID.SoundOn &= ~unit_num;ID.PlayCount[x]=0;
			BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key, CD[x].PS->region.owner,0,CD[x].PS->region.user_data,0);
			}
		}

/*||RECORD||*/

	if(event->type == se_cmd_trigger_record)
	/***************************************/
		{
		if (((struct ActiveSample *)event->arg1)->key->handler == hand)
			{
			CD[3].PS=(struct ActiveSample *)event->arg1;

			SetSRate(CD[3].PS->region.parms.rate);
			last=QuerySmpteTime();
			Forbid();
			if (StudioBase->defaults.flags&SBD_SMPTE_ON) 
				{
				x=0;
				while (last==(new=QuerySmpteTime()) && x++ < 500000) ;
				}
			else new=0;
			if(CD[3].PS->key->flags & CK_TAP_OUTPUT) SendW(RecordOutOn);
			else SendW(RecordInOn);
			CD[3].PS->region.parms.starttimecode=new;
			Permit();
			Disable();
			RecSamps=0;RecFrames=0;
			Enable();
			}
		}

	if(event->type == se_cmd_stop_recording)
	/***************************************/
		{
		if (CD[3].PS)
			{
			if (CD[3].PS->key->handler==((struct ChanKey *)event->arg1)->handler)
				{
				SendW(RecordOff);
				Disable();
				fr=RecFrames;sa=RecSamps;
				Enable();
				if (fr) CD[3].PS->region.parms.smpte_sampling_rate=(sa*((double)StudioBase->defaults.smpte_mode))/fr;
/*				if (fr) {rate=CD[3].PS->region.parms.smpte_sampling_rate;kprintf("SR:%d%d\n",rate);}*/
				BroadcastEventParms(se_info_channel_done, CD[3].PS, CD[3].PS->key, CD[3].PS->region.owner,0,0,0);
				CD[3].PS=NULL;
				}
			}
		}
/* * */
	if(event->type == se_cmd_set_channel_volume)
	/***************************************/
		{
		key=(struct ChanKey *)event->arg1;
		if (key->handler==hand)
			{
			x=event->arg2;
			if (key->flags & CK_CHAN_PLAY)
				{
				z=key->chan_num;
				state->vol_play[z]=x;
				SendW(ChanVol+((z+1)<<8));
				SendW(DBs[MIN(x,3393)]);
				}
			if (key->flags & CK_CHAN_MONITOR)
				{
				state->vol_in[0]=x;
				SendW(ChanVol);
				SendW(DBs[MIN(x,3393)]);
				}
			if (key->flags & CK_CHAN_OUTPUT)
				{
				state->vol_out[0]=x;	/* Record= Output */
				SendW((5<<8)+ChanVol);
				SendW(DBs[MIN(x,3393)]);
				}
			}
		}

	if(event->type == se_cmd_set_input_gain)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			x=event->arg2-OldGainVal;
			if(x>0)	SendW(GainUp+(x<<8));
			if(x<0)	SendW(GainDown+((-x)<<8));
			OldGainVal=event->arg2;
			state->input_gain[0]=event->arg2;
			}
		}

	if(event->type == se_cmd_set_cutoff_freq)
	/***************************************/
	/* event->arg1 = Cutoff frequency in Hz*/
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			x=event->arg2;
			state->filter[0]=event->arg2;
			SendW(SetFreq+FindFreq(x));
			}
		}

	if(event->type == se_cmd_set_sampling_rate)
	/****************************************/
	/* event->arg1 = Sampling Rate samps/sec  */
	/****************************************/
		{
		if (event->arg1==(int)hand)
			{
			rate=(HALF_CLK/event->arg2)-1;
			state->sampling_rate[0]=event->arg2;
			SendW(SetRate);
			SendW(rate);
			}
		}

	if(event->type == se_cmd_select_module)
	/***************************************/
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();
		}

	if(event->type == se_cmd_kill_module)
	/***************************************/
		{
		if ((struct Module *)event->arg1==mod)
			{
			ReplyMsg(event);
			setdown();
			DeleteAudioHandler(hand);
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}

	if(event->type == se_info_event_count)
	/***************************************/
		{
		if(event->arg1 == se_info_hilo)
			{
			HILOcnt=event->arg2;
			if(HILOcnt) StartTimer();
			else StopTimer();
			}
		}


	if(event->type == se_cmd_set_delay)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			SendW(SetDelay);
			SendW(event->arg2);
			}
		}

	if(event->type == se_cmd_set_feedback)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			SendW(SetFeedBack);
			SendW(event->arg2);
			}
		}

	if(event->type == se_cmd_set_mix)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			SendW(SetMix);
			SendW(event->arg2);
			}
		}

	if(event->type == se_cmd_set_delay2)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			SendW(SetDelay2);
			SendW(event->arg2);
			}
		}

	if(event->type == se_cmd_set_timer_rate)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			SendW(SetTimer);
			SendW(event->arg2);
			}
		}

	if(event->type == se_cmd_pause_playback)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			if (event->arg2) SendW(MODE|OFF);
			else SendW(MODE|ON);
			}
		}

	if(event->type == se_cmd_download_code)
	/***************************************/
		{
		if (event->arg2)
			{
			SendW(MODE|OFF);
			if (SendCode(event->arg2)) telluser("AD1012 Handler","Can not Download Dsp Code",NOTICE_RETURN_NOW);
			for(x=0;x<100000;x++) rate=*(port+DATA);
			SendW(MODE|ON);
			}
		else InitCardSware();
		}

	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/********************************************************************/
/********************************************************************/
/********************************************************************/
void SetSRate(x)
int x;
{
int rate;
rate=(HALF_CLK/x)-1;
state->sampling_rate[0]=x;

SendW(SetRate);
SendW(rate);
BroadcastEventParmsLessOne(se_cmd_set_sampling_rate,mod,hand,x,0,0,0,0);
}
/*****************************************************************/
/*****************************************************************/
/********************************************************************/
int FindFreq(t)
int t;
{
extern int ACutoff[28], ACval[28], BCutoff[16], BCval[16], CCutoff[16], CCval[16],Anum,Bnum,Cnum;
int x;
if ((CardFlags&0x03)==0)
	{
	for(x=0;x<Anum && t<ACutoff[x];x++) ;	/* For V4.2 BUtter */
	if (x==Anum) x=Anum-1;
	return(ACval[x]);
	}
if ((CardFlags&0x03)==1)
	{
	for(x=0;x<Bnum && t<BCutoff[x];x++) ;	/* For V5.0 Bessel */
	if (x==Bnum) x=Bnum-1;
	return(BCval[x]);
	}
if ((CardFlags&0x03)==2)
	{
	for(x=0;x<Cnum && t<CCutoff[x];x++) ;	/* For V5.0 Butter */
	if (x==Cnum) x=Cnum-1;
	return(CCval[x]);
	}
}
/*****************************************************************/
/*****************************************************************/
extern long Handler_sigbit;
extern struct Task *tc_Handler;
void PlayFirst(x)
int x;
{
short *data;
LONG ActualLen;
int ASHandlerQueryPlayData(),status;
void WaitErrorFunc();

	ID.PlayCount[x]=3;

WaitingError:	status=ASHandlerQueryPlayData(CD[x].PS,BLOCKWORDS*2,&ActualLen,&data);
		if (status==STDERR_DATA_NOT_AVAILABLE_YET) {WaitErrorFunc();goto WaitingError;}
		if (ActualLen>1) SendFirstBlock(port,data,x,ActualLen);
		if (status==STDERR_DATA_EOF)
			{
			ID.SoundOn=ID.SoundOn & NegPw2[x];
			ID.PlayCount[x]=0;
			BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key, CD[x].PS->region.owner,0,CD[x].PS->region.user_data,0);
			}
		else 
			{
/*			Signal(tc_Handler,1<<Handler_sigbit);*/		/* Fix, does not add command */
			ID.SoundOn=ID.SoundOn|(1<<x);
			}
}
