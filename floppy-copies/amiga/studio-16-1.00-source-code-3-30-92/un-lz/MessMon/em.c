#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "em.data"
/**********************************/
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_MESSAGE_MONITOR";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_hilo;
short se_info_smpte;
short se_info_playback_status;
/**********************************/
/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void Quit();
void WindGadSize();
/**********************************/
/**********************************/
struct Module *mod;
extern struct StudioBase *StudioBase;
struct Window *wind;
/**********************************/
/**********************************/
int Ypos=0;
void prnt();
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(argv)
char *argv;
{
int mask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule(argv);				/** initalize module **/

while (TRUE)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
char *FindFileName();

static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState)};
ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Event Monitor:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module 	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 	= GetEventID("SE_CMD_KILL_MODULE");
se_info_smpte		= GetEventID("SE_INFO_SMPTE");
se_info_playback_status	= GetEventID("SE_INFO_PLAYBACK_STATUS");
se_info_hilo		= GetEventID("SE_INFO_HILO");
se_info_module_opened  = GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed  = GetEventID("SE_INFO_MODULE_CLOSED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==NULL)
				DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			Quit();
			}
		}

	prnt(event);

	ReplyMsg(event);
	}
}
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{HideWind(); return;}
	}
}
/**************************************************************/
/**************************************************************/
SHORT NumOfID;

void DisplayWind()
{
static void InterlaceAdjust();
int i;

Ypos=0;

NewWind.Screen = StudioBase->screen;

if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL))==0)  {telluser("Event Monitor:","Can not open window!",0);Quit();}

WindGadSize();

BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

NumOfID=StudioBase->max_registered_ids;

IgnoreEvent(se_cmd_select_module, mod);
IgnoreEvent(se_cmd_kill_module, mod);

for (i=0; i < NumOfID; i++)
	NotifyMeOnEvent(i,mod);
IgnoreEvent(se_info_smpte, mod);
IgnoreEvent(se_info_playback_status, mod);
IgnoreEvent(se_info_hilo, mod);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;
int i;

for (i=0; i < NumOfID; i++)
	IgnoreEvent(i,mod);

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);  

if (wind)
	{
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);

}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
Ypos=0;
if (wind)
	{
	RefreshWind(wind);
	}
}
/**************************************************************/
/**************************************************************/
void Quit()
{
DeleteModule(mod);
if (wind) HideWind();
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void prnt(e)
struct StudioEvent *e;
{
static BYTE CL=0;
char	a[180];
if (wind)
	{
	if ((Ypos*8+22)>wind->Height) {Ypos=0; CL++;}
	sprintf(a,"%-23.23s  %08x %08x %08x %08x %08x %08x                                                  ",StudioBase->eventidarray[e->type]+3,e->arg1,e->arg2,e->arg3,e->arg4,e->arg4,e->flags);
	if (CL&0x01) SetAPen(wind->RPort, BLACK);
	else SetAPen(wind->RPort, WHITE);
	SetBPen(wind->RPort, BROWN);
	SetDrMd(wind->RPort, JAM2);
	Move(wind->RPort,8,Ypos*8+19);
	a[((wind->Width)>>3)-2]=0;
	Text(wind->RPort,a,((wind->Width)>>3)-2);
	Ypos++;
	}
}
/**************************************************************/
/**************************************************************/
