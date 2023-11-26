#include <exec/types.h>
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "Record.data"
/**********************************/
/**********************************/
#define Clock 10000000
/**********************************/
/**********************************/
long _stack = 4000;
char *_procname = "STUDIO_RECORDER";
long _priority = 0;
long _BackGroundIO = 0;
/**********************************/
/**********************************/
struct ChanKey *AllocChan();
struct ChanKey *mon_key=NULL;
struct ChanKey *play_key=NULL;
struct ChanKey *rec_key=NULL;
/**********************************/
/**********************************/
void ColorGadList();
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
int istc=0;
struct TimeCode *stc=(struct TimeCode *)&istc;
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_input_gain;
short se_cmd_set_cutoff_freq;
short se_cmd_set_sampling_rate;
short se_cmd_set_channel_volume;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_presave_sample;
short se_info_presave_done;
short se_cmd_stop_recording;
short se_info_channel_done;
short se_info_record_done;
short se_info_prefs_changed;
short se_info_block_recorded;
/**********************************/
int GadRef();
/**********************************/
char TempSampName[120] =  {"NoName",0};
int TempSize=0;
int GlobSampRate;
int GlobFilterCut;
int DiskSpaceAvailable;
static UBYTE	AutoFilt=0;
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void handle_gadlibevents();
static void WinGadSize();
/**********************************/
/**********************************/
void NotifyRecording();
void NotifyEndRecording();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod,*Hmod;
struct Window 		*wind;
struct StandardModState *state;
struct AudioHandler 	*hand;
struct Handler12 	*hand_state;
struct Window *Rwind;
/**********************************/
/**********************************/
struct Task *task,*FindTask();
struct MsgPort *gadlibport=0;
char sn[200];
/**********************************/
/**********************************/
int FindBestFilter();
void RecordStats();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
InitModule(argv);

while (TRUE)
	{
	mask=0;
	if (gadlibport) mask|=(1<<gadlibport->mp_SigBit);
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask); 
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if (gadlibport) if (mask & (1<<gadlibport->mp_SigBit)) handle_gadlibevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct Region region;
struct ActiveSample *as=NULL;
struct ChanKey *playkey=NULL;
struct ChanKey *reckey=NULL;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_gadlibevents()
{
void RecSam();
struct GadMsg *msg;
struct GadgetHeader *g;
ULONG flags;
void PlaySamples();

while ((msg=(struct GadMsg *)GetMsg(gadlibport))!=0) 
	{
	g = msg->g;
	flags=msg->flags;
	ReplyMsg(msg);

	if (g==but[2].gad_ptr) 
		{
		if (flags & BUTN_HIT) RecSam(sn);
		else 
			{
			if (as) ASStop(as);
			as=NULL;
			if (reckey) {FreeChan(reckey);reckey=NULL;}
			if (playkey) {FreeChan(playkey);playkey=NULL;}
			NotifyEndRecording();
			}
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
char fn[100];
void RecSam(name)
char *name;
{
struct Disk_Samp *FindSample();
struct Disk_Samp *samp;
int y=0;
int DiskSpaceAvail();

DiskSpaceAvailable=DiskSpaceAvail();

if (DiskSpaceAvailable<1) 
	{
	telluser("Record:","Not enough disk space available");
	but[2].gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(but[2].gad_ptr);
	return;
	}

sprintf(fn,"%s",name);
while(FindSample(fn,-1))
	{
	y++;
	sprintf(fn,"%s_#%d",name,y);
	}

if (CreateSample(fn, 0, &samp)==NO_ERROR)
	{
	region.samp=samp;
	region.start=0;
	region.end=0;
	region.owner=mod;
	region.user_data=NULL;
	region.parms.rate=GlobSampRate;
	region.parms.filter3db=GlobFilterCut;
	region.parms.volume=3200;
	if ((as=(struct ActiveSample *)ASOpen(&region, AS_RECORD|AS_AUTO_CLOSE|AS_AUTO_DATA))!=0)
		{
		as->key=(struct ChanKey *)AllocChan(hand,0,CK_CHAN_RECORD|CK_TAP_INPUT);
		playkey=(struct ChanKey *)AllocChan(hand,3,CK_CHAN_PLAY);
		reckey=as->key;
		if (reckey==0 || playkey==0)
			{
			if (reckey) {FreeChan(reckey);reckey=NULL;}
			if (playkey) {FreeChan(playkey);playkey=NULL;}
			ASClose(as);
			as=NULL;
			CloseSample(samp);
			telluser("Record:","Can't Allocate a record channel!",NOTICE_RETURN_NOW);
			but[2].gad_ptr->flags&=~BUTN_HIT;
			refresh_gadget(but[2].gad_ptr);
			return;
			}
		}
	else 
		{
		CloseSample(samp);
		if (as) ASStop(as);
		as=NULL;
		but[2].gad_ptr->flags&=~BUTN_HIT;
		refresh_gadget(but[2].gad_ptr);
		telluser("Record:","Can not open record sample, check Work: path",NOTICE_RETURN_NOW);
		return;
		}
	}
else {telluser("Record:","Can't create record sample",0);but[2].gad_ptr->flags&=~BUTN_HIT;refresh_gadget(but[2].gad_ptr);return;}
ASTrigger(as);
NotifyRecording();
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule(name)
char *name;
{
char *FindFileName();
struct AudioHandler *FindAudioHandler();
static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState)};

task=FindTask(NULL);

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) {telluser("Record:","Could Not Add Module",0);closelibraries();exit(10);}

state=mod->state;
state->flags|=MS_CHOOSE_HANDLER;

se_cmd_select_module		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_info_channel_done		= GetEventID("SE_INFO_CHANNEL_DONE");
se_info_record_done		= GetEventID("SE_INFO_RECORD_DONE");
se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_info_prefs_changed		= GetEventID("SE_INFO_PREFS_CHANGED");
se_info_block_recorded		= GetEventID("SE_INFO_BLOCK_RECORDED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;
struct ChanKey *tempkey;
int x;
struct ActiveSample *ps;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module && ((struct Module *)event->arg1==mod))
		{
		if (wind==NULL)	DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
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
	if (wind && event->type == se_cmd_set_channel_volume)
		{
		tempkey=(struct ChanKey *)event->arg1;
		if ((tempkey->flags&CK_CHAN_MONITOR) && (hand==tempkey->handler) && (tempkey->chan_num==mon_key->chan_num))
			if(event->arg2)
				if(!(MONITORbutton.gad_ptr->flags & BUTN_HIT)){MONITORbutton.gad_ptr->flags|=BUTN_HIT; refresh_button(MONITORbutton.gad_ptr);}
				else ;
			else
				{MONITORbutton.gad_ptr->flags&=~BUTN_HIT; refresh_button(MONITORbutton.gad_ptr);}
		}

	if (event->type == se_info_prefs_changed)
		/*********************************/
		{
		if (mon_key) FreeChan(mon_key);

		UnlockMod(hand_state);
		hand=state->hand;
		Hmod=hand->handler_mod;
		hand_state=(struct Handler12 *)hand->handler_mod->state;

		if((mon_key=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Record:","COULD NOT GET KEY",0);

		GadRef();

		FREQknob->list_size=FREQgad.list_size;
		FREQknob->top_element=FREQgad.top_element;
		RATEknob->top_element=RATEgad.top_element;
		GAINknob->top_element=GAINgad.top_element;
		move_knob(FREQknob);
		move_knob(RATEknob);
		move_knob(GAINknob);
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_sampling_rate && event->arg1==(int)(hand))
		{
		RATEknob->top_element=660-(((Clock/2)/event->arg2)-1);
		move_knob(RATEknob);
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_cutoff_freq && event->arg1==(int)(hand))
		{
		x=FindBestFilter(event->arg2);
		if (FREQknob->top_element!=hand_state->number_of_filter_settings-x-1)
			{
			FREQknob->top_element=hand_state->number_of_filter_settings-x-1;
			move_knob(FREQknob);
			}
		}

	/*********************************/
	if (wind && event->type == se_cmd_set_input_gain && event->arg1==(int)(hand))
		{
		GAINknob->top_element=event->arg2;
		move_knob(GAINknob);
		}

	/*********************************/
	if (event->type == se_info_block_recorded && as)
		{
		ps=(struct ActiveSample *)event->arg1;
		istc=ps->region.parms.starttimecode;
		TempSize=ASPos(ps);	/** Get Record position **/
		if(TempSize>DiskSpaceAvailable)
			{
			if (as) ASStop(as);
			as=NULL;
			if (reckey) {FreeChan(reckey);reckey=NULL;}
			if (playkey) {FreeChan(playkey);playkey=NULL;}
			but[2].gad_ptr->flags&=~BUTN_HIT;
			refresh_gadget(but[2].gad_ptr);
			NotifyEndRecording();
			telluser("Record:","Disk Full!",NULL);
			}
		else RecordStats();
		}

	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
int x,w,s,i;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

	if(class== CLOSEWINDOW)
		{
		HideWind();
		return;
		}
	if(class== NEWSIZE)
		{
		if (wind) 
			{
			RefreshWind(wind);

			new_gadget_size(RATEgad.gad_ptr,10,32, wind->Width-11,44);
			new_gadget_size(FREQgad.gad_ptr,((wind->Width-20)/2)+3,16, wind->Width-11,28);
			new_gadget_size(GAINgad.gad_ptr,10,16,((wind->Width-20)/2)-3,28);

			s=((wind->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
			w=(s/NUMGADS);
			x=10<<8;

			for(i=0;i<NUMGADS;i++)
				{
				new_gadget_size(but[i].gad_ptr,(x>>8),wind->Height-16,(x+w)>>8,wind->Height-6);
				refresh_gadget(but[i].gad_ptr);
				x=x+w+(5<<8);
				}

			refresh_gadget_chain(wind);
			}
		}
	}
}
/*****************************************************************/
void DisplayWind()
{
int x,w,s,i;
if (!GadRef())
if (wind = OpenNewWind(&Record_NewWindow,mod->state,PREFERENCES,NULL,NULL))
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ColorGadList(&MONITORbutton);
/***/
	s=((wind->Width-21)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		but[i].box_x1=(x>>8);
		but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
		but[i].box_y1=wind->Height-16;
		but[i].box_y2=wind->Height-6;
		}

	RATEgad.box_x2=wind->Width-11;
	FREQgad.box_x2=wind->Width-11;
	FREQgad.box_x1=((wind->Width-20)/2)+3;
	GAINgad.box_x2=((wind->Width-20)/2)-3;


	gadlibport = (struct MsgPort *)CreatePort(0,0);
	but[2].notifyme=gadlibport;
	if (create_gadget_chain(wind,&MONITORbutton)) ;
	FREQknob = (struct slider *)FREQgad.gad_ptr;
	RATEknob = (struct slider *)RATEgad.gad_ptr;
	GAINknob = (struct slider *)GAINgad.gad_ptr;

	if((mon_key=AllocChan(hand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Record:","COULD NOT GET KEY",0);

	NotifyMeOnEvent(se_cmd_set_input_gain, mod);
	NotifyMeOnEvent(se_cmd_set_cutoff_freq, mod);
	NotifyMeOnEvent(se_cmd_set_sampling_rate, mod);
	NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
	NotifyMeOnEvent(se_info_block_recorded, mod);
	sprintf(sn,"%s",TempSampName);
	RecordStats();
	}
else telluser("Record:","Could not Open Window",0);
}
/*****************************************************************/
void HideWind()
{
struct Window *w;
UnlockMod(hand_state);

if (wind && (!rec_key))
	{
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);

	IgnoreEvent(se_cmd_set_input_gain, mod);
	IgnoreEvent(se_cmd_set_cutoff_freq, mod);
	IgnoreEvent(se_cmd_set_sampling_rate, mod);
	IgnoreEvent(se_cmd_set_channel_volume, mod);
	IgnoreEvent(se_info_block_recorded, mod);

	DeletePort(gadlibport);	gadlibport=0;

	if (mon_key) FreeChan(mon_key);
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
int GadRef()
{
int x;

/***/
	if (state->hand==NULL)
		{
		if (hand=FindAudioHandler(NULL,-1,0,NULL,-1))
			{
			state->hand=hand;
			state->handler_name=hand->name;
			state->handler_class_id =hand->class_id;
			}
		else {telluser("Record:","Could not find handler, Closing Window",NULL);return(-1);}
		}
	else	hand=state->hand;

	Hmod=hand->handler_mod;
	hand_state=(struct Handler12 *)hand->handler_mod->state;
	LockMod(hand_state);
/***/
	x=FindBestFilter(hand_state->filter[0]);

	FREQgad.list_size=((struct Handler12 *)hand->handler_mod->state)->number_of_filter_settings;
	FREQgad.top_element=((struct Handler12 *)hand->handler_mod->state)->number_of_filter_settings-x-1;

	RATEgad.top_element=660-(((Clock/2)/hand_state->sampling_rate[0])-1);

	GAINgad.top_element=hand_state->input_gain[0];

	if(((struct Handler12 *)hand->handler_mod->state)->vol_in[0])	MONITORbutton.flags|=BUTN_HIT;
	else	MONITORbutton.flags&=~BUTN_HIT;

return(NULL);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
static void GAINfunc(k)
struct slider *k;
{
char buf[20];
int x;
static int KFLAG=0;
static int VOL=0;

sprintf(buf,"Gain %2d",k->top_element);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

x=k->top_element;

if (KFLAG!=(k->flags&BUTN_HIT)) 
	{
	if (!KFLAG) 
		{
		VOL=hand_state->vol_in[0];
		BroadcastEventParms(se_cmd_set_channel_volume,mon_key,3200,0,0,0);
		}
	else	BroadcastEventParms(se_cmd_set_channel_volume,mon_key,VOL,0,0,0);
	KFLAG=(k->flags&BUTN_HIT);
	}
if (FindTask(NULL)!=task) BroadcastEventParmsLessOne(se_cmd_set_input_gain,mod,hand,x, 0, 0, 0, 0);
}

/*****************************************************************/
static void FREQfunc(k)
struct slider *k;
{
char buf[20];
int	x;

if (FindTask(NULL)!=task)
	{
	x=(hand_state->FreqValFunc)(hand_state->number_of_filter_settings-k->top_element-1);
	BroadcastEventParmsLessOne(se_cmd_set_cutoff_freq,mod,hand,x, 0, 0, 0,0);
	}
else
	{
	x=FindBestFilter(hand_state->filter[0]);
	k->list_size=hand_state->number_of_filter_settings;
	k->top_element=hand_state->number_of_filter_settings-x-1;
	x=(hand_state->FreqValFunc)(x);
	}

sprintf(buf,"Filter %5d",x);
GlobFilterCut=x;
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);
}
/*****************************************************************/
static void RATEfunc(k)
struct slider *k;
{
char buf[20];
int	x,y;
int	c;

x=660-(k->top_element);
x++;
y=(Clock/2)/x;
c=y;
GlobSampRate=c;

sprintf(buf,"Rate %5d",c);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

if (FindTask(NULL)!=task) 
	{
	BroadcastEventParmsLessOne(se_cmd_set_sampling_rate,mod,hand,c,0, 0, 0,0);
	if (AutoFilt)	BroadcastEventParms(se_cmd_set_cutoff_freq,hand,c>>1,0, 0, 0,0);
	}
}
/*****************************************************************/
static void MONITORfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,mon_key,0x0C80,0,0,0);
else
	BroadcastEventParmsLessOne(se_cmd_set_channel_volume,mod,mon_key,0x0000,0,0,0);
}
/*****************************************************************/
/*****************************************************************/
static void FILEfunc(b)
struct button *b;
{
FILEbut = b;
newgetstr(TempSampName,"Sample Name?",8*40);
xor_button(b);
sprintf(sn,"%s",TempSampName);
RecordStats();
}
/*****************************************************************/
static void AUTOFILTfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	{
	BroadcastEventParms(se_cmd_set_cutoff_freq,hand,GlobSampRate>>1, 0, 0, 0,0);
	AutoFilt=1;
	}
else	AutoFilt=0;
}
/*****************************************************************/
/*****************************************************************/
int FindBestFilter(target)
int target;
{
int x;
for(x=0;x < hand_state->number_of_filter_settings && target<(hand_state->FreqValFunc)(x);x++) ;
if (x==hand_state->number_of_filter_settings) x=hand_state->number_of_filter_settings-1;
return(x);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct NewWindow RecordingNW = {
   (640-200)>>1,
   (200-12)>>1,
   200-21,
   14+8*3+4+9,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|SMART_REFRESH|WINDOWDRAG,
   0,0,
   "-- RECORDING --", /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

/*****************************************************************/

void NotifyRecording()
{
struct Window *OpenNewWind();

if (!Rwind)
	{
	Rwind=OpenNewWind(&RecordingNW,NULL,NULL,NULL);
	istc=0;
	TempSize=0;
	RecordStats();
	}
}

void NotifyEndRecording()
{
if (Rwind) CloseWind(Rwind);
Rwind=NULL;
}
/*****************************************************************/
void RecordStats()
{
char str[100];
int q,w;
static flag=0;

LockGadLibRender();
if (Rwind)
	{
	SetAPen(Rwind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetBPen(Rwind->RPort,StudioBase->defaults.colors.gadget_window.background);
	SetDrMd(Rwind->RPort, JAM2);

	if (flag) 
		if(!(StudioBase->flags&SB_SMPTE_ON)) 
			{
			istc=0xFFFFFFFF;
			sprintf(str,"StartTime NA",stc->hours,stc->minutes,stc->seconds,stc->frames);
			flag=0;
			Move(Rwind->RPort, 5,47);
			Text(Rwind->RPort, str,strlen(str));
			}
		else 
			{
			sprintf(str,"StartTime %02d:%02d:%02d:%02d",stc->hours,stc->minutes,stc->seconds,stc->frames);
			flag=0;
			Move(Rwind->RPort, 5,47);
			Text(Rwind->RPort, str,strlen(str));
			}

	if (istc==0) {flag=1;}
	if (flag) 
		{
		sprintf(str,"Name:%-16s",fn);
		Move(Rwind->RPort, 5,20);
		Text(Rwind->RPort, str,strlen(str));
		}


	q=(TempSize*2)/(1024*1024);
	w=((((TempSize*2)-(q*1024*1024))/1024)*100)/1024;
	sprintf(str,"Size:%3d.%02dM",q,w);
	Move(Rwind->RPort, 5,29);
	Text(Rwind->RPort, str,strlen(str));

	q=((DiskSpaceAvailable*2)-(TempSize*2))/(1024*1024);
	w=(((((DiskSpaceAvailable*2)-(TempSize*2))-(q*1024*1024))/1024)*100)/1024;
	sprintf(str,"Left:%3d.%02dM",q,w);
	Move(Rwind->RPort, 5,38);
	Text(Rwind->RPort, str,strlen(str));
	}
UnLockGadLibRender();
}
/*****************************************************************/
int DiskSpaceAvail()
{
struct InfoData diskinfo;
struct FileLock      *lock;
int x;
int RamSpaceAvail();

if (!stricmp(StudioBase->defaults.temp_files_path,"ram:"))
	return(RamSpaceAvail());

lock=(struct FileLock *)Lock(StudioBase->defaults.temp_files_path,ACCESS_READ);
if (lock)
	{
	if(Info(lock,&diskinfo))
		{


		x= 
(
	(
		((diskinfo.id_NumBlocks-diskinfo.id_NumBlocksUsed)-(diskinfo.id_NumBlocks-diskinfo.id_NumBlocksUsed)/70)
		*
		(diskinfo.id_BytesPerBlock/2)
	)
	-
	(
		(
			(StudioBase->defaults.active_buf_size*4)
			+
			StudioBase->defaults.record_safety_size
		)
	)
);
		}
	UnLock(lock);
	return(x);
	}
else
	{
	telluser("Record:","Does the disk volume exist?");
	but[2].gad_ptr->flags&=~BUTN_HIT;
	refresh_gadget(but[2].gad_ptr);
	return(0);
	}
}
/*****************************************************************/
int RamSpaceAvail()
{
int x;
int RamSpaceAvail();

		x= 
(
	(
		((AvailMem (NULL)*70)/71)/2
	)
	-
	(
		(
			(StudioBase->defaults.active_buf_size*8)
			+
			StudioBase->defaults.record_safety_size
		)
	)
);
return(x);
}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
