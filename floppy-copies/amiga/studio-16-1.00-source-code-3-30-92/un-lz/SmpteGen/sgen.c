#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "Sgen.data"
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_SMPTE_GENERATOR";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
#define NUMGADS	4
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
int SMPTE_REZ=0;
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int  handle_intuievents();
void SmpteData();
void Quit();
void DisplaySmpte();
/**********************************/
/**********************************/
struct Module *mod;
struct AudioHandler *hand;
extern struct StudioBase *StudioBase;
static struct Window *wind;
/**********************************/
/**********************************/
static void WindGadSize();
/**********************************/
/**********************************/
extern unsigned long smpte_sigmask;		/* Signal from smpte timer */
extern short SmpteOn;
extern int internal_smpte;
/**********************************/
/**********************************/
short SmpteCnt=0;
int sg;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
#define PLAY	2
extern byte mode;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct SmpteSource *OldSMPTEsrc,*SetSmpteSource();
struct SmpteSource *SMPTEsrc;
struct NewSmpteSource NewSMPTEsrc={"SMPTE Generator",0,NULL};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;

InitModule(argv);				/** initalize module **/

while (TRUE)
	{
	mask=smpte_sigmask;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if (mask&smpte_sigmask) 
		{
		DisplaySmpte(&internal_smpte);
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule(name)
char *name;
{
int InitSmpteGen();
char *FindFileName();
static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState),MOD_SINGLE_RESIDENT};
struct NewAudioHandler HandData = {"SMPTE GENERATOR",0,0,0,0,0,0,0,0,-1,0};
openlibraries();
StandardEnviroment();
ColorGadList(&but[4]);
ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) {telluser("SMPTE Gen:","Could Not Add Module",0);closelibraries();exit(10);}

mod->state->flags|=MS_CHOOSE_HANDLER|MS_CHOOSE_WINDOW;

HandData.handler_mod=mod;

se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;
void HandEvnt();
void StopFunc();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			Quit();
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

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{
		HideWind();
		return(0);
		}
	}
return(0);
}
/*****************************************************************/
void DisplayWind()
{
static void InterlaceAdjust();
struct Window *OpenNewWind();
struct AudioHandler *FindAudioHandler();
struct SmpteSource *AddSmpteSource();
int x,w,s,i;

if (wind = OpenNewWind(&SMON_NewWindow,mod->state,NULL,NULL,NULL))
	{
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	SMPTE_REZ=StudioBase->defaults.smpte_mode;

	s=((wind->Width-20)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		but[i].box_x1=(x>>8);
		but[i].box_x2=(x+w)>>8;
		but[i].box_y1=30;
		but[i].box_y2=wind->Height-6;
		x=x+w+(5<<8);
		}
	but[4].box_x2=wind->Width-10;

	x=0;
	DisplaySmpte(&x);
	create_gadget_chain(wind,&but[4]);
	WindGadSize();

	SMPTEsrc=AddSmpteSource(&NewSMPTEsrc);
	OldSMPTEsrc=SetSmpteSource(SMPTEsrc);

	if ((sg=InitSmpteGen())!=NO_ERROR) {telluser("SMPTE Gen:","Can not add interupt",NULL);Quit();}

	}
else telluser("SMPTE Gen:","Can not open window!",0);
}
/*****************************************************************/
void HideWind()
{
struct Window *w;
int FreeSmpteGen();

if (wind)
	{
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	if (sg==NO_ERROR) FreeSmpteGen();
	DeleteSmpteSource(SMPTEsrc);
	SetSmpteSource(OldSMPTEsrc);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void DisplaySmpte(x)
struct TimeCode *x;
{
char buff[14];

if (wind)
	{
	sprintf(buff,"%02d:%02d:%02d:%02d",x->hours,x->minutes,x->seconds,x->frames);

	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.hitbox_unselected);
	SetDrMd(wind->RPort, JAM2);
	Move(wind->RPort,13 ,24);
	Text(wind->RPort,buff,11);
	}
}

/**************************************************************/
/**************************************************************/
static void WindGadSize()
{
int x,r,s,i,w,h;

if (wind)
	{
	RefreshWind(wind);

	s=((wind->Width-20)<<8) - ((5<<8)*(NUMGADS-1));
	r=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		new_gadget_size(but[i].gad_ptr,(x>>8),30,(x+r)>>8,wind->Height-6);
		refresh_gadget(but[i].gad_ptr);
		x=x+r+(5<<8);
		}
		new_gadget_size(but[4].gad_ptr,110,16,wind->Width-10,26);
		refresh_gadget(but[4].gad_ptr);

	x=0;
	DisplaySmpte(&x);
	draw_indent(wind->RPort,10,16,103,26,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	SetAPen(wind->RPort,StudioBase->defaults.colors.slider_knob.draw_unselected);
	SetDrMd(wind->RPort, JAM2);

	w=(but[0].gad_ptr->box_x2-but[0].gad_ptr->box_x1)/6;
	h=(but[0].gad_ptr->box_y2-but[0].gad_ptr->box_y1)/6;
/*** PLAY ***/
	Move(wind->RPort,but[0].gad_ptr->box_x1+w,but[0].gad_ptr->box_y1+h);
	Draw(wind->RPort,but[0].gad_ptr->box_x2-w,(but[0].gad_ptr->box_y1+h+but[0].gad_ptr->box_y2-h)>>1);
	Draw(wind->RPort,but[0].gad_ptr->box_x1+w,but[0].gad_ptr->box_y2-h);
	Draw(wind->RPort,but[0].gad_ptr->box_x1+w,but[0].gad_ptr->box_y1+h);
/*** STOP ***/
	Move(wind->RPort,but[1].gad_ptr->box_x1+w,but[1].gad_ptr->box_y1+h);
	Draw(wind->RPort,but[1].gad_ptr->box_x2-w,but[1].gad_ptr->box_y1+h);
	Draw(wind->RPort,but[1].gad_ptr->box_x2-w,but[1].gad_ptr->box_y2-h);
	Draw(wind->RPort,but[1].gad_ptr->box_x1+w,but[1].gad_ptr->box_y2-h);
	Draw(wind->RPort,but[1].gad_ptr->box_x1+w,but[1].gad_ptr->box_y1+h);
/*** REWD ***/
	Move(wind->RPort,but[2].gad_ptr->box_x2-w,but[2].gad_ptr->box_y1+h+h);
	Draw(wind->RPort,(but[2].gad_ptr->box_x1+w+but[2].gad_ptr->box_x2-w)>>1,(but[2].gad_ptr->box_y1+h+but[2].gad_ptr->box_y2-h)>>1);
	Draw(wind->RPort,but[2].gad_ptr->box_x2-w,but[2].gad_ptr->box_y2-h-h);
	Draw(wind->RPort,but[2].gad_ptr->box_x2-w,but[2].gad_ptr->box_y1+h+h);

	Move(wind->RPort,(but[2].gad_ptr->box_x1+w+but[2].gad_ptr->box_x2-w)>>1,but[2].gad_ptr->box_y1+h+h);
	Draw(wind->RPort,but[2].gad_ptr->box_x1+w,(but[2].gad_ptr->box_y1+h+but[2].gad_ptr->box_y2-h)>>1);
	Draw(wind->RPort,(but[2].gad_ptr->box_x1+w+but[2].gad_ptr->box_x2-w)>>1,but[2].gad_ptr->box_y2-h-h);
	Draw(wind->RPort,(but[2].gad_ptr->box_x1+w+but[2].gad_ptr->box_x2-w)>>1,but[2].gad_ptr->box_y1+h+h);
/*** FORW ***/
	Move(wind->RPort,but[3].gad_ptr->box_x1+w,but[3].gad_ptr->box_y1+h+h);
	Draw(wind->RPort,(but[3].gad_ptr->box_x1+w+but[3].gad_ptr->box_x2-w)>>1,(but[3].gad_ptr->box_y1+h+but[3].gad_ptr->box_y2-h)>>1);
	Draw(wind->RPort,but[3].gad_ptr->box_x1+w,but[3].gad_ptr->box_y2-h-h);
	Draw(wind->RPort,but[3].gad_ptr->box_x1+w,but[3].gad_ptr->box_y1+h+h);

	Move(wind->RPort,(but[3].gad_ptr->box_x1+w+but[3].gad_ptr->box_x2-w)>>1,but[3].gad_ptr->box_y1+h+h);
	Draw(wind->RPort,but[3].gad_ptr->box_x2-w,(but[3].gad_ptr->box_y1+h+but[3].gad_ptr->box_y2-h)>>1);
	Draw(wind->RPort,(but[3].gad_ptr->box_x1+w+but[3].gad_ptr->box_x2-w)>>1,but[3].gad_ptr->box_y2-h-h);
	Draw(wind->RPort,(but[3].gad_ptr->box_x1+w+but[3].gad_ptr->box_x2-w)>>1,but[3].gad_ptr->box_y1+h+h);
	}
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (mod) DeleteModule(mod);
if (wind) HideWind();
if (hand) DeleteAudioHandler(hand);
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
/*void PlayFunc(){}
void StopFunc(){}
void FastForwardFunc(){}
void RewindFunc(){}
void ResetFunc(){}
int FreeSmpteGen() {return(NO_ERROR);}
int InitSmpteGen() {return(NO_ERROR);}*/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
