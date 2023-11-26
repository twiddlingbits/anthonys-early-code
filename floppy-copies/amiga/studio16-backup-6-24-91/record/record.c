#include <exec/types.h>
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "Record.data"
#include "/include/CardCmds.h"
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/

#define Clock 10000000.0
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_input_gain;
short se_cmd_set_cutoff_freq;
short se_cmd_set_sampling_rate;
short se_cmd_record_sample;
short se_cmd_stop_recording;
short se_cmd_set_channel_volume;

/**********************************/

char TempSampName[120] =  {"NoName",0};

int GlobSampRate;

extern struct studioBase *studioBase;

static UBYTE	AutoFilt=0;

struct Window *wind;

/*****************************************************************/
extern int Cutoff[];
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
/**********************************/
/**********************************/
struct Module *mod;
extern struct studioBase *studioBase;
static struct Window *wind;
/**********************************/
/**********************************/
/**********************************/
static void WinGadSize();
/****************************************************************/
int Cutoff[28] =
	{ 33333,22222,20833,16667,13889,13333,11111,10417,9524,8333,
	7407,6944,6667,6068,5952,5556,5208,5128,4762,4630,4444,4167,
	3718,3472,3205,2976,2778,2604};
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
void main()
{
int mask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule();				/** initalize module **/
DisplayWind();				/** open window **/

while (TRUE)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind)
		if (mask&(1<<wind->UserPort->mp_SigBit)) handle_intuievents();
	if (mod)
		if (mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
void InitModule()
{
static struct NewModule ModData ={"Recorder",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Could Not Add Module","",0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_input_gain	= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_cutoff_freq	= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_sampling_rate= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_cmd_record_sample	= GetEventID("SE_CMD_RECORD_SAMPLE");
se_cmd_stop_recording	= GetEventID("SE_CMD_STOP_RECORDING");

se_cmd_set_channel_volume 	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	if(event->type == se_cmd_select_module)
		{
		/*********************************/
			if ((struct Module *)event->arg1==mod)
				if (wind==NULL)
					DisplayWind();
		}

	if(event->type == se_cmd_kill_module)
		{
		/*********************************/
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}
	ReplyMsg(event);
	}
}
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
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

	if(class== CLOSEWINDOW)
		HideWind();
	}
}

/*****************************************************************/

void DisplayWind()
{
Record_NewWindow.Screen = studioBase->screen;

if ((wind = OpenWind(&Record_NewWindow))==0)  telluser("Could not Open Window","",0);

if (create_gadget_chain(wind,&MONITORbutton)==0) exit(10);

FREQknob = (struct slider *)FREQgad.gad_ptr;
RATEknob = (struct slider *)RATEgad.gad_ptr;

}
/*****************************************************************/

void HideWind()
{
struct Window *w;

if (wind)
	{
	SaveGadgetState(&MONITORbutton);
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
}

/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
int OldVal=50;
static void GAINfunc(k)
struct slider *k;
{
char buf[20];
int x;

sprintf(buf,"Gain %2d",k->top_element);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

x=k->top_element;

BroadcastEventParms(se_cmd_set_input_gain,x-OldVal, 0, 0, 0, 0,0);

OldVal=x;
}

/*****************************************************************/
static void FREQfunc(k)
struct slider *k;
{

char buf[20];
short	x;

x=27-k->top_element;

sprintf(buf,"Filter %5d",Cutoff[x]);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);

BroadcastEventParms(se_cmd_set_cutoff_freq,Cutoff[x], 0, 0, 0, 0,0);

}
/*****************************************************************/
static void RATEfunc(k)
struct slider *k;
{
char buf[20];
float	x,y;
int	c;
USHORT	z;

x=600-(k->top_element)+60;
y=(Clock/2)/x;
c=y;
GlobSampRate=c;

sprintf(buf,"Rate %5d",c);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

BroadcastEventParms(se_cmd_set_sampling_rate,c,0,0, 0, 0,0);

if (AutoFilt) 
	{
	z=0;
	while(((Cutoff[z]*1.5) > c>>1)&& z!=27)  z++;
	if(FREQknob->top_element!=(27-z))
		{
		FREQknob->top_element=27-z;
		move_knob(FREQknob);
		}
	}
}
/*****************************************************************/
static void MONITORfunc(b)
struct button *b;
{

if (b->flags&BUTN_HIT)
	BroadcastEventParms(se_cmd_set_channel_volume,4,0x7FFF,mod,0,0,0);
else
	BroadcastEventParms(se_cmd_set_channel_volume,4, 0x0000,mod,0,0,0);
}
/*****************************************************************/
static void RECORDfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	BroadcastEventParms(se_cmd_record_sample,TempSampName,GlobSampRate,0,0,0,0);
else
	BroadcastEventParms(se_cmd_stop_recording,0,0,0,0,0,0);
}
/*****************************************************************/
static void FILEfunc(b)
struct button *b;
{
FILEbut = b;
newgetstr(TempSampName,"Sample Name?",8*40);
xor_button(b);
}
/*****************************************************************/
static void AUTOFILTfunc(b)
struct button *b;
{
float	x,y;
int	c;
USHORT	z;


if (b->flags&BUTN_HIT)
	{
	x=600-(RATEknob->top_element)+60;
	y=(Clock/2)/x;
	c=y;

	z=0;
	while(((Cutoff[z]*1.5) > c>>1) && z!=27)  z++;
	if(FREQknob->top_element!=(27-z))
		{
		FREQknob->top_element=27-z;
		BroadcastEventParms(se_cmd_set_cutoff_freq,Cutoff[27-z], 0, 0, 0, 0,0);
		move_knob(FREQknob);
		}
	AutoFilt=1;
	}
else
	AutoFilt=0;
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*=*/
