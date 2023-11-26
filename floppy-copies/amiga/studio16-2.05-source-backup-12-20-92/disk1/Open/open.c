#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/gadlib.h"
#include "/include/studio16.h"
#include "/include/interface.h"
#include "/include/soundedit.h"
#include "open.h"

#include "open.pwc"

/**** CBACK.o defines *****/

long _stack = CBACK_STACK;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_OPENLIST";	/* The name of the task to create         */
long _priority = 0;					/* The priority to run us at              */
long _BackGroundIO = 0;				/* Flag to tell it we want to do I/O      */

struct Window *open_list_win;
struct Module *thismod;
struct Module *edit_mod=0;
static int count;
struct MsgPort *gadlibport;
char *draw_region_text;

extern struct StudioBase *StudioBase;
extern struct MenuItem Items[];

void DoGhost();
void VerifyWidth();
void CheckShowRegions();
void DrawRegionText();
void UpdateRegionList();
void handle_studioevents();
void handle_intuievents();
void InitOpenList();
void DoOpenList();
void DoCloseList();
void add_dir();
void BuildOLPaths();
void UpdateSampPaths();
void ProcessSamples();
void StopAllPlayback();
void AddNewPath();
void RemovePath();
void RefreshOLWin();
void handle_gadlibevents();
struct Window *OpenNewWind();
struct Module *GetFreeEditor();
char *StrFileName();
char *StrLastDir();
int FindOLPath();
void PlaySamples();
int PrePlay();
int EditSample();
int DiscardSample();
int MyRenSample();
int ConvertSample();
int Counter();
void ClearEntries();
void UpdatePaths();

/** Gadget's used in this module **/

struct NewList newregionlist =
	{NULL,1,1,1,1,LIST_COOL|LIST_SORTED|MANUAL_XOR|HIDE_GADGET|LIST_DRAGABLE|TEXT_ONLY_REGION_NAME|LIST_SINGLESELECT,NULL,0,0,0,0,0,0};

struct NewList defaultnewlist=
	{NULL,1,1,1,1,LIST_COOL|LIST_SORTED|MANUAL_XOR|HIDE_GADGET|LIST_DRAGABLE|TEXT_ONLY_FILE_NAME,NULL,0,0,0,0,0,0};

static struct NewButton defaultnewbutton = /** 20 X 8 **/
	{NULL,1,1,1,1,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE|HIDE_GADGET,
	0,0,0,1,0,0,0};

struct OLPath olpath[OL_MAX_PATHS];

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
short se_info_sample_path_changed;

/***********************************************************/

void _main()
{
unsigned long mask;
unsigned long waitmask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitOpenList();					/** initalize module **/

while (TRUE) {
	waitmask=(1<<thismod->notifyme->mp_SigBit);
	if (open_list_win)
		waitmask |=(1<<open_list_win->UserPort->mp_SigBit);
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
static struct NewModule open_list_data ={"OpenList",0,sizeof(struct OpenListModState),0};
short i;
char *path;

if ((thismod=(struct Module *)AddModule(&open_list_data))==0) {
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	closelibraries();
	exit(10);
	}

se_info_sample_created	=GetEventID("SE_INFO_SAMPLE_CREATED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_select_module		=GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_edit_sample		=GetEventID("SE_CMD_EDIT_SAMPLE");
se_cmd_play_sample		=GetEventID("SE_CMD_PLAY_SAMPLE");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_info_module_closed	=GetEventID("SE_INFO_MODULE_CLOSED");
se_info_module_opened	=GetEventID("SE_INFO_MODULE_OPENED");
se_info_sample_path_changed	= GetEventID("SE_INFO_SAMPLE_PATH_CHANGED");

NotifyMeOnEvent(se_info_sample_created, thismod);
NotifyMeOnEvent(se_info_sample_deleted, thismod);
NotifyMeOnEvent(se_cmd_select_module, thismod);
NotifyMeOnEvent(se_cmd_kill_module, thismod);

/** Initilitze OLPaths **/

gadlibport = (struct MsgPort *)CreatePort(0,0);
defaultnewbutton.notifyme=gadlibport;
defaultnewlist.notifyme=gadlibport;

for (i=0; i < OL_MAX_PATHS; i++) {
	olpath[i].state=OL_EMPTY;
	olpath[i].newlist=defaultnewlist;
	olpath[i].newbut=defaultnewbutton;
	olpath[i].newlist.c=&StudioBase->defaults.colors;
	olpath[i].newbut.c=&StudioBase->defaults.colors;
	olpath[i].newlist.user_data=(APTR)i;
	olpath[i].newbut.user_data=(APTR)i;
	}

/** Copy StudioBase.defaults.samp_paths to our OLPath **/

i=0;
for (path=StudioBase->defaults.samp_paths; *path; path=(char *)strchr(path, 0)+1) {
	olpath[i].state=OL_NEEDSFILLING;
	strcpy(olpath[i++].path, path);
	}

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), thismod, 0,0,0,0,0);
}

/***********************************************************/

void DoOpenList()
{
int i,cnt;
extern struct Menu Titles[];

cnt=AllListsCount();
if (NewWindowStructure1.Width < (cnt*OL_LISTWID+30)) {
	NewWindowStructure1.Width=cnt*OL_LISTWID+30;
	}
NewWindowStructure1.MinWidth=cnt*OL_LISTWID+30;
NewWindowStructure1.Screen=StudioBase->screen;
open_list_win=OpenNewWind(&NewWindowStructure1,thismod ? thismod->state : NULL, NULL,Titles);
if (open_list_win==NULL) {
	return;
	}

/** Let everyone else who cares know a new window is open **/

BroadcastEventParms(se_info_module_opened, thismod, 0,0,0,0,0);

/** Initlized list gadgets **/

for (i=0; i < OL_MAX_PATHS; i++) {
	create_gadget(open_list_win, &olpath[i].newlist);
	create_gadget(open_list_win, &olpath[i].newbut);
	}

newregionlist.c=&StudioBase->defaults.colors;
create_gadget(open_list_win, &newregionlist);

/******* Set any CHECKMARKS needed *******/

if (((struct OpenListModState *)thismod->state)->flags&OL_SHOW_REGIONS) {
	show_gadget(newregionlist.gad_ptr);  /** RefreshOLWin will draw it **/
	Items[11].Flags|=CHECKED;
	}
else {
	Items[11].Flags&=(~CHECKED);
	}

/** Ghost/Unghost menus as appropriate **/

DoGhost();

/** Read directories into lists and draw them **/

BuildOLPaths();
RefreshOLWin();
}

/***********************************************************/

void DoCloseList()
{
short i;

if (open_list_win) {
	BroadcastEventParms(se_info_module_closed, thismod, 0,0,0,0,0); 
	for (i=0; i < OL_MAX_PATHS; i++)
		hide_gadget(olpath[i].newlist.gad_ptr);
	delete_gadget_chain(open_list_win);  /** delete all gadgets in this window */

	CloseWind(open_list_win);
	open_list_win=0;
	}
}

/***********************************************************/

void BuildOLPaths()
{
short i,first,needx;

/** First, fill in any lists that need filling **/

first=-1;
needx=TRUE;
for (i=0; i < OL_MAX_PATHS; i++) {
	if (olpath[i].state==OL_NEEDSFILLING) {
		olpath[i].state=OL_READY;
		add_dir(olpath[i].newlist.gad_ptr, olpath[i].path, i);
		}
	if (olpath[i].state!=OL_EMPTY) {
		if (first==-1)
			first=i;
		if (olpath[i].newbut.gad_ptr->flags&BUTN_HIT)
			needx=FALSE;
		}
	}

/** If no X gadget hit, set the first one **/

if (needx)
	xor_button(olpath[first].newbut.gad_ptr);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
char *path;
struct list *l;
int i;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {

		if (event->type==se_info_sample_created && open_list_win) {
			path=(char *)event->arg1;
			i=FindOLPath(path);
			if (i>=0) {
				l=(struct list *)(olpath[i].newlist.gad_ptr);
				if (l)
					if (find_list_entry_by_text(l,path)==NULL) /** make sure not already in list! (IMPORTANT) **/
							add_list_entry(l, path, i);
				}
			}

		else if (event->type==se_info_sample_deleted && open_list_win) {
			i=FindOLPath(event->arg1);
			if (i>=0) {
				l=(struct list *)(olpath[i].newlist.gad_ptr);
				if (l)
					delete_list_entry_by_text(l, event->arg1);
				}
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
			DeletePort(gadlibport);
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
char path[STUDIO16_MAX_PATH_SIZE];
USHORT code,menunum,itemnum;
ULONG class;
struct IntuiMessage *message;
struct Gadget *g;

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
		BuildOLPaths();
		RefreshOLWin();
		}

	menunum=MENUNUM(code);
	itemnum=ITEMNUM(code);

	if (class==MENUPICK && menunum==2) {
		switch (itemnum) {
			case 0:		/** Play Sample **/
				ProcessSamples(PrePlay);
				PlaySamples();
				break;
			case 1:		/** Stop Playing **/
				StopAllPlayback();
				break;
			case 2:		/** Rename Sample **/
				ProcessSamples(MyRenSample);
				break;
			case 3:		/** Delete **/
				count=0;
				ProcessSamples(Counter);
				if (count) {
					sprintf(path,"OK to remove %d samples?",count);
					if (askuser(path,0,0))
						ProcessSamples(DiscardSample);
					}
				break;
			case 4:		/** Convert **/
				ProcessSamples(ConvertSample);
				break;
			case 5:		/** Edit **/
				edit_mod=GetFreeEditor();
				ProcessSamples(EditSample);
				edit_mod=NULL;
				break;
			case 7:		/** Add New Path **/
				AddNewPath();
				break;
			case 8:		/** Remove Path **/
				RemovePath();
				break;
			case 9:		/** Update Paths **/
				UpdatePaths();
				break;
			case 11:		/** Show Regions **/
				CheckShowRegions();
				break;
			}
		}
	}
}

/***********************************************************/

void handle_gadlibevents()
{
struct GadMsg *msg;
struct GadgetHeader *g;

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) {
	g = msg->g;
	switch(msg->type) {
			case GADMSG_DRAG_AND_DROP:
				if (msg->from!=g)
					if (IsOpenListGad(msg->from))
						if (RenameSample(msg->text,0,olpath[(int)g->user_data].path,StrFileName(msg->text))!=NO_ERROR)
							DoMoveSample(msg->text,0,olpath[(int)g->user_data].path,StrFileName(msg->text));
				break;
			case GADMSG_ACTIVE:
				if (g->flags&GAD_BUTTON)
					UpdateSampPaths();	/** X button changed **/
				else {	/** must be list **/
					if (!msg->shift_down)  /** clear all list entries! **/
						ClearEntries(g);
					if (((struct OpenListModState *)thismod->state)->flags&OL_SHOW_REGIONS)
						if (((struct list*)g)->active_entry)
							UpdateRegionList(((struct list*)g)->active_entry->text);
						else
							UpdateRegionList(0);
					}
				break;
		}
	ReplyMsg(msg);
	}
DoGhost();
}

/***********************************************************/

IsOpenListGad(g)
struct GadgetHeader *g;
{
short k;

for (k=0; k < OL_MAX_PATHS; k++)
	if (olpath[k].state!=OL_EMPTY)
		if (olpath[k].newlist.gad_ptr==g)
			return(TRUE);
return(FALSE);
}

/***********************************************************/

void ClearEntries(notme)
struct list *notme;
{
int k,i,cnt;
struct list *l;
struct MinNode *node;
struct list_entry *le;
short doref;

cnt=OLCount();	/** count number of lists open **/
k=0;
doref=FALSE;
for (i=0; i < cnt; i++) {
	while (olpath[k].state!=OL_READY)	/* find next structure */
		k++;
	l=(struct list *)olpath[k++].newlist.gad_ptr;
	if (l!=notme) {
		node=l->entries.mlh_Head;
		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				le->flags&=(~LE_SELECTED);
				doref=TRUE;
				}
			}
		if (doref) {
			refresh_list_text(l);
			doref=FALSE;
			}
		}
	}
}

/***********************************************************/

void UpdateRegionList(name)
char *name;
{
int i;
char comprn[STUDIO16_MAX_PATH_SIZE];
struct SampleFilePointer *sfp;

new_pointer(PTR_BUSY);
if (name)
	draw_region_text=StrFileName(name);
else
	draw_region_text=NULL;
DrawRegionText();
hide_gadget(newregionlist.gad_ptr);
while(((struct list *)newregionlist.gad_ptr)->list_size)
	delete_list_entry(newregionlist.gad_ptr,0);
if (name) {
	sfp=(struct SampleFilePointer *)SampleOpen(name,SP_MODE_OLDFILE,0);
	if (sfp) {
		for (i=0; i < SFT_NUM_REGIONS; i++) {
			if (sfp->tag.dregions[i].region_name[0]!=NULL) {
				sprintf(comprn,"%s%c%s",sfp->samp_name,REGION_DELIMITER_CHAR,sfp->tag.dregions[i].region_name);
				add_list_entry(newregionlist.gad_ptr, comprn, 0);
				}
			}
		SampleClose(sfp);
		}
	}
show_gadget(newregionlist.gad_ptr);
refresh_gadget(newregionlist.gad_ptr);
new_pointer(PTR_NORMAL);
}

/***********************************************************/

void ProcessSamples(func)
int (*func)();
{
int k;
struct list *l;
struct MinNode *node;
struct list_entry *le;

for (k=0; k < OL_MAX_PATHS; k++) {
	if (olpath[k].state==OL_READY) {	/* find next structure */
		l=(struct list *)olpath[k].newlist.gad_ptr;
		node=l->entries.mlh_Head;

		while (node->mln_Succ) {
			le=(struct list_entry *)node;
			node = node->mln_Succ;
			if (le->flags&LE_SELECTED) {
				if (!(*func)(l,le,olpath[k].path))	
					return;
				}
			}
		}
	}
}

/***********************************************************/

int OLCount()
{
int count=0;
int i;

for (i=0; i < OL_MAX_PATHS; i++)
	if (olpath[i].state!=OL_EMPTY)
		count++;
return(count);
}

/***********************************************************/

int AllListsCount()
{
if (((struct OpenListModState *)thismod->state)->flags&OL_SHOW_REGIONS)
	return(OLCount()+1);
else
	return(OLCount());
}

/***********************************************************/

static char *playnames[16];
static short playcount=0;

/***********************************************************/
int PrePlay(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
if (playcount < 16) {
	playnames[playcount++]=le->text;
	return(TRUE);
	}
else {
	return(FALSE);
	}
}

/***********************************************************/

void PlaySamples()
{
playnames[playcount]=NULL;		/** end indicator **/
BroadcastEventParms(se_cmd_play_sample,playnames,0,0,0,0,0);	/** 0 start & end = use entire sample **/
playcount=0;
}

/***********************************************************/

int EditSample(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
if (edit_mod)
	SendMsg(edit_mod,se_cmd_edit_sample,le->text,0,0,0,0,0);
return(TRUE);
}

/***********************************************************/

void StopAllPlayback()
{
BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
}

/***********************************************************/

int Counter(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
count++;
return(TRUE);
}

/***********************************************************/
 
DiscardSample(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
int retval;

new_pointer(PTR_BUSY);
if (DeleteSample(le->text)!=NO_ERROR) {
	telluser("OPEN_LIST:","Sample in use! Can't Delete it",0);
	retval=FALSE;
	}
else {
	retval=TRUE;
	}
new_pointer(PTR_NORMAL);
return(retval);
}

/***********************************************************/

int MyRenSample(l,le,path)
struct list *l;
struct list_entry *le;
char *path;
{
char NewSampName[80];
char NewTitle[80];
int retval=TRUE;

sprintf(NewTitle,"Rename %s to?",StrFileName(le->text));
strcpy(NewSampName,StrFileName(le->text));
newgetstr(NewSampName,NewTitle,8*40);
ParseStringForIllegalChars(NewSampName,&":/");
if (NewSampName[0])
	if (!(retval=(NO_ERROR==RenameSample(le->text,0,path,NewSampName))))
		telluser("OpenList","Error while trying to rename sample.",0);
return(retval);
}

/***********************************************************/

int DoMoveSample(src_path,src_name,dest_path,dest_name)
{
char *from;
char *to;
int retval;
long tfp=0, ffp=0;
long length;
int error;

new_pointer(PTR_BUSY);
from=(char *)AllocCompletePath(src_path,src_name);
to=(char *)AllocCompletePath(dest_path,dest_name);

retval=FALSE;
error=CANT_OPEN_FILE;

tfp=Open(to,MODE_NEWFILE);
if (tfp) {
	ffp=Open(from,MODE_OLDFILE);
	if (ffp) {
		length=Seek(ffp, 0, OFFSET_END);	
		length=Seek(ffp, 0, OFFSET_BEGINNING);	
		if ((error=QuickCopy(tfp,ffp,length))==NO_ERROR) {
			BroadcastAndLock(se_info_sample_created,to,0,0,0,0,0);
			retval=TRUE;
			}
		Close(ffp);
		}
	Close(tfp);
	}

if (error!=NO_ERROR)	/** if disk filled up, remove partial copy **/
	DeleteFile(to);

FreeMem(from, strlen(from)+1);
UnlockedFreeMem(to, strlen(to)+1);
new_pointer(PTR_NORMAL);
show_error(error);
return(retval);
}

/******************************************************/

int QuickCopy(tofp, fromfp, length)
long tofp, fromfp;
int length;
{
char *buf;
int tomove;

if (length==0)
	return(NO_ERROR);

buf = (char *)AllocMem(StudioBase->defaults.copy_buf_size, 0);
if (buf==0)
	return(OUT_OF_MEMORY);

do {
	tomove = min(StudioBase->defaults.copy_buf_size, length);
	if (Read(fromfp, buf, tomove)!=tomove)
		return(READ_ERROR);
	if (Write(tofp, buf, tomove)!=tomove)
		return(WRITE_ERROR);
	length = length - tomove;
	} while (length);

FreeMem(buf, StudioBase->defaults.copy_buf_size);
return(NO_ERROR);
}

/***********************************************************/

void AddNewPath()
{
char path[STUDIO16_MAX_PATH_SIZE];
char name[40];
int i,k;


VerifyWidth();
strcpy(path,StudioBase->defaults.samp_paths);
name[0]=0;
i=FileRequest("Select New Path:",path,name,FR_SINGLE_SELECT|FR_PATH_ONLY);
if (i) {
	k=stclen(path); if(path[k-1]=='/') path[k-1]=0;

	for (i=0; i < OL_MAX_PATHS; i++) {
		if (olpath[i].state!=OL_EMPTY) {
			if (stricmp(olpath[i].path,path)==0) {
				telluser("OpenList","Path already in open list.",0);
				return;
				}
			}
		}

	for (i=0; i < OL_MAX_PATHS; i++) {
		if (olpath[i].state==OL_EMPTY) {
			strcpy(olpath[i].path,path);
			olpath[i].state=OL_NEEDSFILLING;
			BuildOLPaths();
			UpdateSampPaths();
			SetAPen(open_list_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
			RectFill(open_list_win->RPort,10,16,open_list_win->Width-10,open_list_win->Height-5);
			RefreshOLWin();
			break;
			}
		}
	DoGhost();
	}
}

/***********************************************************/

void VerifyWidth()
{
if ((open_list_win->Width-30)/(AllListsCount()+1) < OL_LISTWID) {
	if (StudioBase->screen->Width < (open_list_win->LeftEdge+open_list_win->Width+OL_LISTWID))
		MoveWindow(open_list_win,-OL_LISTWID,0);	/** make sure SizeWindow() will succeed **/
	SizeWindow(open_list_win,OL_LISTWID,0);
	WaitPort(open_list_win->UserPort);	/** Wait for NEWSIZE msg **/
	PutMsg(open_list_win->UserPort,GetMsg(open_list_win->UserPort));
	RefreshWind(open_list_win);
	}
}

/***********************************************************/

void RemovePath()
{
int i;
char *path;

if (OLCount()==1) {
	telluser("OpenList:","Only one path in list.  Can't remove it.",0);
	return;
	}

path=(char *)AskPathToRemove();
if (path) {
	for (i=0; i < OL_MAX_PATHS; i++) {
		if (olpath[i].state!=OL_EMPTY && stricmp(olpath[i].path,path)==NULL) {
			hide_gadget(olpath[i].newlist.gad_ptr);
			hide_gadget(olpath[i].newbut.gad_ptr);
			while(((struct list *)olpath[i].newlist.gad_ptr)->list_size) 
				delete_list_entry((struct list *)olpath[i].newlist.gad_ptr,0);
			olpath[i].state=OL_EMPTY;
			olpath[i].newbut.gad_ptr->flags &= (~BUTN_HIT);
			BuildOLPaths();
			UpdateSampPaths();
			SetAPen(open_list_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
			RectFill(open_list_win->RPort,10,16,open_list_win->Width-10,open_list_win->Height-5);
			RefreshOLWin();
			DoGhost();
			return;
			}
		}
	telluser("OpenList:","Path not in Open List.",0);	
	}
}

/***********************************************************/

void CheckShowRegions()
{
((struct OpenListModState *)thismod->state)->flags&=(~OL_SHOW_REGIONS);
if (Items[11].Flags&CHECKED) {
	VerifyWidth();
	((struct OpenListModState *)thismod->state)->flags|=OL_SHOW_REGIONS;
	show_gadget(newregionlist.gad_ptr); 
	}
else {
	hide_gadget(newregionlist.gad_ptr);
	draw_region_text=NULL;
	while(((struct list *)newregionlist.gad_ptr)->list_size)
		delete_list_entry(newregionlist.gad_ptr,0);
	}
BuildOLPaths();		/** recalc list sizes **/
SetAPen(open_list_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
RectFill(open_list_win->RPort,10,16,open_list_win->Width-10,open_list_win->Height-5);
RefreshOLWin();
}

/***********************************************************/

void UpdatePaths()
{
int i;

new_pointer(PTR_BUSY);
for (i=0; i < OL_MAX_PATHS; i++) {
	if (olpath[i].state!=OL_EMPTY) {
		hide_gadget(olpath[i].newlist.gad_ptr);
		while(((struct list *)olpath[i].newlist.gad_ptr)->list_size) 
			delete_list_entry((struct list *)olpath[i].newlist.gad_ptr,0);
		olpath[i].state=OL_NEEDSFILLING;
		}
	}

BuildOLPaths();
RefreshOLWin();
new_pointer(PTR_NORMAL);
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
      if (strnicmp(m->name,"Editor",4)==0) {
			if (!(((struct EditState *)(m->state))->es_flags&ES_IN_USE)) {
				((struct EditState *)(m->state))->es_flags|=ES_IN_USE;
				ReleaseSharedSemaphore(&StudioBase->lockstudio);
   	      return(m);
				}
			}
   }
ReleaseSharedSemaphore(&StudioBase->lockstudio);
m=LoadModule("Editor");
if (m)
	((struct EditState *)(m->state))->es_flags|=ES_IN_USE;
return(m);
}

/************************************************************/

void add_dir(l,path, ud)
struct list *l;
char *path;
int ud;
{
struct FileLock      *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;
short len;
char *cp;

if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) 
	{
	return;
	}

if ( ! Examine(fkey, dinfo) ) 
	{
	UnLock(fkey);
	return;
	}

if ( dinfo->fib_DirEntryType < 0L ) 
	{
	UnLock(fkey);
	return;
	}

while( ExNext( fkey, dinfo ) )
	{
	if (dinfo->fib_EntryType<0) 
		{
		len=strlen(dinfo->fib_FileName);
		if (len < 6) {
			cp=(char *)AllocCompletePath(path,dinfo->fib_FileName);
			add_list_entry(l, cp, ud);
			FreeMem(cp, strlen(cp)+1);
			}
		else {
			if (stricmp(&dinfo->fib_FileName[len-6],".graph")) {
				cp=(char *)AllocCompletePath(path,dinfo->fib_FileName);
				add_list_entry(l, cp, ud);
				FreeMem(cp, strlen(cp)+1);
				}
			}
		}
	}

UnLock(fkey);
}

/***********************************************************/

int FindOLPath(path)
char *path;
{
int i;

for (i=0; i < OL_MAX_PATHS; i++)
	if (olpath[i].state!=OL_EMPTY && fplcmp(olpath[i].path,path))
		return(i);
return(-1);
}

/******************/

fplcmp(path,pathplus)
char *path;				/** Path only **/
char *pathplus;		/** Path to compare, plus filename **/
							/** which i dont care if matches **/
{
short len;

len=strlen(path);
if (strnicmp(pathplus,path,len)==NULL) {	/** NULL=match **/
	return(!strchr(&pathplus[len+1],'/'));	/** Any more subdirs? **/
	}
return(FALSE);
}

/***********************************************************/

void UpdateSampPaths()
{
int i;
char *path;

path=StudioBase->defaults.samp_paths;

for (i=0; i < OL_MAX_PATHS; i++) 
	if (olpath[i].state!=OL_EMPTY && olpath[i].newbut.gad_ptr->flags&BUTN_HIT) {
		path=(char *)stpcpy(path,olpath[i].path);
		path++;
		break;
		}

for (i=0; i < OL_MAX_PATHS; i++) 
	if (olpath[i].state!=OL_EMPTY && !(olpath[i].newbut.gad_ptr->flags&BUTN_HIT)) { 
		path=(char *)stpcpy(path,olpath[i].path);
		path++;
		}
*path=NULL;
BroadcastEventParms(se_info_sample_path_changed,0,0,0,0,0,0);
}

/***********************************************************/

void RefreshOLWin()
{
int i,k,cnt;
int width;
char *str;

/** Count the number of lists to draw **/
cnt=AllListsCount();
open_list_win->MinWidth=cnt*OL_LISTWID+30;

/** Calculate size,position of each list  **/

width=(open_list_win->Width-10)/cnt;
cnt=OLCount();
k=0;
for (i=0; i < cnt; i++) {
	while (olpath[k].state!=OL_READY)	/* find next structure */
		k++;
	new_gadget_size(olpath[k].newlist.gad_ptr,
		10+i*width,	/* x1 */
		26,			/* y1 */
		(i+1)*width,/* x2 */
		open_list_win->Height-5);
	new_gadget_size(olpath[k].newbut.gad_ptr,
		10+i*width,	/* x1 */
		16,			/* y1 */
		10+i*width+10,/* x2 */
		23);
	if (olpath[k].newlist.gad_ptr->flags&HIDE_GADGET) {
		show_gadget(olpath[k].newlist.gad_ptr);
		show_gadget(olpath[k].newbut.gad_ptr);
		}
	k++;
	}

if (((struct OpenListModState *)thismod->state)->flags&OL_SHOW_REGIONS) {
	new_gadget_size(newregionlist.gad_ptr,
		10+i*width,	/* x1 */
		26,			/* y1 */
		(i+1)*width,/* x2 */
		open_list_win->Height-5);
	}

/** Draw the Text lable over each list **/

LockGadLibRender();
SetDrMd(open_list_win->RPort, JAM1);
SetAPen(open_list_win->RPort, StudioBase->defaults.colors.button.draw_unselected);
k=0;

for (i=0; i < cnt; i++) {
	while (olpath[k].state!=OL_READY)	/* find next structure */
		k++;
	Move(open_list_win->RPort, 24+i*width, 23);
	str=StrLastDir(olpath[k].path);
	Text(open_list_win->RPort, str, min(strlen(str),(width-24)/8));
	k++;
	}

if (((struct OpenListModState *)thismod->state)->flags&OL_SHOW_REGIONS)
	DrawRegionText();
UnLockGadLibRender();

refresh_gadget_chain(open_list_win);
}

/***********************************************************/

void DrawRegionText()
{
char str[92];
int cnt;
int width;

setmem(str,90, ' ');
str[90]=0;
if (draw_region_text)
	movmem(draw_region_text,str,strlen(draw_region_text));

cnt=AllListsCount();
width=(open_list_win->Width-10)/cnt;
cnt--;
LockGadLibRender();
SetDrMd(open_list_win->RPort, JAM2);
SetAPen(open_list_win->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(open_list_win->RPort,StudioBase->defaults.colors.list.hitbox_unselected);
Move(open_list_win->RPort,10+cnt*width,23);
Text(open_list_win->RPort, str, (width-10)/8);
UnLockGadLibRender();
}

/***********************************************************/

void DoGhost()
{
int i;
int cnt;

for (i=0; i < 12; i++)
	Items[i].Flags&=~ITEMENABLED;

cnt=OLCount();

Items[1].Flags|=ITEMENABLED;			/* stop playback */
Items[9].Flags|=ITEMENABLED;			/* update path */
Items[6].Flags|=ITEMENABLED;			/* ····· */
Items[10].Flags|=ITEMENABLED;			/* ····· */
Items[11].Flags|=ITEMENABLED;			/* show regions */
if (cnt < OL_MAX_PATHS)
	Items[7].Flags|=ITEMENABLED;		/* add new path */
if (cnt > 1)
	Items[8].Flags|=ITEMENABLED;		/* remove path */

if (AnythingSelected()) {
	Items[0].Flags|=ITEMENABLED;		/* play */
	Items[2].Flags|=ITEMENABLED;		/* rename */
	Items[3].Flags|=ITEMENABLED;		/* delete */
	Items[4].Flags|=ITEMENABLED;		/* convert */
	Items[5].Flags|=ITEMENABLED;		/* edit */
	}
}

/***********************************************************/

AnythingSelected()
{
count=0;
ProcessSamples(Counter);
if (count)
	return(TRUE);
else
	return(FALSE);
}

/***********************************************************/
