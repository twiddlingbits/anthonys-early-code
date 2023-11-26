#include <exec/types.h>
#include <exec/exec.h>
#include <exec/libraries.h>
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "exec/execbase.h"

/** To access the StudioBase, a module must obtain a Shared or **/
/** an Exclusive semaphore on the StudioBase->lockstudio.  This **/
/** applies to all lists in StudioBase, EXCEPT the NotfiyArray, **/
/** CountArray.  These two arrays are access by Forbid()/Permit() **/
/** for speed.	 As such, they can't be accessed inside an inturrupt. **/
/** Access to SendMsg() resources is also performed inside **/
/** Permit/Forbid **/

extern struct ExecBase *SysBase;

#define NUM_MSGS 200

/** modules that want to hear about a certain event **/

struct MinList NotifyArray[MAX_EVENT_TYPE];	

struct SignalSemaphore loadmodlock;
/*struct SignalSemaphore sendmsglock;*/
struct MinList CountArray[MAX_EVENT_TYPE];
char *EventIDArray[MAX_EVENT_TYPE];
struct MsgPort *studio_reply_port;
struct Task *tc_replytask;
unsigned long reply_kill_mask;
struct StudioBase *StudioBase;
struct MinList FreeEventStructs;
extern struct StudioBase LibBase;

short se_cmd_select_module;
short se_info_state_added;
short se_info_state_deleted;
short se_info_module_added;
short se_info_module_deleted;
short se_info_file_filter_added;
short se_info_file_filter_deleted;
short se_info_sample_created;
short se_info_sample_deleted;
short se_info_event_count;
short se_info_preload_done;
short se_info_presave_done;
short se_info_playback_done;
short se_info_record_done;
short se_info_block_recorded;
short se_info_block_played;
short se_cmd_trigger_record;
short se_cmd_trigger_playback;
short se_cmd_stop_recording;
short se_cmd_stop_playing;

/***** functions *********/

void RemStudioLib();
BOOL InitStudioLib();
void MsgReplyHandler();
void BroadcastEventParms();
void NotifyMeOnEvent();
void ClearNotifyList();
void IgnoreEvent();
void LoadDefaults();
void SetOriginalDefaults();
void SendMsg();
void InitSendMsg();
void UseSendMsg();
void FreeAllSendMsgs();
struct StudioEvent *AddFreeEvent();
void BroadcastCount();
void Delay();
struct ChanKey *GetKey();
struct ChanKey *AllocChan();
void FreeChan();
void InitModState();
int LoadModState();
int SaveModState();
void SaveWindState();


/**************************************************************/

void RemStudioLib()
{
struct MinNode *node;
struct Node *bignode;
struct Disk_Samp *s;
struct Module *mod;
struct StandardModState *state;
int i;

node=StudioBase->samps.mlh_Head;
while (node->mln_Succ) {
	s=(struct Disk_Samp *)node;
	node = node->mln_Succ;
	DeleteSample(s);	
	}

/*** Free any module structures that are still around **/

node=StudioBase->modules.mlh_Head;
while (node->mln_Succ) {
	mod=(struct Module *)node;
	node = node->mln_Succ;
	DeleteModule(mod);
	}

/** Free ModState structures **/

bignode=StudioBase->module_states.lh_Head;
while (bignode->ln_Succ) {
	state=(struct StandardModState *)bignode;
	bignode = bignode->ln_Succ;
	DeleteModState(state);
	}

/** Delete the task that removes all replyed messages **/

Signal(tc_replytask,reply_kill_mask);
while (studio_reply_port)
	WaitTOF();

/** Free all memory used for StudioEvent structures **/

FreeAllSendMsgs();

/** Clear any remaining NotifyLink structures **/

for (i=0; i < StudioBase->max_registered_ids; i++)
	ClearNotifyList(i);

/** Free memory use for Event names **/

for (i = 0; i < StudioBase->max_registered_ids; i++) 
	FreeMem(EventIDArray[i], strlen(EventIDArray[i])+1);

/** Delete SlowFreeMem() task, port, etc. **/

UnInitSlowFreeMem();

if (!(StudioBase->flags&SB_APP_SCREEN)) {
	if (StudioBase->back_window)
		CloseWindow(StudioBase->back_window);

	if (StudioBase->screen)
		CloseScreen(StudioBase->screen);
	}
}

/**************************************************************/

BOOL InitStudioLib()
{
int i;

StudioBase = &LibBase;

if (sizeof(struct StudioBase) > 1200) /** see libtool.src ds.l 300 **/
	Alert(0x35000000,FindTask(0));	/** AN_Unknown **/
setmem((char *)StudioBase+sizeof(struct Library),sizeof(struct StudioBase)-sizeof(struct Library),0);
/***** TODD ****/
NewList(&StudioBase->SmpteSources);
NewList(&StudioBase->SmpteSinks);
/***** TODD ****/

NewList(&StudioBase->modules);
NewList(&StudioBase->regions);
NewList(&StudioBase->samps);
NewList(&StudioBase->cue_events);
NewList(&StudioBase->handlers);
NewList(&StudioBase->file_filters);
NewList(&StudioBase->module_states);
InitSharedSemaphore(&StudioBase->lockstudio);
InitSemaphore(&loadmodlock);
/*InitSemaphore(&sendmsglock);*/
SetOriginalDefaults();
StudioBase->eventidarray=&EventIDArray[0];
StudioBase->notifyarray=&NotifyArray[0];

/** allocate all events used by library (like se_info_sample_created) **/

if (AllocInternalEvents()==-1) {
	return(FALSE);
	}

tc_replytask=(struct Task *)CreateTask("Studio16MsgReplys",1,MsgReplyHandler,4096);
if (tc_replytask==0) {
	for (i = 0; i < StudioBase->max_registered_ids; i++) 
		FreeMem(EventIDArray[i], strlen(EventIDArray[i])+1);
	return(FALSE);
	}

while (studio_reply_port==0) WaitTOF();

InitSendMsg();
InitSlowFreeMem();
return(TRUE);
}

/**************************************************************/

int AllocInternalEvents()
{
if ((se_cmd_select_module=GetEventID("SE_CMD_SELECT_MODULE"))==-1)
	return(-1);
if ((se_info_state_deleted=GetEventID("SE_INFO_STATE_DELETED"))==-1)
	return(-1);
if ((se_info_state_added=GetEventID("SE_INFO_STATE_ADDED"))==-1)
	return(-1);
if ((se_info_module_added=GetEventID("SE_INFO_MODULE_ADDED"))==-1)
	return(-1);
if ((se_info_module_deleted=GetEventID("SE_INFO_MODULE_DELETED"))==-1)
	return(-1);
if ((se_info_sample_created=GetEventID("SE_INFO_SAMPLE_CREATED"))==-1)
	return(-1);
if ((se_info_sample_deleted=GetEventID("SE_INFO_SAMPLE_DELETED"))==-1)
	return(-1);
if ((se_info_event_count=GetEventID("SE_INFO_EVENT_COUNT"))==-1)
	return(-1);
if ((se_info_file_filter_added=GetEventID("SE_INFO_FILE_FILTER_ADDED"))==-1)
	return(-1);
if ((se_info_file_filter_deleted=GetEventID("SE_INFO_FILE_FILTER_DELETED"))==-1)
	return(-1);
if ((se_info_preload_done=GetEventID("SE_INFO_PRELOAD_DONE"))==-1)
	return(-1);
if ((se_info_presave_done=GetEventID("SE_INFO_PRESAVE_DONE"))==-1)
	return(-1);
if ((se_info_playback_done=GetEventID("SE_INFO_PLAYBACK_DONE"))==-1)
	return(-1);
if ((se_info_record_done=GetEventID("SE_INFO_RECORD_DONE"))==-1)
	return(-1);
if ((se_info_block_recorded=GetEventID("SE_INFO_BLOCK_RECORDED"))==-1)
	return(-1);
if ((se_info_block_played=GetEventID("SE_INFO_BLOCK_PLAYED"))==-1)
	return(-1);
if ((se_cmd_trigger_playback=GetEventID("SE_CMD_TRIGGER_PLAYBACK"))==-1)
	return(-1);
if ((se_cmd_trigger_record=GetEventID("SE_CMD_TRIGGER_RECORD"))==-1)
	return(-1);
if ((se_cmd_stop_playing=GetEventID("SE_CMD_STOP_PLAYING"))==-1)
	return(-1);
if ((se_cmd_stop_recording=GetEventID("SE_CMD_STOP_RECORDING"))==-1)
	return(-1);
return(0);
}
/**************************************************************/

void MsgReplyHandler()
{
struct StudioEvent *e;
unsigned long port_mask;
unsigned long mask;

reply_kill_mask=1<<AllocSignal(-1);	/** Should always return valid bit **/
studio_reply_port=(struct MsgPort *)CreatePort(0,0);	/** LOOK FOR ERRORS!!!! **/
port_mask=1<<studio_reply_port->mp_SigBit;

do {
	mask=Wait(port_mask|reply_kill_mask);
	while (e = (struct StudioEvent *)GetMsg(studio_reply_port)) {
		UseSendMsg(e);
		}
	} while (!(mask&reply_kill_mask));

DeletePort(studio_reply_port);
studio_reply_port=NULL;
}

/*############################################################
NAME	
	BroadcastEventParms -

FUNCTION
	Sends an event message to all all modules who are listening for
	the type being broadcast.	See GetEventID() for an example.
INPUTS
	type - numeric value of message type. Returned from GetEventID()
	arg1 - arg5 message dependant arguments
	flags - message dependant flags
RESULTS
	NONE
##############################################################*/

void BroadcastEventParms(type, arg1, arg2, arg3, arg4, arg5, flags)

short type;
unsigned long arg1;
unsigned long arg2;
unsigned long arg3;
unsigned long arg4;
unsigned long arg5;
unsigned long flags;

{
struct MinNode *node;

/*ObtainSharedSemaphore(&StudioBase->lockstudio);*/
Forbid();
for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	SendMsg(((struct NotifyLink *)node)->notifyme, type, arg1, arg2, arg3, arg4, arg5, flags);
	}
/*ReleaseSharedSemaphore(&StudioBase->lockstudio);*/
Permit();
}

/*############################################################
NAME	
	BroadcastEventParmsLessOne -

FUNCTION
	Sends an event message to all modules who are listening for
	the type being broadcast except the calling module.
	
INPUTS
	type - numeric value of message type. Returned from GetEventID()
	thismod - Don't send the message to this module
	arg1 - arg5 message dependant arguments
	flags - message dependant flags
RESULTS
	NONE
##############################################################*/

void BroadcastEventParmsLessOne(type, thismod, arg1, arg2, arg3, arg4, arg5, flags)

short type;
struct Module *thismod;
unsigned long arg1;
unsigned long arg2;
unsigned long arg3;
unsigned long arg4;
unsigned long arg5;
unsigned long flags;

{
struct MinNode *node;

/*ObtainSharedSemaphore(&StudioBase->lockstudio);*/
Forbid();
for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	if (thismod!=((struct NotifyLink *)node)->notifyme)
		SendMsg(((struct NotifyLink *)node)->notifyme, type, arg1, arg2, arg3, arg4, arg5, flags);
	}
/*ReleaseSharedSemaphore(&StudioBase->lockstudio);*/
Permit();
}

/*############################################################
NAME	
	NotifyMeOnEvent -
FUNCTION
	Tells studio16 that you want to recieve events of a certian
	type when they are broadcast.
INPUTS
	type ID of event you wish to receive
	module the module that wants to receive events
RESULTS
	NONE
##############################################################*/

void NotifyMeOnEvent(type, module)
short type;
struct Module *module;
{
struct NotifyLink *newlink;

if (type >= StudioBase->max_registered_ids || type < 0) {
	return;
	}
	
newlink = (struct NotifyLink *)AllocMem(sizeof(struct NotifyLink),MEMF_CLEAR);
if (newlink) {
	newlink->notifyme=module;
/* ObtainExclusiveSemaphore(&StudioBase->lockstudio);*/
	Forbid();
	AddTail(&NotifyArray[type], newlink);
	BroadcastCount(type);
/* ReleaseExclusiveSemaphore(&StudioBase->lockstudio);*/
	Permit();
	}
}

/**************************************************************/
/** Call to this must be made inside a forbid/permit lock! **/
void BroadcastCount(type)
short type;
{
struct MinNode *node;
short count;

if (CountArray[type].mlh_Head->mln_Succ) {
	count=0;
	for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ)
		count++;
	for (node=CountArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
		SendMsg(((struct NotifyLink *)node)->notifyme, se_info_event_count, type, count, 0, 0,0, 0);
		}
	}
}

/*############################################################
NAME	
	NotifyMeOnCount -
FUNCTION
	Tells studio16 to inform a module every time the number of
	modules listening for a particular ID changes.	The module
	wanting notices will receive messages of type "SE_INFO_EVENT_COUNT"
	(arg1 is the event type and arg 2 is the number of modules
	listening for event)
INPUTS
	type - message type to keep track of listners
	module - module to recieve SE_INFO_EVENT_COUNT messages
RESULTS
	NONE
##############################################################*/

void NotifyMeOnCount(type, module)
short type;
struct Module *module;
{
struct NotifyLink *newlink;
struct MinNode *node;
short count;

if (type >= StudioBase->max_registered_ids || type < 0) {
	return;
	}
	
newlink = (struct NotifyLink *)AllocMem(sizeof(struct NotifyLink),MEMF_CLEAR);
if (newlink) {
	newlink->notifyme=module;
/* ObtainExclusiveSemaphore(&StudioBase->lockstudio);*/
	Forbid();
	AddTail(&CountArray[type], newlink);
	count=0;
	for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ)
		count++;
	SendMsg(module, se_info_event_count, type, count, 0, 0,0, 0);
/* ReleaseExclusiveSemaphore(&StudioBase->lockstudio);*/
	Permit();
	}
}

/*############################################################
NAME	
	IgnoreCount-
FUNCTION
	Tells studio16 to quit sending SE_INFO_EVENT_COUNT messages
	to a specifed module.
INPUTS
	type - type of messages to quit counting
	module - module to quit receiving messages
RESULTS
	NONE
##############################################################*/

void IgnoreCount(type, module)
short type;
struct Module *module;
{
struct MinNode *node, *newnode;

if (type >= StudioBase->max_registered_ids || type < 0) 
	return;

/*ObtainExclusiveSemaphore(&StudioBase->lockstudio);*/
Forbid();
node=CountArray[type].mlh_Head;
while(node->mln_Succ) {
	newnode = node->mln_Succ;
	if (module==(((struct NotifyLink *)node)->notifyme)) {
		Remove(node);
		FreeMem(node, sizeof(struct NotifyLink));
		}
	node=newnode;
	}
/*ReleaseExclusiveSemaphore(&StudioBase->lockstudio);*/
Permit();
}

/*############################################################
NAME	
	IgnoreEvent -
FUNCTION
	Tells studio16 to quit sending events of the specified type
	to the specified module.  For example, if you have been listening
	to smpte messages, but don't want to any more you could say

		IgnoreEvent(se_info_smpte, thismodptr);

	where se_info_smpte was a short filled in with a call to
	GetEventID(); 
INPUTS
	type ID of message type to quit receiving
	module pointer to module that doesn't want messages any more
RESULTS
	NONE
##############################################################*/

void IgnoreEvent(type, module)
short type;
struct Module *module;
{
struct MinNode *node, *newnode;

if (type >= StudioBase->max_registered_ids || type < 0) 
	return;

/*ObtainExclusiveSemaphore(&StudioBase->lockstudio);*/
Forbid();
node=NotifyArray[type].mlh_Head;
while(node->mln_Succ) {
	newnode = node->mln_Succ;
	if (module==(((struct NotifyLink *)node)->notifyme)) {
		Remove(node);
		FreeMem(node, sizeof(struct NotifyLink));
		}
	node=newnode;
	}
BroadcastCount(type);
/*ReleaseExclusiveSemaphore(&StudioBase->lockstudio);*/
Permit();
}

/*############################################################
NAME	
	ClearNotifyList-
FUNCTION
	Tells studio16 to stop sending events of a specified type
	to everyone currently listening for them.	 Be carefull using this
	since it may cause modules expecting certain events to
	suddenly quit receiving them without notice.
INPUTS
	type ID of event to quit broadcasting
RESULTS
	NONE
##############################################################*/

void ClearNotifyList(type)
short type;
{
struct MinNode *node, *newnode;

if (type >= StudioBase->max_registered_ids || type < 0) {
	return;
	}

/*ObtainExclusiveSemaphore(&StudioBase->lockstudio);*/
Forbid();
node=NotifyArray[type].mlh_Head;
while (node->mln_Succ) {
	newnode = node->mln_Succ;
	Remove(node);
	FreeMem(node, sizeof(struct NotifyLink));
	node=newnode;
	}
/*ReleaseExclusiveSemaphore(&StudioBase->lockstudio);*/
Permit();
}

/*############################################################
NAME	
	GetEventID -

FUNCTION
	Takes a pointer to a string and returns its ID number.  In 
	Studio 16, "Events" are used to communicate between modules.
	In order to broadcast an event, you must get its ID. ID's are
	assigned dynamicaly at runtime for maximum flexability.	For
	example, to play a sample you could use the sequence:
	
	int playid;
	playid = GetEventID("SE_CMD_PLAY_SAMPLE");
	if (playid!=-1) {
		BroadcastEventParms(playid, samp, 0, samp->length, 0,0,0);
		}

INPUTS
	name	pointer to the name of a studio event
RESULTS
	Event id (int) or -1 if out of memory
##############################################################*/

GetEventID(name)
char *name;
{
int index;
int val;

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (index = 0; index < StudioBase->max_registered_ids; index++) {
	if (strcmp(EventIDArray[index], name)==0) {
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
		return(index);
		}
	}

ObtainExclusiveSemaphore(&StudioBase->lockstudio);

if (index==MAX_EVENT_TYPE) {
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	ReleaseSharedSemaphore(&StudioBase->lockstudio);
	Alert(0x35000000,FindTask(0));	/** AN_Unknown **/
	return(-1);
	}
else {
	EventIDArray[StudioBase->max_registered_ids]=(char *)AllocMem(strlen(name)+1,0);
	if (EventIDArray[StudioBase->max_registered_ids]) {
		NewList(&NotifyArray[StudioBase->max_registered_ids]);
		NewList(&CountArray[StudioBase->max_registered_ids]);
		strcpy(EventIDArray[StudioBase->max_registered_ids++], name);
		val=StudioBase->max_registered_ids-1;
		ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
		if (val >=MAX_EVENT_TYPE)
			Alert(0x35000000,FindTask(0));	/** AN_Unknown **/
		return(val);
		}
	else {
		ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
		ReleaseSharedSemaphore(&StudioBase->lockstudio);
		return(-1);
		}
	}
}


/*############################################################
NAME
	LoadDefaults -
FUNCTION
	trys to load defaults from the specified file.	Defaults
	are things like screen size, colors.  Buffer sizes, etc.
	(all things in MasterPrefs.)
INPUTS	
	ARGS:
		1	pointer to string containing complete path of file
			containing defaults to try and load.  NULL means use
			default ("s:studio16base.config")
RETURNS
	NONE
##############################################################*/

void LoadDefaults(file)
char *file;		/** complete path/file pointing to defaults or NULL **/
{
long fp;

if (file==NULL)
	file = "s:Studio16Base.config";

fp = Open(file, MODE_OLDFILE);
if (fp) {
	Read(fp, &StudioBase->defaults, sizeof(StudioBase->defaults));
	Close(fp);
	}
}

/*############################################################
NAME
	SaveDefaults -
FUNCTION
	saves the current StudioBase.defaults to disk
INPUTS	
	ARGS:
		1	pointer to string containing complete path of file
			containing defaults to try and write.	Zero means
			use default name.
RETURNS
	NONE
##############################################################*/

void SaveDefaults(file)
char *file;		/** complete path/file pointing to defaults or NULL **/
{
long fp;
struct ViewPort *vport;
short i;

if (file==NULL)
	file = "s:Studio16Base.config";

fp = Open(file, MODE_NEWFILE);
if (fp) {
	vport=&StudioBase->screen->ViewPort;
	for (i=0; i < 16; i++)
		StudioBase->defaults.palette[i]=GetRGB4(vport->ColorMap,i);
	Write(fp, &StudioBase->defaults, sizeof(StudioBase->defaults));
	Close(fp);
	}
}

/****************************************************************/
#if 0			/*** ORIGINAL BROWN COLORS! ***/
#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,LIGHT_BROWN,WHITE,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BLUE,BLACK,BLACK}
#define DROPCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,WHITE,BLACK}
#define WINDCOLORS {BROWN,WHITE,DARK_BROWN,BROWN,LIGHT_BROWN,BLACK,BLACK}
#endif

#define GCL 4
#define DARK	6
#define LITE	1
#define MEAD	2
#define DCL 3
#define ECL 5

#define LISTCOLORS {MEAD,DARK,LITE,/*GCL*/MEAD,DCL,ECL,ECL}
#define KNOBCOLORS {MEAD,LITE,DARK,GCL,DCL,ECL,ECL}
#define BUTNCOLORS {MEAD,LITE,DARK,GCL,DCL,ECL,ECL}
#define DROPCOLORS {MEAD,LITE,DARK,GCL,DCL,ECL,ECL}
#define WINDCOLORS {MEAD,LITE,DARK,MEAD,7,LITE,LITE}

/*############################################################
NAME
	SetOriginalDefaults -
FUNCTION
	Sets up StudioBase.defaults to the original factory settings.
	Calling this will trash any user set defaults.
INPUTS	
	NONE
RETURNS
	NONE
##############################################################*/

void SetOriginalDefaults()
{
short i;
struct Screen def_screen;
static struct GadgetColors defcolors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	DROPCOLORS,
	WINDCOLORS};

static USHORT Palette[16] = {
0x0845,0x0edc,0x0788,0x0cb8,0x0AAA,0x0003,0x0334,0x068a,
	0,0,0,0,0,0,0,0		/** 8-15 not used **/
	};

for (i=0; i< 16; i++)
	StudioBase->defaults.palette[i] = Palette[i];

StudioBase->defaults.smpte_mode = SysBase->VBlankFrequency/2;

strcpy(StudioBase->defaults.samp_io_path,"");
strcpy(StudioBase->defaults.temp_files_path,"RAM:");
StudioBase->defaults.instance_config_file[0]=NULL;

if (GetScreenData(&def_screen, sizeof(struct Screen), WBENCHSCREEN, 0)) {
	StudioBase->defaults.screen_width = def_screen.Width;
	if (def_screen.Height >= 400)
		StudioBase->defaults.screen_viewmodes = LACE;
	if (def_screen.Width >= 200)
		StudioBase->defaults.screen_viewmodes |= HIRES;
	}
else {
	StudioBase->defaults.screen_width = 640;
	StudioBase->defaults.screen_viewmodes = HIRES;
	}
StudioBase->defaults.colors = defcolors;
StudioBase->defaults.copy_buf_size = 32768;		/* in bytes */
StudioBase->defaults.active_buf_size = 32768;	/* in words! */
StudioBase->defaults.undo_levels=1; /** currently must be 1 or > **/
StudioBase->defaults.SmpteTimeOut=500;
StudioBase->defaults.MeterSpeed=90;
StudioBase->defaults.record_safety_size=128*1024;

}

/*############################################################
NAME	
	SendMsg -
FUNCTION
	Sends a message to one particular module.	 Use this if you
	know that only one specific module is waiting for a message.
	For example, Modlist sometimes uses this to send out
	SE_CMD_KILL_MODULE messages.	This reduces the message traffic by not
	sending SE_CMD_KILL_MODULE events to modules that don't need them.  
	However, this is also dangerous in that a module may be 
	interested in SE_CMD_KILL_MODULE events directed to other modules.
	Since SE_CMD_KILL_MODULE events are sometimes sent using SendMsg()
	and not BroadcastEventParms(), third party modules can't monitor
	SE_CMD_KILL_MODULE reliably. Use this function carefully.
INPUTS
	mod - module to receive message
	type - flags same as BroadcastEventParms()
RESULTS
	NONE
##############################################################*/

void SendMsg(mod, type, arg1, arg2, arg3, arg4, arg5, flags)
struct Module *mod;
ULONG arg1, arg2, arg3, arg4, arg5,flags;
short type;
{
struct StudioEvent *e;
struct MinNode *node;

/*ObtainSemaphore(&sendmsglock);*/
Forbid();
node = FreeEventStructs.mlh_Head;
if (node->mln_Succ==NULL) {
	e=AddFreeEvent();
	}
else {
	e = (struct StudioEvent *)((char *)node - sizeof(struct StudioEvent) + sizeof (struct MinNode));
	}

if (e) {
	e->type = type; 
	e->arg1 = arg1;
	e->arg2 = arg2;
	e->arg3 = arg3;
	e->arg4 = arg4;
	e->arg5 = arg5;
	e->flags |= flags;
	Remove(&e->node);				/** take out of free msg list **/
	PutMsg(mod->notifyme, e);
	}
/*ReleaseSemaphore(&sendmsglock);*/
Permit();
}

/**************************************************************/

void FreeAllSendMsgs()
{
struct MinNode *node;
struct StudioEvent *e;

/*ObtainSemaphore(&sendmsglock);*/
Forbid();
node = FreeEventStructs.mlh_Head;
while (node->mln_Succ) {
	Remove(node);
	e = (struct StudioEvent *)((char *)node - sizeof(struct StudioEvent) + sizeof (struct MinNode));
	FreeMem(e, sizeof(struct StudioEvent));
	node = FreeEventStructs.mlh_Head;
	}
/*ReleaseSemaphore(&sendmsglock);*/
Permit();
}

/**************************************************************/

void InitSendMsg()
{
int i;

NewList(&FreeEventStructs);

for (i=0; i < NUM_MSGS; i++) {
	AddFreeEvent();
	}
}

/**************************************************************/

struct StudioEvent *AddFreeEvent()
{
struct StudioEvent *e;

e=(struct StudioEvent *)AllocMem(sizeof(struct StudioEvent), MEMF_CLEAR);
if (e) {
	e->message.mn_Node.ln_Type = NT_MESSAGE;
	e->message.mn_ReplyPort = studio_reply_port;
	e->message.mn_Length = sizeof(struct StudioEvent);
	UseSendMsg(e);
	}
return(e);
}

/*************************************************************/

void UseSendMsg(e)
struct StudioEvent *e;
{
/*ObtainSemaphore(&sendmsglock);*/
Forbid();
AddTail(&FreeEventStructs,&e->node);	/** e->node is at bottom of StudioEvent! **/
/*ReleaseSemaphore(&sendmsglock);*/
Permit();
}

void Delay(x)
{

while (x--)
	WaitTOF();
}

/*############################################################
NAME	
	AddAudioHandler -
FUNCTION
	Given a NewAudioHandler struct, this function creates a new
	entry in the StudioBase->handlers List.  This funciton should
	be called by any Module who's job is to be a device driver
	for an audio card designed to be used by studio 16.

	Note that name's pointer is copied but not its text.  Therefore,
	the name's text should be declared static by the module 
	initilizing NewAudioHandler.	This is in contrast to other
	studio 16 functions like AddModule() which make a copy of the
	text.
INPUTS
	pointer to NewAudioHandler struct (see studio.h)
RESULTS
	pointer to AudioHandler struct or NULL if not enough memory
##############################################################*/

struct AudioHandler *AddAudioHandler(nh)

struct NewAudioHandler *nh;
{
struct AudioHandler *ah;

ah = (struct AudioHandler *)AllocMem(sizeof(struct AudioHandler),MEMF_CLEAR);
if (ah) {
	ah->name=nh->name;
	ah->hardware=nh->hardware;
	ah->handler_mod=nh->handler_mod;
	ah->flags=nh->flags;
	ah->num_channels[AHC_PLAY]=nh->play_channels;
	ah->num_channels[AHC_RECORD]=nh->record_channels;
	ah->num_channels[AHC_MONITOR]=nh->monitor_channels;
	ah->num_channels[AHC_OUTPUT]=nh->output_channels;
	ah->num_bits=nh->num_bits;
	ah->node.ln_Pri=nh->priority;
	ah->class_id=nh->class_id;
	ObtainExclusiveSemaphore(&StudioBase->lockstudio);
	Enqueue(&StudioBase->handlers, ah);
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	}
return(ah);
}

/*############################################################
NAME	
	DeleteAudioHandler -
FUNCTION
	Removes an AudioHandler from the system's list.   Call
	DeleteHandler() as part of a device driver's close down sequence,
	along with such functions as DeleteModule().
INPUTS
	pointer to the AudioHandler to remove.
RESULTS
	NONE
##############################################################*/

void DeleteAudioHandler(ah)
struct AudioHandler *ah;
{
struct StandardModState *s;
struct Node *node;

ObtainExclusiveSemaphore(&StudioBase->lockstudio);
Remove(ah);
FreeMem(ah,sizeof(struct AudioHandler));

/** zero all references to this handler! **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) {
	s=(struct StandardModState *)node;
	if (s->hand==ah)
		s->hand=NULL;
	}
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
}	
	
/*############################################################
NAME	
	FindAudioHandler -
FUNCTION
	Searchs for an Amiga audio handler that matches the
	passed parameters
INPUTS
	last_han		Start searching with this handler (do not include
					this handler in the search.) NULL means start
					at beginning of list.
	flags			Search for a handler that matches these flags.
					eg. CK_CHAN_PLAY.	 If you don't care about flags,
					set it to -1;
	num_bits		Search for a handler that is the specified number
					of bits.	 Set to zero if you don't care how
					many bits.
	name			Search for a handler with this name.  NULL if
					you don't care about the name.
	class_id		Search for this particular instance of a handler
					with name 'name'.	 For example, if you had
					two AD1012 cards, there would be two handlers.
					Their class_id's would be 0 & 1.  Use -1 for
					a wildcard.

EXAMPLE
	To search for all 12 bit handlers in the system you would use:

	han = NULL;
	do {
		han = FindAudioHandler(han,-1,12,NULL,-1);
		if (han)
			printf("found han %x %s\n",han,han->name);
		} while (han);

RESULTS
	NULL or the audio handler that matches the request
##############################################################*/

struct AudioHandler *FindAudioHandler(last_han, flags, num_bits, name, class_id)
struct AudioHandler *last_han;
unsigned long flags;
short num_bits;
short class_id;
char *name;
{
struct Node *node;
struct AudioHandler *h;

if (last_han) {
	node = (struct Node *)last_han;
	node = node->ln_Succ;
	}
else {
	node=StudioBase->handlers.lh_Head;
	}

while (node->ln_Succ) {
	h = (struct AudioHandler *)node;
	if ((flags&h->flags)==flags || flags==-1)
		if (num_bits==h->num_bits || num_bits==0)
			if (class_id==h->class_id || class_id==-1)
				if (fah_strcmp(name, h->name)==0)
					return(h);
	node = node->ln_Succ;
	}
return(NULL);
}

int fah_strcmp(a,b)
char *a,*b;
{
if (a==0)		/* equal */
	return(0);
if (b==0)
	return(1);	/* Not equal */
return(strcmp(a,b));
}

/**************************************************************/
/** Handle channel allocation/ deallocations **/
/**************************************************************/

/*############################################################
NAME
	AllocChan -

FUNCTION
	Allocate an audio channel for future use.	 This can be a
	record, playback, monitor, or output channel.

	Flags:
	CK_CHAN_PLAY	A playback channel (eg. Four on the AD1012)
	CK_CHAN_RECORD A record channel	 (eg. One on the AD1012)
	CK_CHAN_OUTPUT Allocate the output jack as a record
						channel.	 This allow's you to record the four
						channel mix, or record the ouput of an effect
						that is attached to an output channel.
	CK_CHAN_MONITOR Allocate the "monitor" channel.	 This isn't
						 an actual channel.	By allocating the "monitor"
						 you can adjust monitor volume levels, etc.
	CK_ID_ONLY		 This means that you don't want to actually
						 allocate a resource.  Instead, you want to
						 Generate a ChanKey that ID's a particual
						 channel.  For example, if you want to send
						 SE_SET_CHANNEL_VOLUME commands, you will
						 need to specify a chan key as an argument.
						 You should use CK_ID_ONLY when allocating the
						 key as you don't want the resources, just an
						 identifier.

INPUTS
	Audio handler pointer (NULL for default) or
	Channel number (-1 for any)
	flags indicating channel type wanted
RESULTS
	struct ChanKey *.	 You must remember this pointer since
	you will need it to FreeChan(). You must remember the
	actual pointer.  You can't regenerate it later by 
	Allocating your own ChanKey and filling in it's elements.
##############################################################*/

struct ChanKey *AllocChan(ah, cn, flags)
struct AudioHandler *ah;	/** audio handler or NULL for any */
int cn;		/** channel number or -1 for any **/					
unsigned long flags;
{
short i;
unsigned long ChanBit;
struct Node *node;
unsigned long chanmask;
struct ChanKey *key;
short chantype;


ObtainExclusiveSemaphore(&StudioBase->lockstudio);

chantype=GetChanType(flags);
if (chantype==-1) {
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(NULL);
	}
	
/** first pick an AudioHandler **/

if (flags&CK_ID_ONLY) { /** Take first guy **/
	if (ah==NULL) {
		if (StudioBase->handlers.lh_Head->ln_Succ) {
			ah=(struct AudioHandler *)StudioBase->handlers.lh_Head;
			}
		else {
			ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
			return(NULL);		/** no handlers in system **/
			}
		}
	if (cn==-1)
		cn=0;
	key=GetKey(ah, cn, flags);
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(key);
	}

if (ah==NULL) {
	for (node=StudioBase->handlers.lh_Head; node->ln_Succ; node = node->ln_Succ) {
		ah=(struct AudioHandler *)node;
		chanmask = intpow(ah->num_channels[chantype],2)-1;
		if ((ah->chans_in_use[chantype]&chanmask)!=chanmask) 
			break;
		}
	}

if (ah==NULL) {		/** no handlers in the system! **/
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(NULL);
	}

/** Now pick a free channel **/

if (cn!=-1) {
	ChanBit=(1<<cn);
	if ((ah->chans_in_use[chantype]&ChanBit)==0) {		/** Free channel !! **/
		ah->chans_in_use[chantype] |= ChanBit;
		key=GetKey(ah, cn, flags);
		ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
		return(key);
		}
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(NULL);
	}

else {
	for (i=0; i < ah->num_channels[chantype] ; i++) {
		ChanBit=(1<<i);
		if ((ah->chans_in_use[chantype]&ChanBit)==0) {		/** Free channel !! **/
			ah->chans_in_use[chantype] |= ChanBit;
			key=GetKey(ah, i, flags);
			ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
			return(key);
			}
		}
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	return(NULL);
	}
}

/**************************************************************/
/** convert flags (eg. CK_CHAN_PLAY) to AHC_ (eg. AHC_PLAY) **/

int GetChanType(flags)
unsigned long flags;
{
int chantype;

if (flags&CK_CHAN_PLAY)
	chantype=AHC_PLAY;
else if (flags&CK_CHAN_RECORD)
	chantype=AHC_RECORD;
else if (flags&CK_CHAN_OUTPUT)
	chantype=AHC_OUTPUT;
else if (flags&CK_CHAN_MONITOR)
	chantype=AHC_MONITOR;
else chantype=-1;
return(chantype);
}

/**************************************************************/

/** Call Obtain/Release Semaphore around this **/

struct ChanKey *GetKey(ah, cn, flags)

struct AudioHandler *ah;
unsigned long cn;
unsigned long flags;
{
struct ChanKey *key;
short type;

key=(struct ChanKey *)AllocMem(sizeof(struct ChanKey),MEMF_CLEAR);
if (key) {
	key->handler=ah;
	key->chan_num=cn;
	key->chan_mask=(1<<cn);
	key->flags=flags;
	}
else {
	type = GetChanType(flags);
	if (type!=-1)
		ah->chans_in_use[type] &= (~(1<<cn));
	}
return(key);
}

/*############################################################
NAME	
	FreeChan -

FUNCTION
	Frees a previously allocated channel.
INPUTS
	*key returned by AllocChan()
RESULTS
	NONE
##############################################################*/

void FreeChan(key)
struct ChanKey *key;
{
if (key==0)
	return;
	
ObtainExclusiveSemaphore(&StudioBase->lockstudio);
if (!(key->flags&CK_ID_ONLY))
	key->handler->chans_in_use[GetChanType(key->flags)] &= (~(1<<key->chan_num));
ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
SlowFreeMem(key, sizeof (struct ChanKey));
}

/**************************************************************/

int intpow (exponent,base)
int exponent,base;
{
register int i,z;

z=1;
for (i=0;i<exponent;i++)
	z=z*base;
return (z);
}

/*############################################################
NAME	
	UpdateStandardModState(m,w) -

FUNCTION
	OBSOLETE!!!
	Copies parameters into the m->state structure
INPUTS
RESULTS
	NONE
##############################################################*/

void UpdateStandardModState(m,w)
struct Module *m;
struct Window *w;
{
if (w) {
	m->state->win_le=w->LeftEdge;
	m->state->win_te=w->TopEdge;
	m->state->win_w=w->Width;
	m->state->win_h=w->Height;
	m->state->flags |= MS_WINDOW_OPEN;
	}
else {
	m->state->flags &= (~MS_WINDOW_OPEN);
	}
}

/*############################################################
NAME
	RestoreWindowParms-

FUNCTION
	OBSOLETE!!!
INPUTS
RESULTS
	NONE
##############################################################*/


void RestoreWindowParms(m,nw)
struct Module *m;
struct NewWindow *nw;
{
nw->LeftEdge	=m->state->win_le;
nw->TopEdge		=m->state->win_te;
nw->Width		=m->state->win_w;
nw->Height		=m->state->win_h;
}

/*############################################################
NAME	
	LoadModule -

FUNCTION
	Given a module name, this function will load its code
	and launch it as a new process.
INPUTS
	char * name		module file name (Studio16:runtime is added)
RESULTS
	struct *Module
##############################################################*/

struct Module *LoadModule(name)
char *name;
{
struct Module *mod;
char path[150];
short i;
long nil;

/** Only one call can be processed at a time **/

ObtainSemaphore(&loadmodlock);

/** First launch the module async. **/

sprintf(path,"Studio16:runtime/%s",name);
nil = Open("NIL:",MODE_NEWFILE);
Execute(path,0L,nil);		/** zero nil is sort of ok if Open failed **/
if (nil) {
	Close(nil);
	}

/** Now wait for the module to register itself.	 We are assuming **/
/** that the modname==file name! This should be true **/
for (i=0; i < 150; i++) {	/** timeout in 10 seconds **/
	WaitTOF();WaitTOF();		/** 1/15 sec delay **/
	if (mod=(struct Module *)FindLatestModule(name))
		break;
	}
ReleaseSemaphore(&loadmodlock);

return(mod);
}

/*############################################################
NAME	
	WaitUntilListening -

FUNCTION
	Wait for the indicated module to ask for notification on
	the passed event type before returning. (Specified mod
	must call NotifyMeOnEvent(type, mod)). Currently will
	TimeOut in 8 seconds.
INPUTS
	module, event type
RESULTS
	NONE
##############################################################*/

void WaitUntilListening(type, mod)
short type;
struct Module *mod;
{
short i;

if (mod) {
	for (i=0; i < 120; i++) {	/** timeout in 8 seconds **/
		WaitTOF();WaitTOF();		/** 1/15 sec delay **/
		if (IsListening(mod, type))
			break;
		}
	WaitTOF();WaitTOF();		/** just for good measure **/
	}
}

/**************************************************************/

IsListening(mod,type)
struct Module *mod;
short type;
{
struct MinNode *node;

/*ObtainSharedSemaphore(&StudioBase->lockstudio);*/
Forbid();
for(node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	if (((struct NotifyLink *)node)->notifyme==mod) {
/*		ReleaseSharedSemaphore(&StudioBase->lockstudio);*/
		Permit();
		return(TRUE);
		}
	}
/*ReleaseSharedSemaphore(&StudioBase->lockstudio);*/
Permit();
return(FALSE);
}

/*############################################################
NAME
	SaveModState -
FUNCTION
	saves the current list of ModStates.  The result is a snap
	shot of what windows are open, closed, etc.	It also saves
	any settings that a module has stored in a custom ModState.
INPUTS	
	ARGS:
		1	pointer to string containing complete path of file
			To write to.  Zero means use default name
			 (s:Studio16Instance.config)
RETURNS
	standard error code.
##############################################################*/


int SaveModState(file)
char *file;
{
struct StandardModState *s;
struct StandardModState *ls=(struct StandardModState *)1;
struct Node *node;
unsigned long fp,zero;
USHORT lowpri;

if (file==NULL)
	file = "s:Studio16Instance.config";
zero = 0;
fp=Open(file,MODE_NEWFILE);
if (fp==0)
	return(CANT_OPEN_FILE);

ObtainSharedSemaphore(&StudioBase->lockstudio);

/** Save Utilities first (device driver's are utilities) **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ)
	{
	s=(struct StandardModState *)node;
	if (s->window) SaveWindState(s->window);	/** Copy W->Width, Height, etc. to ModState **/
	if ((s->flags&MS_UTILITY_MODULE) && !(s->flags&MS_STATE_SAVED)) {SaveOneState(fp, s);s->flags|=MS_STATE_SAVED;}
	}

/** Save Closed Windows **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) 
	{
	s=(struct StandardModState *)node;
	if (!(s->window) && !(s->flags&MS_UTILITY_MODULE) && !(s->flags&MS_STATE_SAVED)) {SaveOneState(fp, s);s->flags|=MS_STATE_SAVED;}
	}

/** Save Open Windows **/

while(ls)
	{
	ls=NULL;
	lowpri=0xFFFF;
	for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) 
		{
		s=(struct StandardModState *)node;
		if ((s->window) && !(s->flags&MS_UTILITY_MODULE) && !(s->flags&MS_STATE_SAVED))
			if ((s->window->RPort->Layer->priority) <= lowpri) 
				{lowpri=s->window->RPort->Layer->priority; ls=s;}
		}

	if (ls) {SaveOneState(fp, ls);ls->flags|=MS_STATE_SAVED;}
	}

/** Miss Anything? **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) 
	{
	s=(struct StandardModState *)node;
	if (!(s->flags&MS_STATE_SAVED)) {SaveOneState(fp, s);s->flags|=MS_STATE_SAVED;}
	}

/** Reset Flag **/

for (node=StudioBase->module_states.lh_Head; node->ln_Succ; node = node->ln_Succ) 
	{
	s=(struct StandardModState *)node;
	s->flags&=~MS_STATE_SAVED;
	}

Write(fp,&zero,sizeof(s->state_size));
Close(fp);
ReleaseSharedSemaphore(&StudioBase->lockstudio);
return(NO_ERROR);
}

/***********************************************************/

int SaveOneState(fp, s)
struct StandardModState *s;
unsigned long fp;
{
short size;

/** size of state structure **/

Write(fp,&s->state_size,sizeof(s->state_size));

/** size of instance_name (2 bytes) followed by text **/

size = strlen(s->instance_name);
Write(fp,&size,2);
Write(fp,s->instance_name,size);

/** size of module_name (2 bytes) followed by text **/

size = strlen(s->module_name);
Write(fp,&size,2);
Write(fp,s->module_name,size);

/** size of handler_name (2 bytes) followed by text **/

if (s->hand)
	size = strlen(s->hand->name);
else 
	size = 0;

Write(fp,&size,2);
if (size)
	Write(fp,s->hand->name,size);

/** now save this handler's class_id so LoadModState can find **/
/** the correct handler and re-assign state->hand **/

if (s->hand)
	size=s->hand->class_id;
else
	size=0;

Write(fp,&size,2);

/** Module struct less node,instance_name,module_name **/

Write(fp, &s->state_size, s->state_size - sizeof(s->node)-8);

return(NO_ERROR);
}

/*############################################################
NAME
	LoadModState -
FUNCTION
	Load's a file containg ModStates and appends them to 
	StudioBase's list.  This is the opposite of SaveModState();
	Existing modstates are not altered.
INPUTS	
	ARGS:
		1	pointer to string containing complete path of file
			To load from.	Zero means use default name
			 (s:Studio16Instance.config)
RETURNS
	standard error code.
##############################################################*/

int LoadModState(file)
char *file;
{
struct StandardModState *s;
unsigned long fp;
short shortval;
short len;
short handlerclass;
char instname[80];
char modname[80];
char handlername[80];

if (file==NULL)
	file = "s:Studio16Instance.config";

fp=Open(file,MODE_OLDFILE);
if (fp==0)
	return(CANT_OPEN_FILE);

len=Read(fp,&shortval,2);
while (len==2 && shortval!=NULL) {

/** Read instance_name text **/

	Read(fp,&len,2);
	if (len >=79) {
		Close(fp);
		return(STDERR_UNKNOWN_FILE_FORMAT);
		}
	Read(fp,instname,len);
	instname[len]=0;

/** Read module_name text **/

	Read(fp,&len,2);
	if (len >=79) {
		Close(fp);
		return(STDERR_UNKNOWN_FILE_FORMAT);
		}
	Read(fp,modname,len);
	modname[len]=0;

/** Read handler_name text **/

	Read(fp,&len,2);
	if (len >=79) {
		Close(fp);
		return(STDERR_UNKNOWN_FILE_FORMAT);
		}
	Read(fp,handlername,len);
	handlername[len]=0;

/** now read in handler_class_id. this plus the name allow us **/
/** to search for the handler and assign it automatically **/

	Read(fp,&handlerclass,2);

	/** create the ModState struct and fill it in **/

	ObtainExclusiveSemaphore(&StudioBase->lockstudio); /** prevent se_new_modstate  msg's till done! **/
	s=(struct StandardModState *)AddModState(shortval,modname,instname);
	Read(fp,&s->state_size,s->state_size - sizeof(s->node) - 8);
	s->hand=FindAudioHandler(0,-1,0,handlername,handlerclass);
	s->handler_name=NULL;
	s->attached_module=NULL;
	s->pref_display_func=NULL;
	s->pref_hide_func=NULL;
	s->lock=0;
	s->window=0;
	ReleaseExclusiveSemaphore(&StudioBase->lockstudio);
	InitModState(s);
	len=Read(fp,&shortval,2);
	}
Close(fp);
return(NO_ERROR);
}

/**************************************************************/

void InitModState(s)
struct StandardModState *s;
{
struct Module *mod;

if (s->flags&(MS_KEEP_RESIDENT|MS_WINDOW_OPEN)) {
	if (s->attached_module==NULL) {
		s->flags|=MS_NEED_MODULE;	/** mark so module will pick it when launched **/
		mod=(struct Module *)LoadModule(s->module_name);
		if (mod)
			s->flags |= MS_STATE_INITILIZED;
		}

	if ((s->flags&MS_WINDOW_OPEN) && mod) {
		if (stricmp(mod->name,"edit")) {	/** ! Edit? KLUDGE **/
			WaitUntilListening(se_cmd_select_module, mod);
			BroadcastEventParms(se_cmd_select_module,mod,0,0,0,0,0);
			}
		else {	/** KLUDGE Continuation **/
			s->flags&=(~MS_WINDOW_OPEN);
			}
		}
	}
}

/*******************************************************************/

void SaveWindState(wind)	/** From Todd's windfunc.c!!!! **/
struct Window *wind;
{
struct WindowParms *wp=NULL;
struct StandardModState *state=NULL;

if (wind) wp=(struct WindowParms *)wind->UserData;
if (wp) state=wp->state;

if (state)
	{
	state->win_le=wind->LeftEdge;
	state->win_te=wind->TopEdge;
	state->win_w=wind->Width;
	state->win_h=wind->Height;
	}
}
/*############################################################
NAME	
	MoveRequester - Note, This function is not in stubs yet
FUNCTION
	Calling this will move all error requestors to the studio
	screen
INPUTS
	pointer to a process structure
RESULTS
	NONE
###############################################################*/
void MoveRequester(pr)
struct Process *pr;
{
if(StudioBase->back_window) pr->pr_WindowPtr=(APTR)StudioBase->back_window;
}
