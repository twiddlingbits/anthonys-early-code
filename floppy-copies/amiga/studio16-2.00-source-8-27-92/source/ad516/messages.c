#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "/include/studio16.h"
#include "/include/cardcmds.h"
#include "handler.data"
#include "messages.data"
#include "clib/macros.h"
/**********************************/
/**********************************/
void SetSRateNM();
void ModeOn();
void ModeOff();
/**********************************/
/**********************************/
USHORT CntrlParms[2]={0x212C,0x2200};
#if CRYSTAL
USHORT DataParms[2]={0x4000,0xC5F5};
#else
USHORT DataParms[2]={0xC000,0xC5F5};
#endif
/**********************************/
BOOL PAUSED=FALSE;
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
extern USHORT DCmd[0x1001];
extern struct Task *tc_Handler;
extern long Handler_sigbit;
extern long rate_sigbit;
/**********************************/
int OldGainVal;
/**********************************/
short SMPTERRORcnt=0,HILOcnt=0;
extern struct StudioBase *StudioBase;

extern struct AudioHandler *hand;
extern struct HandlerModState *state;
/**********************************/
/**********************************/
/**********************************/
struct ActiveSample *PS;

struct ChanData CD[16];
/**********************************/
USHORT DSP_BUFF_PTR;
/**********************************/
/**********************************/
extern struct IntData ID;

extern USHORT *Rbuff,*RbuffEND;
/****************************************************************/
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
short se_cmd_set_channel_pan;
short se_cmd_set_cutoff_freq;
short se_cmd_set_input_gain;
short se_cmd_set_sampling_rate;
short se_info_event_count;
short se_info_hilo_tick;
short se_cmd_pause_playback;

short se_cmd_download_code;
short se_info_block_played;
short se_cmd_trigger_playback;
short se_cmd_trigger_timecode;

short se_cmd_set_delay;
short se_cmd_set_feedback;
short se_cmd_set_timer_rate;
short se_cmd_set_delay2;
short se_cmd_set_mix;
short se_info_sampling_rate_error;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitStudioMessages()
{
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");

se_info_hilo_tick		= GetEventID("SE_INFO_HILO_TICK");
se_cmd_trigger_record		= GetEventID("SE_CMD_TRIGGER_RECORD");
se_info_channel_done		= GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_stop_playing		= GetEventID("SE_CMD_STOP_PLAYING");
se_cmd_stop_recording		= GetEventID("SE_CMD_STOP_RECORDING");
se_cmd_kill_module		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_cmd_set_channel_pan		= GetEventID("SE_CMD_SET_CHANNEL_PAN");
se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_info_event_count		= GetEventID("SE_INFO_EVENT_COUNT");
se_cmd_pause_playback		= GetEventID("SE_CMD_PAUSE_PLAYBACK");

se_cmd_trigger_playback		= GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_cmd_trigger_timecode		= GetEventID("SE_CMD_TRIGGER_TIMECODE");
se_info_block_played		= GetEventID("SE_INFO_BLOCK_PLAYED");

se_cmd_set_delay		=GetEventID("SE_CMD_SET_DELAY");
se_cmd_set_feedback		=GetEventID("SE_CMD_SET_FEEDBACK");
se_cmd_set_timer_rate		= GetEventID("SE_CMD_SET_TIMER_RATE");
se_cmd_set_delay2		= GetEventID("SE_CMD_SET_DELAY2");
se_cmd_set_mix			= GetEventID("SE_CMD_SET_MIX");
se_cmd_download_code		= GetEventID("SE_CMD_DOWNLOAD_CODE");
se_info_sampling_rate_error	= GetEventID("SE_INFO_SAMPLING_RATE_ERROR");

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
NotifyMeOnEvent(se_cmd_set_channel_pan,mod);
NotifyMeOnEvent(se_cmd_set_input_gain,mod);
NotifyMeOnEvent(se_cmd_set_sampling_rate,mod);
NotifyMeOnEvent(se_cmd_select_module,mod);
NotifyMeOnEvent(se_cmd_kill_module,mod);
NotifyMeOnEvent(se_info_event_count, mod);
NotifyMeOnEvent(se_cmd_pause_playback,mod);

NotifyMeOnEvent(se_cmd_trigger_playback,mod);
NotifyMeOnEvent(se_cmd_trigger_timecode,mod);

NotifyMeOnCount(se_info_hilo_tick, mod);
NotifyMeOnEvent(se_info_hilo_tick, mod);
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

int lasttc;
struct ActiveSample *lastAS;

int CHEEZE=0;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
void SetSRate();
struct ChanKey *key;
struct ActiveSample *as;
struct StudioEvent *event;
int unit_num,rate,x,z,y;
ULONG last,new;
double fr,sa;
void PlayFirst();
void InitCardSware();
void SendVolumePan();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{

/*||PLAY||*/
	if(event->type == se_cmd_trigger_playback)
	/***************************************/
		{
		PS=(struct ActiveSample *)event->arg1;
		key=PS->key;
		if(NULL==key) telluser("AD516 Handler:","KEY IS NULL!!!",NULL);

		if (key->handler==hand)
			{
			x=key->chan_num;
			CD[x].PS=PS;

			if (ID.SoundOn && RateValue(BestRate(PS->region.parms.rate))!=hand->current_sampling_rate) BroadcastEventParms(se_info_sampling_rate_error,1,key,0,0,0,0);
			SetSRate(PS->region.parms.rate);

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				SendVolumePan(key,PS->region.parms.volume,PS->region.parms.pan);
				}

			PlayFirst(x,0);
			key->as=PS;

			z=key->parms.volume;
			y=key->parms.pan;
			key->parms=PS->region.parms;
			*key->state_parms=PS->region.parms;

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,key,PS->region.parms.volume,0,0,0,0);
				BroadcastEventParmsLessOne(se_cmd_set_channel_pan,mod,key,PS->region.parms.pan,0,0,0,0);
				}
			else
				{
				key->parms.volume=z;
				key->parms.pan=y;
				key->state_parms->volume=z;
				key->state_parms->pan=y;
				}
			}
		}

	if(event->type == se_cmd_trigger_timecode)
	/***************************************/
		{
		PS=(struct ActiveSample *)event->arg1;
		key=PS->key;
		if(NULL==key) telluser("AD516 Handler:","AS->KEY IS NULL!!!",NULL);

		if (key->handler==hand)
			{
			x=key->chan_num;
			CD[x].PS=PS;

			if (ID.SoundOn && RateValue(BestRate(PS->region.parms.rate))!=hand->current_sampling_rate) BroadcastEventParms(se_info_sampling_rate_error,1,key,0,0,0,0);
			SetSRate(PS->region.parms.rate);

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				SendVolumePan(key,PS->region.parms.volume,PS->region.parms.pan);
				}

			if (lasttc==event->arg2) PlayFirst(x,1);
			else 
				{
				PlayFirst(x,0);
				lasttc=event->arg2;
				lastAS=PS;

				}
			key->as=PS;

			z=key->parms.volume;
			y=key->parms.pan;
			key->parms=PS->region.parms;
			*key->state_parms=PS->region.parms;

			if(!(PS->flags&AS_IGNORE_DEFAULT_VOLUME))
				{
				BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,key,PS->region.parms.volume,0,0,0,0);
				BroadcastEventParmsLessOne(se_cmd_set_channel_pan,mod,key,PS->region.parms.pan,0,0,0,0);
				}
			else
				{
				key->parms.volume=z;
				key->parms.pan=y;
				key->state_parms->volume=z;
				key->state_parms->pan=y;
				}
			}
		}

	if(event->type == se_cmd_stop_playing)
	/***************************************/
		{
		if (key=(struct ChanKey *)event->arg1)
			{
			unit_num=key->chan_mask;
			x=key->chan_num;

			if (ID.SoundOn & unit_num)
				{
				if (lastAS==key->as) lasttc=NULL;

				ID.SoundOn &= ~unit_num;ID.PlayCount[x]=0;
				BroadcastEventParms(se_info_channel_done, key->as, key,0,0,key->as->region.user_data,0);
				key->as=NULL;
				}
			}
		}

/*||RECORD||*/
	if(event->type == se_cmd_trigger_record)
	/***************************************/
		{
		Forbid();
		if (StudioBase->flags&SB_SMPTE_ON) 
			{
			last=QuerySmpteTime();
			x=0;
			while (last==(new=QuerySmpteTime()) && x++ < 500000) ;
			}
		else new=0;

		x=0;y=0;
		as=(struct ActiveSample *)event->arg1;
TRigGErLOop:
		if (as->key->handler == hand)
			{
			key=as->key;
			key->as=as;
			if (key->flags & CK_CHAN_LEFT)
				{
				x=x|CHANLEFT;y=14;
				CD[14].PS=as;
				SetSRate(as->region.parms.rate);
				as->region.parms.starttimecode=new;
				}
			if (key->flags & CK_CHAN_RIGHT)
				{
				x=x|CHANRIGHT;
				CD[15].PS=as;y=15;
				SetSRate(as->region.parms.rate);
				as->region.parms.starttimecode=new;
				}
			}
		as=(struct ActiveSample *)event->arg2;
		event->arg2=NULL;
		if (as) goto TRigGErLOop;
		Permit();


		Disable();
		RecSamps=0;RecFrames=0;
		Enable();

		if (y)
			{
			if(CD[y].PS->region.parms.flags & SI_TAP_OUTPUT) {SendW(RecordOutOn|x);}
			else {SendW(RecordInOn|x);}
			}
		}

	if(event->type == se_cmd_stop_recording)
	/***************************************/
		{
		key=(struct ChanKey *)event->arg1;
STopLOop:
		if (key->handler == hand)
			{
			if (CD[14].PS && key->flags&CK_CHAN_LEFT)
				{
				SendW(RecordOff|CHANLEFT);
				Disable();
				fr=RecFrames;sa=RecSamps;
				Enable();
				if (fr) CD[14].PS->region.parms.smpte_sampling_rate=(sa*((double)StudioBase->defaults.smpte_mode))/fr;
				BroadcastEventParms(se_info_channel_done, key->as, key,0,0,0,0);
				CD[14].PS=NULL;
				key->as=NULL;
				}
			if (CD[15].PS && key->flags&CK_CHAN_RIGHT)
				{
				SendW(RecordOff|CHANRIGHT);
				Disable();
				fr=RecFrames;sa=RecSamps;
				Enable();
				if (fr) CD[15].PS->region.parms.smpte_sampling_rate=(sa*((double)StudioBase->defaults.smpte_mode))/fr;
				BroadcastEventParms(se_info_channel_done, key->as, key, 0,0,0,0);
				CD[15].PS=NULL;
				key->as=NULL;
				}
			}
		key=(struct ChanKey *)event->arg2;
		if (key) goto STopLOop;
		}
/* * */
	if(event->type == se_cmd_set_channel_volume)
	/***************************************/
		{
		key=(struct ChanKey *)event->arg1;
		if (key->handler==hand)
			{
			SendVolumePan(key,event->arg2,key->parms.pan);
			}
		}
	if(event->type == se_cmd_set_channel_pan)
	/***************************************/
		{
		key=(struct ChanKey *)event->arg1;
		if (key->handler==hand)
			{
			SendVolumePan(key,key->parms.volume,event->arg2);
			}
		}

	if(event->type == se_cmd_set_input_gain)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			x=(event->arg2-ZERO_DB)/48;
			if (x>0x000F) x=0x000F;
			if (x<0) x=0;
			DataParms[1]=(DataParms[1]&0xF0F0) + x + (x<<8);
			SendW(DataRegAdj);
			SendW(DataParms[0]);
			SendW(DataParms[1]);
			hand->current_input_gain=x*48+ZERO_DB;
			state->input_gain=x*48+ZERO_DB;
			}
		}

	if(event->type == se_cmd_set_sampling_rate)
	/****************************************/
	/* event->arg2 = Sampling Rate samps/sec  */
	/****************************************/
		{
		if (event->arg1==(int)hand)
			{
			SetSRateNM(event->arg2);
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
		if (!CHEEZE)
		if (HILOcnt>1)
			{
			Disable();
			ID.CmdBuffPtr=(ID.CmdBuffPtr+1) & 0x0FFF;
			DCmd[ID.CmdBuffPtr]=1;
			Enable();
			Signal(tc_Handler,1<<Handler_sigbit);
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
			x=event->arg2;
			if (x<25) x=25;
			SendW(SetTimer);
			SendW(x);
			}
		}

	if(event->type == se_cmd_pause_playback)
	/***************************************/
		{
		if (event->arg1==(int)hand)
			{
			if (event->arg2) {PAUSED=TRUE;SendW(PINT|OFF);}
			else {SendW(PINT|ON);PAUSED=FALSE;}
			}
		}

	if(event->type == se_cmd_download_code)
	/***************************************/
		{
		CHEEZE=event->arg2;
		WaitTOF();
		WaitTOF();
		WaitTOF();
		if (event->arg2)
			{
			ModeOff();
			if (SendCode(event->arg2)) telluser("AD516 Handler","Can not Download Dsp Code",NOTICE_RETURN_NOW);
			for(x=0;x<100000;x++) rate=*(port+DATA);
			ModeOn();
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
void SetSRateNM(rate)
int rate;
{
int BestRate();
int RateValue();
int x;
extern USHORT SRateVal[];

x=BestRate(rate);

state->sampling_rate=RateValue(x);
hand->current_sampling_rate=RateValue(x);
hand->current_filter=hand->current_sampling_rate>>1;

CntrlParms[0]=(CntrlParms[0]&0xFFC7)+((SRateVal[x]&0x0007)<<3);
CntrlParms[1]=(CntrlParms[1]&0xCFFF)+((SRateVal[x]&0x0030)<<8);
Signal(tc_Handler,1<<rate_sigbit);
}
/********************************************************************/
void SetSRate(rate)
int rate;
{
int x;
SetSRateNM(rate);
x=BestRate(rate);
BroadcastEventParmsLessOne(se_cmd_set_sampling_rate,mod,hand,RateValue(x),0,0,0,0);
}
/*****************************************************************/
/*****************************************************************/
void SendVolumePan(key,vol,pan)
struct ChanKey *key;
int vol,pan;
{
int z;

key->parms.volume=vol;
key->parms.pan=pan;
key->state_parms->volume=vol;
key->state_parms->pan=pan;
if (key->flags & CK_CHAN_PLAY)
	{
	z=key->chan_num;
	vol=MIN(vol,3393);
	if (pan==ZERO_DB)
		{
		SendW(ChanVol+((z+1)<<8));
		SendW(DBs[vol]);
		SendW(DBs[vol]);
		}
	if (pan<ZERO_DB)
		{
		SendW(ChanVol+((z+1)<<8));
		SendW(DBs[vol]);
		SendW(DBs[MAX(vol+(pan-ZERO_DB),0)]);
		}
	if (pan>ZERO_DB)
		{
		SendW(ChanVol+((z+1)<<8));
		SendW(DBs[MAX(vol-(pan-ZERO_DB),0)]);
		SendW(DBs[vol]);
		}
	}
if (key->flags & CK_CHAN_MONITOR)
	{
	vol=MIN(vol,3393);
	SendW(ChanVol);
	SendW(DBs[vol]);
	SendW(DBs[vol]);
	}
if (key->flags & CK_CHAN_OUTPUT)
	{
	vol=MIN(vol,3393);
	SendW((9<<8)+ChanVol);
	SendW(DBs[vol]);
	SendW(DBs[vol]);
	}
}
/********************************************************************/
/*****************************************************************/
extern USHORT DCmd[0x1001];
ULONG PlayCntr;
/*****************************************************************/
void PlayFirst(x,flag)
int x;
int flag;
{
short *data;
LONG ActualLen;
int ASHandlerQueryPlayData(),status;
void WaitErrorFunc();

if (flag==0)
	{
	ID.PlayCount[x]=3;
	PlayCntr=3;
	}
else
	{
	ID.PlayCount[x]=PlayCntr;
	}

WaitingError:	status=ASHandlerQueryPlayData(CD[x].PS,BLOCKWORDS*2,&ActualLen,&data);
		if (status==STDERR_DATA_NOT_AVAILABLE_YET) {WaitErrorFunc();goto WaitingError;}
		if (ActualLen>1) 
			if (flag==0) SendFirstBlock(port,data,8-x,ActualLen);
			else SendSyncedBlock(port,data,8-x,ActualLen);
		if (status==STDERR_DATA_EOF)
			{
			ID.SoundOn=ID.SoundOn & NegPw2[x];
			ID.PlayCount[x]=0;
			BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key,0,0,CD[x].PS->region.user_data,0);
			}
		else 
			{
			ID.SoundOn=ID.SoundOn|(1<<x);

			Disable();
			ID.CmdBuffPtr=(ID.CmdBuffPtr+1) & 0x0FFF;
			DCmd[ID.CmdBuffPtr]=15;
			Enable();
			Signal(tc_Handler,1<<Handler_sigbit);
			}
}

/********************************************************************/
/********************************************************************/
extern int ErrorLevel;
/********************************************************************/
void InitCard()
{
extern USHORT DBs[];
int x;
extern USHORT *port;

ErrorLevel=11; 
/*for(x=0;x<2049;x++) {c=*(port+DATA);c=*(port+FIFO);SendW(0x003B);}*/
ErrorLevel=12;

hand->current_filter=0;
hand->current_sampling_rate=state->sampling_rate;
hand->current_input_gain=state->input_gain;

ErrorLevel=14;
BroadcastEventParms(se_cmd_set_sampling_rate,hand,hand->current_sampling_rate,0, 0, 0,0);
ErrorLevel=15;
BroadcastEventParms(se_cmd_set_input_gain,hand,hand->current_input_gain, 0, 0, 0, 0);
ErrorLevel=16;

for(x=0;x<12;x++)
	{
	hand->channel[x].parms=state->ChanParms[x];
	hand->channel[x].state_parms=&state->ChanParms[x];
	SendVolumePan(&hand->channel[x],hand->channel[x].parms.volume,hand->channel[x].parms.pan);
	}

ErrorLevel=20;
}
/********************************************************************/
/********************************************************************/
void ChangeSamplingRate()
{
static USHORT CP0,CP1;
if (CP0!=CntrlParms[0] || CP1!=CntrlParms[1])
	{
	if (PAUSED==FALSE)
		{
		ModeOff();
		SendW(SetRate);
		SendW(CntrlParms[0]);
		SendW(CntrlParms[1]);
		ModeOn();
		CP0=CntrlParms[0];
		CP1=CntrlParms[1];
		}
	else
		{
		telluser("You can not change the sampling rate","while playback is paused.",NOTICE_RETURN_NOW);
		if (CP0==0 || CP1==0)
			{
			CP0=CntrlParms[0];
			CP1=CntrlParms[1];
			}
		else
			{
			CntrlParms[0]=CP0;
			CntrlParms[1]=CP1;
			}
		}
	}
}
