#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "prefs.data"
#include "clib/macros.h"
#include "stdio.h"
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_MODULE_PREFERENCES";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/


struct StandardModState *edit_state;
int	flag=0;
/**********************************/
/**********************************/
short se_cmd_edit_prefs;
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_info_prefs_changed;
short se_cmd_hide_window;
/**********************************/
/**********************************/
struct window *OpenWind();
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
struct Window *wind,*Hwind,*Cwind;
/**********************************/
/**********************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(argv)
char *argv;
{
int mask;

InitModule(argv);				/** initalize module **/

while (TRUE)
	{
	mask=0;
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
char *FindFileName();
static struct NewModule ModData ={NULL,MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct StandardModState),MOD_SINGLE_RESIDENT};

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);
if ((mod=(struct Module *)AddModule(&ModData))==0)  {telluser("Preferences:","Could Not Add Module",0);Quit();}

se_info_prefs_changed	= GetEventID("SE_INFO_PREFS_CHANGED");
se_cmd_edit_prefs	= GetEventID("SE_CMD_EDIT_PREFS");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window		= GetEventID("SE_CMD_HIDE_WINDOW");

NotifyMeOnEvent(se_cmd_edit_prefs, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
NotifyMeOnEvent(se_info_module_closed, mod);
NotifyMeOnEvent(se_cmd_hide_window, mod);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{

	/*********************************/
	if (event->type == se_cmd_hide_window && event->arg1)
		{
/*		kprintf("HIDE_WINDOW %d\n",event->arg1);*/
		if ((struct Window *)event->arg1 == wind)
			HideWind();
		if ((struct Window *)event->arg1 == Hwind)
			{
			HideHandlerWind();
			Hwind=NULL;
if (edit_state->attached_module) SendMsg(edit_state->attached_module,se_info_prefs_changed,edit_state->attached_module,0,0,0,0,0);
			if (NULL == wind)  {UnlockMod(edit_state);}
			}
		if ((struct Window *)event->arg1 == Cwind)
			{
			(edit_state->pref_hide_func)();
			Cwind=NULL;
if (edit_state->attached_module) SendMsg(edit_state->attached_module,se_info_prefs_changed,edit_state->attached_module,0,0,0,0,0);
			if (NULL == wind)  {UnlockMod(edit_state);}
			}
		}

	/*********************************/
	if(event->type == se_info_module_closed)
		{
		if (wind!=NULL)
		if (edit_state->attached_module==(struct module *)event->arg1)
			{
			HideWind();
			}
		}

	/*********************************/
	if(event->type == se_cmd_edit_prefs)
		{
		if (wind!=NULL || Cwind!=NULL || Hwind!=NULL) 
			{
			HideWind();
			}
		DisplayWind(event->arg1,event->arg2,event->arg3);
		}

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
/**************************************************************/
/**************************************************************/
void DisplayWind(state,le,te)
int le,te;
struct StandardModState *state;
{
struct Window *OpenNewWind();
void DisplayHandlerWind();
ULONG Fgs;

edit_state=(struct StandardModState *)state;

NewWind.LeftEdge=le-(NewWind.Width+6);
NewWind.TopEdge=te+3;


Fgs=edit_state->flags & (MS_CHOOSE_HANDLER|MS_CUSTOM_PREFS);
if (Fgs==NULL)
	{
	telluser("There are no Preferences for this window.","",NULL);
	return;
	}

LockMod(edit_state);  /* NOTE SHOULD BE SET BY CALLER */

if (Fgs==MS_CHOOSE_HANDLER || Fgs==MS_CUSTOM_PREFS)
	{
		if (Fgs & MS_CHOOSE_HANDLER)
			{
			if (Hwind==NULL) DisplayHandlerWind(edit_state,le,te);
			}
		if (Fgs & MS_CUSTOM_PREFS)
			{
			if (edit_state->attached_module)
				{
				Cwind=(edit_state->pref_display_func)(edit_state,le,te);
				}
			else telluser("Preferences:","Can not edit custom prefs, Module not in memory",NULL);
			}
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	return;
	}

if (wind = OpenNewWind(&NewWind,mod->state,HIDEMESSAGE|DONTMOVEWINDOW|IL_NOSIMPLE,NULL))
	{
	ColorGadList(&PREFlist);

	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetDrMd(wind->RPort, JAM1);
	Move(wind->RPort,10 ,20);
	Text(wind->RPort,edit_state->instance_name,MIN(16,strlen(edit_state->instance_name)));

	if (create_gadget_chain(wind,&PREFlist)==0) return;

	if(edit_state->flags & MS_CHOOSE_HANDLER) add_list_entry(PREFlist.gad_ptr,"Choose Handler",MS_CHOOSE_HANDLER);
	if(edit_state->flags & MS_CUSTOM_PREFS) add_list_entry(PREFlist.gad_ptr,edit_state->pref_name,MS_CUSTOM_PREFS);

	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	}
else telluser("Preferences:","Can not open window!",0);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;
void HideHandlerWind();

if (Cwind) (edit_state->pref_hide_func)();
Cwind=NULL;
if (Hwind) HideHandlerWind();

if (edit_state->attached_module) SendMsg(edit_state->attached_module,se_info_prefs_changed,edit_state->attached_module,0,0,0,0,0);

if (wind) UnlockMod(edit_state);
if (wind)
	{
	hide_gadget(PREFlist.gad_ptr);
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
edit_state=NULL;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();
if (mod)  DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
void PrefFunc(g)
struct	list *g;
{
void DisplayHandlerWind();

if(g->flags & LIST_DOUBLESELECT)
	{
	if (g->active_entry->user_data & MS_CHOOSE_HANDLER)
		{
		if (Hwind==NULL) DisplayHandlerWind(edit_state,wind->LeftEdge+wind->Width,wind->TopEdge);
		}
	if (g->active_entry->user_data & MS_CUSTOM_PREFS)
		{
		if (edit_state->attached_module)
			{
			Cwind=(edit_state->pref_display_func)(edit_state,wind->LeftEdge+wind->Width,wind->TopEdge);
			}
		else telluser("Preferences:","Can not edit custom prefs, Module not in memory",NULL);
		}
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
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/* \  /								\  /  */
/*  XX			CHOOSE HANDLER				 XX   */
/* /  \								/  \  */
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
void DisplayHandlerWind(state,le,te)
int le,te;
struct StandardModState *state;
{
char buff[50];
struct Window *OpenNewWind();
void HideHandlerWind();
struct AudioHandler *han,*FindAudioHandler();

int i=0;

HNewWind.LeftEdge=le-(HNewWind.Width+6);
HNewWind.TopEdge=te+3;
if ((Hwind = OpenNewWind(&HNewWind,NULL,HIDEMESSAGE|IL_NOSIMPLE,NULL))==NULL)  {telluser("Preferences:","Can not open window!",0);Quit();}

ColorGadList(&HANDLERlist);

if (create_gadget_chain(Hwind,&HANDLERlist)==0) Quit();

han = NULL;
han = FindAudioHandler(han,-1,0,NULL,-1);
while (han) 
	{
/*	han->handler_mod->state->flags |= MS_MODULE_IN_USE;*/ /* ADD ME LATER */
	sprintf(buff,"%s #%d",han->name,han->class_id);
	add_list_entry(HANDLERlist.gad_ptr,buff,han);
	if(han==edit_state->hand)
		handle_new_select(HANDLERlist.gad_ptr,get_list_entry_ptr(HANDLERlist.gad_ptr,i),i);
	han = FindAudioHandler(han,-1,0,NULL,-1);
	i++;
	}
}
/**************************************************************/
void HideHandlerWind()
{
struct Window *w;

if (Hwind)
	{
	if (((struct list *)(HANDLERlist.gad_ptr))->active_entry!=NULL)
		if (edit_state)
			{
			edit_state->hand=(struct AudioHandler *)((struct list *)(HANDLERlist.gad_ptr))->active_entry->user_data;
			edit_state->handler_name=edit_state->hand->name;
			edit_state->handler_class_id =edit_state->hand->class_id;
			}

	hide_gadget(HANDLERlist.gad_ptr);
	delete_gadget_chain(Hwind);

	w=Hwind;
	Hwind=NULL;
	CloseWind(w);
	}
}
/**************************************************************/
void HandFunc(g)
struct	list *g;
{
void HideHandlerWind();
if(g->flags & LIST_DOUBLESELECT)
	BroadcastEventParms(se_cmd_hide_window,Hwind,0,0,0,0,0);
/*if(g->flags & LIST_DOUBLESELECT)
	HideHandlerWind();*/
}
/**************************************************************/
