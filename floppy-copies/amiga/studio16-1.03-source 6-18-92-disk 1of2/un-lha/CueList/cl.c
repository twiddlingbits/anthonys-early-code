#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include <libraries/dos.h>
#include "intuition/intuition.h"
#include "clib/macros.h"
#include "/include/psound.h"
#include "cl.data"
#include "cl.h"

/**********************************/
/**********************************/
long _stack = 8000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_CUELIST";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
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
/**********************************/
void DumpSamples();
struct MsgPort *gadlibport;
/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
void SetEndTimes();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void Quit();
void WindGadSize();
void DumpSample();
/**********************************/
/**********************************/
struct Module *mod=NULL;
struct Task *task;
extern struct StudioBase *StudioBase;
struct Window *wind=NULL;
long preload_sigbit=-1;
long smpte_error_sigbit=-1;
long preload_sigmask;
long smpte_error_sigmask;
long SmpteErrorVal=0;
/**********************************/
/**********************************/
#define NUMGADS 5
/**************************************************************/
void PreFunc();
static struct NewSmpteSink NewSMPTEsnk = {0,NULL,PreFunc,NULL};
static struct SmpteSink *SMPTEsnk;
/**************************************************************/
/**************************************************************/
/**************************************************************/
void select_list_entry_by_userdata();
/**************************************************************/
/**************************************************************/
/**************************************************************/
void handle_raw_keys();
struct NewKeyNotice rawkey = {NULL,handle_raw_keys,0};
struct key_notice *keynotice;
/**************************************************************/
/**************************************************************/
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
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	if (mask & preload_sigmask) PreloadSamples();
	if (mask & smpte_error_sigmask) smpte_error();
	if (gadlibport) if (mask & (1<<gadlibport->mp_SigBit)) handle_gadlibevents();
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

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) 
	{
	g = msg->g;
	ReplyMsg(msg);

	if (g==but[0].gad_ptr) ADDfunc();
	if (g==but[1].gad_ptr) DELETEfunc();
	if (g==but[2].gad_ptr) PLAYfunc();
	if (g==but[3].gad_ptr) LoadF();
	if (g==but[4].gad_ptr) SaveF();
	}
}
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;
static BOOL ListChanged=FALSE;

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
		ListChanged=TRUE;
		}

	ReplyMsg(event);
	}

if (ListChanged==TRUE)
	{
	SetEndTimes();
	Signal(task,1<<preload_sigbit);
	ListChanged=FALSE;
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
#define KEY_F1		0x50
#define KEY_F2		0x51
#define KEY_F3		0x52
#define KEY_F4		0x53
#define KEY_F5		0x54
#define KEY_F6		0x55
#define KEY_F7		0x56
#define KEY_F8		0x57
#define KEY_F9		0x58
#define KEY_F10	0x59
#define KEY_UP		0x4c
#define KEY_DOWN	0x4d
#define KEY_RIGHT	0x4e
#define KEY_LEFT	0x4f
/**************************************************************/
extern BOOL KeyOk;
/**************************************************************/
void handle_raw_keys(keynotice,code,qualifier)
unsigned short code;
unsigned short qualifier;
struct new_key_notice *keynotice;
{
void DELETEfunc();
struct Task *CreatePlayProc();
/*** There is a bug here, Basicaly it is possible for 2 tasks to be deleteing
     Entries out of the same list, This could occure if f1 is processed while
     a load (deleteSamples()) was occuring ***/
/** To fix Above I have added KeyOk var */

if (code==0x51 && KeyOk) DELETEfunc();
if (code==0x50 && KeyOk) ADDfunc();
}
/**************************************************************/
/**************************************************************/
struct Gadget *ASGad=&StartTimeGad;
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
struct Gadget *gad;
void Updatefunc();

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	gad=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{HideWind(); return;}
	if (class == GADGETUP)
		{
		Updatefunc();
		if (gad==&StartTimeGad) {ASGad=&VolumeGad; ActivateGadget(&VolumeGad,wind,NULL);}
		if (gad==&VolumeGad) 	{ASGad=&NameGad; ActivateGadget(&NameGad,wind,NULL);}
		if (gad==&NameGad) 	{ASGad=&RemarkGad; ActivateGadget(&RemarkGad,wind,NULL);}
		if (gad==&RemarkGad) 	{ASGad=&StartTimeGad; ActivateGadget(&StartTimeGad,wind,NULL);}
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
void DisplayWind()
{
struct Window *OpenNewWind();
int x,w,s,i;
void Gad2Struct();
void Struct2Line();
struct CueEntry *ce;
void PlayTask();

if ((wind = OpenNewWind(&NewWind,mod->state,PREFERENCES,NULL))!=NULL)
	{
	BroadcastEventParms(GetEventID("SE_INFO_MODULE_OPENED"), mod, 0,0,0,0,0);

	CueList.box_x1=10;
	CueList.box_x2=wind->Width-10;
	CueList.box_y1=24;
	CueList.box_y2=wind->Height-32;

	s=((wind->Width-28)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		but[i].box_x1=(x>>8);
		but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
		but[i].box_y1=wind->Height-15;
		but[i].box_y2=wind->Height-5;
		}

	but[4].notifyme=gadlibport;
	but[3].notifyme=gadlibport;
	but[2].notifyme=gadlibport;
	but[1].notifyme=gadlibport;
	but[0].notifyme=gadlibport;

	create_gadget_chain(wind,&CueList);
	CueL=(struct list *)CueList.gad_ptr;
	show_gadget(CueList.gad_ptr);
	WindGadSize();

	if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) 
		{
		telluser("Cue List:","Memory Allocation Error",NOTICE_RETURN_NOW);
		Quit();
		}

	Gad2Struct(ce);
	Struct2Line(ce);

	add_list_entry(CueL,ce->line,ce);
	select_list_entry_by_userdata(CueL,ce);

	ActivateGadget(&StartTimeGad,wind,NULL);
	ASGad=&StartTimeGad;

	rawkey.w=wind;
	keynotice=(struct key_notice *)create_key_notice(&rawkey);

	NotifyMeOnEvent(se_info_sample_created, mod);
	NotifyMeOnEvent(se_info_sample_deleted, mod);
	NotifyMeOnEvent(se_info_sample_edited,mod);
	SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);
	Signal(task,1<<preload_sigbit);
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
	if (keynotice) delete_key_notice(keynotice);
	if(gadlibport) DeletePort(gadlibport);	gadlibport=0;

	w=wind;
	wind=NULL;
	CloseWind(w);

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
int x,w,s,i;
if (wind)
	{
	RefreshWind(wind);

	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(wind->RPort,12+1,20+1);
	Text(wind->RPort,Top, MIN(56,(wind->Width-45)/8));
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(wind->RPort,12,20);
	Text(wind->RPort,Top, MIN(56,(wind->Width-45)/8));

	new_gadget_size(CueList.gad_ptr,10,24,wind->Width-10,wind->Height-32);

	s=((wind->Width-20)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		new_gadget_size(but[i].gad_ptr,(x>>8),wind->Height-15,(x+w)>>8,wind->Height-5);
		refresh_gadget(but[i].gad_ptr);
		x=x+w+(5<<8);
		}

	StartTimeGad.TopEdge=wind->Height-27;
	Gadget2.TopEdge=wind->Height-27;
	VolumeGad.TopEdge=wind->Height-27;
	NameGad.TopEdge=wind->Height-27;
	RemarkGad.TopEdge=wind->Height-27;
	RemarkGad.Width=(((wind->Width-RemarkGad.LeftEdge-9)>>3)<<3);

	draw_indent(wind->RPort,11,wind->Height-28,102+8,wind->Height-19,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,219,wind->Height-28,246+8,wind->Height-19,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,259,wind->Height-28,398,wind->Height-19,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,RemarkGad.LeftEdge-2,wind->Height-28,RemarkGad.Width+RemarkGad.LeftEdge+1,wind->Height-19,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	RefreshGList(&StartTimeGad,wind,NULL,4);
	refresh_gadget(CueList.gad_ptr);
	}
}
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
void HidePrefWind();
struct Window *DisplayPrefWind();
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

if ((mod=(struct Module *)AddModule(&ModData))==0)	{telluser("Cue List:","Could Not Add Module",0);Quit();}

if (!(mod->state->flags & MS_STATE_INITILIZED)) 
	{
	((struct CueModState *)(mod->state))->PlayFlags=0x07;
	((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded=0x05;
	}

mod->state->flags|=MS_CUSTOM_PREFS;
mod->state->pref_name="Cue List Prefs";
mod->state->pref_display_func=DisplayPrefWind;
mod->state->pref_hide_func=HidePrefWind;

ColorGadList(&CueList);

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

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

if((gadlibport = (struct MsgPort *)CreatePort(0,0))==0) Quit();
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
		SaveF();
		}
	else edits=0;
	}

if (wind) HideWind();
if (mod->state->lock) BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
if (mod) DeleteModule(mod);
if (preload_sigbit!=-1) FreeSignal(preload_sigbit);
if (smpte_error_sigbit!=-1) FreeSignal(smpte_error_sigbit);

IgnoreEvent(se_cmd_hide_window, mod);
IgnoreEvent(se_cmd_select_module, mod);
IgnoreEvent(se_cmd_kill_module, mod);
closelibraries();
exit(0);
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

if(g->active_entry)
	{
	ce=(struct CueEntry *)g->active_entry->user_data;

	if (g->active_entry->text)
		{
		Struct2Gad(ce);
		RefreshGList(&StartTimeGad,wind,NULL,4);
		ActivateGadget(ASGad,wind,NULL);
		}
	}
}
/**************************************************************/
/**************************************************************/
void PLAYfunc() 
{
CreatePlayProc(CueL->active_entry->user_data,PP_TRIGGER_NOW);
}
/**************************************************************/
/**************************************************************/
void Updatefunc() 
{
void Gad2Struct();
void Struct2Line();
int GadStructCmp();
struct CueEntry *ce=(struct CueEntry *)CueL->active_entry->user_data;

if (GadStructCmp(ce))
	{
	DumpSample(ce);

	Gad2Struct(ce);
	Struct2Line(ce);

	hide_gadget(CueL);
	delete_list_entry_by_userdata(CueL,ce);
	add_list_entry(CueL,ce->line,ce);
	show_gadget(CueL);

	select_list_entry_by_userdata(CueL,ce);

	Signal(task,1<<preload_sigbit);
	edits++;
	}
}
/**************************************************************/
/**************************************************************/
void DELETEfunc()
{
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce=(struct CueEntry *)CueL->active_entry->user_data;
int ae;

if (CueL->list_size>1)
	{
	DumpSample(ce);

	ae=CueL->current;
	delete_list_entry_by_userdata(CueL,ce);

	FreeMem(ce,sizeof(struct CueEntry));

	if (ae > CueL->list_size-1) ae = CueL->list_size-1;
	le=find_list_entry_by_index(CueL,ae);
	ce=(struct CueEntry *)le->user_data;
	select_list_entry_by_userdata(CueL,ce);

	Signal(task,1<<preload_sigbit);
	edits++;
	}
else telluser("Cue List:","You must have at least one entry in this list",NOTICE_RETURN_NOW);
}
/**************************************************************/
/**************************************************************/
void ADDfunc() 
{
void Gad2Struct();
void Struct2Line();
struct CueEntry *ce;
if ((ce=(struct CueEntry *)AllocMem(sizeof(struct CueEntry),MEMF_CLEAR))==NULL) {telluser("Cue List:","Memory Allocation Error",NOTICE_RETURN_NOW);return;}

Gad2Struct(ce);
Struct2Line(ce);

add_list_entry(CueL,ce->line,ce);
select_list_entry_by_userdata(CueL,ce);
Signal(task,1<<preload_sigbit);
edits++;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void smpte_error()
{
int x;
struct list_entry *le,*find_list_entry_by_index();
struct CueEntry *ce;
void DumpSample();

if (SmpteErrorVal&((struct CueModState *)(mod->state))->PlayFlags)
	{
	for(x=0;x<CueL->list_size;x++)
		{
		le=find_list_entry_by_index(CueL,x);
		ce=(struct CueEntry *)le->user_data;
		if (ce->flags&CEF_PLAYING) DumpSample(ce);
		}
	Signal(task,1<<preload_sigbit);
	}

/*	BroadcastEventParms(se_cmd_stop_all_playback,0,0,0,0,0,0);
	Signal(task,1<<preload_sigbit);*/
}
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

for(x=0;x<CueL->list_size;x++)
{
le=find_list_entry_by_index(CueL,x);
ce=(struct CueEntry *)le->user_data;

tc=QuerySmpteTime();

if (n<((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded)
	{

	if (ce->End)	/* is sample in openlist? */
		{

/* After - Sample should not be preloaded */
		if (ce->End < tc)
			{
			DumpSample(ce);
			}

/* Durring - Sample must be preloaded */

		else if (ce->Start <= tc && ce->End >= tc )
			{
			if (!ce->task)
				{
				if (ce->End >= AddTimeCode(tc,0x00000200,StudioBase->defaults.smpte_mode) )
					{
					CreatePlayProc(ce,NULL);
					n++;
					}
				}
			else	n++;
			}

/* Before */

		else if (ce->Start > tc)
			{
			if (ce->Start < AddTimeCode(tc,PRELOAD_TIME,StudioBase->defaults.smpte_mode)) 
				{
				n++;
				if(!ce->task) CreatePlayProc(ce,NULL);
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
while (ce->task)
	{
	Disable();
	if (ce->task)
		{
		Signal(ce->task,ce->UnloadSigMask);
		Enable();
		WaitTOF();WaitTOF();
		WaitTOF();WaitTOF();
		}
	else	Enable();
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
