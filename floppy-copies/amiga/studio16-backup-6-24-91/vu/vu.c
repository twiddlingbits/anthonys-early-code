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
#include "VU.menu"
#include "/include/CardCmds.h"
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void SaveGadgetState();
/**********************************/
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_hilo;
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
void HandleMenu();
void UpdateGraph();
/**********************************/
/**********************************/
struct Module *mod;
extern struct studioBase *studioBase;
static struct Window *wind;
/**********************************/
/**********************************/
/**********************************/
/**********************************/
int inflag=1;
int NumOfVU=2,VUs=2;

int GRAPHstart=36+35,DIGITstart=0;

int stuff=ALL;
int NewWinHei,OldWinHei;

void WinGadSize();
/**********************************/
/**********************************/
/**********************************/
/**********************************/

/********************************************************************/
/********************************************************************/
/********************************************************************/
void main()
{
int mask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule();				/** initalize module **/
DisplayWind();				/** open window **/

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
void InitModule()
{
static struct NewModule ModData ={"Meters",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Could Not Add Module","",0);
	closelibraries();
	exit(10);
	}

se_info_hilo		= GetEventID("SE_INFO_HILO");
se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*****************************************************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	if(event->type == se_info_hilo)
		/*********************************/
			if (wind && SUSPEND) UpdateGraph(event->arg1);

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
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}
	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
USHORT code;
USHORT menunum,itemnum,subnum;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
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
		VUs=NumOfVU;
		WinGadSize();
		}

	if (class == MENUPICK)
		{
		menunum=MENUNUM(code);
		itemnum=ITEMNUM(code);
		subnum=SUBNUM(code);

			switch(menunum)
			{
			case 0:
				switch (itemnum)
				{
				case 0:	/* ANALOG */
					stuff=stuff^ANALOGm;
				break;

				case 1: /* DIGITAL */
					stuff=stuff^DIGITm;
				break;

				case 2: /* GRAPH */
					stuff=stuff^GRAPHm;
				break;
				}

			HandleMenu();
			NewWinSize();
			break;

			case 1:
				SUSPEND=0;
				NumOfVU=itemnum+1;
				NewWinSize();
			break;
			}
		}
	}
return(0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplayWind()
{
static void InterlaceAdjust();
struct Window *OpenWindow();

VU_NewWindow.Screen = studioBase->screen;
if ((studioBase->defaults.screen_viewmodes & LACE) && inflag)
	{
	VU_NewWindow.Height=VU_NewWindow.Height+36+11;
				NewWinHei=89+36+11+6;
	}
if ((wind = OpenWind(&VU_NewWindow))==0)  {telluser("NO window","",0);return;}
NewWinHei=wind->Height;
OldWinHei=wind->Height;

SetMenuStrip(wind,VU_menu);
inflag=1;
HandleMenu();
WinGadSize();
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
	SaveGadgetState(VUselect);
	delete_gadget_chain(wind);
	ClearMenuStrip(wind);
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
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

if (studioBase->defaults.screen_viewmodes & LACE)
	DrawImage(wind->RPort,&VUintIMG,VU->x-1,VU->y);
else
	DrawImage(wind->RPort,&VUstdIMG,VU->x,VU->y);

SetDrMd(wind->RPort,COMPLEMENT);
VU->lp=0;
if (studioBase->defaults.screen_viewmodes & LACE)
	{
	Move(wind->RPort,VU->x+TopX[0],VU->y+(TopY[VU->lp])+(TopY[VU->lp]>>1));
	Draw(wind->RPort,VU->x+(TopX[0]>>2)+52,VU->y+57);
	}

else
	{
	Move(wind->RPort,VU->x+TopX[0],VU->y+TopY[0]);
	Draw(wind->RPort,VU->x+(TopX[0]>>2)+52,VU->y+28);
	}

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
SHORT pp,ad,sr[]= {0,1,1,1,2,2,2,2,2,3,3};

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

if (pp!=VU->lp)
	if (studioBase->defaults.screen_viewmodes & LACE)
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
VU->lp=pp;
}
/********************************************************************/
static void UpdateScroll(VU,h,l)
struct VUmeter *VU;

BYTE h,l;
{
SetAPen(wind->RPort, DARK_BROWN);
SetBPen(wind->RPort, BROWN);
SetDrMd(wind->RPort, JAM2);

/*Forbid();
wind->RPort->Mask=4;*/

if (studioBase->defaults.screen_viewmodes & LACE)
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
/*wind->RPort->Mask=7;
Permit();*/
}
/********************************************************************/
static void UpdateDigit(VU,h)
struct VUmeter *VU;
BYTE h;
{
USHORT	d,l,HLL;

if(!(VU->count--))
	{
	VU->count=30;
			d=(VU->PEEK<<3)+1;
			SetAPen(wind->RPort, BLACK);
			SetBPen(wind->RPort, BLACK);
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			VU->PEEK=VU->PPEEK; VU->PPEEK=0;
			d=(VU->PEEK<<3)+1;
			if(VU->PEEK>14) {SetAPen(wind->RPort, RED);SetBPen(wind->RPort, RED);}
			else {SetAPen(wind->RPort, 5);SetBPen(wind->RPort, 5);}
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
	}
else VU->PPEEK=MAX(VU->PPEEK,VU->LL);
HLL=VU->LL;

l=h;
if (l>128) l=128;
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
			if(HLL>14) {SetAPen(wind->RPort, RED);SetBPen(wind->RPort, RED);}
			else {SetAPen(wind->RPort, 5);SetBPen(wind->RPort, 5);}
			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			}
		}
	else	{
		if (HLL!=VU->PEEK)
			{
			d=(HLL<<3)+1;
			SetAPen(wind->RPort, BLACK);
			SetBPen(wind->RPort, BLACK);

			RectFill(wind->RPort,VU->x+d,DIGITstart-1,VU->x+d+6,DIGITstart+1);
			}
		HLL--;
		}
	}
VU->LL=HLL;
}
/********************************************************************/
/*****************************************************************/
void UpdateGraph(HL)
BYTE	HL[];
{
void UpdateMeter();
int t,y;
UBYTE q;

if (wind)
	{
	for (t=0;t<VUs;t++)
		{
		y=(((struct droplist *)(VUselect[t].gad_ptr))->l->current)<<1;
		q=MAX(HL[y],-HL[y+1]);
		if(stuff&ANALOGm) UpdateVU(&VU[t],q);
		if(stuff&DIGITm)  UpdateDigit(&VU[t],q);
		if(stuff&GRAPHm)  UpdateScroll(&VU[t],HL[y],HL[y+1]);
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
struct Screen *s=studioBase->screen;

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

SaveGadgetState(VUselect);
delete_gadget_chain(wind);
RefreshWind(wind);

for (t=0;t<VUs-1;t++)
	VUselect[t].next=(struct NewGadgetHeader *)&VUselect[t+1];

VUselect[VUs-1].next=NULL;
create_gadget_chain(wind,VUselect);

if (stuff&ANALOGm) 
	for (t=0;t<VUs;t++)
		SetupVU(&VU[t]);

if (stuff&DIGITm) 
	for (t=0;t<VUs;t++)
		{
		VU[t].LL=0; VU[t].PEEK=0;
		SetBPen(wind->RPort, BLACK);
		SetAPen(wind->RPort, BLACK);
		RectFill(wind->RPort,VU[t].x-1,DIGITstart-2,VU[t].x+137,DIGITstart+2);

		SetBPen(wind->RPort, 5);
		SetAPen(wind->RPort, 5);
		RectFill(wind->RPort,VU[t].x+1,DIGITstart-1,VU[t].x+7,DIGITstart+1);
		}

if (stuff&GRAPHm) 
	for (t=0;t<VUs;t++)
	if (studioBase->defaults.screen_viewmodes & LACE)
		{
		draw_indent(wind->RPort,VU[t].x,GRAPHstart-1,VU[t].x+136,GRAPHstart+31+1, DARK_BROWN, WHITE);
		SetAPen(wind->RPort, DARK_BROWN);

		Move(wind->RPort,VU[t].x+2,GRAPHstart+15);
		Draw(wind->RPort,VU[t].x+134,GRAPHstart+15);
		}
	else	{
		draw_indent(wind->RPort,VU[t].x,GRAPHstart-1,VU[t].x+136,GRAPHstart+15+1, DARK_BROWN, WHITE);
		SetAPen(wind->RPort, DARK_BROWN);

		Move(wind->RPort,VU[t].x+2,GRAPHstart+(15>>1));
		Draw(wind->RPort,VU[t].x+134,GRAPHstart+(15>>1));
		}

SUSPEND=1;

}
/********************************************************************/
static void HandleMenu()
{
NewWinHei=33;
if (studioBase->defaults.screen_viewmodes & LACE)
	{
	if (stuff & ANALOGm) {NewWinHei=NewWinHei+63+3; }
	if (stuff & DIGITm)  {DIGITstart=NewWinHei-1; NewWinHei=NewWinHei+8;}
	if (stuff & GRAPHm)  {GRAPHstart=NewWinHei-2; NewWinHei=NewWinHei+33+3;}
	}
else
	{
	if (stuff & ANALOGm) {NewWinHei=NewWinHei+32+3; }
	if (stuff & DIGITm)  {DIGITstart=NewWinHei-1; NewWinHei=NewWinHei+8; }
	if (stuff & GRAPHm)  {GRAPHstart=NewWinHei-2; NewWinHei=NewWinHei+18+3;}
	}

if (stuff & ANALOGm) Items[0].Flags=(Items[0].Flags&0xFEFF)|CHECKED;
if (stuff & DIGITm)  Items[1].Flags=(Items[0].Flags&0xFEFF)|CHECKED;
if (stuff & GRAPHm)  Items[2].Flags=(Items[0].Flags&0xFEFF)|CHECKED;
}
/********************************************************************/
