#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include <devices/timer.h>
#include "cp.data"
#include "clib/macros.h"
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_MIXER";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
struct Task *task,*FindTask();
struct ChanKey *Chan[16],*NChan[16];
/**********************************/
void HidePrefWind();
struct window *DisplayPrefWind();
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
short se_cmd_set_channel_volume;
short se_cmd_set_channel_pan;
short se_info_hilo_tick;
short se_cmd_trigger_playback;
short se_info_playback_done;
short se_cmd_trigger_record;
short se_cmd_trigger_timecode;
short se_info_record_done;
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
char *FindFileName();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void Quit();
void handle_studioevents();
void handle_intuievents();
void WindGadSize();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod,*Hmod;
struct Window 		*wind;
struct AudioHandler 	*hand;
/**********************************/
struct MixerModState
	{
	struct StandardModState state;
	int menucksum;
	byte itemnum[16];
	};
struct MixerModState *state;
/**********************************/
struct MsgPort *timerport;
struct timerequest *timermsg;
struct timeval currentval;
/**********************************/
void update_knob();
void UpdateMeters();
/**************************************************************/
/**************************************************************/
void _main(cmd)
char *cmd;
{
int mask;
void ClipOff();

InitModule(cmd);			/** initalize module **/

while (TRUE)
	{
	mask=NULL;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit) + (1<<timerport->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	if (wind)
		{
		if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
		if(mask & (1<<timerport->mp_SigBit)) 
			{
			GetMsg(timerport);
			currentval.tv_secs = 1;
			currentval.tv_micro = 0;
			timermsg->tr_node.io_Command = TR_ADDREQUEST;
			timermsg->tr_time = currentval;
			SendIO((struct IORequest *) timermsg );
			ClipOff();
			}
		}
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
static struct NewModule ModData ={NULL,NULL,sizeof(struct MixerModState)};

task=FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);
if ((mod=(struct Module *)AddModule(&ModData))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Mixer:","Could Not Add Module",0);
	Quit();
	}

state=(struct MixerModState *)mod->state;
if (!(state->state.flags & MS_STATE_INITILIZED)) 
	{
	state->menucksum=0;
	state->itemnum[0]=1;
	state->itemnum[1]=2;
	state->itemnum[2]=-1;
	}

se_cmd_select_module 		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window		= GetEventID("SE_CMD_HIDE_WINDOW");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_cmd_set_channel_pan		= GetEventID("SE_CMD_SET_CHANNEL_PAN");
se_info_hilo_tick		= GetEventID("SE_INFO_HILO_TICK");
se_cmd_trigger_playback		= GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_playback_done		= GetEventID("SE_INFO_PLAYBACK_DONE");
se_cmd_trigger_record		= GetEventID("SE_CMD_TRIGGER_RECORD");
se_cmd_trigger_timecode		= GetEventID("SE_CMD_TRIGGER_TIMECODE");
se_info_record_done		= GetEventID("SE_INFO_RECORD_DONE");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();
if (mod) DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
void InformStartRecord();
void InformStartPlayback();
void InformEndPlayback();
void InformEndRecord();
struct StudioEvent *event;
struct ChanKey *key;
struct ActiveSample *as;
int x;
void move_knob();
USHORT Convert(),cvrt;
struct slider *slid;
int DB2pan();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_info_hilo_tick)
		UpdateMeters(event->arg1);
	/*********************************/
	if(event->type == se_cmd_trigger_playback || event->type == se_cmd_trigger_timecode)
		{
		as=(struct ActiveSample *)event->arg1;
		for(x=0;x<NumChan;x++)
			{
			if (as->key==Chan[x]) 
				{
				InformStartPlayback(x,event->arg1);
				}
			}
		}
	/*********************************/
	if(event->type == se_cmd_trigger_record)
		{
		as=(struct ActiveSample *)event->arg1;
		for(x=0;x<NumChan;x++)
			{
			if (as->key==Chan[x]) 
				{
				InformStartRecord(x,event->arg1);
				}
			}
		}
	/*********************************/
	if(event->type == se_info_playback_done || event->type == se_info_record_done)
		{
		key=(struct ChanKey *)event->arg2;
		for(x=0;x<NumChan;x++)
			{
			if (key==Chan[x]) 
				{
				if(event->type == se_info_playback_done) InformEndPlayback(x);
				else InformEndRecord(x);
				}
			}
		}
	/*********************************/
	if(event->type == se_cmd_select_module && (struct Module *)event->arg1==mod)
			if (wind==NULL) DisplayWind();

	/*********************************/
	if(event->type == se_cmd_kill_module && (struct Module *)event->arg1 == mod)
		{
		ReplyMsg(event);
		Quit();
		}
	/*********************************/
	if (event->type == se_cmd_set_channel_volume)
		{
		key=(struct ChanKey *)event->arg1;
		for(x=0;x<NumChan;x++)
			{
			if (key==Chan[x]) 
				{
				cvrt=Convert(Chan[x]->parms.volume);
				slid=(struct slider *)MIXgad[x].gad_ptr;
				if (slid->top_element!=cvrt)
					{
					slid->top_element=cvrt;
					move_knob(slid);
					}
				}
			}
		}
	/*********************************/
	if (event->type == se_cmd_set_channel_pan)
		{
		key=(struct ChanKey *)event->arg1;
		for(x=0;x<NumChan;x++)
			{
			if (key==Chan[x]) 
				{
				cvrt=DB2pan(Chan[x]->parms.pan);
				slid=(struct slider *)PANgad[x].gad_ptr;
				if (slid->top_element!=cvrt)
					{
					slid->top_element=cvrt;
					move_knob(slid);
					}
				}
			}
		}
	ReplyMsg(event);
	}
}
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
BOOL flag;
int x,t;
struct MenuItem *FindCheckedItem(),*mi;
struct ChanKey *ItemData();

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

	if (class == NEWSIZE)
		WindGadSize();

	if (class == CLOSEWINDOW)
		{
		HideWind();
		return;
		}

	if (class == MENUPICK)
		{
		t=0;
		mi=Titles[0].FirstItem;

		while ((mi=FindCheckedItem(mi)) && t<6)
			{
			NChan[t]=ItemData(mi);
			state->itemnum[t]=ItemNum(mi);
			t++;
			mi=mi->NextItem;
			}
		while (mi)
			{
			mi->Flags&=~CHECKED;
			mi=mi->NextItem;
			}
		state->itemnum[t]=-1;

		if (t!=NumChan) {NumChan=t;flag=TRUE;}

		for (x=0;x<NumChan;x++)
			{
			if(Chan[x]!=NChan[NumChan-x-1]) {Chan[x]=NChan[NumChan-x-1]; flag=TRUE;}
			}
		if (flag) WindGadSize();
		}
	}
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
struct MenuItem *BuildChannelMenu();
struct AudioHandler *FindAudioHandler();
int t,cksum;
struct MenuItem *FindCheckedItem(),*mi;
struct ChanKey *ItemData();

if (FindAudioHandler(NULL,-1,0,NULL,-1))
{
Titles[0].FirstItem=BuildChannelMenu(CK_CHAN_VOLUME,MENU_MULTI_SELECT);

if ((wind = OpenNewWind(&NewWind,mod->state,NULL,&Titles[0]))!=0)  
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

	cksum=MenuCheckSum(Titles[0].FirstItem);

	if (cksum==state->menucksum || state->menucksum==0)
		{
		t=0;
		while (state->itemnum[t]!=-1)
			{
			CheckNum(Titles[0].FirstItem,state->itemnum[t]);
			t++;
			}
		}
	else state->itemnum[0]=-1;

	state->menucksum=cksum;

	t=0;
	mi=Titles[0].FirstItem;

	while ((mi=FindCheckedItem(mi)) && t<16)
		{
		NChan[t]=ItemData(mi);
		t++;
		mi=mi->NextItem;
		}
	NumChan=t;

	for (t=0;t<NumChan;t++)
		{
		Chan[t]=NChan[NumChan-t-1]; 
		}

	WindGadSize();

	NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
	NotifyMeOnEvent(se_cmd_set_channel_pan, mod);
	NotifyMeOnEvent(se_info_hilo_tick, mod);
	NotifyMeOnEvent(se_cmd_trigger_playback);
	NotifyMeOnEvent(se_cmd_trigger_timecode);
	NotifyMeOnEvent(se_info_playback_done);
	NotifyMeOnEvent(se_cmd_trigger_record);
	NotifyMeOnEvent(se_info_record_done);


	timerport = (struct MsgPort *) CreatePort( 0, 0 );
	timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
	OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );
	currentval.tv_secs = 2;
	currentval.tv_micro = 0;
	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	timermsg->tr_time = currentval;
	SendIO((struct IORequest *) timermsg );
	}
else telluser("Mixer:","Can not open window!",0);
}
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

FreeChannelMenu(Titles[0].FirstItem);

if (wind)
	{
	IgnoreEvent(se_cmd_trigger_playback);
	IgnoreEvent(se_info_playback_done);
	IgnoreEvent(se_cmd_trigger_record);
	IgnoreEvent(se_info_record_done);
	IgnoreEvent(se_cmd_set_channel_pan, mod);
	IgnoreEvent(se_info_hilo_tick, mod);

	if (timerport)
		{
		AbortIO( timermsg );
		WaitIO( timermsg );
		DeletePort(timerport);
		CloseDevice(timermsg);
		DeleteExtIO(timermsg);
		}

	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
extern SHORT posx1[];
extern SHORT posx2[];
int x;
void InformEndPlayback();
struct NewGadgetHeader NGH,*ngh=&NGH;
USHORT Convert();
struct Screen *s=StudioBase->screen;

if (wind)
	{
	ActivateWindow(wind);

	if (s->Height < (wind->TopEdge+wind->Height))
		MoveWindow(wind,0,s->Height - (wind->Height+wind->TopEdge));
	if (s->Width < (posx[MAX(NumChan,1)]+10+wind->LeftEdge))
		MoveWindow(wind,s->Width-((posx[MAX(NumChan,1)]+10)+wind->LeftEdge),0);

	SizeWindow(wind,(posx[MAX(NumChan,1)]+10)-wind->Width,0);

	/******* Wait For Window Sizing *******/
	WaitTOF();WaitTOF();WaitTOF();WaitTOF();
	WaitTOF();WaitTOF();WaitTOF();WaitTOF();
	WaitTOF();WaitTOF();WaitTOF();WaitTOF();
	WaitTOF();WaitTOF();WaitTOF();WaitTOF();
	/**************************************/

	for(x=0;x<6;x++) 
		{
		MIXgad[x].next=NULL;
		PANgad[x].next=NULL;
		}
	delete_gadget_chain(wind);
	RefreshWind(wind);
	draw_indent(wind->RPort,  8,15,wind->Width-9,wind->Height-5,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);

	for(x=0;x<6;x++)
		{
		posx1[x]=posx[x]+mx+2;
		posx2[x]=posx[x]+mx+16;
		}

for(x=0;x<NumChan;x++)
	{
	draw_indent(wind->RPort,posx[x],16,posx[x+1]-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,posx[x],WINDH-78,posx[x+1]-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,posx[x],27,posx[x+1]-1,WINDH-79,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,posx[x]+mx,wind->Height-66,posx[x]+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,posx[x]+mx,wind->Height-56,posx[x]+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
	RectFill(wind->RPort,posx[x]+mx+2,wind->Height-65,posx[x]+mx+16,wind->Height-59);
	RectFill(wind->RPort,posx[x]+mx+2,wind->Height-55,posx[x]+mx+16,wind->Height-9);
	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_unselected);
	Move(wind->RPort,posx[x]+mx+9,wind->Height-55);Draw(wind->RPort,posx[x]+mx+9,wind->Height-9);
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	Move(wind->RPort,posx[x]+10,24);Text(wind->RPort,Chan[x]->name,MIN(9,strlen(Chan[x]->name)));
	MIXgad[x].top_element=Convert(Chan[x]->parms.volume);
	PANgad[x].top_element=DB2pan(Chan[x]->parms.pan);
	ngh->next=(struct NewGadgetHeader *)&MIXgad[x];
	ngh=ngh->next;
	if (Chan[x]->flags&CK_CHAN_PAN)
		{
		ngh->next=(struct NewGadgetHeader *)&PANgad[x];
		ngh=ngh->next;
		}
	if (Chan[x]->as) InformStartPlayback(x,Chan[x]->as);
	}

	if (NumChan)
		{
		ColorGadList(Gadgets);
		if (create_gadget_chain(wind,Gadgets)==0)  {telluser("Mixer:","Could not create gadgets!",0); exit(10);}
		}
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/* \  /								 \  / */
/*  XX				Mixer				  XX  */
/* /  \								 /  \ */
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
void MixKnobAdj();
void PanKnobAdj();
/**************************************************************/
void PANfunc(k)
struct slider *k;
{
PanKnobAdj(k,(int)k->user_data);
}
/**************************************************************/
/**************************************************************/
void PanKnobAdj(k,m)
struct slider *k;
int m;
{
int pan;
int Pan2DB();

pan=Pan2DB(k->top_element);

if (FindTask(NULL)!=task) 
	{
	BroadcastEventParmsLessOne(se_cmd_set_channel_pan,mod,Chan[m],pan,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
void MIXfunc(k)
struct slider *k;
{
MixKnobAdj(k,(int)k->user_data);
}
/**************************************************************/
/**************************************************************/
char frac[] = " ╝╜╛";
void MixKnobAdj(k,m)
struct slider *k;
int m;
{
char buf[20];
SHORT qdbs;
int Value;

qdbs=(6<<2) - k->top_element;
if (qdbs==-161) qdbs=-(100<<2);
Value=((qdbs+(100<<2))<<3);

if (FindTask(NULL)!=task) 
	{
	if (Value>=0) BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,Chan[m],Value,0,0,0);
	else BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,Chan[m],0,0,0,0);
	}

if (Value)
	{
	if (qdbs!=0)
		{
		if (qdbs<0) sprintf(buf,"-%02d%cdB",(-qdbs)>>2,frac[(-qdbs)&3]);
		else sprintf(buf,"+%02d%cdB",qdbs>>2,frac[qdbs&3]);
		}
	else sprintf(buf," 00 dB");
	}
else sprintf(buf,"-OO dB");

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);

Move(k->kw->RPort, k->knob_x+3, k->knob_y+8);
Text(k->kw->RPort, &buf,6);
}
/**************************************************************/
void update_knob(k,x)
struct slider *k;
int x;
{
USHORT Convert();
void move_knob();

k->top_element=Convert(x);
move_knob(k);
}
/**************************************************************/
USHORT Convert(x)
ULONG x;
{
int qdbs;
qdbs=(x>>3)-400;
if (qdbs>(6<<2)) qdbs=6<<2;
if (qdbs<-(40<<2)) return(46*4+1);
else return((6<<2)-qdbs);
}
/**************************************************************/
/**************************************************************/
int Pan2DB(pan)
ULONG pan;
{
int dbs;
if (pan==0) dbs=-100;
else if (pan==62) dbs=100;
else dbs=pan-31;
return((dbs+100)<<5);
}
/**************************************************************/
int DB2pan(dbs)
int dbs;
{
int pan;

dbs=(dbs>>5)-100;
if (dbs<-30) pan=0;
else if (dbs>+30) pan=62;
else pan=dbs+31;
return(pan);
}
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/* \  /								 \  / */
/*  XX				Meters				  XX  */
/* /  \								 /  \ */
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
static SHORT CLIP[]  = {0,0,0,0,0,0};
SHORT posx1[6];
SHORT posx2[6];
static top=WINDH-55,bot=WINDH-9;
/**************************************************************/
void UpdateMeters(cntr)
int cntr;
{
static SHORT COUNT[] = {48,48,48,48,48,48 };
SHORT n,hl_L,hl_R;

LockGadLibRender();

if (wind)
{

SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_unselected);
SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);

for(n=0;n<NumChan;n++)
	{
	hl_L=7-(Chan[n]->hilo_left>>4);
	hl_R=7-(Chan[n]->hilo_right>>4);
	if (hl_L==7 && hl_R==7) COUNT[n]++;
	else COUNT[n]=0;
	if (COUNT[n]<48)
		{
		if(Chan[n]->hilo_left == 0x7F || Chan[n]->hilo_right == 0x7f) CLIP[n]++;
		ScrollRaster(wind->RPort,0,1,posx1[n],top,posx2[n],bot);
		Move(wind->RPort,posx1[n]+hl_L,bot);
		Draw(wind->RPort,posx2[n]-hl_R,bot);
		}
	}

for(n=0;n<NumChan;n++)
	{
	if (CLIP[n]==1) 
		{
		CLIP[n]++;
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_selected);
		SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_selected);
		RectFill(wind->RPort,posx1[n],wind->Height-65,posx2[n],wind->Height-59);
		}
	}
}
UnLockGadLibRender();

}
/**************************************************************/
void ClipOff()
{
SHORT n;

LockGadLibRender();
if (wind)
{

SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);

for(n=0;n<NumChan;n++)
	{
	if (CLIP[n]) {RectFill(wind->RPort,posx1[n],wind->Height-65,posx2[n],wind->Height-59);CLIP[n]=0;}
	}
}
UnLockGadLibRender();
}
/**************************************************************/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/* \  /								 \  / */
/*  XX				Playback			  XX  */
/* /  \								 /  \ */
/*/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\*/
/*\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
/*нннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннннн*/
/**************************************************************/
void InformStartPlayback(chan,as)
int chan;
struct ActiveSample *as;
{
char buff[80];

if (wind)
{
sprintf(buff,"%-10s",StrFileName(as->region.samp_name));

LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan]-(mx-3),35);
Text(wind->RPort,buff,10);
UnLockGadLibRender();
}
}
/**************************************************************/
void InformStartRecord(chan,as)
int chan;
struct ActiveSample *as;
{
char buff[80];

if (wind)
{
sprintf(buff,"%-10s",StrFileName(as->region.samp_name));

LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan]-(mx-3),35);
Text(wind->RPort,buff,10);
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
RectFill(wind->RPort,px5+2,wind->Height-67,px6-28,wind->Height-7);
UnLockGadLibRender();
}
}
/**************************************************************/
void InformEndPlayback(chan)
int chan;
{
if (wind)
{
LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan]-(mx-3),35);
Text(wind->RPort,"          ",10);
UnLockGadLibRender();
}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void InformEndRecord(chan)
int chan;
{
if (wind)
{
LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan]-(mx-3),35);
Text(wind->RPort,"          ",10);
UnLockGadLibRender();
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
