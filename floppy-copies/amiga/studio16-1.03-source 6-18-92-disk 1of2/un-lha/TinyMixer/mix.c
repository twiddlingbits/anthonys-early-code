#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "mix.data"
#include "clib/macros.h"
/**********************************/
/**********************************/
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_TINY_MIXER";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
#define sp 5
#define Xsz 15
#define Ysz 100
#define Xs 20
#define Ys 20
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod,*Hmod;
struct Window 		*wind;
struct StandardModState *state;
struct AudioHandler 	*hand;
struct Handler12 	*hand_state;
/**********************************/
/**********************************/
struct Task *task,*FindTask();
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
struct ChanKey *KEY[6];
struct ChanKey *AllocChan();
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_channel_volume;
short se_info_module_opened;
short se_info_module_closed;
short se_info_prefs_changed;
/**********************************/
/**********************************/
void update_knob();
int GetHandler();
void ForgetHandler();
void WinGadSize();
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int  handle_intuievents();
/**********************************/
/**********************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
InitModule(argv);			/** initalize module **/

for(;;)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

void InitModule(name)
char *name;
{
char *FindFileName();
static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState)};
void ColorGadList();

task=FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0)  {telluser("Tiny Mixer:","Could Not Add Module",0);closelibraries();exit(10);}

state=mod->state;
state->flags|=MS_CHOOSE_HANDLER;

se_cmd_select_module  		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_channel_volume 	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_info_prefs_changed		= GetEventID("SE_INFO_PREFS_CHANGED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

void handle_studioevents()
{
struct StudioEvent *event;
struct ChanKey *key;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
{

	/*********************************/
	if (event->type == se_info_prefs_changed)
		{
		ForgetHandler();
		GetHandler();
		update_knob(MIX0gad.gad_ptr,hand_state->vol_play[0]);
		update_knob(MIX1gad.gad_ptr,hand_state->vol_play[1]);
		update_knob(MIX2gad.gad_ptr,hand_state->vol_play[2]);
		update_knob(MIX3gad.gad_ptr,hand_state->vol_play[3]);
		update_knob(MIXIngad.gad_ptr,hand_state->vol_in[0]);
		update_knob(MIXOutgad.gad_ptr,hand_state->vol_out[0]);
		}
	/*********************************/
	if (event->type == se_cmd_select_module)
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();

	/*********************************/

	/*********************************/
	if (event->type == se_cmd_kill_module)
		{
		if (((struct Module *)event->arg1 == mod) || ((struct Module *)event->arg1 == Hmod))
			{
			ReplyMsg(event);
			if (wind) HideWind();
			DeleteModule(mod);
			closelibraries();
			exit(0);
			}
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
				update_knob(MIX0gad.gad_ptr,event->arg2);
			if (key->chan_num== KEY[1]->chan_num)
				update_knob(MIX1gad.gad_ptr,event->arg2);
			if (key->chan_num== KEY[2]->chan_num)
				update_knob(MIX2gad.gad_ptr,event->arg2);
			if (key->chan_num== KEY[3]->chan_num)
				update_knob(MIX3gad.gad_ptr,event->arg2);
			}

		if (key->flags&CK_CHAN_MONITOR)
			{
			if (key->chan_num==KEY[4]->chan_num)
				update_knob(MIXIngad.gad_ptr,event->arg2);
			}

		if (key->flags&CK_CHAN_OUTPUT)
			{
			if (key->chan_num==KEY[5]->chan_num)
				update_knob(MIXOutgad.gad_ptr,event->arg2);
			}
		}
	}
ReplyMsg(event);
}
}

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

	switch (class)
		{
		case CLOSEWINDOW:
		/*********************************/
			HideWind();
			return(0);
		break;

		case NEWSIZE:
		/*********************************/
			WinGadSize();
		break;
		}
	}
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/***********************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
struct AudioHandler *FindAudioHandler();

if (GetHandler()) return;
if (wind = OpenNewWind(&MIX_NewWindow,mod->state,PREFERENCES,NULL,NULL))
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ColorGadList(&MIXOutgad);
	WinGadSize();
	NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
	}
else	telluser("Tiny Mixer:","Could not open window!",0);
}
/*****************************************************************/

/*****************************************************************/
void HideWind()
{
struct Window *w;
IgnoreEvent(se_cmd_set_channel_volume, mod);

ForgetHandler();

if (wind)
	{
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
}
/*****************************************************************/
/**************************************************************/
/**************************************************************/
void WinGadSize()
{
char frq[] = {"OUT","CH4","CH3","CH2","CH1","MON"};
int Y,Yz,x,xz,xs,z;
int c;
struct NewSlider *ns=&MIXOutgad;
USHORT Convert();

/* This module realy should be rewritten */
MIXIngad.top_element=Convert(hand_state->vol_in[0]);
MIX0gad.top_element=Convert(hand_state->vol_play[0]);
MIX1gad.top_element=Convert(hand_state->vol_play[1]);
MIX2gad.top_element=Convert(hand_state->vol_play[2]);
MIX3gad.top_element=Convert(hand_state->vol_play[3]);
MIXOutgad.top_element=Convert(hand_state->vol_out[0]);

delete_gadget_chain(wind);
RefreshWind(wind);

Y=4+sp*2;
Yz=wind->Height-(sp*2)-3;

xs=(wind->Width-(sp*10))/6;
xz=wind->Width-(sp*2);

SetDrMd(wind->RPort,JAM1);

x=xz-xs-sp+((xs-23)/2);

if (xs>24)
	for(z=0;z < 6; z++)
		{
		c=z;
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
		Move(wind->RPort,(int)x+2,(int)(wind->Height-(sp)+2));
		Text(wind->RPort,&frq[c<<2], 3);
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
		Move(wind->RPort,(int)x+1,(int)(wind->Height-(sp)+1));
		Text(wind->RPort,&frq[c<<2], 3);
		x=x-xs-sp;
		}
else Yz=Yz+7;

while (ns)
	{
	ns->box_y1=Y;
	ns->box_y2=Yz;

	x=xz-sp;
	xz=x-xs;

	ns->box_x1=xz;
	ns->box_x2=x;
	ns=ns->next;
	}
if (create_gadget_chain(wind,&MIXOutgad)==0)  {telluser("Tiny Mixer:","Could not create gadgets!",0); exit(10);}

}
/**************************************************************/
static void MixKnobAdj();

static void MIX0func(k)
struct slider *k;
{
MixKnobAdj(k,0);
}
/**************************************************************/

static void MIX1func(k)
struct slider *k;
{
MixKnobAdj(k,0x01);
}
/**************************************************************/
static void MIX2func(k)
struct slider *k;
{
MixKnobAdj(k,0x02);
}
/**************************************************************/
static void MIX3func(k)
struct slider *k;
{
MixKnobAdj(k,0x03);
}
/**************************************************************/
static void MIXInfunc(k)
struct slider *k;
{
MixKnobAdj(k,0x04);
}
/**************************************************************/
static void MIXOutfunc(k)
struct slider *k;
{
MixKnobAdj(k,0x05);
}
/**************************************************************/
static void MixKnobAdj(k,m)
struct slider *k;
int m;
{
char buf[20];
int w;
int Value,dbs;

dbs=6 - k->top_element;
if (dbs==-41) dbs=-100;
Value=(dbs+100)<<5;

if (FindTask(NULL)!=task) 
	{
	if (Value>=0) BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,KEY[m],Value,0,0,0);
	else BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,KEY[m],0,0,0,0);
	}

if (Value)
	{ 
	if (dbs==0) sprintf(buf," 00dB");
	else	if (dbs<0) sprintf(buf,"-%02ddB",-dbs);
		else sprintf(buf,"+%02ddB",dbs);
	}
else sprintf(buf,"-OOdB");

w=MIN(5,MIN((k->knob_width-4)>>3,stclen(buf)));

SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);

Move(k->kw->RPort, k->knob_x+((k->knob_width-w*8)>>1), k->knob_y+8);
Text(k->kw->RPort, &buf,w);
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
int dbs;
dbs=(x>>5)-100;
if (dbs>6) dbs=6;
if (dbs<-40) return(47);
else return(6-dbs);
}
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
USHORT Convert();

if (state->hand==NULL) 
	{
	hand=FindAudioHandler(NULL,-1,0,NULL,-1);
	if (hand)
		{
		state->hand=hand;
		state->handler_name=hand->name;
		state->handler_class_id =hand->class_id;
		}
	else {telluser("Tiny Mixer:","Could not find a Handler.",NULL);return(-1);}
	}
else	hand=state->hand;
hand_state=(struct Handler12 *)hand->handler_mod->state;
LockMod(hand_state);

Hmod=hand->handler_mod;

if((KEY[0]=AllocChan(hand,0, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
if((KEY[1]=AllocChan(hand,1, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
if((KEY[2]=AllocChan(hand,2, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
if((KEY[3]=AllocChan(hand,3, CK_CHAN_PLAY | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
if((KEY[4]=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
if((KEY[5]=AllocChan(hand,0, CK_CHAN_OUTPUT | CK_ID_ONLY))==NULL) telluser("Tiny Mixer:","COULD NOT GET KEY",0);
return(NULL);
}
/**************************************************************/
