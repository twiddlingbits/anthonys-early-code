#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "/include/studio16.h"
#include "/include/cardcmds.h"
#include "handler.data"
#include "messages.data"
#include "clib/macros.h"

int SmpteStartCount=3;
/**********************************/
#define BLOCKWORDS 0x00002000
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
extern USHORT DCmd[0x1001];
extern long Handler_sigbit;
extern struct Task *tc_Handler;
/**********************************/
/**********************************/
short SMPTERRORcnt=0,HILOcnt=0;
extern struct StudioBase *StudioBase;

extern struct AudioHandler *hand;
extern struct HandlerModState *state;
/**********************************/
/**********************************/
/**********************************/
struct ActiveSample *PS;

struct ChanData CD[4];
struct ChanKey CK[4];
/**********************************/
/**********************************/
/**********************************/

extern struct IntData ID;

/****************************************************************/
struct ChanKey *key[4];
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
short se_info_hilo_tick;
short se_cmd_pause_playback;

/*short se_cmd_download_code;*/
short se_info_block_played;
short se_cmd_trigger_playback;
short se_cmd_trigger_timecode;

short se_info_sampling_rate_error;

short se_info_smpte_timeout;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitStudioMessages()
{
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");

se_info_hilo_tick			= GetEventID("SE_INFO_HILO_TICK");
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
se_cmd_trigger_timecode		= GetEventID("SE_CMD_TRIGGER_TIMECODE");
se_info_block_played		= GetEventID("SE_INFO_BLOCK_PLAYED");

/*se_cmd_download_code		= GetEventID("SE_CMD_DOWNLOAD_CODE");*/
se_info_sampling_rate_error	= GetEventID("SE_INFO_SAMPLING_RATE_ERROR");
se_info_smpte_timeout		= GetEventID("SE_INFO_SMPTE_TIMEOUT");

/*NotifyMeOnEvent(se_cmd_download_code,mod);*/

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
NotifyMeOnEvent(se_cmd_trigger_timecode,mod);

NotifyMeOnCount(se_info_hilo_tick, mod);
NotifyMeOnEvent(se_info_hilo_tick, mod);
NotifyMeOnEvent(se_info_smpte_timeout, mod);
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
double fr,sa,smpte_mode;
void PlayFirst();
void InitCardSware();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
/*||PLAY||*/
	if(event->type == se_cmd_trigger_playback || event->type == se_cmd_trigger_timecode)
	/***************************************/
		{
		PS=(struct ActiveSample *)event->arg1;

		key=PS->key;
		if(NULL==key) telluser("1012Toast Handler:","KEY IS NULL!!!",NULL);

		if (key->handler==hand)
			{
			x=key->chan_num;

			CD[x].PS=PS;

			z=PS->region.parms.filter3db;
			SendW(SetFreq+FindFreq(z));

			if (ID.SoundOn & PS->region.parms.rate!=hand->current_sampling_rate) BroadcastEventParms(se_info_sampling_rate_error,1,key,0,0,0,0);
			SetSRate(PS->region.parms.rate);
			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				SendW(ChanVol+((x+1)<<8));
				SendW(DBs[MIN(PS->region.parms.volume,3393)]);
				}

			key->as=PS;

			PlayFirst(x);

			z=key->parms.volume;
			key->parms=PS->region.parms;
			*(key->state_parms)=PS->region.parms;

			state->filter=key->parms.filter3db;
			hand->current_filter=key->parms.filter3db;

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,key,key->parms.volume,0,0,0,0);
				}
			else 
				{
				key->parms.volume=z;
				key->state_parms->volume=z;
				}
			BroadcastEventParmsLessOne(se_cmd_set_cutoff_freq,mod,hand,key->parms.filter3db,0,0,0,0);
			}
		}

	if(event->type == se_cmd_stop_playing)
	/***************************************/
		{
		if (key=(struct ChanKey *)event->arg1)
		if (key->handler==hand)
			{
			unit_num=key->chan_mask;
			x=key->chan_num;

			if (ID.SoundOn & unit_num)
				{
				ID.SoundOn &= ~unit_num;ID.PlayCount[x]=0;
				BroadcastEventParms(se_info_channel_done, key->as, key, 0,0,key->as->region.user_data,0);
				key->as=NULL;
				}
			}
		}
	

/*||RECORD||*/

	if(event->type == se_cmd_trigger_record)
	/***************************************/
		{
		key=((struct ActiveSample *)event->arg1)->key;
		if (key->handler == hand)
			{
			CD[3].PS=(struct ActiveSample *)event->arg1;
			key->as=(struct ActiveSample *)event->arg1;

			SetSRate(CD[3].PS->region.parms.rate);
			last=QuerySmpteTime();

			Forbid();
			if (StudioBase->flags&SB_SMPTE_ON) 
				{
				x=0;
				while (last==(new=QuerySmpteTime()) && x++ < 500000) ;
				}
			else new=0;
			SendW(RecordInOn);
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
		key=(struct ChanKey *)event->arg1;
		if (CD[3].PS)
			{
			if (CD[3].PS->key->handler==key->handler)
				{
				SendW(RecordOff);
				Disable();
				fr=RecFrames;sa=RecSamps;
				Enable();
				if (StudioBase->defaults.smpte_mode) smpte_mode=StudioBase->defaults.smpte_mode;
				else smpte_mode=29.97;
				if (fr) CD[3].PS->region.parms.smpte_sampling_rate=(sa*smpte_mode)/fr;
				BroadcastEventParms(se_info_channel_done, key->as,key, 0,0,0,0);
				CD[3].PS=NULL;
				key->as=NULL;
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
			key->parms.volume=x;
			key->state_parms->volume=x;
			if (key->flags & CK_CHAN_PLAY)
				{
				z=key->chan_num;
				SendW(ChanVol+((z+1)<<8));
				SendW(DBs[MIN(x,3393)]);
				}
			if (key->flags & CK_CHAN_MONITOR)
				{
				SendW(ChanVol);
				SendW(DBs[MIN(x,3393)]);
				}
			if (key->flags & CK_CHAN_OUTPUT)
				{
				SendW((3<<8)+ChanVol);
				SendW(DBs[MIN(x,3393)]);
				}
			}
		}

	if(event->type == se_cmd_set_input_gain)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			x=(event->arg2>>5)-(state->input_gain>>5);
			if(x>0)	SendW(GainUp+(x<<8));
			if(x<0)	SendW(GainDown+((-x)<<8));
			state->input_gain=event->arg2;
			hand->current_input_gain=event->arg2;
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
			state->filter=event->arg2;
			hand->current_filter=event->arg2;
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
			state->sampling_rate=event->arg2;
			hand->current_sampling_rate=event->arg2;
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
		if(event->arg1 == se_info_hilo_tick)
			{
			HILOcnt=event->arg2;
			}
		}

	if(event->type == se_info_hilo_tick)
	/***************************************/
		{
		if(HILOcnt>1)
			{
			Disable();
			ID.CmdBuffPtr=(ID.CmdBuffPtr+1) & 0x0FFF;
			DCmd[ID.CmdBuffPtr]=1;
			Enable();
			Signal(tc_Handler,1<<Handler_sigbit);
			}
		}


	if(event->type == se_cmd_pause_playback)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			if (event->arg2) SendW(CONVERSION|OFF);
			else SendW(CONVERSION|ON);
			}
		}

	if(event->type == se_info_smpte_timeout)
	/***************************************/
		{
		SmpteStartCount=3;
		}

/*	if(event->type == se_cmd_download_code)*/
	/***************************************/
/*		{
		if (event->arg2)
			{
			SendW(MODE|OFF);
			if (SendCode(event->arg2)) telluser("1012Toast Handler","Can not Download Dsp Code",NOTICE_RETURN_NOW);
			for(x=0;x<100000;x++) rate=*(port+DATA);
			SendW(MODE|ON);
			}
		else InitCardSware();
		}*/

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

state->sampling_rate=x;
hand->current_sampling_rate=x;

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
void PlayFirst(x)
int x;
{
short *data;
LONG ActualLen;
int ASHandlerQueryPlayData(),status;
void WaitErrorFunc();

	ID.PlayCount[x]=7;

WaitingError:	status=ASHandlerQueryPlayData(CD[x].PS,BLOCKWORDS,&ActualLen,&data);
		if (status==STDERR_DATA_NOT_AVAILABLE_YET) {WaitErrorFunc();goto WaitingError;}
		if (ActualLen>1) SendFirstBlock(port,data,x,ActualLen);
		if (status==STDERR_DATA_EOF)
			{
			ID.SoundOn=ID.SoundOn & NegPw2[x];
			ID.PlayCount[x]=0;
			BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key, 0,0,CD[x].PS->region.user_data,0);
			}
		else 
			{
			Disable();
			ID.CmdBuffPtr=(ID.CmdBuffPtr+1) & 0x0FFF;
			DCmd[ID.CmdBuffPtr]=15;
			Enable();
			Signal(tc_Handler,1<<Handler_sigbit);

			ID.SoundOn=ID.SoundOn|(1<<x);
			}
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
void InitCard()
{
extern USHORT DBs[];
int x,c,rate;
extern USHORT *port;

for(x=0;x<2050;x++) {*(port+DATA)=0x003B;c=*(port+DATA);} /* Flush out any half sent buffer */

for(x=0;x<7;x++)
	{
	hand->channel[x].parms=state->ChanParms[x];
	hand->channel[x].state_parms=&state->ChanParms[x];
	}

for(x=0;x<4;x++)
	{
	SendW(ChanVol+((x)<<8));
	SendW(DBs[MIN(hand->channel[x].parms.volume,3393)]);
	}

SendW(SetFreq+FindFreq(state->filter));
BroadcastEventParmsLessOne(se_cmd_set_cutoff_freq,mod,hand,state->filter, 0, 0, 0,0);

rate=(HALF_CLK/state->sampling_rate)-1;
SendW(SetRate);
SendW(rate);
BroadcastEventParmsLessOne(se_cmd_set_sampling_rate,mod,hand,state->sampling_rate,0, 0, 0,0);

SendW(GainDown+(101<<8));
if(state->input_gain) SendW(GainUp+((((int)state->input_gain)>>5)<<8));
BroadcastEventParmsLessOne(se_cmd_set_input_gain,mod,hand,state->input_gain, 0, 0, 0, 0);

hand->current_filter=state->filter;
hand->current_sampling_rate=state->sampling_rate;
hand->current_input_gain=state->input_gain;
}
/********************************************************************/
/********************************************************************/
