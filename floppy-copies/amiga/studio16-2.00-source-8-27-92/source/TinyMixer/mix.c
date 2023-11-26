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
int NumMix=5;
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod,*Hmod;
struct Window 		*wind;
/**********************************/
/**********************************/
struct Task *task,*FindTask();
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void handle_intuievents();
/**********************************/
/**********************************/
struct ChanKey *Chan[33],*NChan[33];
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
void WinGadSize();
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
/**********************************/
/**********************************/
struct TinyMixModState
	{
	struct StandardModState state;
	int menucksum;
	byte itemnum[33];
	};
struct TinyMixModState *state;
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
struct NewModule ModData ={NULL,0,sizeof(struct TinyMixModState)};
void ColorGadList();

task=FindTask(NULL);
openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0)  
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Tiny Mixer:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

state=(struct TinyMixModState *)mod->state;
if (!(state->state.flags & MS_STATE_INITILIZED)) 
	{
	state->menucksum=0;
	state->itemnum[0]=1;
	state->itemnum[1]=2;
	state->itemnum[2]=-1;
	}

se_cmd_select_module  		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_channel_volume 	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_info_prefs_changed		= GetEventID("SE_INFO_PREFS_CHANGED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

void handle_studioevents()
{
struct StudioEvent *event;
struct ChanKey *key;
int x;
void move_knob();
USHORT Convert(),cvrt;
struct slider *slid;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
{

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
		for(x=0;x<NumMix;x++)
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
ReplyMsg(event);
}
}

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

void handle_intuievents()
{
struct ChanKey *FindCheckedChan();
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

	flag=FALSE;

	switch (class)
		{
		case CLOSEWINDOW:
		/*********************************/
			HideWind();
			return;
		break;

		case NEWSIZE:
		/*********************************/
			WinGadSize();
		break;

		case MENUPICK:
		/*********************************/
			t=0;
			mi=Titles[0].FirstItem;

			while ((mi=FindCheckedItem(mi)) && t<32)
				{
				NChan[t]=ItemData(mi);
				state->itemnum[t]=ItemNum(mi);
				t++;
				mi=mi->NextItem;
				}
			state->itemnum[t]=-1;

			if (t!=NumMix) {NumMix=t;flag=TRUE;}

			for (x=0;x<NumMix;x++)
				{
				if(Chan[x]!=NChan[NumMix-x-1]) {Chan[x]=NChan[NumMix-x-1]; flag=TRUE;}
				}
			if (flag) WinGadSize();
		break;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/***********************************************************/
void DisplayWind()
{
struct Window *OpenNewWind();
struct MenuItem *BuildChannelMenu();
int t,cksum;
struct MenuItem *FindCheckedItem(),*mi;
struct ChanKey *ItemData();

Titles[0].FirstItem=BuildChannelMenu(CK_CHAN_VOLUME,MENU_MULTI_SELECT);

if (wind = OpenNewWind(&MIX_NewWindow,mod->state,NULL,&Titles[0]))
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ColorGadList(&MIXgad);

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

	while ((mi=FindCheckedItem(mi)) && t<32)
		{
		NChan[t]=ItemData(mi);
		t++;
		mi=mi->NextItem;
		}
	NumMix=t;

	for (t=0;t<NumMix;t++)
		{
		Chan[t]=NChan[NumMix-t-1]; 
		}


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

FreeChannelMenu(Titles[0].FirstItem);

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
int Y,Yz,z;
int s,w;
USHORT Convert();
char	str[100],lc;
int len;
if (!NumMix) {RefreshWind(wind);return;}

Y=4+sp*2;
Yz=wind->Height-(sp*2)-3;

if (wind->Width< (20+NumMix*16)) 
	{

	if (StudioBase->screen->Width < (wind->LeftEdge+(20+NumMix*16)))
		MoveWindow(wind,StudioBase->screen->Width - (wind->LeftEdge+(20+NumMix*16)),0);

	SizeWindow(wind,(20+NumMix*16)-wind->Width,0);
	return;
	}

WindowLimits(wind,MAX((20+NumMix*16),76),0,0,0);

delete_gadget_chain(wind);
RefreshWind(wind);

s=12; w=wind->Width-(2*s)+sp-1;
z=0;
while (z<NumMix)
	{
	MIXgad[z].box_y1=Y;
	MIXgad[z].box_y2=Yz;

	MIXgad[z].box_x1=s+((w*z)/NumMix);
	MIXgad[z].box_x2=s+((w*(z+1))/NumMix)-sp;

	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	if (Chan[z])
		{
		sprintf(str,"%s",Chan[z]->name);
		len=((MIXgad[z].box_x2-MIXgad[z].box_x1)>>3);
		if (len<strlen(str))
			{
			lc=str[strlen(str)-1];
			if (lc>='0' && lc<='9') str[len-1]=lc;
			str[len]=0;
			}
		S16Text(wind->RPort,str, strlen(str),MIXgad[z].box_x1,wind->Height-5);
		MIXgad[z].top_element=Convert(Chan[z]->parms.volume);
		}

	MIXgad[z].next=&MIXgad[z+1];
	z++;
	}
MIXgad[z-1].next=NULL;

if (create_gadget_chain(wind,&MIXgad)==0)  {telluser("Tiny Mixer:","Could not create gadgets!",0); exit(10);}

}
/**************************************************************/
/**************************************************************/
static void MIXfunc(k)
struct slider *k;
{
int m=(int)k->user_data;
char buf[20];
int w;
int Value,dbs;

if (!Chan[m]) return;

dbs=6 - k->top_element;
if (dbs==-41) dbs=-100;
Value=(dbs+100)<<5;

if (FindTask(NULL)!=task) 
	{
	if (Value>=0) BroadcastEventParms(se_cmd_set_channel_volume,Chan[m],Value,0,0,0);
	else BroadcastEventParms(se_cmd_set_channel_volume,Chan[m],0,0,0,0);
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
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
