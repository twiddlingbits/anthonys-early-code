#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/studio16.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "graphics/view.h"
#include <graphics/gfxmacros.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "clib/macros.h"

#include "VU.data"
/**********************************/
/* Declarations for CBACK */
long _stack = 6000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_METERS";	/* The name of the task to create         */
long _priority = -1;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/

/* Define these messy variables */
#define CUSTOM_STATE	((struct MeterModState *)(mod->state))
#define MOD_STATE	((struct MeterModState *)(mod->state))->state
#define HANDLER_STATE	((struct MeterModState *)(mod->state))->state.hand->handler_mod->state
#define HANDLER		((struct MeterModState *)mod->state)->state.hand

struct MeterModState
	{
	struct StandardModState state;
	UBYTE VUs;
	UBYTE type;
	UBYTE metr[4];
	};

/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
/*void SaveGadgetState();*/
/**********************************/
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_hilo;
short se_info_module_opened;
short se_info_module_closed;
short in_no_of_meters;
short in_graph_type;
/**********************************/
/**********************************/
SHORT SUSPEND=0;
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int handle_intuievents();
void SetupVU();
void UpdateVU();
void NewWinSize();
void UpdateGraph();
/**********************************/
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
struct Module 		*mod,*Hmod;
struct Window 		*wind;
/**********************************/
/**********************************/
/**********************************/
int inflag=1;
int NumOfVU=1;

int GRAPHstart=36+35,DIGITstart=0;

int NewWinHei,OldWinHei;

void WinGadSize();
/**********************************/
/**********************************/
/**********************************/
/**********************************/

/********************************************************************/
/********************************************************************/
/********************************************************************/
void _main(argv)
char *argv;
{
int mask;
void ColorGadList();

openlibraries();
ColorGadList(&VUselect[0]);
ColorGadList(&VUselect[1]);
ColorGadList(&VUselect[2]);
ColorGadList(&VUselect[3]);
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule(argv);				/** initalize module **/

while (TRUE)
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
void InitModule(name)
char *name;
{
char *FindFileName();
void HidePrefWind();
struct Window *DisplayPrefWind();
static struct NewModule ModData ={NULL,0,sizeof(struct MeterModState)};
ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Meters:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

if (!(MOD_STATE.flags & MS_STATE_INITILIZED)) 
	{
	CUSTOM_STATE->type=ALL;
	CUSTOM_STATE->VUs=2;
	CUSTOM_STATE->metr[0]=0;
	CUSTOM_STATE->metr[1]=5;
	CUSTOM_STATE->metr[2]=1;
	CUSTOM_STATE->metr[3]=2;
	}

VUselect[0].initial_value=CUSTOM_STATE->metr[0];
VUselect[1].initial_value=CUSTOM_STATE->metr[1];
VUselect[2].initial_value=CUSTOM_STATE->metr[2];
VUselect[3].initial_value=CUSTOM_STATE->metr[3];

NumOfVU=CUSTOM_STATE->VUs;

MOD_STATE.flags|=MS_CHOOSE_HANDLER|MS_CUSTOM_PREFS;
MOD_STATE.pref_name="Meter Prefs";
MOD_STATE.pref_display_func=DisplayPrefWind;
MOD_STATE.pref_hide_func=HidePrefWind;

se_info_hilo		= GetEventID("SE_INFO_HILO");
se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");

in_no_of_meters	= GetEventID("IdontLikeHaVingTOSEndMessagesTOMYself");
in_graph_type	= GetEventID("IdontLikeHaVingTOSEndMessagesTOMYselfAtAll");


NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*****************************************************************/
int missed;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event,evn,*HLevnt=&evn;
void HandEvnt();

missed=0;
while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=NULL)
	{
	if (event->type != se_info_hilo) HandEvnt(event);
	else {movmem(event,HLevnt,sizeof(struct StudioEvent));	missed++;}
	ReplyMsg(event);
	}

if(HLevnt->type == se_info_hilo)
/*********************************/
	if (wind && SUSPEND && HANDLER==(struct AudioHandler *)HLevnt->arg1) UpdateGraph(HLevnt->arg2,missed);

}

void HandEvnt(event)
struct StudioEvent *event;
{
	if(event->type == in_no_of_meters)
		{
		/*********************************/
		NumOfVU=(int)event->arg1;
		NewWinSize();
		}
	if(event->type == in_graph_type)
		{
		/*********************************/
		CUSTOM_STATE->type ^= (UBYTE)event->arg1; /*(ULONG)g->user_data;*/
		NewWinSize();
		}

	if(event->type == se_cmd_select_module)
		{
		/*********************************/
			if ((struct Module *)event->arg1==mod)
				if (wind==0)
					DisplayWind();
		}

	if(event->type == se_cmd_kill_module)
		{
		/*********************************/
		Hmod=NULL; if(HANDLER) Hmod=HANDLER->handler_mod;
		if (((struct Module *)event->arg1 == mod) || ((struct Module *)event->arg1 == Hmod))
			{
			ReplyMsg(event);
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	code=message->Code;
	ReplyMsg(message);

	if (class==CLOSEWINDOW)
		{
		HideWind();
		return(0);
		}

	if (class==NEWSIZE) 
		{
		CUSTOM_STATE->VUs=NumOfVU;
		WinGadSize();
		SUSPEND=1;
		}
	}
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplayWind()
{
static void InterlaceAdjust();
struct AudioHandler *FindAudioHandler();

if (!HANDLER) HANDLER = FindAudioHandler(NULL,-1,0,NULL,-1);
if (!HANDLER) {telluser("Meters:","No handlers loaded",NULL); return;}

if ((StudioBase->defaults.screen_viewmodes & LACE) && inflag)
	{
	VU_NewWindow.Height=VU_NewWindow.Height+36+11;
				NewWinHei=89+36+11+6;
	}
if ((wind = OpenNewWind(&VU_NewWindow,mod->state,PREFERENCES,NULL))==0)  {telluser("Meters:","NO vU window BE coUld OPen",0);return;}
NewWinHei=wind->Height;
OldWinHei=wind->Height;

BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

inflag=1;

NewWinSize();
/*WinGadSize();*/
inflag=0;

NotifyMeOnEvent(se_info_hilo, mod);

}
/*****************************************************************/
void HideWind()
{
struct Window *w;

IgnoreEvent(se_info_hilo, mod);

if (wind)
	{
/*	SaveGadgetState(VUselect);*/
	delete_gadget_chain(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
static USHORT	TopX[] = 
	{12,13,14,15,
	16,17,18,19,
	20,21,22,23,24,
	25,26,27,28,29,
	30,31,32,33,34,35,
	36,37,38,39,40,41,42,
	43,44,45,46,47,48,49,50,51,52,53,
	54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
	85,86,87,88,89,90,91,92,93,94,95,
	96,97,98,99,100,101,102,
	103,104,105,106,107,108,
	109,110,111,112,113,
	114,115,116,117,118,
	119,120,121,122,
	123,124,125,126,
	127,128,129,
	130};

static USHORT	TopY[] =
	{14,14,14,14,
	13,13,13,13,
	12,12,12,12,12,
	11,11,11,11,11,
	10,10,10,10,10,10,
	9,9,9,9,9,9,9,
	8,8,8,8,8,8,8,8,8,8,8,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,8,
	9,9,9,9,9,9,9,
	10,10,10,10,10,10,
	11,11,11,11,11,
	12,12,12,12,12,
	13,13,13,13,
	14,14,14,14,
	15,15,15,
	16};

static int lp=0;

static void SetupVU(VU)
struct VUmeter *VU;
{
/*USHORT z;*/

LockGadLibRender();

if (StudioBase->defaults.screen_viewmodes & LACE)
	DrawImage(wind->RPort,&VUintIMG,VU->x-1,VU->y);
else
	DrawImage(wind->RPort,&VUstdIMG,VU->x,VU->y);

SetDrMd(wind->RPort,COMPLEMENT);
VU->lp=0;
if (StudioBase->defaults.screen_viewmodes & LACE)
	{
	Move(wind->RPort,VU->x+TopX[0],VU->y+(TopY[VU->lp])+(TopY[VU->lp]>>1));
	Draw(wind->RPort,VU->x+(TopX[0]>>2)+52,VU->y+57);
	}

else
	{
	Move(wind->RPort,VU->x+TopX[0],VU->y+TopY[0]);
	Draw(wind->RPort,VU->x+(TopX[0]>>2)+52,VU->y+28);
	}

UnLockGadLibRender();

/*for( z=0;z<110;z++)
	UpdateVU(VU,z);
for( z=109;z>0;z--)
	UpdateVU(VU,z);*/

SetDrMd(wind->RPort,JAM2);
}

/*****************************************************************/
static void UpdateVU(VU,p)
struct VUmeter *VU;
USHORT p;
{
SHORT pp,ad,sr[]= {1,1,1,2,2,3,4,5,7,9,12};/*{1,1,1,1,2,2,2,2,2,3,3};*/

SetDrMd(wind->RPort,COMPLEMENT);
pp=VU->lp;
ad=0;
if (p>pp)
	{
	if (p>pp+8)	ad=((p-pp)>>3);
	if (ad>10)	pp=pp+sr[10];
	else		pp=pp+sr[ad];
	if (pp>118) pp=118;
	}
else
	{
	if (p<pp-8)	ad=((pp-p)>>3);
	if (ad>10)	pp=pp-sr[10];
	else		pp=pp-sr[ad];
	if (pp<0) pp=0;
	}

LockGadLibRender();

if (pp!=VU->lp)
	if (StudioBase->defaults.screen_viewmodes & LACE)
		{
		Move(wind->RPort,VU->x+TopX[VU->lp],VU->y+(TopY[VU->lp])+(TopY[VU->lp]>>1));
		Draw(wind->RPort,VU->x+(TopX[VU->lp]>>2)+52,VU->y+57);
		Move(wind->RPort,VU->x+TopX[pp],VU->y+(TopY[pp])+(TopY[pp]>>1));
		Draw(wind->RPort,VU->x+(TopX[pp]>>2)+52,VU->y+57);
		}
	else 
		{
		Move(wind->RPort,VU->x+TopX[VU->lp],VU->y+TopY[VU->lp]);
		Draw(wind->RPort,VU->x+(TopX[VU->lp]>>2)+52,VU->y+28);
		Move(wind->RPort,VU->x+TopX[pp],VU->y+TopY[pp]);
		Draw(wind->RPort,VU->x+(TopX[pp]>>2)+52,VU->y+28);
		}
SetDrMd(wind->RPort,JAM2);

UnLockGadLibRender();

VU->lp=pp;
}
/********************************************************************/
static void UpdateScroll(VU,h,l,m)
struct VUmeter *VU;
int m;
BYTE h,l;
{
SetAPen(wind->RPort,6);		/*StudioBase->defaults.colors.gadget_window.bottomright_3d);*/
SetBPen(wind->RPort,2);		/*StudioBase->defaults.colors.gadget_window.hitbox_unselected);*/
SetDrMd(wind->RPort, JAM2);

LockGadLibRender();
/*wind->RPort->Mask=4;*/
if (m>132) m=132;
if (m>2)
if (StudioBase->defaults.screen_viewmodes & LACE)
	{
	ScrollRaster(wind->RPort,m,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+31);

	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(wind->RPort,VU->x+134-m,GRAPHstart+16);
	Draw(wind->RPort,VU->x+134,GRAPHstart+16);
	}
else	{
	ScrollRaster(wind->RPort,m,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+15);

	SetAPen(wind->RPort, StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(wind->RPort,VU->x+134-m,GRAPHstart+(16>>1));
	Draw(wind->RPort,VU->x+134,GRAPHstart+(16>>1));
	}


if (StudioBase->defaults.screen_viewmodes & LACE)
	{
	Move(wind->RPort,VU->x+134 ,(h>>3)+GRAPHstart+16);
	Draw(wind->RPort,VU->x+134 ,(l>>3)+GRAPHstart+16);
	ScrollRaster(wind->RPort,1,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+31);
	}
else	{
	Move(wind->RPort,VU->x+134 ,(h>>4)+GRAPHstart+8);
	Draw(wind->RPort,VU->x+134 ,(l>>4)+GRAPHstart+8);

/*	BltBitMap(wind->RPort->BitMap,VU->x+3,GRAPHstart,wind->RPort->BitMap,VU->x+2,GRAPHstart,134,15,0xFF,4);*/
	ScrollRaster(wind->RPort,1,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+15);
	}

/*wind->RPort->Mask=7;*/
UnLockGadLibRender();


}
/********************************************************************/
static void UpdateDigit(VU,h)
struct VUmeter *VU;
BYTE h;
{
USHORT	d,l,HLL;

LockGadLibRender();

if(!(VU->count--))
	{
	VU->count=30;
			d=(VU->PEEK<<3)+1;
			SetAPen(wind->RPort, 6);
			SetBPen(wind->RPort, 6);
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			VU->PEEK=VU->PPEEK; VU->PPEEK=0;
			d=(VU->PEEK<<3)+1;
			if(VU->PEEK>14) {SetAPen(wind->RPort, 7);SetBPen(wind->RPort, 7);}
			else {SetAPen(wind->RPort, 1);SetBPen(wind->RPort, 1);}
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
	}
else VU->PPEEK=MAX(VU->PPEEK,VU->LL);
HLL=VU->LL;

l=h;
if (l==127) l=128;
l=l>>3;

if(l!=HLL)
	{
	if (l>HLL)
		{
		while(l!=HLL)
			{
			HLL++;
			if (VU->PEEK<HLL) VU->PEEK=HLL;
			d=(HLL<<3)+1;
			if(HLL>14) {SetAPen(wind->RPort, 7);SetBPen(wind->RPort, 7);}
			else {SetAPen(wind->RPort, 1);SetBPen(wind->RPort, 1);}
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			}
		}
	else	{
		if (HLL!=VU->PEEK)
			{
			d=(HLL<<3)+1;
			SetAPen(wind->RPort, 6);
			SetBPen(wind->RPort, 6);

			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			}
		HLL--;
		}
	}
UnLockGadLibRender();

VU->LL=HLL;
}
/********************************************************************/
/*****************************************************************/
void UpdateGraph(HL,mis)
BYTE	HL[];
int mis;
{
void UpdateMeter();
int t,y;
UBYTE q;

if (wind)
	{
	for (t=0;t<CUSTOM_STATE->VUs;t++)
		{
		y=(((struct droplist *)(VUselect[t].gad_ptr))->l->current)<<1;
		q=HL[y];if (q>0x7F) {q=0x7f;/*kprintf("ERROR IN HILO INFO SEND!!\n");*/}
		if(CUSTOM_STATE->type&ANALOGm) UpdateVU(&VU[t],q);
		if(CUSTOM_STATE->type&DIGITm)  UpdateDigit(&VU[t],q);
		if(CUSTOM_STATE->type&GRAPHm)  UpdateScroll(&VU[t],q,-q,mis);
		}
	}
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
static void NewWinSize()
{
struct Screen *s=StudioBase->screen;

SUSPEND=0;
ActivateWindow(wind);

NewWinHei=33;
if (StudioBase->defaults.screen_viewmodes & LACE)
	{
	if (CUSTOM_STATE->type & ANALOGm) {NewWinHei=NewWinHei+63+3; }
	if (CUSTOM_STATE->type & DIGITm)  {DIGITstart=NewWinHei-1; NewWinHei=NewWinHei+8;}
	if (CUSTOM_STATE->type & GRAPHm)  {GRAPHstart=NewWinHei-2; NewWinHei=NewWinHei+33+3;}
	}
else
	{
	if (CUSTOM_STATE->type & ANALOGm) {NewWinHei=NewWinHei+32+3; }
	if (CUSTOM_STATE->type & DIGITm)  {DIGITstart=NewWinHei-1; NewWinHei=NewWinHei+8; }
	if (CUSTOM_STATE->type & GRAPHm)  {GRAPHstart=NewWinHei-2; NewWinHei=NewWinHei+18+3;}
	}


if (s->Height < (NewWinHei+wind->TopEdge))
	MoveWindow(wind,0,s->Height - (NewWinHei+wind->TopEdge));

if (s->Width < (VU[NumOfVU].x+2+wind->LeftEdge))
	MoveWindow(wind,s->Width - (VU[NumOfVU].x+2+wind->LeftEdge),0);

SizeWindow(wind,(VU[NumOfVU].x+2)-wind->Width,NewWinHei-OldWinHei);

OldWinHei=NewWinHei;
}
/********************************************************************/
static void WinGadSize()
{
int t;

SUSPEND=0;
/*SaveGadgetState(VUselect);*/
delete_gadget_chain(wind);
RefreshWind(wind);

for (t=0;t<CUSTOM_STATE->VUs-1;t++)
	VUselect[t].next=(struct NewGadgetHeader *)&VUselect[t+1];

VUselect[CUSTOM_STATE->VUs-1].next=NULL;
create_gadget_chain(wind,VUselect);

if (CUSTOM_STATE->type&ANALOGm) 
	for (t=0;t<CUSTOM_STATE->VUs;t++)
		SetupVU(&VU[t]);

if (CUSTOM_STATE->type&DIGITm) 
	for (t=0;t<CUSTOM_STATE->VUs;t++)
		{
		VU[t].LL=0; VU[t].PEEK=0;
		SetBPen(wind->RPort, 6);
		SetAPen(wind->RPort, 6);
		RectFill(wind->RPort,VU[t].x-1,DIGITstart-2,VU[t].x+137,DIGITstart+2);

		SetBPen(wind->RPort, 1);
		SetAPen(wind->RPort, 1);
		RectFill(wind->RPort,VU[t].x+1,DIGITstart-1,VU[t].x+7,DIGITstart+1);
		}

if (CUSTOM_STATE->type&GRAPHm) 
	for (t=0;t<CUSTOM_STATE->VUs;t++)
	if (StudioBase->defaults.screen_viewmodes & LACE)
		{
		draw_indent(wind->RPort,VU[t].x,GRAPHstart-1,VU[t].x+136,GRAPHstart+31+1, DARK_BROWN, WHITE);
		SetAPen(wind->RPort, DARK_BROWN);

		Move(wind->RPort,VU[t].x+2,GRAPHstart+16);
		Draw(wind->RPort,VU[t].x+134,GRAPHstart+16);
		}
	else	{
		draw_indent(wind->RPort,VU[t].x,GRAPHstart-1,VU[t].x+136,GRAPHstart+15+1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
		SetAPen(wind->RPort, StudioBase->defaults.colors.gadget_window.bottomright_3d);

		Move(wind->RPort,VU[t].x+2,GRAPHstart+(16>>1));
		Draw(wind->RPort,VU[t].x+134,GRAPHstart+(16>>1));
		}

}
/********************************************************************/
void DropFunc(d)
struct droplist *d;
{
/*kprintf("%x%x",d->l->current);*/
CUSTOM_STATE->metr[((int)d->user_data)] = d->l->current;
VUselect[((int)d->user_data)].initial_value = d->l->current;
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
static struct NewWindow PNewWind = {
   30,
   12,
   200,
   66,
   BLACK,
   WHITE,
   CLOSEWINDOW,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   "Meter Prefs", /** TITLE **/
   0,0,
   104,37,1024,1024,
   CUSTOMSCREEN
   };
struct Window *Pwind=NULL;

void NumOfMeters();

static struct NewButton button4 = 
	{NULL,					/* *next (gadget) */
	150,43,170,53,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumOfMeters,					/* Call Function */
	(APTR)4,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"4"};				/* *text */

static struct NewButton button3 = 
	{&button4,					/* *next (gadget) */
	150,29,170,39,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumOfMeters,				/* Call Function */
	(APTR)3,				/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"3"};					/* *text */

static struct NewButton button2 = 
	{&button3,					/* *next (gadget) */
	112,43,130,53,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumOfMeters,					/* Call Function */
	(APTR)2,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"2"};				/* *text */

static struct NewButton button1 = 
	{&button2,					/* *next (gadget) */
	112,29,130,39,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumOfMeters,					/* Call Function */
	(APTR)1,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"1"};				/* *text */

void Switches();

static struct NewButton GRAPHbutton = 
	{&button1,					/* *next (gadget) */
	8,43,100,53,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR,	/* Flags */
	Switches,				/* Call Function */
	(APTR)GRAPHm,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Graph"};				/* *text */

static struct NewButton DIGITbutton = 
	{&GRAPHbutton,					/* *next (gadget) */
	8,29,100,39,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR,	/* Flags */
	Switches,				/* Call Function */
	(APTR)DIGITm,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Digital"};				/* *text */

static struct NewButton ANALOGbutton = 
	{&DIGITbutton,					/* *next (gadget) */
	8,15,100,25,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR,	/* Flags */
	Switches,				/* Call Function */
	(APTR)ANALOGm,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Analog"};				/* *text */

/********************************************************************/
struct Window *DisplayPrefWind(state,le,te)
int le,te;
struct MeterModState *state;
{
struct Window *OpenNewWind();
void HidePrefWind();

if (Pwind==NULL)
	{

PNewWind.LeftEdge=le-(PNewWind.Width+6);
PNewWind.TopEdge=te+3;
	if ((Pwind = OpenNewWind(&PNewWind,NULL,HIDEMESSAGE,NULL))==NULL)  {telluser("Meters:","Can not open window!",0);return(NULL);}

	ColorGadList(&ANALOGbutton);
	if (CUSTOM_STATE->type&ANALOGm) ANALOGbutton.flags |=BUTN_HIT;
	else	ANALOGbutton.flags &=~BUTN_HIT;

	if (CUSTOM_STATE->type&DIGITm) DIGITbutton.flags |=BUTN_HIT;
	else	DIGITbutton.flags &=~BUTN_HIT;

	if (CUSTOM_STATE->type&GRAPHm) GRAPHbutton.flags |=BUTN_HIT;
	else	GRAPHbutton.flags &=~BUTN_HIT;

	if (state->VUs==1) button1.flags |=BUTN_HIT;
	else button1.flags &=~BUTN_HIT;
	if (state->VUs==2) button2.flags |=BUTN_HIT;
	else button2.flags &=~BUTN_HIT;
	if (state->VUs==3) button3.flags |=BUTN_HIT;
	else button3.flags &=~BUTN_HIT;
	if (state->VUs==4) button4.flags |=BUTN_HIT;
	else button4.flags &=~BUTN_HIT;
	
	if (create_gadget_chain(Pwind,&ANALOGbutton)==0) return(Pwind);
	}
return(Pwind);
}
/**************************************************************/
void HidePrefWind()
{
struct Window *w;

if (Pwind)
	{
	delete_gadget_chain(Pwind);
	w=Pwind;
	Pwind=NULL;
	CloseWind(w);
	}
}
/**************************************************************/

void NumOfMeters(g)
struct button *g;
{
if (g->flags & BUTN_HIT)
	SendMsg(mod,in_no_of_meters,g->user_data,0,0,0,0,0);
}
/*******************************************************************/
void Switches(g)
struct button *g;
{
SendMsg(mod,in_graph_type,g->user_data,0,0,0,0,0);
}
