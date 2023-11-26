#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "open_list.pwc"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_OPENLIST";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */

struct Window *open_list_win;
struct list *open_list;
char fname[40],fullpath[120];
struct Module *thismod;
struct MsgPort *gadlibport;

extern struct StudioBase *StudioBase;

void func_open_list();
void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
void InitOpenList();
void DoOpenList();
void DoCloseList();
struct Window *OpenNewWind();
struct Module *GetFreeEditor();

/** Gadget's used in this module **/
#define renamegd but[5]
#define close but[4]
#define open but[3]
#define save but[2]
#define edit but[1]
#define play but[0]
#define NUMGADS 6

static struct NewButton but[] = {
	{&edit   ,11, 90,53,100,BUTN_COOL,NULL,0,0,0,0,0,"Play"},
	{&save   ,60, 90,102,100,BUTN_COOL,NULL,0,0,0,0,0,"Edit"},
	{&open   ,111,90,153,100,BUTN_COOL,NULL,0,0,0,0,0,"Save"},
	{&close  ,160,90,202,100,BUTN_COOL,NULL,0,0,0,0,0,"Open"},
	{&renamegd ,210,90,259,100,BUTN_COOL,NULL,0,0,0,0,0,"Erase"},
	{NULL    ,210,90,259,100,BUTN_COOL,NULL,0,0,0,0,0,"Rename"}};

static struct NewList newopenlist = 
	{&play,10,16,260,86,LIST_COOL|LIST_SORTED,func_open_list,0,0,0,0,0,0};

/** events I'm interested in **/

short se_info_sample_created;
short se_info_sample_deleted;
short se_cmd_select_module;
short se_cmd_kill_module;	 
short se_cmd_edit_sample;
short se_cmd_play_sample;
short se_cmd_stop_all_playback;
short se_info_module_closed;
short se_info_module_opened;

void RefreshPlayStop();

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitOpenList();					/** initalize module **/

while (TRUE) {
	if (open_list_win)
		waitmask=(1<<open_list_win->UserPort->mp_SigBit) | (1<<thismod->notifyme->mp_SigBit);
	else
		waitmask=(1<<thismod->notifyme->mp_SigBit);

	if (gadlibport)
		waitmask|=(1<<gadlibport->mp_SigBit);
	
	mask=Wait(waitmask);

	if (open_list_win)
		if (mask&(1<<open_list_win->UserPort->mp_SigBit))
			handle_intuievents();
	if (mask & (1<<thismod->notifyme->mp_SigBit))
		handle_studioevents();
	if (gadlibport)
		if (mask & (1<<gadlibport->mp_SigBit))
			handle_gadlibevents();
	}
}

/***********************************************************/

void InitOpenList()
{
static struct NewModule open_list_data ={"OpenList",0,sizeof(struct StandardModState),0};

if ((thismod=(struct Module *)AddModule(&open_list_data))==0) {
	closelibraries();
	exit(10);
	}

fname[0]=0;

se_info_sample_created	=GetEventID("SE_INFO_SAMPLE_CREATED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");

NotifyMeOnEvent(se_info_sample_created, thismod);
NotifyMeOnEvent(se_info_sample_deleted, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_stop_all_playback, thismod);
}

/***********************************************************/

void DoOpenList()
{
struct MinNode *node;
struct Disk_Samp *samp;
int x,w,s,i;

NewWindowStructure1.Screen=StudioBase->screen;
open_list_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL, NULL,NULL);
if (open_list_win==NULL) {
	return;
	}

/** Let everyone else who cares know a new window is open **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);

gadlibport = (struct MsgPort *)CreatePort(0,0);

newopenlist.c=&StudioBase->defaults.colors;
open.c=&StudioBase->defaults.colors;
close.c=&StudioBase->defaults.colors;
save.c=&StudioBase->defaults.colors;
edit.c=&StudioBase->defaults.colors;
play.c=&StudioBase->defaults.colors;
renamegd.c=&StudioBase->defaults.colors;

open.notifyme=gadlibport;
close.notifyme=gadlibport;
save.notifyme=gadlibport;
edit.notifyme=gadlibport;
play.notifyme=gadlibport;
renamegd.notifyme=gadlibport;

s=((open_list_win->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
w=(s/NUMGADS);
x=10<<8;

for(i=0;i<NUMGADS;i++)
	{
	but[i].box_x1=(x>>8);
	but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
	but[i].box_y1=open_list_win->Height-16;
	but[i].box_y2=open_list_win->Height-6;
	}

newopenlist.box_x2=open_list_win->Width-11;
newopenlist.box_y2=open_list_win->Height-20;

create_gadget_chain(open_list_win, &newopenlist);
open_list = (struct list *)newopenlist.gad_ptr;
if (open_list==0) {
/*   kprintf("open_list!!!ERROR!!!!!"); */
	}

/** Build displayed sample list by searching StudioBase sample list **/
/** This can cause problems because it is entirly possible that **/
/** samples may have just been created (thus we have been sent **/
/** se_info_sample_created messages) but haven't processed them yet **/

ObtainSharedSemaphore(&StudioBase->lockstudio);

for (node=StudioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	samp=(struct Disk_Samp *)node;
	add_list_entry(open_list, samp->name, samp);
	}

ReleaseSharedSemaphore(&StudioBase->lockstudio);

/* printf("open_list.c: StudioBase size %d\n",sizeof(struct StudioBase)); */

RefreshPlayStop();
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Disk_Samp *samp;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_info_sample_created && open_list_win) {
			samp = (struct Disk_Samp *)event->arg1;
			ObtainSharedSemaphore(&StudioBase->lockstudio);
			if (GetSampleNum(samp)!=-1) /** make sure sample still exists!! (IMPORTANT) **/
				if (find_list_entry_by_userdata(open_list,samp)==NULL) /** make sure not already in list! (IMPORTANT) **/
					add_list_entry(open_list, samp->name, samp);
			ReleaseSharedSemaphore(&StudioBase->lockstudio);
			}

		else if (event->type==se_info_sample_deleted && open_list_win) {
			delete_list_entry_by_userdata(open_list, event->arg1);
			}

		else if (event->type==se_cmd_select_module) {
			if ((struct Module *)event->arg1==thismod) {
				if (open_list_win==0) {
					DoOpenList();
					}
				}
			}

		else if (event->type==se_cmd_kill_module && (struct Module *)event->arg1==thismod) {
			ReplyMsg(event);		/** need because of exit() **/
			DoCloseList();
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
int x,w,s,i;

while ((message=(struct IntuiMessage *)GetMsg(open_list_win->UserPort))!=0) {
	HandleIntuitMess(open_list_win,message);

	class=message->Class;
	code=message->Code;
	g=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class==CLOSEWINDOW) {
		DoCloseList();
		return;
		}
	if (class==NEWSIZE) 
		{
		RefreshWind(open_list_win);
		new_gadget_size(newopenlist.gad_ptr,10,
			16, open_list_win->Width-11, open_list_win->Height-20);

		s=((open_list_win->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
		w=(s/NUMGADS);
		x=10<<8;

		for(i=0;i<NUMGADS;i++)
			{
			new_gadget_size(but[i].gad_ptr,(x>>8),open_list_win->Height-16,(x+w)>>8,open_list_win->Height-6);
			refresh_gadget(but[i].gad_ptr);
			x=x+w+(5<<8);
			}

		refresh_gadget_chain(open_list_win);
		}
	}
}

/***********************************************************/

void DoCloseList()
{
if (open_list_win) {
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	hide_gadget(newopenlist.gad_ptr);
	delete_gadget_chain(open_list_win);  /** delete all gadgets in this window */

	DeletePort(gadlibport);
	gadlibport=0;

	CloseWind(open_list_win);
	open_list_win=0;
	}
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *msg;
struct GadgetHeader *g;
struct Disk_Samp *samp,*samp2;
int i;
struct list_entry *le;
struct MinNode *node;
short spf;
char OldSampName[100],NewSampName[100],path1[200],path2[200];
char NewTitle[100];
struct Module *modptr;
while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	g = msg->g;
	ReplyMsg(msg);

	if (g==edit.gad_ptr) {
		for (node=open_list->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
			if (((struct list_entry *)node)->flags & LE_SELECTED) {
				if (modptr=GetFreeEditor()) 
					SendMsg(modptr,se_cmd_edit_sample,((struct list_entry *)node)->user_data,0,0,0,0,0);
				((struct list_entry *)node)->flags &= (~LE_SELECTED);
				}
			}
		refresh_list_text(open_list);
		RefreshPlayStop();
		}

	if (g==save.gad_ptr) {
		node=open_list->entries.mlh_Head;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				samp = (struct Disk_Samp *)le->user_data;
				new_pointer(PTR_BUSY);
				i=save_samp(open_list_win,StudioBase->defaults.samp_io_path,samp->name,samp);
				show_error(i);
				new_pointer(PTR_NORMAL);
				}
			}
		}


	if (g==open.gad_ptr) {
		new_pointer(PTR_BUSY);
		i=load_samp(open_list_win,StudioBase->defaults.samp_io_path);
		show_error(i);
		new_pointer(PTR_NORMAL);
		}

	if (g==play.gad_ptr) {
		node=open_list->entries.mlh_Head;
		spf=TRUE;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				spf=FALSE;
				samp = (struct Disk_Samp *)le->user_data;
				BroadcastEventParms(se_cmd_play_sample,samp,0,samp->length,0,0,0);
				}
			}
		for (node=open_list->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ)
			((struct list_entry *)node)->flags &= (~LE_SELECTED);
		refresh_list_text(open_list);
		if (spf) {	
			BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
			}
		RefreshPlayStop();
		}

	if (g==close.gad_ptr) {
		node=open_list->entries.mlh_Head;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				if (askuser("OK to remove sample:",le->text,0)) {
					new_pointer(PTR_BUSY);
					if (!DeleteWorkingFile(le->user_data)) {
						telluser("OPEN_LIST:","Sample in use! Can't Delete it",0);
						}
					else {
						DeleteGraphFile(((struct Disk_Samp *)le->user_data)->name);
						DeleteSample(le->user_data);
						}
					new_pointer(PTR_NORMAL);
					}
				}
			}
		}

	if (g==renamegd.gad_ptr) 
		{
		node=open_list->entries.mlh_Head;
		while (node->mln_Succ) 
			{
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) 
				{
				samp = (struct Disk_Samp *)le->user_data;
				sprintf(NewTitle,"Rename %s to?",samp->name);
				strcpy(NewSampName,samp->name);
				newgetstr(NewSampName,NewTitle,8*40);
				if(NewSampName[0])
				samp2=(struct DiskSamp *)FindSample(NewSampName,-1);
				if (samp2==NULL || (!stricmp(samp->name,samp2->name)))
					{
					if (samp2==NULL)
						{
						strcpy(path1,StudioBase->defaults.temp_files_path);add(path1,samp->name);strcat(path1,".graph");
						strcpy(path2,StudioBase->defaults.temp_files_path);add(path2,NewSampName);strcat(path2,".graph");
						rename(path1,path2);
						}
					strcpy(OldSampName,samp->name);
					if(QuickSaveSamp(StudioBase->defaults.temp_files_path,"__TEMP_OR_ARY__",samp)) telluser("Rename Error:","Error closing sample",NULL);
					else if(QuickLoadSamp(StudioBase->defaults.temp_files_path,"__TEMP_OR_ARY__",NewSampName)) 
						{telluser("Rename Error:","Returning Sample to Original Name",NULL);QuickLoadSamp(StudioBase->defaults.temp_files_path,"__TEMP_OR_ARY__",OldSampName);}
					}
				else telluser("Can Not Rename Sample,","Name already in use",NULL);
				}
			}
		}
	}
}

/***********************************************************/

void func_open_list(l)
struct list *l;
{
struct Module *find_module();
extern struct StudioBase *StudioBase;
/*struct Module *m;*/
void RefreshPlayStop();

if (l->flags&LIST_NEWSELECT) {
	RefreshPlayStop();
	}

}

/************************************************/

void RefreshPlayStop()
{
struct MinNode *node;
struct list_entry *le;
short spf=TRUE;
static short ospf=FALSE;

if (play.gad_ptr)
	{
	node=open_list->entries.mlh_Head;
	while (node->mln_Succ && spf==TRUE)
		{
		le=(struct list_entry *)node;
		node = node->mln_Succ;
		if (le->flags&LE_SELECTED) spf=FALSE;
		}

	if (spf!=ospf)
		{
		if (spf==FALSE) {movmem(&"Play",((struct button *)play.gad_ptr)->text,4);}
		else {movmem(&"Stop",((struct button *)play.gad_ptr)->text,4);}
 		refresh_gadget(play.gad_ptr);
		}
	ospf=spf;
	}
}

/***********************************************************/

int DeleteGraphFile(name)
char *name;
{
char temp[40];
char buf[110];
int size=sizeof(buf);

sprintf(temp,"%s.graph",name);

if (strlen(temp)+strlen(StudioBase->defaults.temp_files_path) > (size-3)) {
	return(OUT_OF_MEMORY);
	}

strcpy(buf,StudioBase->defaults.temp_files_path);
add(buf,temp);	 /** append filename to path **/

DeleteFile(buf);
return(NO_ERROR);
}

/***********************************************************/

struct Module *GetFreeEditor()
{
struct Module *LoadModule();
extern struct StudioBase *StudioBase;
struct Module *m;
struct MinNode *node;

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   m=(struct Module *)node;
   if (m->name)
      if (strnicmp(m->name,"Edit",4)==0) {
			if (!(((struct EditState *)(m->state))->es_flags&ES_IN_USE)) {
				((struct EditState *)(m->state))->es_flags|=ES_IN_USE;
				ReleaseSharedSemaphore(&StudioBase->lockstudio);
   	      return(m);
				}
			}
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
m=LoadModule("Edit");
if (m)
	((struct EditState *)(m->state))->es_flags|=ES_IN_USE;
return(m);
}
