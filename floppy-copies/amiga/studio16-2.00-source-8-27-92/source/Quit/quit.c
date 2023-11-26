#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/**** CBACK.o defines *****/

long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_QUIT";	/* The name of the task to create         */
long _priority = -5;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct StudioBase *StudioBase;
struct Library *GadLibBase;
struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;

void _main()

{
static struct NewModule mod_data ={"Quit",0,sizeof(struct StandardModState)};
struct Module *thismod;
struct StudioEvent *event;
short se_cmd_kill_module;
short se_cmd_select_module;
struct MinNode *node;
struct Module *mod;
int AnyLeft;

StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (StudioBase==NULL) {
	exit(10);
	}

GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0);
if (GadLibBase==NULL) {
	CloseLibrary(StudioBase);
	exit(10);
	}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{
	CloseLibrary(StudioBase);
	CloseLibrary(GadLibBase);
	exit(10);
	}

if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{
	CloseLibrary(GfxBase);
	CloseLibrary(StudioBase);
	CloseLibrary(GadLibBase);
	exit(10);
	}


if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	CloseLibrary(GfxBase);
	CloseLibrary(StudioBase);
	CloseLibrary(GadLibBase);
	CloseLibrary(IntuitionBase);
   exit(10);
   }

se_cmd_kill_module	=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module	=GetEventID("SE_CMD_SELECT_MODULE");
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);

while (TRUE) {
	Wait(1<<thismod->notifyme->mp_SigBit);
	while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
		if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);
			DeleteModule(thismod);

			CloseLibrary(IntuitionBase);
			CloseLibrary(GfxBase);
			CloseLibrary(StudioBase);
			CloseLibrary(GadLibBase);
			exit(0);
			}

		else if (event->type==se_cmd_select_module && (struct Module *)event->arg1==thismod) 
			{
			ReplyMsg(event);
			if (askuser("Are you sure you want to quit?",0,0)) 
				{
				ScreenToBack(StudioBase->screen);
				/**** KILL ALL NON UTILITIES ****/
				ObtainSharedSemaphore(&StudioBase->lockstudio);
				for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
					{
					mod=(struct Module *)node;
					if(!(mod->state->flags&MS_UTILITY_MODULE)) 
						SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
					}
				ReleaseSharedSemaphore(&StudioBase->lockstudio);

				/**** WAIT ****/
				AnyLeft=2;
				while (AnyLeft>1)		/* 1 because quit is counted */
					{
					AnyLeft=0;
					WaitTOF();WaitTOF();
					ObtainSharedSemaphore(&StudioBase->lockstudio);
					for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
						{
						mod=(struct Module *)node;
						if(!(mod->state->flags&MS_UTILITY_MODULE)) {AnyLeft++;}
						}
					ReleaseSharedSemaphore(&StudioBase->lockstudio);
					}

				/** HALT ANY PLAYING SOUND ****/

				BroadcastEventParms(GetEventID("SE_CMD_STOP_ALL_PLAYBACK"),0,0,0,0,0,0);

				/**** KILL ALL UTILITIES ****/
				ObtainSharedSemaphore(&StudioBase->lockstudio);
				for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
					{
					mod=(struct Module *)node;
					SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
					}
				if (StudioBase->quit_task) Signal(StudioBase->quit_task,1<<StudioBase->quit_sigbit);

				ReleaseSharedSemaphore(&StudioBase->lockstudio);
				}
			}
		else {
			ReplyMsg(event);	/** Reply to Unneeded messages **/
			}
		}
	}
}
