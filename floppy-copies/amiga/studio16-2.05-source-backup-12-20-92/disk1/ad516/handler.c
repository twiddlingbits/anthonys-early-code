#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "handler.data"
#include "/include/CardCmds.h"
#include "clib/macros.h"
/**********************************/
#define Clock 10000000
/**********************************/
/* Declarations for CBACK */
long _stack = 8000;				/* Amount of stack space our task needs   */
char *_procname = "STUDIO_AD516_HANDLER";	/* The name of the task to create         */
long _priority = 51;				/* The priority to run us at              */
long _BackGroundIO = 0;				/* Flag to tell it we want to do I/O      */
/**********************************/
struct HandlerModState *state;
/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
extern USHORT CardNum;
/**********************************/
void InitModule();
void InitStudioMessages();
void DisplayWind();
void HideWind();
extern void handle_studioevents();
int handle_intuievents();
extern void handle_interuptevents();
void prnt();
extern long Handler_sigbit;
extern long rate_sigbit;
/**********************************/
/**********************************/
struct Module *mod;
extern struct StudioBase *StudioBase;
struct Window *wind;
/**********************************/
/**********************************/
extern short se_info_module_opened;
extern short se_info_module_closed;
/**********************************/
struct AudioHandler *hand;
/**********************************/
void setup();
void setdown();
/****************************************************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int FilterValue(num2)
int num2;
{
return(1);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int BestFilter(filter)
int filter;
{
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int GainValue(num)
int num;
{
int gain;
gain=((num*3)+(100<<1))<<4;
return(gain);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int BestGain(gain)
int gain;
{
extern struct NewAudioHandler me;
int x;

for(x=hand->number_of_gain_settings-1;x>0;x--)
	{
	if ( gain >= (*hand->GainValueFunc)(x)) break;
	}
if (x<0) x=0;
return(x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int SRate[14] =
	{  5513 ,  6615,  8000,  9600, 11025, 16000, 18900, 22050, 27429, 32000, 33075, 37800, 44100, 48000};
USHORT SRateVal[14] = 
	{0x0020 ,0x0027,0x0010,0x0017,0x0021,0x0011,0x0022,0x0023,0x0012,0x0013,0x0026,0x0024,0x0025,0x0016};

int RateValue(num)
int num;
{
extern struct NewAudioHandler me;

return(SRate[num]);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int BestRate(rate)
int rate;
{
extern struct NewAudioHandler me;
int x;

/*for(x=0;x<hand->number_of_rate_settings;x++)
	if ( rate <= (*hand->RateValueFunc)(x)) break;
if (x>number_of_rate_settings-1) x=number_of_rate_settings-1;*/

for(x=hand->number_of_rate_settings-1;x>0;x--)
	{
	if ( rate >= (*hand->RateValueFunc)(x)) break;
	}
if (x<0) x=0;
return(x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
BOOL AHAllocChan(chan,flags)
struct ChanKey *chan;
int flags;
{
return(TRUE);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void AHFreeChan(chan)
struct ChanKey *chan;
{
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int AHTriggerList();

/**********************************/
struct NewAudioHandler me = {"AD516" ,0,0,AH_STEREO|AH_GAIN_ADJ|AH_RATE_ADJ,4,1,1,1,12,10,0,
	1,FilterValue,BestFilter,
	16,GainValue,BestGain,
/*2624-262*/
	14,RateValue,BestRate,
	NULL,NULL,NULL,NULL,NULL,NULL,AHTriggerList,NULL,NULL,AHAllocChan,AHFreeChan,
/**********************************/
	12,
	"Input"  ,CK_CHAN_VOLUME|CK_CHAN_STEREO|CK_CHAN_HILO|CK_CHAN_INPUT,0,
	"Play 1",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,0,
	"Play 2",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,1,
	"Play 3",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,2,
	"Play 4",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,3,
	"Play 5",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,4,
	"Play 6",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,5,
	"Play 7",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,6,
	"Play 8",CK_CHAN_VOLUME|CK_CHAN_PAN|CK_CHAN_HILO|CK_CHAN_PLAY,7,
	"Record L",CK_CHAN_RECORD|CK_TAP_INPUT|CK_TAP_OUTPUT|CK_CHAN_LEFT,0,
	"Record R",CK_CHAN_RECORD|CK_TAP_INPUT|CK_TAP_OUTPUT|CK_CHAN_RIGHT,0,
	"Output",CK_CHAN_VOLUME|CK_CHAN_STEREO|CK_CHAN_HILO|CK_CHAN_OUTPUT,0,
};
/**********************************/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
void ChangeSamplingRate();

openlibraries();
InitModule(argv);				/** initalize module **/

while (TRUE)
	{
	mask=(1<<Handler_sigbit)|(1<<rate_sigbit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mask && (1<<Handler_sigbit)) handle_interuptevents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) {handle_intuievents();}
	if (mask & (1<<rate_sigbit)) ChangeSamplingRate();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct Task *task;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule(name)
char *name;
{
char *FindFileName();
struct AudioHandler *AddAudioHandler();
void InitCard();
void GetCard();
int x;

static struct NewModule ModData ={NULL,0,sizeof(struct HandlerModState)};
ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) Fail("Can not add handler Module",1);
mod->state->flags|=MS_UTILITY_MODULE|MS_KEEP_RESIDENT;

GetCard();

me.handler_mod=mod;
me.class_id=CardNum;

state=(struct HandlerModState *)mod->state;

if((hand=AddAudioHandler(&me))==0) Fail("Can not add audio handler",2);

if (!(mod->state->flags & MS_STATE_INITILIZED)) 
	{
	state->sampling_rate=44100;
	state->filter=20000;
	state->input_gain=ZERO_DB;
	for (x=0;x<12;x++)
		{
		state->ChanParms[x].volume=ZERO_DB;
		state->ChanParms[x].rate=44100;
		state->ChanParms[x].filter3db=20000;
		state->ChanParms[x].pan=ZERO_DB;
		}
	state->ChanParms[0].volume=0;
	}

InitStudioMessages();
setup();

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	code=message->Code;
	ReplyMsg(message);

	switch (class)
		{
		case CLOSEWINDOW:
		/*********************************/
			HideWind();
			return(0);
		break;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern USHORT CardRev,CardMemory,CardPal,CardFlags,CardSoftRev,CardSoft;
extern ULONG  CardSerialNum;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplayWind()
{
char	a[180],*s[4]={"Digital 64x","ER","ERR","ERROR"};
struct Window *OpenNewWindow();
extern int Ypos;

NewWind.Screen = StudioBase->screen;
if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL))==NULL)  telluser("AD516 Handler:","Could not Open Window",0);
Ypos=0;
BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

sprintf(a,"AD516 Revision %x.%02x",(CardRev>>8),(CardRev&0x00FF));prnt(a);
sprintf(a,"---------------------");prnt(a);
sprintf(a,"Memory     :%dK Words",(CardMemory+1)*8);prnt(a);
sprintf(a,"DSP PAL    :%d",CardPal&0x0f);prnt(a);
sprintf(a,"AUTO PAL   :%d",(CardPal&0xf0)>>8);prnt(a);
sprintf(a,"Filter Type:%s",s[CardFlags&0x03]);prnt(a);
sprintf(a,"Eprom Rev  :%x.%02x",(CardSerialNum&0x0000FF00)>>8,(CardSerialNum&0x000000FF));prnt(a);
sprintf(a,"SoftW Type :%x",CardSoft);prnt(a);
sprintf(a,"SoftW Rev  :%x.%02x",(CardSoftRev>>8),(CardSoftRev&0x00FF));prnt(a);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void HideWind()
{
struct Window *w;

if (wind)
	{
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
