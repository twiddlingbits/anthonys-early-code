#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "eq.data"
/**********************************/
/**********************************/
#define sp 5.0
#define Xsz 15
#define Ysz 100
#define Xs 20
#define Ys 20
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
/**********************************/
/**********************************/
void WinGadSize();
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int handle_intuievents();
/**********************************/
/**********************************/
struct Module *mod;
extern struct studioBase *studioBase;
struct Window *wind;
/**********************************/
/**********************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
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
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule()
{
static struct NewModule ModData ={"Equalizer",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Could Not Add Module","",0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module  = GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module = GetEventID("SE_CMD_KILL_MODULE");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{

	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
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
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
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
			return(0);
		break;
		case NEWSIZE:
		/*********************************/
			WinGadSize();
		break;
		}
	}
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*****************************************************************/

void DisplayWind()
{
EQ_NewWindow.Screen = studioBase->screen;

if ((wind = OpenWind(&EQ_NewWindow))==NULL)  telluser("Could not Open Window","",0);
WinGadSize();
}
/*****************************************************************/

void HideWind()
{
struct Window *w;

if (wind)
	{
	SaveGadgetState(&EQ16000gad);
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
}

/**************************************************************/

static void EQ31func()
{
}
static void EQ62func()
{
}
static void EQ125func()
{
}
static void EQ250func()
{
}
static void EQ500func()
{
}
static void EQ1000func()
{
}
static void EQ2000func()
{
}
static void EQ4000func()
{
}
static void EQ8000func()
{
}
static void EQ16000func()
{
}

/**************************************************************/

void WinGadSize()
{
char frq[] = {"16k","8k ","4k ","2k ","1k ","500","250","125","62 ","31 "};
double Y,Yz,x,xz,xs,z;
int c;
struct NewSlider *ns=&EQ16000gad;

SaveGadgetState(&EQ16000gad);
delete_gadget_chain(wind);
RefreshWind(wind);

Y=4+sp*2;
Yz=wind->Height-(sp*2)-3;

xs=(wind->Width-(sp*15))/10;
xz=wind->Width-(sp*2);

x=xz-xs-sp+((xs-23)/2);

SetAPen(wind->RPort, DARK_BROWN);
SetDrMd(wind->RPort,JAM1);
if (xs>24)
	for(z=0;z < 10; z++)
		{
		c=z;
		SetAPen(wind->RPort, DARK_BROWN);
		Move(wind->RPort,(int)x,(int)(wind->Height-(sp)));
		Text(wind->RPort,&frq[c<<2], 3);
		SetAPen(wind->RPort, WHITE);
		Move(wind->RPort,(int)x+1,(int)(wind->Height-(sp)+1));
		Text(wind->RPort,&frq[c<<2], 3);
		x=x-xs-sp;
		}
else Yz=Yz+7;

SetAPen(wind->RPort, DARK_BROWN);
SetDrMd(wind->RPort,JAM1);
for(z=0;z < 17; z++)
	{
	Move(wind->RPort,(int)xz,(int)(Y+((Yz-Y-4)/16.0)*z+2.5));
	Draw(wind->RPort,10,(int)(Y+((Yz-Y-4)/16.0)*z+2.5));
	}

while (ns)
	{
	ns->box_y1=Y;
	ns->box_y2=Yz;

	x=xz-sp;
	xz=x-xs;

	ns->box_x1=xz;
	ns->box_x2=x;
	ns=ns->next;
	}
if (create_gadget_chain(wind,&EQ16000gad)==0) telluser("Could not Refresh Gadgets","",0);
}
