#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "mix.data"
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
void update_knob();
/**********************************/
#define sp 5.0
#define Xsz 15
#define Ysz 100
#define Xs 20
#define Ys 20
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
/**********************************/
struct Module *mod;
extern struct studioBase *studioBase;
static struct Window *wind;
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_channel_volume;
/**********************************/
/**********************************/
void WinGadSize();
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int  handle_intuievents();
/**********************************/
/**********************************/
/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/
void main()
{
int mask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule();				/** initalize module **/
DisplayWind();				/** open window **/

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
/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/

/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/

void InitModule()
{
static struct NewModule ModData ={"Mixer",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Could Not Add Module","",0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module  		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_cmd_set_channel_volume 	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}

/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/

void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if (event->type == se_cmd_select_module)
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();

	/*********************************/
	if (event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}
	/*********************************/
	if (event->type == se_cmd_set_channel_volume)
		{
		if ((struct Module *)event->arg3!=mod)
			{
			if (event->arg1==0) update_knob(MIX0gad.gad_ptr,event->arg2);
			if (event->arg1==1) update_knob(MIX1gad.gad_ptr,event->arg2);
			if (event->arg1==2) update_knob(MIX2gad.gad_ptr,event->arg2);
			if (event->arg1==3) update_knob(MIX3gad.gad_ptr,event->arg2);
			if (event->arg1==4) update_knob(MIXmongad.gad_ptr,event->arg2);
			}
		}
	ReplyMsg(event);
	}
}

/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/

int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	class=message->Class;
	code=message->Code;
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
/*���������������������������������������������������������������������*/
/*���������������������������������������������������������������������*/
/***********************************************************/

void DisplayWind()
{
MIX_NewWindow.Screen = studioBase->screen;
if ((wind = OpenWind(&MIX_NewWindow))==0) telluser("Could not open MIXER window!","",0);
WinGadSize();
NotifyMeOnEvent(se_cmd_set_channel_volume, mod);
}

/*****************************************************************/

/*****************************************************************/
void HideWind()
{
struct Window *w;
IgnoreEvent(se_cmd_set_channel_volume, mod);

if (wind)
	{
	SaveGadgetState(&MIXmongad);
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
}
/*****************************************************************/
/**************************************************************/
/**************************************************************/
void WinGadSize()
{
char frq[] = {"MON","CH4","CH3","CH2","CH1"};
double Y,Yz,x,xz,xs,z;
int c;
struct NewSlider *ns=&MIXmongad;

SaveGadgetState(&MIXmongad);
delete_gadget_chain(wind);
RefreshWind(wind);

Y=4+sp*2;
Yz=wind->Height-(sp*2)-3;

xs=(wind->Width-(sp*10))/5;
xz=wind->Width-(sp*2);

SetDrMd(wind->RPort,JAM1);

x=xz-xs-sp+((xs-23)/2);

if (xs>24)
	for(z=0;z < 5; z++)
		{
		c=z;
		SetAPen(wind->RPort, DARK_BROWN);
		Move(wind->RPort,(int)x,(int)(wind->Height-(sp)));
		Text(wind->RPort,&frq[c<<2], 3);
		SetAPen(wind->RPort, WHITE);
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

if (create_gadget_chain(wind,&MIXmongad)==0)  {telluser("Could not create MIXER gadgets!","",0); exit(10);}
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
MixKnobAdj(k,1);
}
/**************************************************************/
static void MIX2func(k)
struct slider *k;
{
MixKnobAdj(k,2);
}
/**************************************************************/
static void MIX3func(k)
struct slider *k;
{

MixKnobAdj(k,3);
}
/**************************************************************/
static void MIXmonfunc(k)
struct slider *k;
{
MixKnobAdj(k,4);
}
/**************************************************************/
static void MixKnobAdj(k,m)
struct slider *k;
int m;
{
char buf[20];
int x,w,y=1;
float z,v;
int c,n;

n=150-k->top_element;
if (n>100) n=100+((n-100)<<1);
z=65535.0/200.0;
v=n;
v=v*z;
c=v;
if (!(k->flags&RENDER_ONLY)) BroadcastEventParms(se_cmd_set_channel_volume,m, c,mod,0,0,0);

x=n;

w=(k->knob_width-4)>>3;

y=0;
if (w>4) w=4;
if ((w==2 || w==1))
	if (x<101) x=n*10;
	else	y=4;

sprintf(buf,"%03d%%++",x);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);

Move(k->kw->RPort, k->knob_x+((k->knob_width-w*8)>>1), k->knob_y+8);
Text(k->kw->RPort, &buf[y],w);
}
/**************************************************************/
void update_knob(k,x)
struct slider *k;
USHORT x;
{
void move_knob();


x=200-(x/(0xffff/200));
if (x>100) x=x-50;
else x=x>>1;

k->top_element=x;
k->flags=k->flags|RENDER_ONLY;
move_knob(k);
k->flags=k->flags&(~RENDER_ONLY);
}