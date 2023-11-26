#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos.h>
#include <stdio.h>
#include "/include/psound.h"

#include "modlist.pwc"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_MODLIST";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *mod_list_win;
struct Module *thismod;
struct MsgPort *gadlibport;

short se_info_module_added;
short se_info_module_deleted;
short se_cmd_kill_module;
short se_cmd_select_module;
short se_info_module_closed;
short se_info_module_opened;

extern struct StudioBase *StudioBase;

void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
void InitModList();
void FuncActivate();
struct Window *OpenNewWind();
void ToggleList();
void ReadDir();
void FuncLaunch();
void BuildResidentList();
void RenderWindowText();
void DoModListWin();
void CloseModListWin();
void SetGadgetSize();

static struct NewList disklist =
			{NULL,10,24,130,90,LIST_COOL|LIST_SINGLESELECT|LIST_SORTED|HIDE_GADGET,0,0,0,0,0,0,0,0,0};

static struct NewList residentlist =
			{&disklist,140,24,265,90,LIST_COOL|LIST_SINGLESELECT|LIST_SORTED|HIDE_GADGET,0,0,0,0,0,0,0,0,0};

/***********************************************************/

void _main()
{
unsigned long ie,se,ge,sigs;

openlibraries();
StandardEnviroment();		/** opens studio16 customscreen **/
InitModList();					/** add module, etc. **/

se = 1<<thismod->notifyme->mp_SigBit;
ge = 1<<gadlibport->mp_SigBit;

while (TRUE) {
	if (mod_list_win) {
		ie = 1<<mod_list_win->UserPort->mp_SigBit;
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

void InitModList()
{
static struct NewModule mod_list_data ={"ModList",0,sizeof(struct StandardModState)};

if ((thismod=(struct Module *)AddModule(&mod_list_data))==0) {
   telluser("Can't add ModList","",0);
	closelibraries();
   exit(10);
   }

/** Turn on the events we want to hear about **/

se_info_module_added		=GetEventID("SE_INFO_MODULE_ADDED");
se_info_module_deleted	=GetEventID("SE_INFO_MODULE_DELETED");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_info_module_added, thismod);
NotifyMeOnEvent(se_info_module_deleted, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_info_module_opened, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);

/*NotifyMeOnCount(se_cmd_kill_module, thismod);*/

/** Gadget stuff that only needs doing once **/

residentlist.c=&StudioBase->defaults.colors;
disklist.c=&StudioBase->defaults.colors;

gadlibport=(struct MsgPort *)CreatePort(0,0);

residentlist.notifyme=gadlibport;
disklist.notifyme=gadlibport;

/** Create gadgets hidden (mod_list_win is null) **/

create_gadget_chain(mod_list_win, &residentlist);

/*** Build disklist by scanning current directory ***/

ReadDir(disklist.gad_ptr,"Studio16:runtime/");

/** Build displayed list by searching for any loaded modules **/

BuildResidentList();
}

/***********************************************************/


void DoModListWin()
{
NewWindowStructure1.Screen=StudioBase->screen;

mod_list_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL,NULL,NULL);
if (mod_list_win==0) { 	
	return;
	}

/** Create all our gadgets **/

set_gadget_window(disklist.gad_ptr,mod_list_win);
set_gadget_window(residentlist.gad_ptr,mod_list_win);

SetGadgetSize();	/** use window size to determin list sizes **/

show_gadget(disklist.gad_ptr);
show_gadget(residentlist.gad_ptr);

refresh_gadget_chain(mod_list_win);

/** place text at top of each list **/

RenderWindowText();

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);
}
/***********************************************************/

void CloseModListWin()
{
if (mod_list_win) {
	if (disklist.gad_ptr) {
		hide_gadget(disklist.gad_ptr);
		hide_gadget(residentlist.gad_ptr);
		set_gadget_window(disklist.gad_ptr,0);	/** IMPORTANT!! **/
		set_gadget_window(residentlist.gad_ptr,0);
		}
	CloseWind(mod_list_win);
	mod_list_win=0;
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0);
	}
}

/***********************************************************/

void RenderWindowText()

{
if (mod_list_win) {
	LockGadLibRender();
	SetDrMd(mod_list_win->RPort, JAM1);
	SetAPen(mod_list_win->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(mod_list_win->RPort,disklist.gad_ptr->box_x1+4,21);
	Text(mod_list_win->RPort,"Disk",4);
	Move(mod_list_win->RPort,residentlist.gad_ptr->box_x1+4,21);
	Text(mod_list_win->RPort,"Resident",8);

	SetAPen(mod_list_win->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(mod_list_win->RPort,disklist.gad_ptr->box_x1+2,20);
	Text(mod_list_win->RPort,"Disk",4);
	Move(mod_list_win->RPort,residentlist.gad_ptr->box_x1+2,20);
	Text(mod_list_win->RPort,"Resident",8);
	UnLockGadLibRender();
	}
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Module *mod;
struct list_entry *le;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
	if (event->type==se_info_module_added) {
		mod = (struct Module *)event->arg1;
		if (find_list_entry_by_userdata(residentlist.gad_ptr, mod)==NULL)
			add_list_entry(residentlist.gad_ptr, mod->name, mod);
		}

	else if (event->type==se_info_module_deleted) {
		le=(struct list_entry *)find_list_entry_by_userdata(residentlist.gad_ptr, event->arg1);
		delete_list_entry_by_ptr(residentlist.gad_ptr, le);
		}

	else if (event->type==se_cmd_select_module) {
		if ((struct Module *)event->arg1==thismod) {
			if (mod_list_win==0) {
				DoModListWin();
				}
			}
		}

	else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
		ReplyMsg(event);
		hide_gadget(disklist.gad_ptr);		/** for quick deleting */
		hide_gadget(residentlist.gad_ptr);
		delete_gadget(disklist.gad_ptr);
		delete_gadget(residentlist.gad_ptr);
		disklist.gad_ptr = NULL;	/** checked by CloseModListWin() **/
		CloseModListWin();
		DeletePort(gadlibport);
		DeleteModule(thismod);
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

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	add_list_entry(residentlist.gad_ptr, mod->name, mod);
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;

while ((event=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	if (event->g==disklist.gad_ptr && event->flags&LIST_DOUBLESELECT) {
		new_pointer(PTR_BUSY);
		LoadModule(((struct list *)(disklist.gad_ptr))->active_entry->text);
		new_pointer(PTR_NORMAL);
		}

	if (event->g==residentlist.gad_ptr && event->flags&LIST_DOUBLESELECT) {
		BroadcastEventParms(se_cmd_kill_module,((struct list *)(residentlist.gad_ptr))->active_entry->user_data,0,0,0,0,0);
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

while ((message=(struct IntuiMessage *)GetMsg(mod_list_win->UserPort))!=0) {
	HandleIntuitMess(mod_list_win,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		CloseModListWin(); 
		return;	/** KEEP THIS!! mod_list_win->UserPort is invalid. **/
		}			/** Can't continue in while loop **/

	else if (class==NEWSIZE) {
		RefreshWind(mod_list_win);
		SetGadgetSize();	
		refresh_gadget_chain(mod_list_win);
		RenderWindowText();
		}
	}
}
/***********************************************************/

void SetGadgetSize()
{
short middle;

middle = mod_list_win->Width/2;

new_gadget_size(disklist.gad_ptr,10,23,
	middle-4, mod_list_win->Height-6);
new_gadget_size(residentlist.gad_ptr,middle+4,23,
	mod_list_win->Width-11, mod_list_win->Height-6);
}

/***********************************************************/

void FuncActivate(l)
struct list *l;
{
struct Module *find_module();
extern struct StudioBase *StudioBase;
struct Module *mod;

if (l->flags&LIST_DOUBLESELECT) {
   mod=(struct Module *)l->active_entry->user_data;
	BroadcastEventParms(se_cmd_select_module,mod,0,0,0,0,0);
/*	SendMsg(mod,se_cmd_select_module,mod,0,0,0,0,0);*/
   }
}

/***********************************************************/

void ReadDir(l,path)
struct list *l;
char *path;
{
struct FileLock      *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;

if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) 
	{
	telluser("ModList:","Studio16:runtime directory does not exist.",0);
	return;
	}

if ( ! Examine(fkey, dinfo) ) 
	{
	telluser("ModList:","Disk Error (reading module names).",0);
	UnLock(fkey);
	return;
	}

if ( dinfo->fib_DirEntryType < 0L ) 
	{
	telluser("ModList:","Path can not include File Name.",0);
	UnLock(fkey);
	return;
	}

while( ExNext( fkey, dinfo ) )
	{
	if (dinfo->fib_EntryType<0) 
		{
		if (!dotinfo(dinfo->fib_FileName))
			add_list_entry(l,dinfo->fib_FileName,0);
		}
	}

UnLock(fkey);


/*  ##################### REMOVE ME #########################
struct FILEINFO __aligned info;	*/ /** force longword alligned **/
/*int error;
kprintf("###\n");
error=dfind(&info,path,0);
while (error==0) {
	if (!dotinfo(info.fib_FileName))
		add_list_entry(l,info.fib_FileName,0);
	error=dnext(&info);
	}*/
}

int dotinfo(str)
char *str;
{
short len;

len = strlen(str);
if (len < 5)
	return(FALSE);

if (strcmp(str+len-5,".info")==0)
	return(TRUE);
else
	return(FALSE);
}
