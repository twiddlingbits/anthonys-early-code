#include "exec/types.h"
#include <exec/lists.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <exec/semaphores.h>
#include <stdio.h>
#include "intuition/intuition.h"
#include "exec/memory.h"
#include "clib/macros.h"
#include "/include/psound.h"

extern struct StudioBase *StudioBase;

#define AWindC {2,1,6,7,7,1,1}
#define IWindC {2,1,6,2,2,1,1}
#define SWindC {0,0,0,0,0,0,0}

static struct HitAreaColor AWindClr = {2,1,6,7,7,5,5};
static struct HitAreaColor IWindClr = {2,1,6,2,2,5,5};

static struct GadgetColors Acolors = {
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	};

static struct GadgetColors Icolors = {
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	};

static struct GadgetColors Scolors = {
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	};

#define SYScolors Icolors

void ViewFile();
void RefreshBar();
void AddTitBarGads();
void DelTitBarGads();
void SaveGadgetPlacement();
void CloseMenuList();

/*############################################################
NAME	
	HandleIntuitMess -
FUNCTION
	This function is a wedge for recieving intuition messages.  It should
	always be called right after recieing intuition messages.  It handles
	changing the title bar colors.
INPUTS
	wind - a pointer to the callers window structure
	message - a pointer to the intuition message just recieved.
RESULTS
	NONE
##############################################################*/
void 	HandleIntuitMess(wind,message)
struct Window *wind;
struct IntuiMessage *message;
{
/*struct WindowParms *wp=(struct WindowParms *)wind->UserData;*/
ULONG class=message->Class;
ULONG code=message->Code;
void MenusOn();
void DelMenuBar();

if (class == MOUSEBUTTONS && (code&1)) /* Right Button */
	if (code&128)
		{
/*		if (wp->curhead) CloseMenuList(wind);
		if (wp->Menu) DelMenuBar(wind);
		AddTitBarGads(wind);*/
		RefreshBar(wind,0);
		}
/*	else
		{
		DelTitBarGads(wind);
		MenusOn(wind);
		}*/

if (class == ACTIVEWINDOW) RefreshBar(wind,1);
if (class == INACTIVEWINDOW) RefreshBar(wind,1);
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
void AddTitBarGads(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;

if (wp->Flags&WINDOWSIZING)	AddGadget(wind,&wp->SizeGad,-1);
if (wp->Flags&WINDOWCLOSE)	wp->CloseButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWDEPTH)	wp->DepthButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWSIZING)	wp->DSizeButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags2&PREFERENCES)	wp->PrefsButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWDRAG)	AddGadget(wind,&wp->DragGad,-1);
}
/****************************************************************/
/****************************************************************/
void DelTitBarGads(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;

	if (wp->Flags&WINDOWSIZING)	RemoveGadget(wind,&wp->SizeGad);
	if (wp->Flags&WINDOWDRAG)	RemoveGadget(wind,&wp->DragGad);
	if (wp->Flags&WINDOWCLOSE)	wp->CloseButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags&WINDOWDEPTH)	wp->DepthButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags&WINDOWSIZING)	wp->DSizeButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags2&PREFERENCES)	wp->PrefsButton.gad_ptr->flags|=HIDE_GADGET;
}
/****************************************************************/
/****************************************************************/
void MenusOn(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct MenuHead *mh2=wp->Menu;
static struct HitAreaColor *clrs;
int x=0;
void AddMenuBar();

if (wind->Flags&WINDOWACTIVE) {clrs=wp->ActiveClrs;}
else {clrs=wp->InactiveClrs;}

SetDrMd(wind->RPort,JAM1);
SetAPen(wind->RPort,clrs->hitbox_unselected);
RectFill(wind->RPort, 2,1, wind->Width-3,9);
draw_indent(wind->RPort,0,0,wind->Width-1,10,clrs->topleft_3d,clrs->bottomright_3d);

if (mh2==NULL) return;

while (mh2)
	{
	x++;
	mh2=mh2->next;
	}
AddMenuBar(wind,x);
}
/****************************************************************/
/****************************************************************/
void AddMenuBar(wind,num)
struct Window *wind;
int num;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct MenuHead *mh=wp->Menu;
struct NewButton *gads;
int x,w,s,i;
void DisplayMenuList();

if((gads=(struct NewButton *)AllocMem(sizeof(struct NewButton)*(num+1),MEMF_CLEAR))==NULL) return;
wp->menugads=gads;
  
s=((wind->Width)<<8) - ((2<<8)*(num-1));
w=(s/num);
x=0;

for(i=0;i<num;i++)
	{
	gads[i].box_x1=(x>>8);
	gads[i].box_x2=(x+w)>>8; x=x+w+(1<<8);
	gads[i].box_y1=0;
	gads[i].box_y2=10;

	gads[i].next=&gads[i+1];
	gads[i].flags=BUTN_COOL|MANUAL_XOR|NOT_IN_CHAIN;
	gads[i].func=DisplayMenuList;
	gads[i].user_data=(APTR)mh;
	gads[i].c=&Acolors;
	gads[i].text=mh->text;

	mh->butt=&gads[i];
	mh->wind=wind;
	mh=mh->next;
	}

create_gadget_chain(wind,gads);

gads[num-1].next=NULL;
}
/****************************************************************/
/****************************************************************/
void DelMenuBar(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct NewButton *next,*gads=wp->menugads;
int i=0;

while (gads)
	{
	i++;
	next=gads->next;
	if(gads->gad_ptr) delete_gadget(gads->gad_ptr);
	gads=next;
	}

FreeMem(wp->menugads,sizeof(struct NewButton)*(i+1));
}
/****************************************************************/
struct NewWindow NMWind = {
   30,
   12,
   200,
   40,
   0,
   0,
   MOUSEMOVE|MOUSEBUTTONS,
   NOCAREREFRESH|BORDERLESS|RMBTRAP|WINDOWACTIVE,
   0,0,
   NULL, /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
/****************************************************************/
void DisplayMenuList(b)
struct button *b;
{
void handle_menu_intuievents();
struct MenuHead *mh=(struct MenuHead *)b->user_data;
struct MenuItm *mi=mh->list;
struct Window *wind=mh->wind;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct Window *OpenWindow();
int num=0,len=0;
struct HitAreaColor *clrs;

clrs=wp->ActiveClrs;

if (wp->curhead) CloseMenuList(wind);

while (mi)
	{
	num++;
	len=max(len,stclen(mi->text));
	mi=mi->next;
	}

mi=mh->list;

wp->curhead=mh;

NMWind.Width=(len+6)*8;
NMWind.Height=num*9+6;
NMWind.LeftEdge=wind->LeftEdge+mh->butt->gad_ptr->box_x1;
NMWind.TopEdge=wind->TopEdge+mh->butt->gad_ptr->box_y2+1;

if(StudioBase->screen->Width < (NMWind.Width+NMWind.LeftEdge)) NMWind.LeftEdge=wind->LeftEdge+mh->butt->gad_ptr->box_x2-NMWind.Width;
if(StudioBase->screen->Height < (NMWind.Height+NMWind.TopEdge)) NMWind.TopEdge=wind->TopEdge-NMWind.Height;

if (!NMWind.Screen) NMWind.Screen = StudioBase->screen;
if ((wp->mwind=OpenWindow(&NMWind))==NULL) return;

SetDrMd(wp->mwind->RPort,JAM1);
SetAPen(wp->mwind->RPort,clrs->hitbox_unselected);
RectFill(wp->mwind->RPort, 2,1,wp->mwind->Width-3,wp->mwind->Height-2);
draw_indent(wp->mwind->RPort,0,0,wp->mwind->Width-1,wp->mwind->Height-1,clrs->topleft_3d,clrs->bottomright_3d);
draw_indent(wp->mwind->RPort,2,1,wp->mwind->Width-3,wp->mwind->Height-2,clrs->bottomright_3d,clrs->topleft_3d);

SetDrMd(wp->mwind->RPort, JAM2);
num=0;
while (mi)
	{
	num++;
	if (mi->flags==MI_SELECTED)
		{
		SetAPen(wp->mwind->RPort,clrs->hitbox_unselected);
		SetBPen(wp->mwind->RPort,clrs->topleft_3d);
		}
	else
		{
		SetAPen(wp->mwind->RPort,clrs->topleft_3d);
		SetBPen(wp->mwind->RPort,clrs->hitbox_unselected);
		}

	Move(wp->mwind->RPort,24,1+9*num);
	Text(wp->mwind->RPort,mi->text,stclen(mi->text));
	mi=mi->next;
	}
handle_menu_intuievents(wp->mwind);
}
/****************************************************************/
void handle_menu_intuievents(wind)
struct Window *wind;
{
struct IntuiMessage *message;
ULONG class;
ULONG code;
int FLAG=0;
SHORT MX, MY;
SHORT itemno;
void HiLight();
static SHORT lasti;

itemno=0;lasti=0;

while(FLAG==0)
	{
/*	Wait(1<<wind->UserPort->mp_SigBit);*/

	MX=wind->MouseX;
	MY=wind->MouseY;

	if (MX>4 && MY>3 && MX<wind->Width-4 && MY<wind->Height-3)
		{
		if (lasti!=((MY-3)/9)+1)
			{
			if (itemno) HiLight(wind,itemno);
			itemno=((MY-3)/9)+1;
			lasti=itemno;
			HiLight(wind,itemno);
			}
		}
	else if (itemno) 
		{
		HiLight(wind,itemno);
		itemno=0;lasti=0;
		}

	while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
		{
		class=message->Class;
		code=message->Code;
		MX=message->MouseX;
		MY=message->MouseY;
		ReplyMsg(message);

		if (class == MOUSEBUTTONS && !(code&1)) /* Left Button */
			if (code&128)
				{
				}
			else
				{
				FLAG++;
				}
		if (class == MOUSEMOVE) {SetAPen(wind->RPort,MY);Move(wind->RPort,MX,MY);Draw(wind->RPort,MX,MY);}
		}
	}
}
/****************************************************************/
void HiLight(wind,y)
struct Window *wind;
SHORT y;
{
y=(y-1)*9+3;
SetAPen(wind->RPort,1);
SetDrMd(wind->RPort,COMPLEMENT);
RectFill(wind->RPort,4,y,wind->Width-5,y+8);
}
/****************************************************************/
void CloseMenuList(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
/*struct NewButton *next,*gads=wp->menugads;
struct MenuHead *mh=(struct MenuHead *)wp->curhead;*/

CloseWindow(wp->mwind);
wp->mwind=NULL;

wp->curhead=NULL;
}
/****************************************************************/
/****************************************************************/

/****************************************************************/
/****************************************************************/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
