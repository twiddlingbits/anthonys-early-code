#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include <devices/timer.h>
#include "time.data"
#include "time.h"
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_TIME";	/* The name of the task to create         */
long _priority = -1;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
char *FindFileName();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void Quit();
void handle_studioevents();
void handle_intuievents();
void WindGadSize();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
/*struct Module *hand;*/
struct Module *mod;
struct Window *wind;
struct StandardModState *state;
struct MsgPort *timerport;
/**********************************/
/**********************************/
int DigX[6];
void wdraw_digits();
void UpdateTime();
void WindTitleChange();
/**************************************************************/
/**************************************************************/
USHORT FLAG=0;
struct timeval currentval;
struct tm *tim;
struct Task *task,*FindTask();
struct MsgPort *timerport;
struct timerequest *timermsg;
/**************************************************************/
/**************************************************************/
void _main(cmd)
char *cmd;
{
int mask,t;
int h,m;

InitModule(cmd);			/** initalize module **/

while (TRUE)
	{
	mask=(1<<timerport->mp_SigBit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	if(mask & (1<<timerport->mp_SigBit)) {GetMsg(timerport);FLAG=0;}

	if (wind)
		{
		time(&t);
		tim=localtime(&t);
		h=tim->tm_hour;m=tim->tm_min;
		if (h>12) h=h-12;
		UpdateTime(h,m);
		if (FLAG==0) 
			{
			FLAG=1;
			currentval.tv_secs = 60-tim->tm_sec;
			currentval.tv_micro = 0;
			timermsg->tr_node.io_Command = TR_ADDREQUEST;
			timermsg->tr_time = currentval;
			SendIO((struct IORequest *) timermsg );
			}
		}
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
static struct NewModule ModData ={NULL,NULL,sizeof(struct StandardModState)};

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) {telluser("Time:","Could Not Add Module",0);Quit();}

state=mod->state;

se_cmd_select_module 	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window	= GetEventID("SE_CMD_HIDE_WINDOW");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
timerport = (struct MsgPort *) CreatePort( 0, 0 );
timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();

if (FLAG==1)
	{AbortIO( timermsg );
	WaitIO( timermsg );}
if (timerport) DeletePort(timerport);
CloseDevice( (struct IORequest *) timermsg );
DeleteExtIO( (struct IORequest *) timermsg );

if (mod) DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module && (struct Module *)event->arg1==mod)
			if (wind==NULL) DisplayWind();

	/*********************************/
	if(event->type == se_cmd_kill_module && (struct Module *)event->arg1 == mod)
		{
		ReplyMsg(event);
		Quit();
		}
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
		{
		HideWind();
		return;
		}
	}
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL))!=0)  
	{
	WindGadSize();
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	}
else telluser("Time:","Can not open window!",0);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

Disable();

if (wind)
	{
	w=wind;
	wind=NULL;
	CloseWind(w);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
Enable();
}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
short x;
int l,cw;
int cellw,cellh;

if (wind)
	{
	cellh = wind->Height-27;
	cw = ((wind->Width-32)<<16)/6;
	cellw=cw>>16;

	l = (16<<16);
	for(x=0;x<5;x++)
		{
		DigX[x]=(l>>16);
		l=l+cw+(cw/4);
		}

/*DigX[0]=16;
DigX[1]=16+(((cw+(cw/4))*1)>>16);
DigX[2]=16+(((cw+(cw/4))*2)>>16);
DigX[3]=16+(((cw+(cw/4))*3)>>16);
DigX[4]=16+(((cw+(cw/4))*4)>>16);*/

	RefreshWind(wind);

	new_big_digit_size(cellh,cellw);
	wdraw_digits();

	draw_indent(wind->RPort,10,16,wind->Width-11,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
 UBYTE ho=11,mo=11;

void UpdateTime(hours,minutes)
int	hours,minutes;
{
short tens();
short ones();
short tens2();

if (wind)
	{
	if (ho!=hours)
		{
		big_digit(wind->RPort,tens2(hours),tens2(ho), DigX[0], 19);
		big_digit(wind->RPort,ones(hours),ones(ho), DigX[1], 19);
		ho=hours;
		}
	if (mo!=minutes)
		{
		big_digit(wind->RPort,tens(minutes),tens(mo), DigX[3], 19);
		big_digit(wind->RPort,ones(minutes),ones(mo), DigX[4], 19);
		mo=minutes;
		}
	}
}

/**************************************************************/
/**************************************************************/

void wdraw_digits()
{
big_digit(wind->RPort,tens2(ho),10,DigX[0], 19);
big_digit(wind->RPort,ones(ho),10,DigX[1], 19);
big_digit(wind->RPort,10,0,DigX[2], 19);
big_digit(wind->RPort,tens(mo),10,DigX[3], 19);
big_digit(wind->RPort,ones(mo),10,DigX[4], 19);
}
/**************************************************************/
/**************************************************************/
short tens2(x)
UBYTE x;
{
x=x/10;
if (x==0) x=11;
return((short)x);
}
short tens(x)
UBYTE x;
{
return(x/10);
}
short ones(x)
UBYTE x;
{
return(x%10);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
