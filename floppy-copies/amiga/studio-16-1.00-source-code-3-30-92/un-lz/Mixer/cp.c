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
char *_procname = "STUDIO_CONTROL_PANEL";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
struct Task *task,*FindTask();
/**********************************/
void HidePrefWind();
struct window *DisplayPrefWind();
/**********************************/
struct ChanKey *KEY[6],*AllocChan();
int GetHandler();
void ForgetHandler();
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
short se_cmd_set_channel_volume;
short se_info_hilo;
short se_cmd_trigger_playback;
short se_info_playback_done;
short se_cmd_trigger_record;
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
struct StandardModState *state;
struct AudioHandler 	*hand;
struct Handler12 	*hand_state;
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
static struct NewModule ModData ={NULL,NULL,sizeof(struct StandardModState)};

task=FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);
if ((mod=(struct Module *)AddModule(&ModData))==0) {telluser("Mixer:","Could Not Add Module",0);Quit();}

state=mod->state;

se_cmd_select_module 		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window		= GetEventID("SE_CMD_HIDE_WINDOW");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_hilo			= GetEventID("SE_INFO_HILO");
se_cmd_trigger_playback		= GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_info_playback_done		= GetEventID("SE_INFO_PLAYBACK_DONE");
se_cmd_trigger_record		= GetEventID("SE_CMD_TRIGGER_RECORD");
se_info_record_done		= GetEventID("SE_INFO_RECORD_DONE");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
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
struct ChanKey *keyarr[6]={0,0,0,0,0,0};
short chanarr[6]={0,0,0,0,0,0};
void handle_studioevents()
{
void InformStartRecord();
void InformStartPlayback();
void InformEndPlayback();
void InformEndRecord();
struct StudioEvent *event;
struct ChanKey *key;
int x;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_info_hilo)
		if (hand==(struct AudioHandler *)event->arg1) UpdateMeters(event->arg2);
	/*********************************/
	if(event->type == se_cmd_trigger_playback)
		{
		key=(struct ChanKey *)event->arg2;
		if (key->handler==hand)
			{
			for(x=0;x<6;x++) if (keyarr[x]==0) {keyarr[x]=key; chanarr[x]=key->chan_num; break;}
			InformStartPlayback(key->chan_num,event->arg1);
			}
		}
	/*********************************/
	if(event->type == se_cmd_trigger_record)
		{
		key=(struct ChanKey *)event->arg2;
		if (key->handler==hand)
			{
			for(x=0;x<6;x++) if (keyarr[x]==0) {keyarr[x]=key; chanarr[x]=3; break;}
			InformStartRecord(3,event->arg1);
			}
		}
	/*********************************/
	if(event->type == se_info_playback_done || event->type == se_info_record_done)
		{
		key=(struct ChanKey *)event->arg2;
		for(x=0;x<6;x++) 
			{
			if (keyarr[x]==key)
				{
				if(event->type == se_info_playback_done) InformEndPlayback(chanarr[x]);
				else InformEndRecord(chanarr[x]);
				keyarr[x]=0;
				break;
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
		if (key->handler==hand) /* POINTING TO THE PROPER HANDLER */
			{
			if (key->flags & CK_CHAN_PLAY)
				{
				if (key->chan_num == KEY[0]->chan_num)
					update_knob(MIX1gad.gad_ptr,event->arg2);
				if (key->chan_num== KEY[1]->chan_num)
					update_knob(MIX2gad.gad_ptr,event->arg2);
				if (key->chan_num== KEY[2]->chan_num)
					update_knob(MIX3gad.gad_ptr,event->arg2);
				if (key->chan_num== KEY[3]->chan_num)
					update_knob(MIX4gad.gad_ptr,event->arg2);
				}

			if (key->flags&CK_CHAN_MONITOR)
				{
				update_knob(MIXmongad.gad_ptr,event->arg2);
				}

			if (key->flags&CK_CHAN_OUTPUT)
				{
				update_knob(MIXoutgad.gad_ptr,event->arg2);
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
	}
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
USHORT Convert();

GetHandler();
if ((wind = OpenNewWind(&NewWind,mod->state,PREFERENCES,NULL))!=0)  
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ColorGadList(Gadgets);
	WindGadSize();
MIX1gad.top_element=Convert(hand_state->vol_play[0]);
MIX2gad.top_element=Convert(hand_state->vol_play[1]);
MIX3gad.top_element=Convert(hand_state->vol_play[2]);
MIX4gad.top_element=Convert(hand_state->vol_play[3]);
MIXmongad.top_element=Convert(hand_state->vol_in[0]);
MIXoutgad.top_element=Convert(hand_state->vol_out[0]);

	if (create_gadget_chain(wind,Gadgets)==0)  {telluser("Mixer:","Could not create gadgets!",0); exit(10);}

	NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
	NotifyMeOnEvent(se_info_hilo, mod);
	NotifyMeOnEvent(se_cmd_trigger_playback);
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
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

if (wind)
	{
	IgnoreEvent(se_cmd_trigger_playback);
	IgnoreEvent(se_info_playback_done);
	IgnoreEvent(se_cmd_trigger_record);
	IgnoreEvent(se_info_record_done);
	IgnoreEvent(se_cmd_set_channel_volume, mod);
	IgnoreEvent(se_info_hilo, mod);

	if (timerport)
		{
		AbortIO( timermsg );
		WaitIO( timermsg );
		DeletePort(timerport);
		}

	ForgetHandler();

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
void InformEndPlayback();
if (wind)
	{
	RefreshWind(wind);
	draw_indent(wind->RPort,  8,15,wind->Width-9,wind->Height-5,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
/* TOP */
	draw_indent(wind->RPort,px1,16,px2-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px2,16,px3-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px3,16,px4-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px4,16,px5-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px5,16,px6-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px6,16,px7-1,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
/* MID */
	draw_indent(wind->RPort,px1,27,px2-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px2,27,px3-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px3,27,px4-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px4,27,px5-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px5,27,px6-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px6,27,px7-1,wind->Height-69,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
/* BOT */
	draw_indent(wind->RPort,px1,wind->Height-68,px2-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px2,wind->Height-68,px3-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px3,wind->Height-68,px4-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px4,wind->Height-68,px5-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px5,wind->Height-68,px6-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px6,wind->Height-68,px7-1,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

/* CLIPS */
	draw_indent(wind->RPort,px1+mx,wind->Height-66,px1+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px2+mx,wind->Height-66,px2+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px3+mx,wind->Height-66,px3+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px4+mx,wind->Height-66,px4+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px5+mx,wind->Height-66,px5+mx+18,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px6+mx,wind->Height-66,px6+mx+18+16,wind->Height-58,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
	SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
	RectFill(wind->RPort,px1+mx+2,wind->Height-65,px1+mx+16,wind->Height-59);
	RectFill(wind->RPort,px2+mx+2,wind->Height-65,px2+mx+16,wind->Height-59);
	RectFill(wind->RPort,px3+mx+2,wind->Height-65,px3+mx+16,wind->Height-59);
	RectFill(wind->RPort,px4+mx+2,wind->Height-65,px4+mx+16,wind->Height-59);
	RectFill(wind->RPort,px5+mx+2,wind->Height-65,px5+mx+16,wind->Height-59);
	RectFill(wind->RPort,px6+mx+2,wind->Height-65,px6+mx+16+16,wind->Height-59);

/* GRAPHS */
	draw_indent(wind->RPort,px1+mx,wind->Height-56,px1+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px2+mx,wind->Height-56,px2+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px3+mx,wind->Height-56,px3+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px4+mx,wind->Height-56,px4+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px5+mx,wind->Height-56,px5+mx+18,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,px6+mx,wind->Height-56,px6+mx+18+16,wind->Height-8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
	SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
	RectFill(wind->RPort,px1+mx+2,wind->Height-55,px1+mx+16,wind->Height-9);
	RectFill(wind->RPort,px2+mx+2,wind->Height-55,px2+mx+16,wind->Height-9);
	RectFill(wind->RPort,px3+mx+2,wind->Height-55,px3+mx+16,wind->Height-9);
	RectFill(wind->RPort,px4+mx+2,wind->Height-55,px4+mx+16,wind->Height-9);
	RectFill(wind->RPort,px5+mx+2,wind->Height-55,px5+mx+16,wind->Height-9);
	RectFill(wind->RPort,px6+mx+2,wind->Height-55,px6+mx+16+16,wind->Height-9);

	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_selected);
	SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_selected);
	Move(wind->RPort,px1+mx+9,wind->Height-55);Draw(wind->RPort,px1+mx+9,wind->Height-9);
	Move(wind->RPort,px2+mx+9,wind->Height-55);Draw(wind->RPort,px2+mx+9,wind->Height-9);
	Move(wind->RPort,px3+mx+9,wind->Height-55);Draw(wind->RPort,px3+mx+9,wind->Height-9);
	Move(wind->RPort,px4+mx+9,wind->Height-55);Draw(wind->RPort,px4+mx+9,wind->Height-9);
	Move(wind->RPort,px5+mx+9,wind->Height-55);Draw(wind->RPort,px5+mx+9,wind->Height-9);
	Move(wind->RPort,px6+mx+9+8,wind->Height-55);Draw(wind->RPort,px6+mx+9+8,wind->Height-9);
/* Text */
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
	Move(wind->RPort,px1+10,24);Text(wind->RPort,"  INPUT  ",9);
	Move(wind->RPort,px2+6,24);Text(wind->RPort,"CHANNEL #1",10);
	Move(wind->RPort,px3+6,24);Text(wind->RPort,"CHANNEL #2",10);
	Move(wind->RPort,px4+6,24);Text(wind->RPort,"CHANNEL #3",10);
	Move(wind->RPort,px5+6,24);Text(wind->RPort,"CHANNEL #4",10);
	Move(wind->RPort,px6+6+8,24);Text(wind->RPort,"  OUTPUT  ",10);

	InformEndPlayback(0);
	InformEndPlayback(1);
	InformEndPlayback(2);
	InformEndPlayback(3);
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
/**************************************************************/
void MIXoutfunc(k)
struct slider *k;
{
MixKnobAdj(k,0x05);
}
/**************************************************************/
void MIXmonfunc(k)
struct slider *k;
{
MixKnobAdj(k,0x04);
}
/**************************************************************/
void MIX1func(k)
struct slider *k;
{
MixKnobAdj(k,0x00);
}
/**************************************************************/
void MIX2func(k)
struct slider *k;
{
MixKnobAdj(k,0x01);
}
/**************************************************************/
void MIX3func(k)
struct slider *k;
{
MixKnobAdj(k,0x02);
}
/**************************************************************/
void MIX4func(k)
struct slider *k;
{
MixKnobAdj(k,0x03);
}
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
	if (Value>=0) BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,KEY[m],Value,0,0,0);
	else BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,KEY[m],0,0,0,0);
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
static SHORT posx1[] = {px1+mx+2 ,px2+mx+2 ,px3+mx+2 ,px4+mx+2 ,px5+mx+2,px6+mx+2 };
static SHORT posx2[] = {px1+mx+16,px2+mx+16,px3+mx+16,px4+mx+16,px5+mx+16,px6+mx+16+16};
static top=WINDH-55,bot=WINDH-9;
/**************************************************************/
void UpdateMeters(HL)
USHORT	HL[];
{
UBYTE *HLB=(UBYTE *)HL;
static SHORT COUNT[] = {48,48,48,48,48,48 };
SHORT n,hl;


LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_selected);
SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);

for(n=0;n<5;n++)
	{
	hl=7-(HLB[n<<1]>>4);
	if (hl==7) COUNT[n]++;
	else COUNT[n]=0;
	if (COUNT[n]<48)
		{
		if(HL[n] == 0x7FFF) CLIP[n]++;
		ScrollRaster(wind->RPort,0,1,posx1[n],top,posx2[n],bot);
		Move(wind->RPort,posx1[n]+hl,bot);
		Draw(wind->RPort,posx2[n]-hl,bot);
		}
	}

n=5;
hl=15-(HLB[n<<1]>>3);
if (hl==15) COUNT[n]++;
else COUNT[n]=0;
if (COUNT[n]<48)
	{
	if(HL[n] == 0x7FFF) CLIP[n]++;
	ScrollRaster(wind->RPort,0,1,posx1[n],top,posx2[n],bot);
	Move(wind->RPort,posx1[n]+hl,bot);
	Draw(wind->RPort,posx2[n]-hl,bot);
	}


for(n=0;n<6;n++)
	{
	if (CLIP[n]==1) 
		{
		SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_selected);
		SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_selected);
		RectFill(wind->RPort,posx1[n],wind->Height-65,posx2[n],wind->Height-59);
		}
	}
UnLockGadLibRender();

}
/**************************************************************/
void ClipOff()
{
SHORT n;

LockGadLibRender();

SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);
SetBPen(wind->RPort,StudioBase->defaults.colors.slider_knob.hitbox_unselected);

for(n=0;n<6;n++)
	{
	if (CLIP[n]) {RectFill(wind->RPort,posx1[n],wind->Height-65,posx2[n],wind->Height-59);CLIP[n]=0;}
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

sprintf(buff,"%-10s",as->region.samp->name);

LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan+1]-(mx-3),35);
Text(wind->RPort,"Playing...",10);
Move(wind->RPort,posx1[chan+1]-(mx-3),35+9);
Text(wind->RPort,buff,10);
UnLockGadLibRender();
}
/**************************************************************/
void InformStartRecord(chan,as)
int chan;
struct ActiveSample *as;
{
char buff[80];

sprintf(buff,"%-10s",as->region.samp->name);

LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan+1]-(mx-3),35);
Text(wind->RPort,"Recording.",10);
Move(wind->RPort,posx1[chan+1]-(mx-3),35+9);
Text(wind->RPort,buff,10);
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
RectFill(wind->RPort,px5+2,wind->Height-67,px6-28,wind->Height-7);
UnLockGadLibRender();
hide_gadget(MIX4gad.gad_ptr);
}
/**************************************************************/
void InformEndPlayback(chan)
int chan;
{
LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan+1]-(mx-3),35);
Text(wind->RPort,"          ",10);
Move(wind->RPort,posx1[chan+1]-(mx-3),35+9);
Text(wind->RPort,"          ",10);
UnLockGadLibRender();
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void InformEndRecord(chan)
int chan;
{
LockGadLibRender();
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
SetDrMd(wind->RPort,JAM2);
Move(wind->RPort,posx1[chan+1]-(mx-3),35);
Text(wind->RPort,"          ",10);
Move(wind->RPort,posx1[chan+1]-(mx-3),35+9);
Text(wind->RPort,"          ",10);
UnLockGadLibRender();
if (MIX4gad.gad_ptr)
	{
	show_gadget(MIX4gad.gad_ptr);
	refresh_gadget(MIX4gad.gad_ptr);
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
void ForgetHandler()
{
if (KEY[0]) FreeChan(KEY[0]);
if (KEY[1]) FreeChan(KEY[1]);
if (KEY[2]) FreeChan(KEY[2]);
if (KEY[3]) FreeChan(KEY[3]);
if (KEY[4]) FreeChan(KEY[4]);
if (KEY[5]) FreeChan(KEY[5]);

UnlockMod(hand_state);
hand=NULL;hand_state=NULL;
Hmod=NULL;
}
/**************************************************************/
int GetHandler()
{
struct AudioHandler *FindAudioHandler();

if (state->hand==NULL) 
	{
	hand=FindAudioHandler(NULL,-1,0,NULL,-1);
	if (hand)
		{
		state->hand=hand;
		state->handler_name=hand->name;
		state->handler_class_id =hand->class_id;
		}
	else {telluser("Mixer:","Could not find a Handler",NULL);return(-1);}
	}
else	hand=state->hand;
hand_state=(struct Handler12 *)hand->handler_mod->state;
LockMod(hand_state);

Hmod=hand->handler_mod;

if((KEY[0]=AllocChan(hand,0, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
if((KEY[1]=AllocChan(hand,1, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
if((KEY[2]=AllocChan(hand,2, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
if((KEY[3]=AllocChan(hand,3, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
if((KEY[4]=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
if((KEY[5]=AllocChan(hand,0, CK_CHAN_OUTPUT | CK_ID_ONLY))==NULL) 	telluser("Mixer:","COULD NOT GET KEY",0);
return(NULL);
}
/**************************************************************/
