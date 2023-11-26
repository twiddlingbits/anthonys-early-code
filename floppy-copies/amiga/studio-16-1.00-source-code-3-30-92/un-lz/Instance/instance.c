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
short se_cmd_refresh_instance_list;
short se_info_module_added;
short se_cmd_kill_module;
short se_cmd_select_module;
short se_info_module_closed;
short se_info_module_opened;


extern struct StudioBase *StudioBase;

void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
void InitInstanceList();
void FuncActivate();
struct Window *OpenNewWind();
void ToggleList();
void ReadDir();
void FuncLaunch();
void BuildInstanceList();
void DoInstanceListWin();
void CloseInstanceListWin();
struct Window *InstancePrefsOpen();
void InstancePrefsClose();
struct StandardModState *AddModState();

static struct NewList instancelist =
			{NULL,10,16,0,0,LIST_COOL|LIST_SINGLESELECT|LIST_SORTED|LIST_USEKEYS,0,0,0,0,0,0,0,0,0};

#define save but[3]
#define erase but[2]
#define dup but[1]
#define edit but[0]
#define NUMGADS 4

static struct NewButton but[] = {
	{&dup,10,0,50,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Edit"},
	{&erase,55,0,87,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Duplicate"},
	{&save,92,0,148,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Remove"},
	{&instancelist,153,0,193,0,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Save"}};

/***********************************************************/

void _main()
{
unsigned long ie,se,ge,sigs;

openlibraries();
StandardEnviroment();		/** opens studio16 customscreen **/
InitInstanceList();					/** add module, etc. **/
DoInstanceListWin();				/** open my window **/

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
	if (sigs&ie)
		handle_intuievents();
	if (sigs&se)
		handle_studioevents();
	if (sigs&ge)
		handle_gadlibevents();
	}
}

/***********************************************************/

void InitInstanceList()
{
static struct NewModule windows_list_data ={"Instance",0,sizeof(struct InstanceModState),0};

if ((thismod=(struct Module *)AddModule(&windows_list_data))==0) {
   telluser("Can't add window_list","");
	closelibraries();
   exit(10);
   }

/** set up preferences **/

thismod->state->flags|=MS_CUSTOM_PREFS;
thismod->state->pref_name="Display Mode";
thismod->state->pref_display_func=InstancePrefsOpen;
thismod->state->pref_hide_func=InstancePrefsClose;

/** Initlize if required **/

if (!(thismod->state->flags&MS_STATE_INITILIZED))
	((struct InstanceModState *)thismod->state)->utility_mode = FALSE;


/** Turn on the events we want to hear about **/

se_info_state_added				=GetEventID("SE_INFO_STATE_ADDED");
se_info_state_deleted			=GetEventID("SE_INFO_STATE_DELETED");
se_cmd_refresh_instance_list	=GetEventID("SE_CMD_REFRESH_INSTANCE_LIST");
se_info_instance_name_changed	=GetEventID("SE_INFO_INSTANCE_NAME_CHANGED");
se_info_module_added				=GetEventID("SE_INFO_MODULE_ADDED");
se_cmd_kill_module				=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module				=GetEventID("SE_CMD_SELECT_MODULE");
se_info_module_closed			=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened			=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_info_state_added, thismod);
NotifyMeOnEvent(se_info_state_deleted, thismod);
NotifyMeOnEvent(se_cmd_refresh_instance_list, thismod);
NotifyMeOnEvent(se_info_instance_name_changed, thismod);
NotifyMeOnEvent(se_info_module_added, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_info_module_closed, thismod);
NotifyMeOnEvent(se_info_module_opened, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);

/** Gadget stuff that only needs doing once **/

instancelist.c=&StudioBase->defaults.colors;
edit.c=&StudioBase->defaults.colors;
dup.c=&StudioBase->defaults.colors;
erase.c=&StudioBase->defaults.colors;
save.c=&StudioBase->defaults.colors;

gadlibport=(struct MsgPort *)CreatePort(0,0);

instancelist.notifyme=gadlibport;
edit.notifyme=gadlibport;
dup.notifyme=gadlibport;
erase.notifyme=gadlibport;
save.notifyme=gadlibport;
}

/***********************************************************/


void DoInstanceListWin()
{
static short first_call=TRUE;
int x,w,s,i;

NewWindowStructure1.Screen=StudioBase->screen;

thiswin=OpenNewWind(&NewWindowStructure1,thismod->state,PREFERENCES,NULL);
if (thiswin==0) { 	/**!!!! ERROR!!!!! **/
	return;
	}

/** Create all our gadgets **/

if (first_call) {
	instancelist.box_x2=thiswin->Width-11;
	instancelist.box_y2=thiswin->Height-20;

	s=((thiswin->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		but[i].box_x1=(x>>8);
		but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
		but[i].box_y1=thiswin->Height-16;
		but[i].box_y2=thiswin->Height-6;
		}

	create_gadget_chain(thiswin, &edit);
	first_call=FALSE;

	/** Build displayed list by searching for any loaded modules **/
	/** There is a BUG here! We need to syncronize the below search with add modules **/

	BuildInstanceList();
	}
else {
	set_gadget_window(instancelist.gad_ptr,thiswin);
	show_gadget(instancelist.gad_ptr);
	refresh_gadget_chain(thiswin);
	}

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
/*sprintf(bug,"INS-SE#%d: ",event->type);
kprintf(bug);*/
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
/*kprintf("mod_closed\n");*/
		mod = (struct Module *)event->arg1;
		if (CorrectStateType(mod->state))
			add_list_entry(instancelist.gad_ptr, mod->state->instance_name, mod->state);
		if (!(mod->state->flags&MS_KEEP_RESIDENT)) {
			BroadcastEventParms(se_cmd_kill_module,mod,0,0,0,0,0);
			}
		}

	else if (event->type==se_cmd_refresh_instance_list) {
/*kprintf("refresh\n");*/
		BuildInstanceList();
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
		delete_gadget(instancelist.gad_ptr);
		instancelist.gad_ptr = NULL;	/** checked by CloseInstanceListWin() **/
		CloseInstanceListWin();
		BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
		DeletePort(gadlibport);
		DeleteModule(thismod);
		closelibraries();
		exit(0);
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

list = (struct list *)instancelist.gad_ptr;

hide_gadget(instancelist.gad_ptr);
while (list->list_size > 0)
	delete_list_entry_by_index(list, 0);

show_gadget(instancelist.gad_ptr);
refresh_gadget(instancelist.gad_ptr);
hide_gadget(instancelist.gad_ptr);

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

show_gadget(instancelist.gad_ptr);
refresh_gadget(instancelist.gad_ptr);
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
struct list *list;
struct StandardModState *state;

while ((event=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	if (event->g==instancelist.gad_ptr) {
		FuncActivate(instancelist.gad_ptr,event->flags);
		}
	else if (event->g==edit.gad_ptr) {
		list = (struct list *)instancelist.gad_ptr;
		if (list->active_entry) {
			state=(struct StandardModState *)list->active_entry->user_data;
			EditInstance(state);
			}
		else 
			telluser("You must select an instance","in the list to edit.",0);
		}
	else if (event->g==erase.gad_ptr) {
		list = (struct list *)instancelist.gad_ptr;
		if (list->active_entry) {
			state=(struct StandardModState *)list->active_entry->user_data;
			if (state->attached_module==NULL) {
				DeleteModState(state);
				}
			else {	
				if (state->flags&MS_WINDOW_OPEN && !(state->flags&MS_KEEP_RESIDENT))
					telluser("You must close this moudle's","window first.",0);
				else
					telluser("You must unload this module's","code first. (use modlist)",0);
				}
			}
		else 
			telluser("You must select an instance","in the list to edit.",0);
		}

	else if (event->g==dup.gad_ptr) {
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
		}

	else if (event->g==save.gad_ptr) {
	   new_pointer(PTR_BUSY);
		SaveModState(0);	/** save the instance list **/
	   new_pointer(PTR_NORMAL);
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
int x,w,s,i;

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
			16, thiswin->Width-11, thiswin->Height-20);


		s=((thiswin->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
		w=(s/NUMGADS);
		x=10<<8;

		for(i=0;i<NUMGADS;i++)
			{
			new_gadget_size(but[i].gad_ptr,(x>>8),thiswin->Height-16,(x+w)>>8,thiswin->Height-6);
			refresh_gadget(but[i].gad_ptr);
			x=x+w+(5<<8);
			}

		refresh_gadget_chain(thiswin);
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
/**!! BUG need to not wait on modules like diskio, etc. **/
			WaitUntilListening(se_cmd_select_module, mod);
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
if (((struct InstanceModState *)thismod->state)->utility_mode) {
	if (state->flags&MS_UTILITY_MODULE)
		return(TRUE);
	}
else {
	if (!(state->flags&MS_UTILITY_MODULE))
		return(TRUE);
	}
return(FALSE);
}

