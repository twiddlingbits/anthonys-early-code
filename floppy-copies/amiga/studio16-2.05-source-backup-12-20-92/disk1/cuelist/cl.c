#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "intuition/intuition.h"
#include "clib/macros.h"
#include "/include/psound.h"
#include "cl.data"
#include "cl.h"
#include "Menu.H"
/**********************************/
/**********************************/
long _stack = 14000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_CUELIST";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
#define NUMGADS 5
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
short se_info_sample_created;
short se_info_sample_deleted;
short se_info_prefs_changed;
short se_info_sample_edited;
short se_cmd_stop_all_playback;
short se_info_sampling_rate_error;
short se_cmd_trigger_timecode;
/**********************************/
/**********************************/
void DumpSamples();
struct MsgPort *gadlibport;
int ReactionOffset=0;
/**********************************/
/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
void DeleteEntry();
void DuplicateEntry();
void UpdateEntry();
void NewEntry();
void FigurePath();
void AddEntryFromGads();
void UpdateEntryFromGads();


void ClearFunc();
void OffsetFunc();
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void Quit();
void WindGadSize();
void DumpSample();
int FindRegion();
char *FindSample();
void MyFindRegion();
void MenuGhost();
/**********************************/
/**********************************/
struct Module *mod=NULL;
struct Task *task;
extern struct StudioBase *StudioBase;
struct Window *wind=NULL;
struct Window *prefwind=NULL;
long preload_sigbit=-1;
long smpte_error_sigbit=-1;
long preload_sigmask;
long smpte_error_sigmask;
long SmpteErrorVal=0;
/**********************************/
/**********************************/
void PreFunc();
static struct NewSmpteSink NewSMPTEsnk = {0,NULL,PreFunc,NULL};
static struct SmpteSink *SMPTEsnk;
/**********************************/
struct Gadget *ASGad;
/**********************************/
void select_list_entry_by_userdata();
void Struct2Line();
void HidePrefWind();
void handle_pref_intuievents();
struct list_entry *find_list_entry_by_index();
/**********************************/
/**********************************/
BOOL LIST_EMPTY=TRUE;
BOOL SUSPENDED=FALSE;
int edits;
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(argv)
char *argv;
{
int mask;
void handle_gadlibevents();
void PreloadSamples();
void smpte_error();
InitModule(argv);				/** initalize module **/
while (TRUE)
	{
	mask=preload_sigmask|smpte_error_sigmask;
	if (gadlibport) mask|=(1<<gadlibport->mp_SigBit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	if (prefwind) mask |= 1<<prefwind->UserPort->mp_SigBit;
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	if ((mask & preload_sigmask) && OnOffbut.gad_ptr->flags&BUTN_HIT) PreloadSamples();
	if (mask & smpte_error_sigmask) smpte_error();
	if (gadlibport) if (mask & (1<<gadlibport->mp_SigBit)) handle_gadlibevents();
	if (prefwind) if(mask&(1<<prefwind->UserPort->mp_SigBit)) handle_pref_intuievents();
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
void InitModule(name)
char *name;
{
char *FindFileName();
void ColorGadList();
static struct NewModule ModData ={NULL,0,sizeof(struct CueModState),MOD_SINGLE_RESIDENT};
task=(struct Task *)FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

if ((preload_sigbit=AllocSignal(-1))==-1) {telluser("Cue List:","Cant get SigBit",NULL);Quit();}
preload_sigmask=1<<preload_sigbit;

if ((smpte_error_sigbit=AllocSignal(-1))==-1) {telluser("Cue List:","Cant get SigBit",NULL);Quit();}
smpte_error_sigmask=1<<smpte_error_sigbit;

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Cue List:","Could Not Add Module",0);
	Quit();
	}

if (!(mod->state->flags & MS_STATE_INITILIZED)) 
	{
	strcpy(((struct CueModState *)(mod->state))->path,&"Studio16:CueLists");
	((struct CueModState *)(mod->state))->PlayFlags=0x07;
	((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded=0x05;
	}

ColorGadList(&CueList);

se_info_sampling_rate_error	=GetEventID("SE_INFO_SAMPLING_RATE_ERROR");
se_info_sample_created	=GetEventID("SE_INFO_SAMPLE_CREATED");
se_info_sample_deleted	=GetEventID("SE_INFO_SAMPLE_DELETED");
se_cmd_select_module 	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window	= GetEventID("SE_CMD_HIDE_WINDOW");
se_info_prefs_changed	= GetEventID("SE_INFO_PREFS_CHANGED");
se_info_sample_edited	= GetEventID("SE_INFO_SAMPLE_EDITED");
se_cmd_stop_all_playback=GetEventID("SE_CMD_STOP_ALL_PLAYBACK");
se_cmd_trigger_timecode	=GetEventID("SE_CMD_TRIGGER_TIMECODE");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

if((gadlibport = (struct MsgPort *)CreatePort(0,0))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	Quit();
	}

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void Quit()
{
char buff[80];
void SaveF();

while (edits)
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(askuser(buff,"Do you wish to save these changes?",0))
		{
		SaveF(0);
		}
	else edits=0;
	}

if (wind) HideWind();
if (mod->state->lock) BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
if (mod) DeleteModule(mod);
if (preload_sigbit!=-1) FreeSignal(preload_sigbit);
if (smpte_error_sigbit!=-1) FreeSignal(smpte_error_sigbit);
if(gadlibport) DeletePort(gadlibport);	gadlibport=0;

IgnoreEvent(se_cmd_hide_window, mod);
IgnoreEvent(se_cmd_select_module, mod);
IgnoreEvent(se_cmd_kill_module, mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
void Gad2Struct();
void PlayTask();

if ((wind = OpenNewWind(&NewWind,mod->state,NULL,Titles))!=NULL)
	{
	BroadcastEventParms(GetEventID("SE_INFO_MODULE_OPENED"), mod, 0,0,0,0,0);

	CueList.box_x1=10;
	CueList.box_x2=wind->Width-10;
	CueList.box_y1=24;
	CueList.box_y2=wind->Height-18;
	OnOffbut.box_x1=120;
	OnOffbut.box_x2=209;
	OnOffbut.box_y1=wind->Height-14;
	OnOffbut.box_y2=wind->Height-5;
	SampleName.box_y1=wind->Height-15;
	SampleName.box_y2=wind->Height-15+11;

	SampleName.notifyme=gadlibport;
	CueList.notifyme=gadlibport;

	create_gadget_chain(wind,&CueList);
	CueL=(struct list *)CueList.gad_ptr;
	show_gadget(CueList.gad_ptr);
	WindGadSize();

	MenuGhost(FALSE);
	MenuGhost(TRUE);

	StartTimeGad.TopEdge=wind->Height-13;
/*	Gadget2.TopEdge=wind->Height-13;*/
	VolumeGad.TopEdge=wind->Height-13;
	NameGad.TopEdge=wind->Height-13;

	RemarkGad.TopEdge=wind->Height-13;
	RemarkGad.Width=(((wind->Width-RemarkGad.LeftEdge-27)>>3)<<3);
	AddGList(wind,&StartTimeGad,NULL,NULL,-1,NULL);

	ActivateGadget(&StartTimeGad,wind,NULL);
	ASGad=&StartTimeGad;

	NotifyMeOnEvent(se_info_sampling_rate_error,mod);
	NotifyMeOnEvent(se_info_sample_created, mod);
	NotifyMeOnEvent(se_info_sample_deleted, mod);
	NotifyMeOnEvent(se_info_sample_edited,mod);

	SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);
	}
else telluser("Cue List:","Can not open window!",0);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
int x;
void SaveF();
char buff[80];

if (edits)
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(!askuser(buff,"They will be lost. OK to continue?",0))
		{
		((struct WindowParms *)(wind->UserData))->Flags2 &= ~NOMESSAGE;
		return;
		}
	else edits=0;
	}
DumpSamples();

if (prefwind) HidePrefWind(mod->state);
prefwind=NULL;
if (wind)
	{
	hide_gadget(CueList.gad_ptr);

	for(x=0;x<CueL->list_size;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		FreeMem(ce,sizeof(struct CueEntry));
		}

	delete_gadget_chain(wind);

	RemoveGList(wind,&StartTimeGad,-1);

	w=wind;
	wind=NULL;
	CloseWind(w);

	IgnoreEvent(se_info_sampling_rate_error,mod);
	IgnoreEvent(se_info_sample_created,mod);
	IgnoreEvent(se_info_sample_deleted,mod);
	IgnoreEvent(se_info_sample_edited,mod);

	DeleteSmpteSink(SMPTEsnk);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
char *Top="Start Time   End Time     Vol  Sample             Remark";
void OnOfffunc();
if (wind)
	{
	RefreshWind(wind);

	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(wind->RPort,12+1,20+1);
	Text(wind->RPort,Top, MIN(56,(wind->Width-45)/8));
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(wind->RPort,12,20);
	Text(wind->RPort,Top, MIN(56,(wind->Width-45)/8));

	new_gadget_size(CueList.gad_ptr,10,24,wind->Width-10,wind->Height-18);
	new_gadget_size(OnOffbut.gad_ptr,120,wind->Height-14,209,wind->Height-5);

	StartTimeGad.TopEdge=wind->Height-13;
/*	Gadget2.TopEdge=wind->Height-13;*/
	VolumeGad.TopEdge=wind->Height-13;
	NameGad.TopEdge=wind->Height-13;

	SampleName.gad_ptr->box_y1=wind->Height-15;
	SampleName.gad_ptr->box_y2=wind->Height-15+11;

	RemarkGad.TopEdge=wind->Height-13;
	RemarkGad.Width=(((wind->Width-RemarkGad.LeftEdge-27)>>3)<<3);

	draw_indent(wind->RPort,11,wind->Height-14,102+8,wind->Height-5,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,219,wind->Height-14,246+8,wind->Height-5,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,259,wind->Height-14,398,wind->Height-5,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,257,wind->Height-15,400,wind->Height-4,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(wind->RPort,RemarkGad.LeftEdge-2,wind->Height-14,RemarkGad.Width+RemarkGad.LeftEdge+1,wind->Height-5,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	RefreshGList(&StartTimeGad,wind,NULL,4);
	refresh_gadget(CueList.gad_ptr);
	refresh_gadget(OnOffbut.gad_ptr);
	OnOfffunc(OnOffbut.gad_ptr);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void handle_gadlibevents()
{
struct GadMsg *msg;
struct GadgetHeader *g;
void LoadF();
void SaveF();
void PLAYfunc();
void ADDEntry();
struct Region regn;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) 
	{
	g = msg->g;
	if (CueL->active_entry) ce=(struct CueEntry *)CueL->active_entry->user_data;
	else ce=NULL;

	if (msg->type==GADMSG_DRAG_AND_DROP)
		{
		if (g==SampleName.gad_ptr)
			{
			if (!ce || strcmp(ce->path,msg->text))
				{
				setmem(&regn,sizeof(struct Region),0);
				if (!FindRegion(msg->text,&regn))
					{
					strcpy(SG_SampName,msg->text);
					refresh_gadget(msg->g);
					UpdateEntryFromGads();
					}
				else telluser("Cue List:","Could not find Region.",NOTICE_RETURN_NOW);
				}
			}
		if (g==CueList.gad_ptr)
			{
			setmem(&regn,sizeof(struct Region),0);
			if (!FindRegion(msg->text,&regn))
				{
				if(regn.parms.starttimecode==0) 
					{
					if (LIST_EMPTY==FALSE)
						{
						le=find_list_entry_by_index(CueL,CueL->list_size-1);
						ce=(struct CueEntry *)le->user_data;
						if (ce->end) regn.parms.starttimecode=AddTimeCode(ce->end,0x00000010,StudioBase->defaults.smpte_mode);
						else regn.parms.starttimecode=AddTimeCode(ce->regn.parms.starttimecode,0x00000100,StudioBase->defaults.smpte_mode);
						}
					else regn.parms.starttimecode=0x00000100;
					}
				ADDEntry(&regn,"");
				}
			else telluser("Cue List:","Could not find Region.",NOTICE_RETURN_NOW);
			}
		}
	ReplyMsg(msg);
	}
}
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;
static BOOL ListChanged=FALSE;
BOOL UpdateSparms();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==NULL)
				DisplayWind();
		}

	/*********************************/
	else if(event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			Quit();
			}
		}

	/*********************************/
	else if (event->type == se_info_prefs_changed)
		{
		Signal(task,1<<preload_sigbit);
		}

	/*********************************/
	else if (event->type==se_info_sample_created 
		|| event->type==se_info_sample_deleted
			|| event->type == se_info_sample_edited)
		{
		ListChanged=UpdateSparms(event->arg1);
		}

	/*********************************/
	else if (event->type==se_info_sampling_rate_error)
		{
		if(event->arg1==1 && OnOffbut.gad_ptr->flags&BUTN_HIT)
			telluser("Warning, Two samples with different","sampling rates are playing.",NOTICE_RETURN_NOW);
		}
	ReplyMsg(event);
	}

if (ListChanged==TRUE)
	{
	Signal(task,1<<preload_sigbit);
	ListChanged=FALSE;
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void handle_pref_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(prefwind->UserPort))!=0)
	{
	HandleIntuitMess(prefwind,message);
	class=message->Class;
	ReplyMsg(message);
	if (class == CLOSEWINDOW)
		{HidePrefWind();prefwind=NULL;return;}
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class,code;
struct Gadget *gad;
void PLAYfunc();
int x;
struct Region regn;
struct CueEntry *ce;
struct Window *DisplayPrefWind();
BOOL SetRange();

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	code=message->Code;
	gad=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{HideWind(); return;}
	if (class == GADGETUP)
		{
		if (gad==&StartTimeGad) {ParseStringForIllegalChars(SG_StartTime,NULL);ASGad=&VolumeGad; ActivateGadget(&VolumeGad,wind,NULL);}
		if (gad==&VolumeGad) 	{ParseStringForIllegalChars(SG_Volume,NULL);ASGad=&NameGad; ActivateGadget(&NameGad,wind,NULL);}
		if (gad==&RemarkGad) 	{ParseStringForIllegalChars(SG_Remark,NULL);ASGad=&StartTimeGad; ActivateGadget(&StartTimeGad,wind,NULL);}
		if (gad==&NameGad)
			{
			ParseStringForIllegalChars(SG_SampName,NULL);
			ASGad=&RemarkGad;
			ActivateGadget(&RemarkGad,wind,NULL);
			}
		UpdateEntryFromGads();
		if (gad==&NameGad)
			{
			ce=(struct CueEntry *)CueL->active_entry->user_data;
			if (ce->path[0]) set_box_text(SampleName.gad_ptr,ce->path);
			else set_box_text(SampleName.gad_ptr,NULL);
			}
		}
	if (class==MENUPICK)
		{
		if (MENUNUM(code)==FIRST_MENU)
			{
			x=ITEMNUM(code);
			if (x==0) ClearFunc();
			if (x==1) LoadF(1);
			if (x==2) LoadF(0);
			if (x==3) SaveF(1);
			if (x==4) SaveF(0);
			if (x==6) {if (SetRange())OffsetFunc();}
			if (x==7) prefwind=DisplayPrefWind(mod->state);
			}
		if (MENUNUM(code)==FIRST_MENU+1)
			{
			x=ITEMNUM(code);
			if (x==0) NewEntry();
			if (x==1) DuplicateEntry(CueL->active_entry->user_data);
			if (x==2) if(CueL->active_entry) DeleteEntry(CueL->active_entry->user_data);
			if (x==4)
				{
				setmem(&regn,sizeof(struct Region),0);
				regn.parms.starttimecode=QuerySmpteTime();
				regn.parms.starttimecode=AddTimeCode(regn.parms.starttimecode,ReactionOffset,StudioBase->defaults.smpte_mode);
				regn.parms.pan=ZERO_DB;
				regn.parms.volume=ZERO_DB;
				ADDEntry(&regn,"");
				}
			if (x==5)
				{
				ce=(struct CueEntry *)CueL->active_entry->user_data;
				ce->regn.parms.starttimecode=QuerySmpteTime();
				regn.parms.starttimecode=AddTimeCode(regn.parms.starttimecode,ReactionOffset,StudioBase->defaults.smpte_mode);
				UpdateEntry(ce);
				}
			if (x==7) PLAYfunc();
			if (x==8) {BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);Signal(task,1<<preload_sigbit);}
			}
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
void EditEntry(g)
struct	list *g;
{
void Struct2Gad();
struct CueEntry *ce;

if(g->active_entry && !SUSPENDED)
	{
	ce=(struct CueEntry *)g->active_entry->user_data;

	if (g->active_entry->text)
		{
		Struct2Gad(ce);
		if (ce->path[0]) set_box_text(SampleName.gad_ptr,ce->path);
		else set_box_text(SampleName.gad_ptr,NULL);
		RefreshGList(&StartTimeGad,wind,NULL,4);
/*		ActivateGadget(ASGad,wind,NULL);*/
		}
	}
}
/**************************************************************/
/**************************************************************/
void PLAYfunc() 
{
CreatePlayProc(CueL->active_entry,PP_TRIGGER_NOW);
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
void AddEntryFromGads()
{
void Gad2Struct();
struct CueEntry *ce;

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))!=NULL)
	{
	Gad2Struct(ce);
	ADDEntry(&ce->regn,ce->Rem);
	FreeMem(ce,sizeof(struct CueEntry));
	}
}
/**************************************************************/
/**************************************************************/
void UpdateEntryFromGads()
{
void Gad2Struct();
int GadStructCmp();
struct CueEntry *ce;

if (LIST_EMPTY==TRUE) {AddEntryFromGads();return;}

ce=(struct CueEntry *)CueL->active_entry->user_data;

if (GadStructCmp(ce))
	{
	Gad2Struct(ce);
	UpdateEntry(ce);
	}
}
/**************************************************************/
/**************************************************************/
void NewEntry()
{
struct Region regn;
setmem(&regn,sizeof(struct Region),0);
regn.parms.pan=ZERO_DB;
regn.parms.volume=ZERO_DB;
regn.parms.starttimecode=0x00000100;
ADDEntry(&regn,&"");
MenuGhost(FALSE);
}
/**************************************************************/
/**************************************************************/
void UpdateEntry(ce)
struct CueEntry *ce;
{
struct Region CopyRegion;
char remark[100];

if (!ce) return;
CopyRegion=ce->regn;
strcpy(remark,ce->Rem);
SUSPENDED=TRUE;
DeleteEntry(ce);
SUSPENDED=FALSE;
ADDEntry(&CopyRegion,remark);
edits--;
}
/**************************************************************/
/**************************************************************/
void ADDEntry(regn,remark)
struct Region *regn;
char *remark;
{
void Gad2Struct();
void Struct2Line();
struct CueEntry *ce;

if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue List:","Memory Allocation Error",NOTICE_RETURN_NOW);return;}

ce->regn=*regn;
strcpy(ce->Rem,remark);
FigurePath(ce);

Struct2Line(ce);

add_list_entry(CueL,ce->line,ce);
select_list_entry_by_userdata(CueL,ce);
scroll_to_show(CueL,get_list_entry_index(CueL,find_list_entry_by_userdata(CueL,ce)));
Signal(task,1<<preload_sigbit);
MenuGhost(FALSE);
edits++;
}
/**************************************************************/
/**************************************************************/
void DeleteEntry(ce)
struct CueEntry *ce;
{
struct list_entry *le,*find_list_entry_by_index();
int ae;

if (ce)
	{
	if (CueL->list_size==1) MenuGhost(TRUE);
	DumpSample(ce);

	ae=CueL->current;
	delete_list_entry_by_userdata(CueL,ce);

	FreeMem(ce,sizeof(struct CueEntry));

	if (LIST_EMPTY==FALSE)
		{
		if (ae > CueL->list_size-1) ae = CueL->list_size-1;
		le=find_list_entry_by_index(CueL,ae);
		ce=(struct CueEntry *)le->user_data;
		select_list_entry_by_userdata(CueL,ce);
		Signal(task,1<<preload_sigbit);
		}
	edits++;
	}
}
/**************************************************************/
/**************************************************************/
void DuplicateEntry(ce)
struct CueEntry *ce;
{
if (!ce) return;
ADDEntry(&ce->regn,"");
Signal(task,1<<preload_sigbit);
edits++;
}
/**************************************************************/
/**************************************************************/
void OnOfffunc(b)
struct button *b;
{
void DumpSamples();
char *text;
int len;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
struct ChanKey *key;

if (b->flags&BUTN_HIT) 
	{
	text="ON";
	len=2;
	if (LIST_EMPTY==FALSE)
		{
		le=find_list_entry_by_index(CueL,0);
		ce=(struct CueEntry *)le->user_data;
		if (ce->regn.parms.rate)
			{
			key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY | CK_ID_ONLY);
			BroadcastEventParms(GetEventID("SE_CMD_SET_SAMPLING_RATE"),key->handler,ce->regn.parms.rate,0, 0, 0,0);
			}
		}
	}
else
	{
	text="OFF";
	len=3;
	}
SetAPen(wind->RPort, b->c->slider_knob.draw_selected);
if (b->flags & BUTN_HIT) SetBPen(wind->RPort,  b->c->slider_knob.hitbox_selected);
else SetBPen(wind->RPort,  b->c->slider_knob.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,b->box_x1+(b->box_x2-b->box_x1)/2-(len*4),b->box_y1+7);
Text(wind->RPort, text,strlen(text));

if (b->flags&BUTN_HIT) 
	{
	Signal(task,1<<preload_sigbit);
	}
else
	{
	DumpSamples();
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
void FigurePath(ce)
struct CueEntry *ce;
{
ce->path[0]=0;
strcpy(ce->path,ce->regn.samp_name);
if (ce->regn.region_name[0])
	{
	strcat(ce->path,&REGION_DELIMITER_STRING);
	strcat(ce->path,ce->regn.region_name);
	}
}
/**************************************************************/
/**************************************************************/
BOOL UpdateSparms(path)
char *path;
{
int x;
BOOL flag=FALSE;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;


for(x=0;x<CueL->list_size;x++)
	{
	le=find_list_entry_by_index(CueL,x);
	ce=(struct CueEntry *)le->user_data;

	if (!stricmp(path,ce->regn.samp_name))
		{
		DumpSample(ce);
		MyFindRegion(path,ce);
		UpdateEntry(ce);
		flag=TRUE;
		}
	}
return(flag);
}
/**************************************************************/
/**************************************************************/
void MyFindRegion(name,ce)
char *name;
struct CueEntry *ce;
{
int stc,vol;
char *path,*regn;
char cpath[STUDIO16_MAX_PATH_SIZE];
int x;

stc=ce->regn.parms.starttimecode;
vol=ce->regn.parms.volume;

if (!FindRegion(name,&ce->regn))
	{
	ce->regn.parms.starttimecode=stc;
	ce->regn.parms.volume=vol;
	return;
	}

path=FindSample(name);
if (path)
	{
	strcpy(cpath,path);
	StrPathFileCat(cpath,name);
	if (!FindRegion(cpath,&ce->regn)) 
		{
		ce->regn.parms.starttimecode=stc;
		ce->regn.parms.volume=vol;
		return;
		}
	}

regn=NULL;
for (x=0;x<strlen(name);x++)
	{
	if (name[x]==REGION_DELIMITER_CHAR) {name[x]=NULL; regn=&name[x+1];}
	}


strcpy(ce->regn.samp_name,name);
if (regn) strcpy(ce->regn.region_name,regn);
ce->regn.parms.rate=0;
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
void DumpSample(ce)
struct CueEntry *ce;
{
int x=0;

while (ce->task)
	{
	Disable();
	if (ce->task)
		{
		Signal(ce->task,ce->UnloadSigMask);
		}
	Enable();
	WaitTOF();WaitTOF();
	x++;
	if (x==5000) {x=0;telluser("Cuelist:","Could not dump sample!",NULL);}
	}
}
/**************************************************************/
/**************************************************************/
void DumpSamples()
{
int x;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
for(x=0;x<CueL->list_size;x++)
	{
	le=find_list_entry_by_index(CueL,x);
	ce=(struct CueEntry *)le->user_data;
	DumpSample(ce);
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
/**************************************************************/
void PreloadSamples()
{
int x;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
int n=0;
ULONG	tc;
int max=((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded;

for(x=0;x<CueL->list_size && n < max;x++)
{
le=find_list_entry_by_index(CueL,x);
ce=(struct CueEntry *)le->user_data;

tc=QuerySmpteTime();

if (n < max)
	{
	if (ce->end)	/* is sample in openlist? */
		{

/* After - Sample should not be preloaded */
		if (ce->end < tc)
			{
			DumpSample(ce);
			}

/* Durring - Sample must be preloaded */

		else if (ce->regn.parms.starttimecode <= tc && ce->end >= tc )
			{
			if (!ce->task)
				{
				if (ce->end >= AddTimeCode(tc,0x00000200,StudioBase->defaults.smpte_mode) )
					{
					CreatePlayProc(le,NULL);
					n++;
					}
				}
			else	n++;
			}

/* Before */

		else if (ce->regn.parms.starttimecode > tc)
			{
			if (ce->regn.parms.starttimecode < AddTimeCode(tc,PRELOAD_TIME,StudioBase->defaults.smpte_mode)) 
				{
				n++;
				if(!ce->task) CreatePlayProc(le,NULL);
				}
			else DumpSample(ce); /* Preload Time Has not been reached */
			}
		}
	else DumpSample(ce);
	}
else DumpSample(ce);
}
}
/**************************************************************/
/**************************************************************/
void smpte_error()
{
int x;
struct list_entry *le;
struct CueEntry *ce;

if (SmpteErrorVal&((struct CueModState *)(mod->state))->PlayFlags)
	{
	for(x=0;x<CueL->list_size;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		if (ce->task) DumpSample(ce);
		}
	Signal(task,1<<preload_sigbit);
	}
}
/**************************************************************/
/**************************************************************/
void MenuGhost(LE)
BOOL LE;
{

if (LE!=LIST_EMPTY)
	{
	LIST_EMPTY=LE;
	if (LIST_EMPTY==TRUE)
		{
		Items[0].Flags&=~ITEMENABLED;
		Items[2].Flags&=~ITEMENABLED;
		Items[3].Flags&=~ITEMENABLED;
		Items[4].Flags&=~ITEMENABLED;
		Items[6].Flags&=~ITEMENABLED;
		Items[9].Flags&=~ITEMENABLED;
		Items[10].Flags&=~ITEMENABLED;
		Items[13].Flags&=~ITEMENABLED;
		Items[15].Flags&=~ITEMENABLED;
		}
	else
		{
		Items[0].Flags|=ITEMENABLED;
		Items[2].Flags|=ITEMENABLED;
		Items[3].Flags|=ITEMENABLED;
		Items[4].Flags|=ITEMENABLED;
		Items[6].Flags|=ITEMENABLED;
		Items[9].Flags|=ITEMENABLED;
		Items[10].Flags|=ITEMENABLED;
		Items[13].Flags|=ITEMENABLED;
		Items[15].Flags|=ITEMENABLED;
		}
	}
else LIST_EMPTY=LE;
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
void ClearFunc()
{
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce=NULL;
int x,y;
char buff[100];
void DumpSamples();

if (edits) 
	{
	sprintf(buff,"%d changes have been made to cue list.",edits);
	if(!askuser(buff,"They will be lost. OK to continue?",0)) return;
	}

new_pointer(PTR_BUSY);

hide_gadget(CueL);
DumpSamples();

y=CueL->list_size;
for(x=0;x<y;x++)
	{
	le=find_list_entry_by_index(CueL,0);
	ce=(struct CueEntry *)le->user_data;
	delete_list_entry_by_index(CueL,0);
	FreeMem(ce,sizeof(struct CueEntry));
	}

MenuGhost(TRUE);
show_gadget(CueL);
refresh_gadget(CueL);
edits=0;
new_pointer(PTR_NORMAL);
}
/**************************************************************/
/**************************************************************/
void OffsetFunc()
{
char oset[21];
int x,y;
int start=-1,end=-1;
int flag,tc;
struct list_entry *le;
struct CueEntry *ce;
struct CueEntry **cea;
void DumpSamples();
extern int OffS,OffE;

if (OnOffbut.gad_ptr->flags&BUTN_HIT) {telluser("Cuelist:","Turn off Cuelist before selecting Offset function.",NULL);return;}

strcpy(oset,&"+00:00:00:00");
newgetstr(oset,"CueList Offset?",8*20);

y=strlen(oset);
flag=0;
for(x=0;x<y && flag==0;x++) 
	{
	if(oset[x]=='-') flag=-1;
	}
tc=TCstr2int(oset);
if (tc)
	{
	DumpSamples();
	y=CueL->list_size;
	for(x=0;x!=y;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		if (ce->regn.parms.starttimecode<=OffE) end=x;
		}

	for(x=y-1;x>=0;x--)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		if (ce->regn.parms.starttimecode>=OffS) start=x;
		}
	if (start==-1 || end==-1 || start>end) return;

	cea=(struct CueEntry **)AllocMem((y+1)*4,NULL); /* Allocs more mem than needed */
	if (!cea) {telluser("CueList:","Could Not AllocMem for Offset.",NULL);return;}

	hide_gadget(CueL);
	for(x=start;x<=end;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		cea[x]=(struct CueEntry *)le->user_data;
		}

	for(x=start;x<=end;x++)
		{
		if (flag==-1) cea[x]->regn.parms.starttimecode=SubTimeCode(cea[x]->regn.parms.starttimecode,tc,StudioBase->defaults.smpte_mode);
		else cea[x]->regn.parms.starttimecode=AddTimeCode(cea[x]->regn.parms.starttimecode,tc,StudioBase->defaults.smpte_mode);

		Struct2Line(cea[x]);
		delete_list_entry_by_userdata(CueL,cea[x]);
		}

	for(x=start;x<=end;x++)
		{
		add_list_entry(CueL,cea[x]->line,cea[x]);
		}

	le=find_list_entry_by_index(CueL,0);
	ce=(struct CueEntry *)le->user_data;
	select_list_entry_by_userdata(CueL,ce);
	show_gadget(CueL);
	refresh_gadget(CueL);

	FreeMem(cea,(y+1)*4);
	edits++;
	Signal(task,1<<preload_sigbit);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
