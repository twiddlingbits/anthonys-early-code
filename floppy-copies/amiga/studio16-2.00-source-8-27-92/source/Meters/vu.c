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
long _stack = 8000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_METERS";	/* The name of the task to create         */
long _priority = -1;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/

/* Define these messy variables */
#define CUSTOM_STATE	((struct MeterModState *)(mod->state))
#define MOD_STATE	((struct MeterModState *)(mod->state))->state

struct MeterModState
	{
	struct StandardModState state;
	int MenuCkSum;
	UBYTE Speed;
	UBYTE type;
	UBYTE metr[4];
	};

/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
struct ChanKey *Mchan[4]={NULL,NULL,NULL,NULL};
struct ChanKey *tchan[4]={NULL,NULL,NULL,NULL};
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_hilo_tick;
short se_info_module_opened;
short se_info_module_closed;
/**********************************/
/**********************************/
SHORT SUSPEND=0;
void RevMchan();
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
struct Module 		*mod;
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
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("Meters:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

if (!(MOD_STATE.flags & MS_STATE_INITILIZED)) 
	{
	CUSTOM_STATE->type=ALL;
	CUSTOM_STATE->Speed=2;
	CUSTOM_STATE->metr[0]=2;
	CUSTOM_STATE->metr[1]=1;
	CUSTOM_STATE->metr[2]=-1;
	CUSTOM_STATE->metr[3]=-1;
	CUSTOM_STATE->MenuCkSum=0;
	}

se_info_hilo_tick		= GetEventID("SE_INFO_HILO_TICK");
se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

/** Tell studio.library that we are initilitzed **/

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/*****************************************************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event,evn,*HLevnt=&evn;
void HandEvnt();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=NULL)
	{
	if (event->type != se_info_hilo_tick) HandEvnt(event);
	else {movmem(event,HLevnt,sizeof(struct StudioEvent));}
	ReplyMsg(event);
	}
if(HLevnt->type == se_info_hilo_tick)
/*********************************/
	{
	if (wind && SUSPEND) UpdateGraph(HLevnt->arg1);
	HLevnt->type = NULL;
	}
}

void HandEvnt(event)
struct StudioEvent *event;
{
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
		if (((struct Module *)event->arg1 == mod))
			{
			ReplyMsg(event);
			IgnoreEvent(se_cmd_select_module, mod);
			IgnoreEvent(se_cmd_kill_module, mod);
			if (wind) HideWind();
			DeleteModule(mod);
			closelibraries();
			exit(0);
			}
		}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct ChanKey *ItemData();
void DrawWindowText();
int t;
struct MenuItem *FindCheckedItem(),*mi;
struct IntuiMessage *message;
ULONG class;
ULONG code;
int x;
BOOL flag;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	code=message->Code;

	if (class==CLOSEWINDOW)
		{
		HideWind();
		ReplyMsg(message);
		return(0);
		}
	if (class==NEWSIZE) 
		{
		SUSPEND=0;
		WinGadSize();
		SUSPEND=1;
		DrawWindowText();
		}

	if (class==MENUPICK)
		{
		flag=FALSE;

		x=CUSTOM_STATE->type;
		CUSTOM_STATE->type=0;
		if (Items[0].Flags&CHECKED) CUSTOM_STATE->type|=ANALOGm;
		if (Items[1].Flags&CHECKED) CUSTOM_STATE->type|=DIGITm;
		if (Items[2].Flags&CHECKED) CUSTOM_STATE->type|=GRAPHm;
		if (x!=CUSTOM_STATE->type) flag=TRUE;

		SUSPEND=0;
		t=0;
		mi=Titles[1].FirstItem;
		while ((mi=FindCheckedItem(mi)) && t<4)
			{
			tchan[t]=ItemData(mi);
			CUSTOM_STATE->metr[t]=ItemNum(mi);
			if (Mchan[NumOfVU-t-1]!=tchan[t]) {flag=TRUE;}
			t++;
			mi=mi->NextItem;
			}
		while (mi)
			{
			mi->Flags&=~CHECKED;
			mi=mi->NextItem;
			}

		if (NumOfVU!=t) flag=TRUE;
		NumOfVU=t;

		t=NumOfVU;
		while (t<4) 
			{
			Mchan[t]=NULL;
			CUSTOM_STATE->metr[t]=-1;
			t++;
			}

		if (flag) 
			{
			for(t=0;t<4;t++) Mchan[t]=NULL;
			for(t=0;t<NumOfVU;t++) Mchan[NumOfVU-t-1]=tchan[t];
			NewWinSize();
			}
		SUSPEND=1;
		DrawWindowText();
		}
	ReplyMsg(message);
	}
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DrawWindowText()
{
char str[40];
int ItemNum();
void S16Text();

SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
SetDrMd(wind->RPort,JAM2);


if (Mchan[0]) sprintf(str,"%6s %-10s",Mchan[0]->handler->name,Mchan[0]->name);
else sprintf(str,"%-17s","");
S16Text(wind->RPort,str,17,10,21);

if (NumOfVU>1)
	{
	if (Mchan[1]) sprintf(str,"%6s %-10s",Mchan[1]->handler->name,Mchan[1]->name);
	else sprintf(str,"%-17s","");
	S16Text(wind->RPort,str,17,10+145,21);
	}

if (NumOfVU>2)
	{
	if (Mchan[2]) sprintf(str,"%6s %-10s",Mchan[2]->handler->name,Mchan[2]->name);
	else sprintf(str,"%-17s","");
	S16Text(wind->RPort,str,17,10+145*2,21);
	}

if (NumOfVU>3)
	{
	if (Mchan[3]) sprintf(str,"%6s %-10s",Mchan[3]->handler->name,Mchan[3]->name);
	else sprintf(str,"%-17s","");
	S16Text(wind->RPort,str,17,10+145*3,21);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplayWind()
{
static void InterlaceAdjust();
struct AudioHandler *FindAudioHandler();
struct ChanKey *CheckNum();
struct MenuItem *BuildChannelMenu();
int x,n=0,cksum;

if (!FindAudioHandler(NULL,-1,0,NULL,-1))
	{telluser("Meters:","No handlers loaded",NULL); return;}

Titles[1].FirstItem=BuildChannelMenu(CK_CHAN_HILO,MENU_MULTI_SELECT);

cksum=MenuCheckSum(Titles[1].FirstItem);

if (cksum==CUSTOM_STATE->MenuCkSum || CUSTOM_STATE->MenuCkSum==0)
	for(x=0;x<4;x++)
		{
		if (CUSTOM_STATE->metr[x]!=-1)
			{
			Mchan[x]=CheckNum(Titles[1].FirstItem,CUSTOM_STATE->metr[x]);
			n++;
			}
		else Mchan[x]=NULL;
		}
NumOfVU=n;
RevMchan();
CUSTOM_STATE->MenuCkSum=cksum;

	if (CUSTOM_STATE->type&ANALOGm) Items[0].Flags |=CHECKED;
	else Items[0].Flags &=~CHECKED;
	if (CUSTOM_STATE->type&DIGITm) Items[1].Flags |=CHECKED;
	else Items[1].Flags &=~CHECKED;
	if (CUSTOM_STATE->type&GRAPHm) Items[2].Flags |=CHECKED;
	else Items[2].Flags &=~CHECKED;

NewWinHei=33-4;
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

if (mod->state)
	{
	mod->state->win_w=VU[NumOfVU].x+2;
	mod->state->win_h=NewWinHei;
	}

if ((wind = OpenNewWind(&VU_NewWindow,mod->state,NULL,&Titles[0]))==0)  {telluser("Meters:","NO vU window BE coUld OPen",0);return;}
NewWinHei=wind->Height;
OldWinHei=wind->Height;

BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

inflag=1;

NewWinSize();
inflag=0;

NotifyMeOnEvent(se_info_hilo_tick, mod);

}
/*****************************************************************/
void HideWind()
{
struct Window *w;
IgnoreEvent(se_info_hilo_tick, mod);

if (wind)
	{
	w=wind;
	wind=NULL;
	FreeChannelMenu(Titles[1].FirstItem);
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

SetDrMd(wind->RPort,JAM2);
}

/*****************************************************************/
static void UpdateVU(VU,p)
struct VUmeter *VU;
USHORT p;
{
SHORT pp,ad,sr[]= {0,1,1,1,2,2,3,4,5,7,9,12};

SetDrMd(wind->RPort,COMPLEMENT);
pp=VU->lp;
ad=0;

if (p>pp)
	{
	if (p>pp+8)	ad=((p-pp)>>3);
	if (ad>11)	pp=pp+sr[11];
	else		pp=pp+sr[ad];
	if (pp>118) pp=118;
	}
else
	{
	if (p<pp-8)	ad=((pp-p)>>3);
	if (ad>11)	pp=pp-sr[11];
	else		pp=pp-sr[ad];
	if (ad==0 && pp<9 && pp!=0) pp--;
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
if (VU->cntr>132)
	if ((h>>3)==(l>>3)) return;

SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
SetDrMd(wind->RPort, JAM2);

LockGadLibRender();
if (m>131) m=131;
if (m>2)
	{
	VU->cntr=0;
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
	}

if (StudioBase->defaults.screen_viewmodes & LACE)
	{
	Move(wind->RPort,VU->x+134 ,(h>>3)+GRAPHstart+16);
	Draw(wind->RPort,VU->x+134 ,(l>>3)+GRAPHstart+16);
	if ((h>>3)==(l>>3)) VU->cntr++;
	else VU->cntr=0;
	ScrollRaster(wind->RPort,1,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+31);
	}
else	{
	Move(wind->RPort,VU->x+134 ,(h>>4)+GRAPHstart+8);
	Draw(wind->RPort,VU->x+134 ,(l>>4)+GRAPHstart+8);
	if ((h>>4)==(l>>4)) VU->cntr++;
	else VU->cntr=0;
	ScrollRaster(wind->RPort,1,0,VU->x+2,GRAPHstart,VU->x+134,GRAPHstart+15);
	}

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
			SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
			SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			VU->PEEK=VU->PPEEK; VU->PPEEK=0;
			d=(VU->PEEK<<3)+1;
			if(VU->PEEK>14) {SetAPen(wind->RPort, StudioBase->defaults.colors.gadget_window.hitbox_selected);SetBPen(wind->RPort, StudioBase->defaults.colors.gadget_window.hitbox_selected);}
			else {SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);}
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
			if(HLL>14) {SetAPen(wind->RPort, StudioBase->defaults.colors.gadget_window.hitbox_selected);SetBPen(wind->RPort, StudioBase->defaults.colors.gadget_window.hitbox_selected);}
			else {SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);}
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			}
		}
	else	{
		if (HLL!=VU->PEEK)
			{
			d=(HLL<<3)+1;
			SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
			SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);

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
void UpdateGraph(this)
int this;
{
static int last;
int mis=this-last;
void UpdateMeter();
int t;
UBYTE hilo;

if (wind)
	{
	for (t=0;t<NumOfVU;t++)
		{
		if (Mchan[t])
			{
			hilo=MAX(Mchan[t]->hilo_left,Mchan[t]->hilo_right);
			if(CUSTOM_STATE->type&ANALOGm) UpdateVU(&VU[t],hilo);
			if(CUSTOM_STATE->type&DIGITm)  UpdateDigit(&VU[t],hilo);
			if(CUSTOM_STATE->type&GRAPHm)  UpdateScroll(&VU[t],Mchan[t]->hilo_left,-Mchan[t]->hilo_right,mis);
			}
		}
	}
last=this;
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
int x;

ActivateWindow(wind);

NewWinHei=33-4;
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

if (NumOfVU)
	{
	if (s->Width < (VU[NumOfVU].x+2+wind->LeftEdge))
		MoveWindow(wind,s->Width - (VU[NumOfVU].x+2+wind->LeftEdge),0);

	SizeWindow(wind,(VU[NumOfVU].x+2)-wind->Width,NewWinHei-OldWinHei);
	}
else
	{
	if (s->Width < (VU[1].x+2+wind->LeftEdge))
		MoveWindow(wind,s->Width - (VU[1].x+2+wind->LeftEdge),0);

	SizeWindow(wind,(VU[1].x+2)-wind->Width,NewWinHei-OldWinHei);
	}

for(x=0;x<5;x++)
	{
	VU[x].p=0;
	VU[x].lp=0;
	VU[x].LL=0;
	VU[x].PEEK=0;
	VU[x].PPEEK=0;
	VU[x].count=0;
	}

OldWinHei=NewWinHei;
}
/********************************************************************/
static void WinGadSize()
{
int t;

RefreshWind(wind);

if (CUSTOM_STATE->type&ANALOGm) 
	for (t=0;t<NumOfVU;t++)
		SetupVU(&VU[t]);

if (CUSTOM_STATE->type&DIGITm) 
	for (t=0;t<NumOfVU;t++)
		{
		VU[t].LL=0; VU[t].PEEK=0;
		SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
		RectFill(wind->RPort,VU[t].x-1,DIGITstart-2,VU[t].x+137,DIGITstart+2);

		SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
		SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
		RectFill(wind->RPort,VU[t].x+1,DIGITstart-1,VU[t].x+7,DIGITstart+1);
		}

if (CUSTOM_STATE->type&GRAPHm) 
	for (t=0;t<NumOfVU;t++)
	if (StudioBase->defaults.screen_viewmodes & LACE)
		{
		draw_indent(wind->RPort,VU[t].x,GRAPHstart-1,VU[t].x+136,GRAPHstart+31+1,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
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
/********************************************************************/
void RevMchan()
{
struct ChanKey *Tchan[4];
int x;

for(x=0;x<4;x++) Tchan[x]=Mchan[x];
for(x=0;x<NumOfVU;x++) Mchan[x]=Tchan[NumOfVU-x-1];
x=NumOfVU;
while(x<4)
	Mchan[x++]=NULL;
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
