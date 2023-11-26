#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "clib/macros.h"
/**********************************/
short se_cmd_set_sampling_rate;
short se_cmd_set_cutoff_freq;
short se_cmd_set_input_gain;
short se_cmd_set_channel_volume;
/**********************************/
struct Window  *spwind=NULL;
struct MsgPort *spport=NULL;
struct Task *sptask,*FindTask();
/**********************************/
extern struct AudioHandler 	*hand;
struct AudioHandler 		*sphand;
extern struct Module 		*mod;
extern int GlobSampRate;
extern int GlobFilterCut;
extern int plock;
/**********************************/
BOOL SPEXIT=FALSE;
#define WINDW 311
#define WINDH 64
/**********************************/
void GAINfunc();
void FILTERfunc();
void RATEfunc();
void SPDisplayWindow();
void SPHideWindow();
void sphandle_intuievents();
void sphandle_studioevents();
void SampleParmsMain();
void OpenSampleParmsWindow();
/**********************************/
/**********************************/
/**********************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct NewWindow SPNewWind = {
   -1,
   -1,
   WINDW,
   WINDH,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   "Transport - Handler Parameters", /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void GAINfunc();
struct NewSlider GAINgad=
	{NULL,					/* *next (gadget) */
	10,16,300,16+12,			/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ|MANUAL_XOR|SLIDER_ARROWS,			/* Flags */
	GAINfunc,				/* Call Function */
	NULL,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	100,1,51,				/* list_size | show_size | top */
	8*7+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void FILTERfunc();
struct NewSlider FILTERgad=
	{&GAINgad,					/* *next (gadget) */
	10,31,300,31+12,				/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,			/* Flags */
	FILTERfunc,				/* Call Function */
	NULL,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	28,1,26,					/* list_size | show_size | top */
	8*12+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RATEfunc();
struct NewSlider RATEgad=
	{&FILTERgad,					/* *next (gadget) */
	10,46,300,46+12,				/* Box x1,y1,x2,y2 */
	SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,			/* Flags */
	RATEfunc,				/* Call Function */
	NULL,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	600,1,550,					/* list_size | show_size | top */
	8*10+8,-1};				/* knob_width | knob_Height */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void OpenSampleParmsWindow()
{
if (spwind) {WindowToFront(spwind);return;}
CreateTask("STUDIO_TRANSPORT_HANDLER_PARMS",0,SampleParmsMain,4000);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SampleParmsMain()
{
int mask;
sphand=hand;
plock++;
sptask=FindTask(NULL);
SPDisplayWindow();
SPEXIT=FALSE;
while (!SPEXIT)
	{
	mask=NULL;
	if (spport) mask|=(1<<spport->mp_SigBit);
	if (spwind) mask |= (1<<spwind->UserPort->mp_SigBit);
	mask=Wait(mask);
	if (spwind) if(mask&(1<<spwind->UserPort->mp_SigBit))  sphandle_intuievents();
	if (spport) if (mask & (1<<spport->mp_SigBit)) sphandle_studioevents();
	}
SPHideWindow();
plock--;
return;
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void sphandle_studioevents()
{
struct StudioEvent *event;
int x;

while ((event=(struct StudioEvent *)GetMsg(spport))!=0)
	{
	/*********************************/
	if (event->type == se_cmd_set_sampling_rate && event->arg1==(int)(sphand))
		{
		x=(*sphand->FindBestRate)(sphand->current_sampling_rate);
		if (x!=((struct slider *)(RATEgad.gad_ptr))->top_element)
			{
			((struct slider *)(RATEgad.gad_ptr))->top_element=x;
			move_knob(RATEgad.gad_ptr);
			}
		}

	/*********************************/
	if (event->type == se_cmd_set_cutoff_freq && event->arg1==(int)(sphand))
		{
		if (sphand->flags&AH_FILTER_ADJ)
			{
			x=(*sphand->FindBestFilter)(sphand->current_filter);
			if (((struct slider *)(FILTERgad.gad_ptr))->top_element!=x)
				{
				((struct slider *)(FILTERgad.gad_ptr))->top_element=x;
				move_knob(FILTERgad.gad_ptr);
				}
			}
		}

	/*********************************/
	if (event->type == se_cmd_set_input_gain && event->arg1==(int)(sphand))
		{
		x=(*sphand->FindBestGain)(sphand->current_input_gain);
		if (x!=((struct slider *)(GAINgad.gad_ptr))->top_element)
			{
			((struct slider *)(GAINgad.gad_ptr))->top_element=x;
			move_knob(GAINgad.gad_ptr);
			}
		}
	/*********************************/
	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void sphandle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(spwind->UserPort))!=0)
	{
	HandleIntuitMess(spwind,message);
	class=message->Class;
	ReplyMsg(message);

	if (class == CLOSEWINDOW)
		{
		SPEXIT=TRUE;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SPHideWindow()
{
if (spwind)
	{
	IgnoreEventPort(se_cmd_set_sampling_rate,spport);
	IgnoreEventPort(se_cmd_set_cutoff_freq,spport);
	IgnoreEventPort(se_cmd_set_input_gain,spport);
	DeletePort(spport);	spport=NULL;
	delete_gadget_chain(spwind);
	CloseWind(spwind);
	spwind=NULL;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SPDisplayWindow()
{
struct Window *OpenNewWind();

if (sphand->flags & AH_FILTER_ADJ)
	{
	RATEgad.box_y1=46;
	RATEgad.box_y2=46+12;
	RATEgad.next=(struct NewGadgetHeader *)&FILTERgad;
	SPNewWind.Height=WINDH;
	}
else
	{
	RATEgad.box_y1=31;
	RATEgad.box_y2=31+12;
	RATEgad.next=(struct NewGadgetHeader *)&GAINgad;
	SPNewWind.Height=WINDH-15;
	}

if ((spwind = OpenNewWind(&SPNewWind,NULL,IL_NO_MENUS,NULL))!=0)  
	{
	se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
	se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
	se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
	se_cmd_set_channel_volume	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");

	spport = (struct MsgPort *)CreatePort(0,0);

	NotifyMeOnEventPort(se_cmd_set_sampling_rate,spport);
	NotifyMeOnEventPort(se_cmd_set_cutoff_freq,spport);
	NotifyMeOnEventPort(se_cmd_set_input_gain,spport);

	FILTERgad.list_size=sphand->number_of_filter_settings;
	FILTERgad.top_element=(*sphand->FindBestFilter)(sphand->current_filter);

	RATEgad.list_size=sphand->number_of_rate_settings;
	RATEgad.top_element=(*sphand->FindBestRate)(sphand->current_sampling_rate);

	GAINgad.list_size=sphand->number_of_gain_settings;
	GAINgad.top_element=(*sphand->FindBestGain)(sphand->current_input_gain);

	ColorGadList(&RATEgad);
	if (create_gadget_chain(spwind,&RATEgad)==0)  {telluser("Transport Sample Parms:","Could not create gadgets!",0); SPEXIT=TRUE;}
	}
else telluser("Transport Sample Parms:","Can not open window!",0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RATEfunc(k)
struct slider *k;
{
char buf[20];
int	rate;

rate= (*sphand->RateValueFunc)(k->top_element);
GlobSampRate=rate;

sprintf(buf,"Rate %5d",rate);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

if (FindTask(NULL)!=sptask) BroadcastEventParmsLessOnePort(se_cmd_set_sampling_rate,spport,sphand,rate,0, 0, 0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void FILTERfunc(k)
struct slider *k;
{
char buf[20];
int	freq;

if (FindTask(NULL)!=sptask)
	{
	freq=(sphand->FilterValueFunc)(k->top_element);
	BroadcastEventParmsLessOnePort(se_cmd_set_cutoff_freq,spport,sphand,freq, 0, 0, 0,0);
	}
else
	{
	k->top_element=(*sphand->FindBestFilter)(sphand->current_filter);
	k->list_size=sphand->number_of_filter_settings;
	freq=(sphand->FilterValueFunc)(k->top_element);
	}

sprintf(buf,"Filter %5d",freq);
GlobFilterCut=freq;
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void GAINfunc(k)
struct slider *k;
{
char buf[20];
static int KFLAG=0;
static int VOL=0;
int gain;
struct ChanKey *mon_key=NULL,*AllocChan();

gain= (*sphand->GainValueFunc)(k->top_element);

sprintf(buf,"Gain%3d",(gain>>5)-100);
SetAPen(k->kw->RPort, k->c->slider_knob.draw_selected);
if (k->flags & BUTN_HIT) SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_selected);
else SetBPen(k->kw->RPort,  k->c->slider_knob.hitbox_unselected);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

if ((mon_key=AllocChan(sphand,0, CK_CHAN_MONITOR | CK_ID_ONLY))==NULL) telluser("Transport:","COULD NOT GET MONITOR KEY",0);
if (KFLAG!=(k->flags&BUTN_HIT)) 
	{
	if (!KFLAG) 
		{
		VOL=mon_key->parms.volume;

		BroadcastEventParms(se_cmd_set_channel_volume,mon_key,3200,0,0,0);
		}
	else	BroadcastEventParms(se_cmd_set_channel_volume,mon_key,VOL,0,0,0);
	KFLAG=(k->flags&BUTN_HIT);
	}
if (FindTask(NULL)!=sptask) BroadcastEventParmsLessOnePort(se_cmd_set_input_gain,spport,sphand,gain, 0, 0, 0, 0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
