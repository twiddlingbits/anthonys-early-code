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
/* Declarations for CBACK */
long _stack = 8000;				/* Amount of stack space our task needs   */
char *_procname = "STUDIO_AD1012_HANDLER";	/* The name of the task to create         */
long _priority = 0;				/* The priority to run us at              */
long _BackGroundIO = 0;				/* Flag to tell it we want to do I/O      */
/**********************************/
#define Clock 10000000
/**********************************/
struct AudioHandler *hand;
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
/**********************************/
/**********************************/
struct Module *mod;
extern struct StudioBase *StudioBase;
struct Window *wind;
/**********************************/
/**********************************/
extern short se_info_module_opened;
extern short se_info_module_closed;
void setup();
void setdown();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern USHORT CardRev,CardMemory,CardPal,CardFlags,CardSoftRev,CardSoft;
extern ULONG  CardSerialNum;
/****************************************************************/
/****************************************************************/
struct HandlerModState *state;
/****************************************************************/
/****************************************************************/
int Anum=28;		/* V4.2 Butter  */
int ACutoff[28] =
	{ 33333,22222,20833,16667,13889,13333,11111,10417,9524,8333,
	7407,6944,6667,6068,5952,5556,5208,5128,4762,4630,4444,4167,
	3718,3472,3205,2976,2778,2604};
int ACval[28] = 
	{0x1100,0x1200,0x0100,0x1300,0x0200,0x1400,0x1500,0x0300,0x1600,0x1700,0x1800,0x0500,0x1900,0x1A00,0x0600,0x1B00,0x0700,0x1C00,0x1D00,0x0800,0x1E00,0x1F00,0x0A00,0x0B00,0x0C00,0x0D00,0x0E00,0x0F00};
/****************************************************************/
int Bnum=15;		/* V5.0 Bessel  */
int BCutoff[15] =
	{ 99999,50000,33333,25000,20000,16667,14286,12500,11111,10000,9091,8333,
7692,7143,6666};
int BCval[15] = 
	{0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,0x1E00,0x1F00};
/****************************************************************/
int Cnum=15;		/* V5.0 Butter  */
int CCutoff[15] =
	{50000,33333,25000,20000,16667,14286,12500,11111,10000,9091,8333,
7692,7143,6666,6250};
int CCval[15] = 
	{0x1100,0x1200,0x1300,0x1400,0x1500,0x1600,0x1700,0x1800,0x1900,0x1A00,0x1B00,0x1C00,0x1D00,0x1E00,0x1F00};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int FilterValue(num2)
int num2;
{
int num;
int freq=0;
num=hand->number_of_filter_settings-num2-1;

if ((CardFlags&0x03)==0) freq=ACutoff[num];
if ((CardFlags&0x03)==1) freq=BCutoff[num];
if ((CardFlags&0x03)==2) freq=CCutoff[num];
return(freq);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int BestFilter(filter)
int filter;
{
int x;

for(x=hand->number_of_filter_settings-1;x>0;x--)
	{
	if ( filter >= (*hand->FilterValueFunc)(x)) break;
	}
if (x<0) x=0;
return(x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int GainValue(num)
int num;
{
int gain;
gain=(num)<<5;
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
int RateValue(num)
int num;
{
extern struct NewAudioHandler me;
int x;

x=(hand->number_of_rate_settings-num)+61;

return((Clock/2)/x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int BestRate(rate)
int rate;
{
extern struct NewAudioHandler me;
int x;

for(x=hand->number_of_rate_settings-1;x>0;x--)
	{
	if ( rate >= (*hand->RateValueFunc)(x)) break;
	}
if (x<0) x=0;
return(x);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct ChanKey *playkey;  GLOBAL
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
BOOL AHAllocChan(chan,flags)
struct ChanKey *chan;
int flags;
{
/*if (chan.flags & CK_CHAN_RECORD)
	{
	playkey=(struct ChanKey *)AllocChan(hand,3,CK_CHAN_PLAY);
	if (playkey) return(TRUE);
	else return(FALSE); UNTESTED
	}*/
return(TRUE);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void AHFreeChan(chan)
struct ChanKey *chan;
{
/*if (chan.flags & CK_CHAN_RECORD)
	{FreeChan(playkey);playkey=NULL;}*/
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int AHTriggerList();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/**********************************/
struct NewAudioHandler me = {"AD1012",0,0,AH_FILTER_ADJ|AH_GAIN_ADJ|AH_RATE_ADJ,4,1,1,1,12,0,0,
	28,FilterValue,BestFilter,
	100,GainValue,BestGain,
	600,RateValue,BestRate,
	NULL,NULL,NULL,NULL,NULL,NULL,AHTriggerList,NULL,NULL,AHAllocChan,AHFreeChan,
	7,
	"Input"  ,CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_INPUT,0,
	"Play 1",CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_PLAY,0,
	"Play 2",CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_PLAY,1,
	"Play 3",CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_PLAY,2,
	"Play 4",CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_PLAY,3,
	"Output",CK_CHAN_VOLUME|CK_CHAN_HILO|CHAN_OUTPUT,0,
	"Record",CHAN_RECORD|CK_TAP_INPUT|CK_TAP_OUTPUT,0,
	};
/**********************************/


/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;

openlibraries();
InitModule(argv);				/** initalize module **/

SetTaskPri(FindTask(NULL),51);
while (TRUE)
	{
	mask=(1<<Handler_sigbit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mask && (1<<Handler_sigbit)) handle_interuptevents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) {handle_intuievents();}
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
int GetFreqVal();
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
	state->input_gain=50<<5;
	for (x=0;x<8;x++)
		{
		state->ChanParms[x].volume=ZERO_DB;
		state->ChanParms[x].rate=44100;
		state->ChanParms[x].filter3db=20000;
		state->ChanParms[x].pan=ZERO_DB;
		}
	state->ChanParms[0].volume=0;
	}
setup();

InitStudioMessages();

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
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
void DisplayWind()
{
char	a[180],*s[4]={"Bessel 3","Butterworth V4.2","Butterworth V5.0","ERROR"};
struct Window *OpenNewWindow();
extern int Ypos;

NewWind.Screen = StudioBase->screen;
if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL))==NULL)  telluser("AD1012 Handler:","Could not Open Window",0);
Ypos=0;
BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

sprintf(a,"AD1012 Revision %x.%02x",(CardRev>>8),(CardRev&0x00FF));prnt(a);
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
int GetFreqVal(n)
int n;
{
if ((CardFlags&0x03)==0) return(ACutoff[n]);
if ((CardFlags&0x03)==1) return(BCutoff[n]);
if ((CardFlags&0x03)==2) return(CCutoff[n]);
}
/********************************************************************/
/********************************************************************/
