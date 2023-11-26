#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "fft.data"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_FFT";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *fft_win;
struct Module *thismod;

extern struct StudioBase *StudioBase;

void handle_studioevents();
void handle_intuievents();
void InitFFT();
void DoOpenFFT();
void DoCloseFFT();
struct Window *OpenNewWind();

/** events I'm interested in **/

short se_cmd_select_module;
short se_cmd_kill_module;	 
short se_info_module_closed;
short se_info_module_opened;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
StandardEnviroment();		/** opens studio16 customscreen **/
InitFFT();						/** initalize module **/

while (TRUE) {
	if (fft_win)
		waitmask=(1<<fft_win->UserPort->mp_SigBit) | (1<<thismod->notifyme->mp_SigBit);
	else
		waitmask=(1<<thismod->notifyme->mp_SigBit);

	mask=Wait(waitmask);

	if (fft_win)
		if (mask&(1<<fft_win->UserPort->mp_SigBit))
			handle_intuievents();
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	}
}

/***********************************************************/

void InitFFT()
{
static struct NewModule fft_data ={"FFT",0,sizeof(struct StandardModState),0};

if ((thismod=(struct Module *)AddModule(&fft_data))==0) {
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

void DoOpenFFT()
{
NewWindowStructure1.Screen=StudioBase->screen;
fft_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL, NULL,NULL);
if (fft_win==NULL) {
	return;
	}

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
				if (fft_win==0) {
					DoOpenFFT();
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DoCloseFFT();
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

while ((message=(struct IntuiMessage *)GetMsg(fft_win->UserPort))!=0) {
	HandleIntuitMess(fft_win,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		DoCloseFFT();
		return;
		}

	else if (class==NEWSIZE) 
		{
		RefreshWind(fft_win);
		}
	}
}

/***********************************************************/

void DoCloseFFT()
{
if (fft_win) {
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	CloseWind(fft_win);
	fft_win=0;
	}
}
