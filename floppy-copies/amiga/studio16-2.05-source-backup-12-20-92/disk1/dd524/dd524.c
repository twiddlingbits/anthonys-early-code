#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/studio16.h"
#include "/include/gadlib.h"

#include "dd524.data"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_DD524";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *dio_win;
struct Module *thismod;
struct MsgPort *gadlib_port;

extern struct StudioBase *StudioBase;

void handle_studioevents();
void handle_intuievents();
void InitDIO();
void DoOpenDIO();
void DoCloseDIO();
struct Window *OpenNewWind();
void handle_gadlibevents();

/** events I'm interested in **/

short se_cmd_select_module;
short se_cmd_kill_module;	 
short se_info_module_closed;
short se_info_module_opened;

/***********************************************************/

static struct NewButton import = {
   NULL,25,22,85,32,BUTN_COOL,0,0,0,0,0,0,"Import"};
static struct NewButton export = {
   &import,25,35,85,45,BUTN_COOL,0,0,0,0,0,0,"Export"};
static struct NewButton stop = {
   &export,25,48,85,58,BUTN_COOL,0,0,0,0,0,0,"Stop"};

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
InitDIO();						/** initalize module **/

while (TRUE) {
	waitmask=(1<<thismod->notifyme->mp_SigBit);
	if (dio_win)
		waitmask|=(1<<dio_win->UserPort->mp_SigBit);
	if (gadlib_port)
		waitmask|=(1<<gadlib_port->mp_SigBit);

	mask=Wait(waitmask);
	if (dio_win)
		if (mask&(1<<dio_win->UserPort->mp_SigBit))
			handle_intuievents();
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	if (gadlib_port)
		if (mask & (1<<gadlib_port->mp_SigBit))
			handle_gadlibevents();
	}
}

/***********************************************************/

void InitDIO()
{
static struct NewModule DIO_data ={"DD524",0,sizeof(struct StandardModState),0};

if ((thismod=(struct Module *)AddModule(&DIO_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}

/***********************************************************/

void DoOpenDIO()
{
NewWindowStructure1.Screen=StudioBase->screen;
dio_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL, NULL,NULL);
if (dio_win==NULL) {
	return;
	}

gadlib_port=(struct MsgPort *)CreatePort(0,0);
stop.notifyme=gadlib_port;
stop.c = &StudioBase->defaults.colors;
import.notifyme=gadlib_port;
import.c = &StudioBase->defaults.colors;
export.notifyme=gadlib_port;
export.c = &StudioBase->defaults.colors;
create_gadget_chain(dio_win,&stop);

/** Let everyone else who cares know a new window is open **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_select_module) {
			if ((struct Module *)event->arg1==thismod) {
				if (dio_win==0) {
					DoOpenDIO();
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DoCloseDIO();
			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

	ReplyMsg(event); 
	}
}

/***********************************************************/

void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
struct Gadget *g;

while ((message=(struct IntuiMessage *)GetMsg(dio_win->UserPort))!=0) {
	HandleIntuitMess(dio_win,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		DoCloseDIO();
		return;
		}

	else if (class==NEWSIZE) 
		{
		RefreshWind(dio_win);
		}
	}
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
short i;
static short exporting=FALSE;
static short importing=FALSE;

while ((event=(struct GadMsg *)GetMsg(gadlib_port))!=0) {

	if (event->g==import.gad_ptr) {
		kprintf("Import\n");
		}
	else if (event->g==export.gad_ptr) {
		if (StartExport())
			exporting=TRUE;
		}
	else {
		if (exporting)
			StopExport();	
		}
	}
}

/***********************************************************/

void DoCloseDIO()
{
if (dio_win) {
	delete_gadget_chain(dio_win);
	DeletePort(gadlib_port);
	gadlib_port=0;
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	CloseWind(dio_win);
	dio_win=0;
	}
}
