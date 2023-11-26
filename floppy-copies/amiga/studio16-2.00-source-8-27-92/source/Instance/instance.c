#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <dos.h>
#include <stdio.h>
#include "/include/psound.h"
#include "instance.h"

#include "instance.pwc"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_INSTANCE";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *thiswin;
struct Module *thismod;
struct MsgPort *gadlibport;

short se_info_state_added;
short se_info_state_deleted;
short se_info_instance_name_changed;
short se_cmd_kill_module;
short se_cmd_select_module;
short se_info_module_closed;
short se_info_module_opened;


extern struct StudioBase *StudioBase;
extern struct MenuItem SubItems[];

void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
void InitInstanceList();
void FuncActivate();
struct Window *OpenNewWind();
void ReadDir();
void FuncLaunch();
void BuildInstanceList();
void DoInstanceListWin();
void CloseInstanceListWin();
struct StandardModState *AddModState();

static struct NewList instancelist =
			{NULL,0,50,0,50,LIST_COOL|LIST_SINGLESELECT|LIST_SORTED|LIST_USEKEYS|MANUAL_XOR|HIDE_GADGET,0,0,0,0,0,0,0,0,0};

/***********************************************************/

void _main()
{
unsigned long ie,se,ge,sigs;

openlibraries();
StandardEnviroment();		/** opens studio16 customscreen **/
InitInstanceList();					/** add module, etc. **/

se = 1<<thismod->notifyme->mp_SigBit;
ge = 1<<gadlibport->mp_SigBit;

while (TRUE) {
	if (thiswin) {
		ie = 1<<thiswin->UserPort->mp_SigBit;
		}
	else {
		ie = 0;
		}
   sigs=Wait(ie|se|ge);
	if (sigs&se)
		handle_studioevents();
	if (sigs&ie)
		handle_intuievents();
	if (sigs&ge)
		handle_gadlibevents();
	}
}

/***********************************************************/

void InitInstanceList()
{
static struct NewModule windows_list_data ={"Instance",0,sizeof(struct InstanceModState),0};

if ((thismod=(struct Module *)AddModule(&windows_list_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
   telluser("Can't add window_list","",0);
	closelibraries();
   exit(10);
   }

/** Initlize if required **/

if (!(thismod->state->flags&MS_STATE_INITILIZED)) {
	((struct InstanceModState *)thismod->state)->flags=INSTANCE_SHOW_APPS;
	thismod->state->flags |= MS_KEEP_RESIDENT;
	}


/** Check appropriate SubMenu (Show Apps or Show Utilities) **/

if(((struct InstanceModState *)thismod->state)->flags&INSTANCE_SHOW_APPS)
	SubItems[0].Flags |= CHECKED;
else
	SubItems[0].Flags &= (~CHECKED);

if(((struct InstanceModState *)thismod->state)->flags&INSTANCE_SHOW_UTILITIES)
	SubItems[1].Flags |= CHECKED;
else
	SubItems[1].Flags &= (~CHECKED);

/** Turn on the events we want to hear about **/

se_info_state_added				=GetEventID("SE_INFO_STATE_ADDED");
se_info_state_deleted			=GetEventID("SE_INFO_STATE_DELETED");
se_info_instance_name_changed	=GetEventID("SE_INFO_INSTANCE_NAME_CHANGED");
se_cmd_kill_module				=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module				=GetEventID("SE_CMD_SELECT_MODULE");
se_info_module_closed			=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened			=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_info_state_added, thismod);
NotifyMeOnEvent(se_info_state_deleted, thismod);
NotifyMeOnEvent(se_info_instance_name_changed, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_info_module_closed, thismod);
NotifyMeOnEvent(se_info_module_opened, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);


/** Gadget stuff that only needs doing once **/

instancelist.c=&StudioBase->defaults.colors;
gadlibport=(struct MsgPort *)CreatePort(0,0);
instancelist.notifyme=gadlibport;

/** Create all our gadgets **/

create_gadget_chain(thiswin, &instancelist);
hide_gadget(instancelist.gad_ptr);

/** Build displayed list by searching for any loaded modules **/
/** There is a BUG here! We need to syncronize the below search with add modules **/

BuildInstanceList();

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}

/***********************************************************/


void DoInstanceListWin()
{
extern struct Menu Titles[];

NewWindowStructure1.Screen=StudioBase->screen;

thiswin=OpenNewWind(&NewWindowStructure1,thismod->state,NULL,Titles);
if (thiswin==0) { 	/**!!!! ERROR!!!!! **/
	return;
	}

set_gadget_window(instancelist.gad_ptr,thiswin);
new_gadget_size(instancelist.gad_ptr,10,
	15, thiswin->Width-11, thiswin->Height-5);
show_gadget(instancelist.gad_ptr);
refresh_gadget_chain(thiswin);

/** place text at top of each list **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);
}
/***********************************************************/

void CloseInstanceListWin()
{
if (thiswin) {
	if (instancelist.gad_ptr) {
		hide_gadget(instancelist.gad_ptr);
		set_gadget_window(instancelist.gad_ptr,0);
		}
	CloseWind(thiswin);
	thiswin=0;
	}
}


/***********************************************************/

void handle_studioevents()
{
struct StandardModState *state;
struct StudioEvent *event;
struct Module *mod;
/*char bug[80];*/

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
/*sprintf(bug,"INS-SE#%d: \n",event->type);*/
/*kprintf(bug);*/
	if (event->type==se_info_state_added) {
/*kprintf("added\n");*/
		state = (struct StandardModState *)event->arg1;
		if (CorrectStateType(state))
			if (find_list_entry_by_userdata(instancelist.gad_ptr, state)==0)
				add_list_entry(instancelist.gad_ptr, state->instance_name, state);
		}

	else if (event->type==se_info_state_deleted) {
/*kprintf("deleted\n");*/
		state = (struct StandardModState *)event->arg1;
		delete_list_entry_by_userdata(instancelist.gad_ptr, state);
		}

		/** Refresh this list entry's data cause it may have changed **/
	else if (event->type==se_info_instance_name_changed) {
/*kprintf("changed\n");*/
		state = (struct StandardModState *)event->arg1;
		hide_gadget(instancelist.gad_ptr);
		if (delete_list_entry_by_userdata(instancelist.gad_ptr, state))
			add_list_entry(instancelist.gad_ptr,state->instance_name, state);
		show_gadget(instancelist.gad_ptr);
		refresh_gadget(instancelist.gad_ptr);
		}

	else if (event->type==se_info_module_closed) {
/*kprintf("mod_closed A\n");*/
		mod = (struct Module *)event->arg1;
		if (CorrectStateType(mod->state)) {
/*kprintf("mod_closed X\n");*/
			add_list_entry(instancelist.gad_ptr, mod->state->instance_name, mod->state);
/*kprintf("mod_closed Y\n");*/
			}
		if (!(mod->state->flags&MS_KEEP_RESIDENT)) {
			BroadcastEventParms(se_cmd_kill_module,mod,0,0,0,0,0);
			}
/*kprintf("mod_closed B\n");*/
		}

	else if (event->type==se_info_module_opened) {
/*kprintf("opened\n");*/
		mod = (struct Module *)event->arg1;
		delete_list_entry_by_userdata(instancelist.gad_ptr, mod->state);
		}

	else if (event->type==se_cmd_select_module) {
/*kprintf("select\n");*/
		if ((struct Module *)event->arg1==thismod) {
			if (thiswin==0) {
				DoInstanceListWin();
				}
			}
		}

	else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
/*kprintf("kill\n");*/
		ReplyMsg(event);
		hide_gadget(instancelist.gad_ptr);
/*		delete_gadget(instancelist.gad_ptr);*/
		delete_gadget_chain(thiswin);
		instancelist.gad_ptr = NULL;	/** checked by CloseInstanceListWin() **/
		CloseInstanceListWin();

		BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
		DeletePort(gadlibport);
		DeleteModule(thismod);
		closelibraries();
		exit(0);	/* This statement causes Enforcer Hit! */
		}
	ReplyMsg(event);
	}
}

/***********************************************************/

void BuildInstanceList()
{
struct Node *node;
struct StandardModState *s;
struct list *list;
long gadgethidden;

new_pointer(PTR_BUSY);
list = (struct list *)instancelist.gad_ptr;

gadgethidden=instancelist.gad_ptr->flags&HIDE_GADGET;
hide_gadget(instancelist.gad_ptr);
while (list->list_size > 0)
	delete_list_entry_by_index(list, 0);

if (!gadgethidden) {
	show_gadget(instancelist.gad_ptr);
	refresh_gadget(instancelist.gad_ptr);
	hide_gadget(instancelist.gad_ptr);
	}

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) {
   s=(struct StandardModState *)node;
	if (s->attached_module && (s->flags&MS_WINDOW_OPEN)) {
		/** Module's code loaded & running? **/ ;
		}
	else {
		if (CorrectStateType(s))
			add_list_entry(list, s->instance_name, s);
		}
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);

if (!gadgethidden) {
	show_gadget(instancelist.gad_ptr);
	refresh_gadget(instancelist.gad_ptr);
	}
new_pointer(PTR_NORMAL);
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;

while ((event=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	if (event->g==instancelist.gad_ptr) {
		FuncActivate(instancelist.gad_ptr,event->flags);
		}
	ReplyMsg(event);
	}
}

/***********************************************************/

void handle_intuievents()        
{
struct IntuiMessage *message;
USHORT code,menunum,itemnum,subnum;
ULONG class;
struct Gadget *g;
struct list *list;
struct StandardModState *state;

while ((message=(struct IntuiMessage *)GetMsg(thiswin->UserPort))!=0) {

	HandleIntuitMess(thiswin,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		CloseInstanceListWin(); 
		BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
		return;	/** KEEP THIS!! thiswin->UserPort is invalid. **/
		}			/** Can't continue in while loop **/

	else if (class==NEWSIZE) {
		RefreshWind(thiswin);
		new_gadget_size(instancelist.gad_ptr,10,
			15, thiswin->Width-11, thiswin->Height-5);
		refresh_gadget_chain(thiswin);
		}

	menunum=MENUNUM(code);
	itemnum=ITEMNUM(code);
	subnum=SUBNUM(code);

	if (class==MENUPICK && menunum==2) {
		switch (itemnum) {
			case 0:		/* edit */
				list = (struct list *)instancelist.gad_ptr;
				if (list->active_entry) {
					state=(struct StandardModState *)list->active_entry->user_data;
					EditInstance(state);
					}
				else 
					telluser("You must select an instance","in the list to edit.",0);
				break;

			case 1:		/* duplicate */
				list = (struct list *)instancelist.gad_ptr;
				if (list->active_entry) {
					state=(struct StandardModState *)list->active_entry->user_data;
					/** create uninitilized modstate ***/
					state=AddModState(state->state_size,state->module_name,NULL);
					/** Have mod's code initilize the state **/
					state->flags|=MS_NEED_MODULE;
					new_pointer(PTR_BUSY);
					LoadModule(state->module_name);
					new_pointer(PTR_NORMAL);
					}
				else { 
					telluser("You must select an instance","in the list to duplicate.",0);
					}
				break;

			case 2:		/* remove */
				list = (struct list *)instancelist.gad_ptr;
				if (list->active_entry) {
					state=(struct StandardModState *)list->active_entry->user_data;
					if (state->attached_module) {
						SendMsg(state->attached_module,se_cmd_kill_module,state->attached_module,0,0,0,0,0);
						while (state->attached_module)
							WaitTOF();
						}
					DeleteModState(state);
					}
				else 
					telluser("You must select an instance","in the list to edit.",0);
				break;

			case 3:		/** show Apps/Utility **/
				((struct InstanceModState *)thismod->state)->flags=0;
				if (SubItems[0].Flags&CHECKED)
					((struct InstanceModState *)thismod->state)->flags|=INSTANCE_SHOW_APPS;
				if (SubItems[1].Flags&CHECKED)
					((struct InstanceModState *)thismod->state)->flags|=INSTANCE_SHOW_UTILITIES;
				BuildInstanceList();
			break;
			}
		}
	}
}

/***********************************************************/

void FuncActivate(l,flags)
struct list *l;
unsigned long flags;	/** l->flags looses LIST_DOUBLESELECT! */
{
static struct StandardModState *last_state_clicked=NULL;
struct Module *find_module();
extern struct StudioBase *StudioBase;
struct StandardModState *s;
struct Module *mod;

if (flags&LIST_NEWSELECT) {
	last_state_clicked=NULL;
	}

if (flags&LIST_DOUBLESELECT) {
   s=(struct StandardModState *)l->active_entry->user_data;
	if (s!=last_state_clicked) {	/** Prevent accidental double select **/
		last_state_clicked=s;
		if (s->attached_module==NULL) {
			s->flags |= MS_STATE_INITILIZED;	/** See Studio16.h for comments **/
			s->flags|=MS_NEED_MODULE;	/** mark so module will pick it when launched **/
			new_pointer(PTR_BUSY);
			mod=(struct Module *)LoadModule(s->module_name);
			if (mod==NULL) { 
				new_pointer(PTR_NORMAL);
				telluser("Instance:","Can't load module",0);
				return;
				}
			new_pointer(PTR_NORMAL);
			}

		BroadcastEventParms(se_cmd_select_module,s->attached_module,0,0,0,0,0);
		last_state_clicked=NULL;
		}
   }
}

/** check to see if this states's flags matches utility_mode **/

CorrectStateType(state)
struct StandardModState *state;
{
if (state->flags&MS_UTILITY_MODULE)
	if (((struct InstanceModState *)thismod->state)->flags&INSTANCE_SHOW_UTILITIES)
		return(TRUE);
	else
		return(FALSE);
else
	if (((struct InstanceModState *)thismod->state)->flags&INSTANCE_SHOW_APPS)
		return(TRUE);
	else
		return(FALSE);
}

