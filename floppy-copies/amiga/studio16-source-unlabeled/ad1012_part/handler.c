#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "handler.data"
/**********************************/
#define BLOCKWORDS 512
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
extern void handle_studioevents();
void handle_intuievents();
extern void handle_interuptevents();
void prnt();
extern long Handler_sigbit;

/**********************************/
/**********************************/
struct Module *mod;
extern struct studioBase *studioBase;
struct Window *wind;
/**********************************/
/**********************************/
short se_dELAYbUFF;
short se_delay_off;
short se_delay_on;
short se_hilo_info;
short se_hilo_off;
short se_hilo_on;
short se_io_begin_playing;
short se_io_begin_recording;
short se_io_chan_done;
short se_io_play_data;
short se_io_ready_for_data;
short se_io_record_data;
short se_io_stop_playing;
short se_io_stop_recording;
short se_kill_module;
short se_module_selected;
short se_monitor_off;
short se_monitor_on;
short se_set_channel_volume;
short se_set_cutoff_freq;
short se_set_input_gain;
short se_set_sampling_rate;
short se_smpte_hm;
short se_smpte_off;
short se_smpte_on;
short se_smpte_sf;


short se_record_sample;
short se_stop_recording;

/**********************************/
/**********************************/
static void WinGadSize();
void setup();
void setdown();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void main()
{
int mask;
	
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule();				/** initalize module **/
DisplayWind();				/** open window **/

for(;;)	{
	mask=Wait((1<<Handler_sigbit) | (1<<wind->UserPort->mp_SigBit) | (1<<mod->notifyme->mp_SigBit));
	if (mask&(1<<Handler_sigbit)) handle_interuptevents();
	if (mask&(1<<wind->UserPort->mp_SigBit)) handle_intuievents();
	if (mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule()
{
static struct NewModule ModData ={"AD1012.handler",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	closelibraries();
	exit(10);
	}

setup();

se_dELAYbUFF		= GetEventID("SE_DelayBuff");
se_delay_off		= GetEventID("SE_DELAY_OFF");
se_delay_on		= GetEventID("SE_DELAY_ON");
se_hilo_info		= GetEventID("SE_HILO_INFO");
se_hilo_off		= GetEventID("SE_HILO_OFF");
se_hilo_on		= GetEventID("SE_HILO_ON");
se_io_begin_playing	= GetEventID("SE_IO_BEGIN_PLAYING");
se_io_begin_recording	= GetEventID("SE_IO_BEGIN_RECORDING");
se_io_chan_done		= GetEventID("SE_IO_CHAN_DONE");
se_io_play_data		= GetEventID("SE_IO_PLAY_DATA");
se_io_ready_for_data	= GetEventID("SE_IO_READY_FOR_DATA");
se_io_record_data	= GetEventID("SE_IO_RECORD_DATA");
se_io_stop_playing	= GetEventID("SE_IO_STOP_PLAYING");
se_io_stop_recording	= GetEventID("SE_IO_STOP_RECORDING");
se_kill_module		= GetEventID("SE_KILL_MODULE");
se_module_selected	= GetEventID("SE_MODULE_SELECTED");
se_monitor_off		= GetEventID("SE_MONITOR_OFF");
se_monitor_on		= GetEventID("SE_MONITOR_ON");
se_set_channel_volume	= GetEventID("SE_SET_CHANNEL_VOLUME");
se_set_cutoff_freq	= GetEventID("SE_SET_CUTOFF_FREQ");
se_set_input_gain	= GetEventID("SE_SET_INPUT_GAIN");
se_set_sampling_rate	= GetEventID("SE_SET_SAMPLING_RATE");
se_smpte_hm		= GetEventID("SE_SMPTE_HM");
se_smpte_off		= GetEventID("SE_SMPTE_OFF");
se_smpte_on		= GetEventID("SE_SMPTE_ON");
se_smpte_sf		= GetEventID("SE_SMPTE_SF");


se_record_sample		=GetEventID("SE_RECORD_SAMPLE");
se_stop_recording		=GetEventID("SE_STOP_RECORDING");

NotifyMeOnEvent(se_io_begin_playing,mod);
NotifyMeOnEvent(se_io_play_data,mod);
NotifyMeOnEvent(se_io_stop_playing,mod);
NotifyMeOnEvent(se_io_begin_recording,mod);
NotifyMeOnEvent(se_io_stop_recording,mod);
NotifyMeOnEvent(se_set_channel_volume,mod);
NotifyMeOnEvent(se_set_input_gain,mod);
NotifyMeOnEvent(se_set_cutoff_freq,mod);
NotifyMeOnEvent(se_set_sampling_rate,mod);
NotifyMeOnEvent(se_monitor_on,mod);
NotifyMeOnEvent(se_monitor_off,mod);
NotifyMeOnEvent(se_hilo_on,mod);
NotifyMeOnEvent(se_hilo_off,mod);
NotifyMeOnEvent(se_smpte_on,mod);
NotifyMeOnEvent(se_smpte_off,mod);
NotifyMeOnEvent(se_delay_on,mod);
NotifyMeOnEvent(se_dELAYbUFF,mod);
NotifyMeOnEvent(se_delay_off,mod);
NotifyMeOnEvent(se_module_selected,mod);
NotifyMeOnEvent(se_kill_module,mod);

NotifyMeOnEvent(se_record_sample, mod);
NotifyMeOnEvent(se_stop_recording, mod);

}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	class=message->Class;
	code=message->Code;
	ReplyMsg(message);

	switch (class)
		{
		case CLOSEWINDOW:
		/*********************************/
			HideWind();
		break;

		case NEWSIZE:
		/*********************************/
			WinGadSize();
		break;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplayWind()
{
struct Window *OpenWindow();

NewWind.Screen = studioBase->screen;
if ((wind = OpenWindow(&NewWind))==0) printf("ERROR, Could not open Window");
WinGadSize();
AddGadget(wind,&SIZEgad,-1);
}

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void HideWind()
{
if (wind)
	{
	delete_gadget_chain(wind);
	NewWind.LeftEdge=wind->LeftEdge;
	NewWind.TopEdge=wind->TopEdge;
	NewWind.Width=wind->Width;
	NewWind.Height=wind->Height;
	CloseWindow(wind);
	wind=NULL;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static void WinGadSize()
{
void RefreshWind();
RefreshWind();
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RefreshWind()
{
SetAPen(wind->RPort, BROWN);
RectFill(wind->RPort, 2,10, wind->Width-3, wind->Height-2);

SetAPen(wind->RPort, DARK_BROWN);
RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

SIZEgad.LeftEdge =wind->Width-12;
SIZEgad.TopEdge =wind->Height-6;
SIZEgad.Width =10;
SIZEgad.Height =5;

SetAPen(wind->RPort,DARK_BROWN);
Move(wind->RPort,SIZEgad.LeftEdge,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(wind->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge);
SetAPen(wind->RPort,WHITE);
Move(wind->RPort,SIZEgad.LeftEdge+2,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(wind->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge+1);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
struct	HLinfo
	{
	USHORT	Input;
	USHORT	Output;
	USHORT	Chan1;
	USHORT	Chan2;
	USHORT	Chan3;
	USHORT	Chan4;
	};
/********************************************************************/
void ProcHiLoInfo()
{
static	struct HLinfo	HL,hl;

HL.Output=GetW();
HL.Input =GetW();
HL.Chan1 =GetW();
HL.Chan2 =GetW();
HL.Chan3 =GetW();
HL.Chan4 =GetW();

CopyMem(&HL,&hl,sizeof(	struct HLinfo));
BroadcastEventParms(se_hilo_info,&hl,0,0,0,0,0);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
