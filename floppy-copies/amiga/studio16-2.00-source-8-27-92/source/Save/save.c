#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_SAVE";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Module *thismod;
extern struct StudioBase *StudioBase;
void handle_studioevents();
void InitSave();

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
InitSave();						/** initalize module **/

while (TRUE) {
	waitmask=(1<<thismod->notifyme->mp_SigBit);
	mask=Wait(waitmask);
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	}
}

/***********************************************************/

void InitSave()
{
static struct NewModule Save_data ={"SaveSetup",0,sizeof(struct StandardModState),0};

if ((thismod=(struct Module *)AddModule(&Save_data))==0) {
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

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_cmd_select_module) {
			if ((struct Module *)event->arg1==thismod) {
				if(askuser("Save Setup:","Save Preferences, Module options, and Window positions?",0)) {
					new_pointer(PTR_BUSY);
					SaveModState(0);	/** save the instance list **/
					SaveDefaults(0);	/** Save StudioBase **/
					new_pointer(PTR_NORMAL);
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DeleteModule(thismod);
			closelibraries();
			exit(0);
			}

	ReplyMsg(event); 
	}
}
