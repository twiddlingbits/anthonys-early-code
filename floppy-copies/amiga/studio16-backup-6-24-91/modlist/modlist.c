#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos.h>
#include <stdio.h>
#include "/include/psound.h"

#include "modlist.pwc"

struct Window *mod_list_win;
struct Module *thismod;
struct MsgPort *gadlibport;

short se_info_module_added;
short se_info_module_deleted;
short se_cmd_kill_module;
short se_cmd_select_module;
short se_info_module_closed;

extern struct studioBase *studioBase;

void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
void InitModList();
void FuncActivate();
struct Window *OpenWindow();
void ToggleList();
void ReadDir();
void FuncLaunch();
void BuildResidentList();

static struct NewList disklist =
			{NULL,10,24,130,90,LIST_COOL|LIST_SINGLESELECT,0,0,0,0,0,0,0,0,0};

static struct NewList residentlist =
			{&disklist,140,24,265,90,LIST_COOL|LIST_SINGLESELECT,FuncActivate,0,0,0,0,0,0,0,0};

static struct NewButton unloadbutn = {
   &residentlist,145,95,210,109,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Unload"};

static struct NewButton button = {
   &unloadbutn,15,95,55,109,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Load"};

/***********************************************************/

void main()
{
unsigned long ie,se,ge,sigs;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModList();					/** open my window, add module, etc. **/

se = 1<<thismod->notifyme->mp_SigBit;
ge = 1<<gadlibport->mp_SigBit;

while (TRUE) {
	if (mod_list_win)
		ie = 1<<mod_list_win->UserPort->mp_SigBit;
	else
		ie = 0;
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

void InitModList()
{
static struct NewModule mod_list_data ={"ModList",0};

NewWindowStructure1.Screen=studioBase->screen;
mod_list_win=OpenWindow(&NewWindowStructure1);
if ((thismod=(struct Module *)AddModule(&mod_list_data))==0) {
   telluser("Can't add mod_list","");
	closelibraries();
   exit(10);
   }

SetAPen(mod_list_win->RPort, BROWN);
RectFill(mod_list_win->RPort, 2,11, mod_list_win->Width-3,mod_list_win->Height-2);

residentlist.c=&studioBase->defaults.colors;
disklist.c=&studioBase->defaults.colors;
button.c=&studioBase->defaults.colors;
unloadbutn.c=&studioBase->defaults.colors;

gadlibport=(struct MsgPort *)CreatePort(0,0);
button.notifyme=gadlibport;
unloadbutn.notifyme=gadlibport;
/*residentlist.notifyme=gadlibport;*/

create_gadget_chain(mod_list_win, &button);

/** place text at top of each list **/

SetAPen(mod_list_win->RPort, WHITE);
SetDrMd(mod_list_win->RPort, JAM1);
Move(mod_list_win->RPort,10,22);
Text(mod_list_win->RPort,"Disk",4);
Move(mod_list_win->RPort,140,22);
Text(mod_list_win->RPort,"Idle",4);

/*** Build disklist by scanning current directory ***/

ReadDir(disklist.gad_ptr,"#?");

/** Turn on the events we want to hear about **/

se_info_module_added		=GetEventID("SE_INFO_MODULE_ADDED");
se_info_module_deleted	=GetEventID("SE_INFO_MODULE_DELETED");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");

/** Build displayed list by searching for any loaded modules **/
/** There is a BUG here! We need to syncronize the below search with add modules **/

#if 0
ObtainSemaphore(&studioBase->lock);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	add_list_entry(residentlist.gad_ptr, mod->name, mod);
	}
ReleaseSemaphore(&studioBase->lock);
#endif

BuildResidentList();

NotifyMeOnEvent(se_info_module_added, thismod);
NotifyMeOnEvent(se_info_module_deleted, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_info_module_closed, thismod);

NotifyMeOnCount(se_cmd_kill_module, thismod);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Module *mod;


while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
	if (event->type==se_info_module_added) {
		mod = (struct Module *)event->arg1;
		add_list_entry(residentlist.gad_ptr, mod->name, mod);
		}

	else if (event->type==se_info_module_deleted) {
		delete_list_entry_by_userdata(residentlist.gad_ptr, event->arg1);
		}

	else if (event->type==se_info_module_closed) {
		mod = (struct Module *)event->arg1;
		add_list_entry(residentlist.gad_ptr, mod->name, mod);
		}

	else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
		ReplyMsg(event);
		DeleteModule(thismod);
		if (mod_list_win) {
			delete_gadget_chain(mod_list_win);  /** delete all gadgets in this window */
			CloseWindow(mod_list_win);
			}
		DeletePort(gadlibport);
		closelibraries();
		exit(0);
		}
	ReplyMsg(event);
	}
}

/***********************************************************/

void BuildResidentList()
{
struct Module *mod;
struct MinNode *node;

#if 0
if (residentlist.gad_ptr) {
	hide_gadget(residentlist.gad_ptr);
	delete_gadget(residentlist.gad_ptr);
	}

create_gadget(mod_list_win,&residentlist);
hide_gadget(residentlist.gad_ptr);
#endif

ObtainSemaphore(&studioBase->lock);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	add_list_entry(residentlist.gad_ptr, mod->name, mod);
	}
ReleaseSemaphore(&studioBase->lock);
#if 0
show_gadget(residentlist.gad_ptr);
refresh_gadget(residentlist.gad_ptr);
#endif
}
/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
char cmd[80];

while ((event=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	if (event->g==button.gad_ptr) {
		sprintf(cmd,"run %s",((struct list *)(disklist.gad_ptr))->active_entry->text);
		Execute(cmd,0,0);
		}

	if (event->g==unloadbutn.gad_ptr) {
		BroadcastEventParms(se_cmd_kill_module,((struct list *)(residentlist.gad_ptr))->active_entry->user_data,0,0,0,0,0);
		}

#if 0
	if (event->g==residentlist.gad_ptr) {
		FuncActivate(residentlist.gad_ptr,event->flags);
		}
#endif

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
struct MinNode *node;
struct Module *mod;

while ((message=(struct IntuiMessage *)GetMsg(mod_list_win->UserPort))!=0) {

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		ObtainSemaphore(&studioBase->lock);
		for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
		   mod=(struct Module *)node;
			SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
			}
		ReleaseSemaphore(&studioBase->lock);
		}
	}
}

/***********************************************************/

void FuncActivate(l)
struct list *l;
{
struct Module *find_module();
extern struct studioBase *studioBase;
struct Module *mod;

if (l->flags&LIST_DOUBLESELECT) {
   mod=(struct Module *)l->active_entry->user_data;
	BroadcastEventParms(se_cmd_select_module,mod,0,0,0,0,0);
	delete_list_entry_by_userdata(residentlist.gad_ptr, mod);
/*	SendMsg(mod,se_cmd_select_module,mod,0,0,0,0,0);*/
   }
}

/***********************************************************/

void ReadDir(l,path)
struct list *l;
char *path;
{
struct FILEINFO info;
int error;

error=dfind(&info,path,0);
if (error==0) {
/*	printf("adding %s\n",info.fib_FileName);	*/
	add_list_entry(l,info.fib_FileName,0);
	while (error==0) {
		error=dnext(&info);
		if (error==0) {
/*			printf("adding %s\n",info.fib_FileName);	*/
			add_list_entry(l,info.fib_FileName,0);
			}
		}
	}
}

