#include <exec/types.h>
#include <exec/exec.h>
#include <exec/libraries.h>
#include <stdio.h>
#include <intuition/intuition.h>
#include "/include/message.h"
#include "/include/studio16.h"

/**** global variables *******/

#define NUM_MSGS 200

/** modules that want to hear about a certain event **/

struct MinList NotifyArray[MAX_EVENT_TYPE];	

/** modules that want to hear when a certain event count changes **/

struct MinList CountArray[MAX_EVENT_TYPE];
char *EventIDArray[MAX_EVENT_TYPE];
struct MsgPort *studio_reply_port;
struct Task *tc_replytask;
struct studioBase *studioBase;
struct MinList FreeEventStructs;
extern struct studioBase LibBase;

short se_info_module_added;
short se_info_module_deleted;
short se_info_sample_created;
short se_info_sample_deleted;
short se_info_event_count;

/***** functions *********/

void RemStudioLib();
void FreeBigBlocks();
BOOL InitStudioLib();
void MsgReplyHandler();
void BroadcastEventParms();
void NotifyMeOnEvent();
void ClearNotifyList();
void IgnoreEvent();
void LoadDefaults();
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


/**************************************************************/

void RemStudioLib()
{
struct MinNode *node;
struct Disk_Samp *s;
struct Module *mod;
struct StudioEvent *e;
int i;

node=studioBase->samps.mlh_Head;
while (node->mln_Succ) {
   s=(struct Disk_Samp *)node;
   node = node->mln_Succ;
   DeleteSample(s);
   }

node=studioBase->modules.mlh_Head;
while (node->mln_Succ) {
   mod=(struct Module *)node;
   node = node->mln_Succ;
   DeleteModule(mod);
   }

/** Delete the task that removes all replyed messages **/

ObtainSemaphore(&studioBase->lock);
DeleteTask(tc_replytask);
ReleaseSemaphore(&studioBase->lock);

/** Delete port that all SendMsg's replys are sent to **/

while (e=(struct StudioEvent *)GetMsg(studio_reply_port)) 
	UseSendMsg(e);

DeletePort(studio_reply_port);

/** Free all memory used for StudioEvent structures **/

FreeAllSendMsgs();

/** Clear any remaining NotifyLink structures **/

for (i=0; i < studioBase->max_registered_ids; i++)
	ClearNotifyList(i);

/** Free memory use for Event names **/

for (i = 0; i < studioBase->max_registered_ids; i++) 
	FreeMem(EventIDArray[i], strlen(EventIDArray[i])+1);

/** Free memory allocated for disk buffers **/

FreeBigBlocks();

if (studioBase->back_window)
   CloseWindow(studioBase->back_window);

if (studioBase->screen)
   CloseScreen(studioBase->screen);
}

/**************************************************************/

void FreeBigBlocks()
{
short i;

for (i=0; i < 4; i++) {
	if (studioBase->bigblock[0][i])
		FreeMem(studioBase->bigblock[0][i], BIG_BLOCK_SIZE*2);
	if (studioBase->bigblock[1][i])
		FreeMem(studioBase->bigblock[1][i], BIG_BLOCK_SIZE*2);
	}
}

/**************************************************************/

BOOL InitStudioLib()
{
int i;

studioBase = &LibBase;

setmem((char *)studioBase+sizeof(struct Library),sizeof(struct studioBase)-sizeof(struct Library),0);
NewList(&studioBase->modules);
NewList(&studioBase->regions);
NewList(&studioBase->samps);
NewList(&studioBase->cue_events);
NewList(&studioBase->handlers);
InitSemaphore(&studioBase->lock);
LoadDefaults(0);
studioBase->eventidarray=&EventIDArray[0];
studioBase->notifyarray=&NotifyArray[0];
for (i=0; i < 4; i++) {
	studioBase->bigblock[0][i]=(unsigned char *)AllocMem(BIG_BLOCK_SIZE*2, 0);
	if (studioBase->bigblock[0][i]==0) {
		FreeBigBlocks();
		return(FALSE);
		}

	studioBase->bigblock[1][i]=(unsigned char *)AllocMem(BIG_BLOCK_SIZE*2, 0);
	if (studioBase->bigblock[1][i]==0) {
		FreeBigBlocks();
		return(FALSE);
		}
	}

/** allocate all events used by library (like se_info_sample_created) **/

if (AllocInternalEvents()==-1) {
	FreeBigBlocks();
	return(FALSE);
	}

tc_replytask=(struct Task *)CreateTask("Studio16MsgReplys",1,MsgReplyHandler,4096);
if (tc_replytask==0) {
	FreeBigBlocks();
	for (i = 0; i < studioBase->max_registered_ids; i++) 
		FreeMem(EventIDArray[i], strlen(EventIDArray[i])+1);
	return(FALSE);
	}

while (studio_reply_port==0) WaitTOF();

InitSendMsg();

return(TRUE);
}

/**************************************************************/

int AllocInternalEvents()
{
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
return(0);
}
/**************************************************************/

void MsgReplyHandler()
{
struct StudioEvent *e;

studio_reply_port=(struct MsgPort *)CreatePort(0,0);	/** LOOK FOR ERRORS!!!! **/

while (TRUE) {
	WaitPort(studio_reply_port);
	while (e = (struct StudioEvent *)GetMsg(studio_reply_port)) {
		UseSendMsg(e);
		}
	}
}

/*############################################################
NAME	
	BroadcastEventParms -

FUNCTION
	Sends an event message to all all modules who are listening for
	the type being broadcast.  See GetEventID() for an example.
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

ObtainSemaphore(&studioBase->lock);
for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	SendMsg(((struct NotifyLink *)node)->notifyme, type, arg1, arg2, arg3, arg4, arg5, flags);
	}
ReleaseSemaphore(&studioBase->lock);
}

/*############################################################
NAME	
	NotidyMeOnEvent -
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

if (type >= studioBase->max_registered_ids || type < 0) {
	return;
	}
	
newlink = (struct NotifyLink *)AllocMem(sizeof(struct NotifyLink),MEMF_CLEAR);
if (newlink) {
	newlink->notifyme=module;
	ObtainSemaphore(&studioBase->lock);
	AddTail(&NotifyArray[type], newlink);
	BroadcastCount(type);
	ReleaseSemaphore(&studioBase->lock);
	}
}

/**************************************************************/

void BroadcastCount(type)
short type;
{
struct MinNode *node;
short count;

if (CountArray[type].mlh_Head->mln_Succ) {
	ObtainSemaphore(&studioBase->lock);
	count=0;
	for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ)
		count++;
	for (node=CountArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ) {
		SendMsg(((struct NotifyLink *)node)->notifyme, se_info_event_count, type, count, 0, 0,0, 0);
		}
	ReleaseSemaphore(&studioBase->lock);
	}
}

/*############################################################
NAME	
	NotifyMeOnCount -
FUNCTION
	Tells studio16 to inform a module every time the number of
	modules listening for a particular ID changes.  The module
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

if (type >= studioBase->max_registered_ids || type < 0) {
	return;
	}
	
newlink = (struct NotifyLink *)AllocMem(sizeof(struct NotifyLink),MEMF_CLEAR);
if (newlink) {
	newlink->notifyme=module;
	ObtainSemaphore(&studioBase->lock);
	AddTail(&CountArray[type], newlink);
	count=0;
	for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ)
		count++;
	SendMsg(module, se_info_event_count, type, count, 0, 0,0, 0);
	ReleaseSemaphore(&studioBase->lock);
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

if (type >= studioBase->max_registered_ids || type < 0) 
	return;

ObtainSemaphore(&studioBase->lock);
node=CountArray[type].mlh_Head;
while(node->mln_Succ) {
	newnode = node->mln_Succ;
	if (module==(((struct NotifyLink *)node)->notifyme)) {
		Remove(node);
		FreeMem(node, sizeof(struct NotifyLink));
		}
	node=newnode;
	}
ReleaseSemaphore(&studioBase->lock);
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

if (type >= studioBase->max_registered_ids || type < 0) 
	return;

ObtainSemaphore(&studioBase->lock);
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
ReleaseSemaphore(&studioBase->lock);
}

/*############################################################
NAME	
	ClearNotifyList-
FUNCTION
	Tells studio16 to stop sending events of a specified type
	to everyone currently listening for them.  Be carefull using this
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

if (type >= studioBase->max_registered_ids || type < 0) {
	return;
	}

ObtainSemaphore(&studioBase->lock);
node=NotifyArray[type].mlh_Head;
while (node->mln_Succ) {
	newnode = node->mln_Succ;
	Remove(node);
	FreeMem(node, sizeof(struct NotifyLink));
	node=newnode;
	}
ReleaseSemaphore(&studioBase->lock);
}

/*############################################################
NAME	
	GetEventID -

FUNCTION
	Takes a pointer to a string and returns its ID number.  In 
	Studio 16, "Events" are used to communicate between modules.
	In order to broadcast an event, you must get its ID. ID's are
	assigned dynamicaly at runtime for maximum flexability.  For
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

ObtainSemaphore(&studioBase->lock);
for (index = 0; index < studioBase->max_registered_ids; index++) {
	if (strcmp(EventIDArray[index], name)==0) {
		ReleaseSemaphore(&studioBase->lock);
		return(index);
		}
	}
if (index==MAX_EVENT_TYPE) {
	ReleaseSemaphore(&studioBase->lock);
	return(-1);
	}
else {
	EventIDArray[studioBase->max_registered_ids]=(char *)AllocMem(strlen(name)+1,0);
	if (EventIDArray[studioBase->max_registered_ids]) {
		NewList(&NotifyArray[studioBase->max_registered_ids]);
		NewList(&CountArray[studioBase->max_registered_ids]);
		strcpy(EventIDArray[studioBase->max_registered_ids++], name);
		val=studioBase->max_registered_ids-1;
		ReleaseSemaphore(&studioBase->lock);
		return(val);
		}
	else {
		ReleaseSemaphore(&studioBase->lock);
		return(-1);
		}
	}
}

/****************************************************************/

#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,LIGHT_BROWN,WHITE,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BLUE,BLACK,BLACK}
#define DROPCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,WHITE,BLACK}

/*############################################################
NAME
	LoadDefaults -
FUNCTION
	trys to load defaults from the specified file.  If the file
	can't be opened, some defauts are generated.  Defaults are
	kept in studioBase->defaults. If a null is passed for arg1,
	a default string "AutoPres" is generated.
INPUTS	
   ARGS:
      1  pointer to string containing complete path of file
         containing defaults to try and load.
RETURNS
	NONE
##############################################################*/

void LoadDefaults(file)
char *file;    /** complete path/file pointing to defaults or NULL **/
{
struct Screen def_screen;
static struct GadgetColors defcolors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	DROPCOLORS};

if (file==NULL)
   file = "AutoPrefs";

/*** put in stuff to load file here!!! **/

strcpy(studioBase->defaults.samp_io_path,"aw:sounds");
strcpy(studioBase->defaults.temp_files_path,"audio:");

if (GetScreenData(&def_screen, sizeof(struct Screen), WBENCHSCREEN, 0)) {
   studioBase->defaults.screen_width = def_screen.Width;
   if (def_screen.Height >= 400)
      studioBase->defaults.screen_viewmodes = LACE;
   if (def_screen.Width >= 200)
      studioBase->defaults.screen_viewmodes |= HIRES;
   }
else {
   studioBase->defaults.screen_width = 640;
   studioBase->defaults.screen_viewmodes = HIRES;
   }
studioBase->defaults.colors = defcolors;
}

/*############################################################
NAME	
	SendMsg -
FUNCTION
	Sends a message to one particular module.  Use this if you
	know that only one specific module is waiting for a message.
	For example, Modlist sometimes uses this to send out
	SE_CMD_KILL_MODULE messages.  This reduces the message trafic by not
	sending SE_CMD_KILL_MODULE events to modules that don't need them.  
	However, this is also dangerous in that a module may be 
	interested in SE_CMD_KILL_MODULE events directed to other modules.
	Since	SE_CMD_KILL_MODULE events are sometimes sent using SendMsg()
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

ObtainSemaphore(&studioBase->lock);
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
ReleaseSemaphore(&studioBase->lock);
}

/**************************************************************/

void FreeAllSendMsgs()
{
struct MinNode *node;
struct StudioEvent *e;

ObtainSemaphore(&studioBase->lock);
node = FreeEventStructs.mlh_Head;
while (node->mln_Succ) {
	Remove(node);
	e = (struct StudioEvent *)((char *)node - sizeof(struct StudioEvent) + sizeof (struct MinNode));
	FreeMem(e, sizeof(struct StudioEvent));
	node = FreeEventStructs.mlh_Head;
	}
ReleaseSemaphore(&studioBase->lock);
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
   e->flags=FREE_MSG;		/** OLD?!!! */
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
ObtainSemaphore(&studioBase->lock);
AddTail(&FreeEventStructs,&e->node);
ReleaseSemaphore(&studioBase->lock);
}

void Delay(x)
{

while (x--)
	WaitTOF();
}

/*************************************************************/

/*############################################################
NAME	
	AddHandler -
FUNCTION
	Given a NewAudioHandler struct, this function creates a new
	entry in the studioBase->handlers List.  This funciton should
	be called by any Module who's job is to be a device driver
	for an audio card designed to be used by studio 16.

	Note that name's pointer is copied but not its text.  Therefore,
	the name's text should be declared static by the module 
	initilizing NewAudioHandler.  This is in contrast to other
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
	ah->play_channels=nh->play_channels;
	ah->record_channels=nh->record_channels;
	ah->num_bits=nh->num_bits;
	ah->node.ln_Pri=nh->priority;
	ObtainSemaphore(&studioBase->lock);
	Enqueue(&studioBase->handlers, ah);
	ReleaseSemaphore(&studioBase->lock);
	}
return(ah);
}

/*############################################################
NAME	
	DeleteHandler -
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
ObtainSemaphore(&studioBase->lock);
Remove(ah);
ReleaseSemaphore(&studioBase->lock);
FreeMem(ah,sizeof(struct AudioHandler));
}	
	
/**************************************************************/
/** Handle channel allocation/ deallocations **/
/** WHAT ABOUT ALLOCATING RECORD CHANNELS!!!!!!!! ***/

struct ChanKey *AllocChan(ah, cn)
struct AudioHandler *ah;	/** audio handler or NULL for any */
int cn;		/** channel number or -1 for any **/					
{
short i;
unsigned long ChanBit;
struct Node *node;
unsigned long chanmask;

/** first pick an AudioHandler **/

if (ah==NULL) {
	ObtainSemaphore(&studioBase->lock);
	for (node=studioBase->handlers.lh_Head; node->ln_Succ; node = node->ln_Succ) {
		ah=(struct AudioHandler *)node;
		chanmask = intpow(ah->play_channels,2)-1;
		if ((ah->ChansInUse&chanmask)!=chanmask) 
			break;
		}
	ReleaseSemaphore(&studioBase->lock);
	}

if (ah==NULL) {		/** no handlers in the system! **/
	return(NULL);
	}

/** Now pick a free channel **/

if (cn!=-1) {
	ChanBit=(1<<cn);
	if ((ah->ChansInUse&ChanBit)==0) {		/** Free chanel !! **/
		ah->ChansInUse |= ChanBit;
		return(GetKey(ah, cn));
		}
	return(NULL);
	}

else {
	for (i=0; i < ah->play_channels ; i++)	{
		ChanBit=(1<<i);
		if ((ah->ChansInUse&ChanBit)==0) {		/** Free chanel !! **/
			ah->ChansInUse |= ChanBit;
			return(GetKey(ah, i));
			}
		}
	return(NULL);
	}
}

/**************************************************************/

struct ChanKey *GetKey(ah, cn)

struct AudioHandler *ah;
unsigned long cn;
{
struct ChanKey *key;

key=(struct ChanKey *)AllocMem(sizeof(struct ChanKey),MEMF_CLEAR);
if (key) {
	key->handler=ah;
	key->chan_num=cn;
	key->chan_mask=(1<<cn);
	}
else {
	ah->ChansInUse &= (~(1<<cn));
	}
return(key);
}

/**************************************************************/

void FreeChan(key)
struct ChanKey *key;
{
key->handler->ChansInUse &= (~(1<<key->chan_num));
FreeMem(key, sizeof (struct ChanKey));
}

/**************************************************************/

int intpow (exponent,base)
int exponent,base;
{
register int i,z;

if (exponent==0) return (1);
z=1;
for (i=0;i<exponent;i++)
   z=z*base;
return (z);
}

