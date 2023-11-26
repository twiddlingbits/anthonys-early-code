#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "/include/cardcmds.h"
#include "handler.data"
/**********************************/
#define BLOCKWORDS 512
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

extern short se_dELAYbUFF;
extern short se_delay_off;
extern short se_delay_on;
extern short se_hilo_info;
extern short se_hilo_off;
extern short se_hilo_on;
extern short se_io_begin_playing;
extern short se_io_begin_recording;
extern short se_io_chan_done;
extern short se_io_play_data;
extern short se_io_ready_for_data;
extern short se_io_record_data;
extern short se_io_stop_playing;
extern short se_io_stop_recording;
extern short se_kill_module;
extern short se_module_selected;
extern short se_monitor_off;
extern short se_monitor_on;
extern short se_set_channel_volume;
extern short se_set_cutoff_freq;
extern short se_set_input_gain;
extern short se_set_sampling_rate;
extern short se_smpte_hm;
extern short se_smpte_off;
extern short se_smpte_on;
extern short se_smpte_sf;


extern short se_record_sample;
extern short se_stop_recording;
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

extern int	PlayFlags[];
extern USHORT	*buff[];
extern int ChanCtr[];
extern int	WordsInBuff[];
extern int RecBuffSize,RecBuffNum;
extern USHORT *RecBuff[2],*Rbuff,*RbuffEND;
/****************************************************************/
int Cutoff[28] =
	{ 33333,22222,20833,16667,13889,13333,11111,10417,9524,8333,
	7407,6944,6667,6068,5952,5556,5208,5128,4762,4630,4444,4167,
	3718,3472,3205,2976,2778,2604};
int FREQval[28] = 
	{0x1100,0x1200,0x0100,0x1300,0x0200,0x1400,0x1500,0x0300,0x1600,0x1700,0x1800,0x0500,0x1900,0x1A00,0x0600,0x1B00,0x0700,0x1C00,0x1D00,0x0800,0x1E00,0x1F00,0x0A00,0x0B00,0x0C00,0x0D00,0x0E00,0x0F00};
/****************************************************************/

/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/
void handle_studioevents()
{
struct StudioEvent *event;
int unit_num,rate,vol,c,x,z;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
/*||PLAY||*/
	if(event->type == se_io_begin_playing)
	/***************************************/
		{
		prnt("BEGIN PLAYING",event);

		rate = event->arg2;
		unit_num=Pw2[event->arg1];

		z=HALF_CLK/event->arg2;

		SendW(VAR0lo+((z & 0x00FF)<<8));
		SendW(VAR0hi+(z & 0xFF00));
		SendW(SetRate);

		vol  = event->arg3;

		if (unit_num&0x0001) {ID.SoundOn=ID.SoundOn|0x0001;ID.PlayCount[0]=3;SendW(MovePlayPtr0);}
		if (unit_num&0x0002) {ID.SoundOn=ID.SoundOn|0x0002;ID.PlayCount[1]=3;SendW(MovePlayPtr1);}
		if (unit_num&0x0004) {ID.SoundOn=ID.SoundOn|0x0004;ID.PlayCount[2]=3;SendW(MovePlayPtr2);}
		if (unit_num&0x0008) {ID.SoundOn=ID.SoundOn|0x0008;ID.PlayCount[3]=3;SendW(MovePlayPtr3);}
		}

	if(event->type == se_io_play_data)
	/***************************************/
		{
		prnt("PLAY DATA    ",event);

		unit_num = event->arg3;

		buff[unit_num]=(USHORT *)event->arg1;
		WordsInBuff[unit_num]=event->arg2;
		PlayFlags[unit_num]=event->flags;
		ChanCtr[unit_num]=ChanCtr[unit_num]+event->arg2;
		}

	if(event->type == se_io_stop_playing)
	/***************************************/
		{
		prnt("STOP PLAYING ",event);

		unit_num=Pw2[event->arg1];
		x=event->arg1;
		if (unit_num&0x0001) {ID.SoundOn=ID.SoundOn&0xFFFE;ID.PlayCount[0]=0;ChanCtr[0]=0;}
		if (unit_num&0x0002) {ID.SoundOn=ID.SoundOn&0xFFFD;ID.PlayCount[1]=0;ChanCtr[1]=0;}
		if (unit_num&0x0004) {ID.SoundOn=ID.SoundOn&0xFFFB;ID.PlayCount[2]=0;ChanCtr[2]=0;}
		if (unit_num&0x0008) {ID.SoundOn=ID.SoundOn&0xFFF7;ID.PlayCount[3]=0;ChanCtr[3]=0;}
		BroadcastEventParms(se_io_chan_done,x, 0, 0,0,0,0);
		}

/*||RECORD||*/
	if(event->type == se_io_begin_recording)
	/***************************************/
		{
		prnt("START RECORD ",event);

		/* channel#=event->arg1 */
		z=5000000/event->arg2;
		RecBuff[0]=(USHORT *)event->arg3;
		RecBuff[1]=(USHORT *)event->arg4;
		RecBuffSize=event->arg5;

		SendW(VAR0lo+((z & 0x00FF)<<8));
		SendW(VAR0hi+(z & 0xFF00));
		SendW(SetRate);

		SendW(RecordOn);

		RecBuffNum=0;
		Rbuff=RecBuff[RecBuffNum];
		RbuffEND=Rbuff+RecBuffSize;
		}

	if(event->type == se_io_stop_recording)
	/***************************************/
		{
		prnt("STOP RECORD  ",event);

		SendW(RecordOff);
		BroadcastEventParms(se_io_record_data, RecBuff[RecBuffNum], Rbuff-RecBuff[RecBuffNum], 0,0,0,LAST_BLOCK);
		}

	if(event->type == se_set_channel_volume)
	/***************************************/
		{
		prnt("Channel Vol  ",event);

		x=event->arg2;
		z=event->arg1;
		SendW(VAR0lo+((x & 0x00FF)<<8));
		SendW(VAR0hi+(x & 0xFF00));
		SendW(SetChannelVol+(z<<8));
		}

	if(event->type == se_set_input_gain)
	/***************************************/
		{
		prnt("Input Gain   ",event);

		x=event->arg1;
		if(x>0)	SendW(GainUp+(x<<8));
		if(x<0)	SendW(GainDown+((-x)<<8));
		}

	if(event->type == se_set_cutoff_freq)
	/***************************************/
	/* event->arg1 = Cutoff frequency in Hz  */
	/***************************************/
		{
		prnt("Cutoff Freq  ",event);

		x=event->arg1;
		for(c=0;c!=27 && x<Cutoff[c];c++) ;
		SendW(SetFreq+FREQval[c]);
		}

	if(event->type == se_set_sampling_rate)
	/****************************************/
	/* event->arg1 = Sampling Rate samps/sec  */
	/****************************************/
		{
		prnt("Sampling Rate",event);

		x=HALF_CLK/event->arg1;
		SendW(VAR0lo+((x & 0x00FF)<<8));
		SendW(VAR0hi+(x & 0xFF00));
		SendW(SetRate);
		}

	if(event->type == se_monitor_on)
	/***************************************/
		{
		prnt("Monitor On   ",event);

		SendW(MonitorOn);
		}

	if(event->type == se_monitor_off)
	/***************************************/
		{
		prnt("Monitor Off  ",event);

		SendW(MonitorOff);
		}

	if(event->type == se_hilo_on)
	/***************************************/
		{
		prnt("HILO on      ",event);

		SendW(HILOon);
		}

	if(event->type == se_hilo_off)
	/***************************************/
		{
		prnt("HILO off     ",event);

		SendW(HILOoff);
		}

	if(event->type == se_smpte_on)
	/***************************************/
		{
		prnt("SMPTE on     ",event);

		SendW(SMPTEon);
/*		SendW(SMPTEreq);*/
		}

	if(event->type == se_smpte_off)
	/***************************************/
		{
		prnt("SMPTE off    ",event);

		SendW(SMPTEoff);
		}
	if(event->type == se_delay_on)
	/***************************************/
		{
		prnt("Delay On     ",event);

		SendW(DelayOn);
		}

	if(event->type == se_dELAYbUFF)
	/***************************************/
		{
		prnt("Delay Buff   ",event);

		x=event->arg1;
		SendW(VAR0lo+((x & 0x00FF)<<8));
		SendW(VAR0hi+(x & 0xFF00));
		SendW(DelayBuff);
		}

	if(event->type == se_delay_off)
	/***************************************/
		{
		prnt("Delay Off    ",event);

		SendW(DelayOff);
		}

	if(event->type == se_module_selected)
	/***************************************/
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();
		}

	if(event->type == se_kill_module)
	/***************************************/
		{
		if ((struct Module *)event->arg1==mod)
			{
			ReplyMsg(event);
			setdown();
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}



	if(event->type == se_record_sample)
		prnt("se_record_sample",event);

	if(event->type == se_stop_recording)
		prnt("se_stop_recording",event);


	ReplyMsg(event);
	}
}
/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/

/********************************************************************/
/********************************************************************/
/********************************************************************/