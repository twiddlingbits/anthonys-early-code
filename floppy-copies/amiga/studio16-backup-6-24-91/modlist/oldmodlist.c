#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <dos.h>
#include <stdio.h>
#include "/include/psound.h"

#include "modlist.pwc"

struct Window *mod_list_win;
struct list *mod_list;
struct Module *thismod;
struct looplist *ll;
struct MsgPort *gadlibport;

short se_module_added;
short se_module_deleted;
short se_kill_module;
short se_module_selected;


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

static char *entries[] = {"In Memory","On Disk",""};
static unsigned long eud[] = {0,1};

static struct NewList disklist =
			{NULL,10,14,260,80,LIST_COOL|LIST_SINGLESELECT|HIDE_GADGET,0,0,0,0,0,0,0,0,0};

static struct NewList residentlist =
			{&disklist,10,14,260,80,LIST_COOL|LIST_SINGLESELECT,FuncActivate,0,0,0,0,0,0,0,0};

static struct NewLoopList newlooplist = {
   &residentlist,10,85,104,99,LOOPLIST_COOL,ToggleList,0,0,0,0,0, entries, eud,0};

static struct NewButton button = {
   &newlooplist,108,85,150,99,BUTN_COOL|MANUAL_XOR,0,0,0,0,0,0,"Load"};

/***********************************************************/

void main()
{
unsigned long ie,se,ge,sigs;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModList();					/** open my window, add module, etc. **/

ie = 1<<mod_list_win->UserPort->mp_SigBit;
se = 1<<thismod->notifyme->mp_SigBit;
ge = 1<<gadlibport->mp_SigBit;

while (TRUE) {
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
struct MinNode *node;
struct Module *mod;

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
newlooplist.c=&studioBase->defaults.colors;
button.c=&studioBase->defaults.colors;
gadlibport=(struct MsgPort *)CreatePort(0,0);
button.notifyme=gadlibport;
create_gadget_chain(mod_list_win, &button);
mod_list = (struct list *)residentlist.gad_ptr;
ll	= (struct looplist *)newlooplist.gad_ptr;
if (mod_list==0) {
   printf("mod_list!!!ERROR!!!!!");
   }

/*** Build disklist by scanning current directory ***/

ReadDir(disklist.gad_ptr,"#?");

/** Turn on the events we want to hear about **/

se_module_added=GetEventID("SE_MODULE_ADDED");
se_module_deleted=GetEventID("SE_MODULE_DELETED");
se_kill_module=GetEventID("SE_KILL_MODULE");
se_module_selected=GetEventID("SE_MODULE_SELECTED");

/** Build displayed list by searching for any loaded modules **/
/** There is a BUG here! We need to syncronize the below search with add modules **/

for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	add_list_entry(mod_list, mod->name, mod);
	}

NotifyMeOnEvent(se_module_added, thismod);
NotifyMeOnEvent(se_module_deleted, thismod);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Module *mod;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
	if (event->type==se_module_added) {
		mod = (struct Module *)event->arg1;
		add_list_entry(mod_list, mod->name, mod);
		}
	else if (event->type==se_module_deleted) {
		delete_list_entry(mod_list, event->arg1);
		}
	ReplyMsg(event);
	}
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *event;
char cmd[80];

while ((event=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	if (event->g==button.gad_ptr) {
		sprintf(cmd,"run %s",((struct list *)(disklist.gad_ptr))->active_entry->text);
		printf("trying '%s'\n",cmd);		
		Execute(cmd,0,0);
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

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		BroadcastEventParms(se_kill_module,0,0,0,0,0,0);
		DeleteModule(thismod);
		delete_gadget_chain(mod_list_win);  /** delete all gadgets in this window */
		CloseWindow(mod_list_win);
		closelibraries();
		exit(0);
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
	BroadcastEventParms(se_module_selected,mod,0,0,0,0,0);
   }
}

/***********************************************************/

void ToggleList(b)
struct button *b;
{
if (ll->l->current==0) {		/** "IN MEMMORY" **/
	hide_gadget(disklist.gad_ptr); 
	show_gadget(residentlist.gad_ptr);
	refresh_gadget(residentlist.gad_ptr);
	}
else {
	hide_gadget(residentlist.gad_ptr);
	show_gadget(disklist.gad_ptr);
	refresh_gadget(disklist.gad_ptr);
	}
}

/***********************************************************/

void ReadDir(l,path)
struct list *l;
char *path;
{
struct FILEINFO info;
int error;

printf("readdir called\n");

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

