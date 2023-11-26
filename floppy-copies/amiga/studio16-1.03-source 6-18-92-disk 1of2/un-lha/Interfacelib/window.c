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
extern struct LayersBase *LayersBase;

#define AWindC {2,1,6,7,0,1,1}
#define IWindC {2,1,6,2,7,1,1}
#define SWindC {0,0,0,0,0,0,0}

static struct HitAreaColor AWindClr = {2,1,6,7,2,5,5};
static struct HitAreaColor IWindClr = {2,1,6,2,7,5,5};

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

struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void ViewFile();
void RefreshBar();
void AddTitBarGads();
void DelTitBarGads();
void SaveGadgetPlacement();
void CloseMenuList();

struct Window *OpenWindow(),*GlobActivWind=NULL;

struct Gadget RESIZEgad = { NULL,NULL,NULL,10,5,GADGHNONE,GADGIMMEDIATE|RELVERIFY,SIZING,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL };
struct Gadget DRAGgad = { NULL,24,0,NULL,11,GADGHNONE,GADGIMMEDIATE|RELVERIFY,WDRAGGING,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL };

void DepthWindow();
struct NewButton DEPTHbutton = 
	{NULL,				/* *next (gadget) */
	NULL,0,NULL,10,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|NOT_IN_CHAIN,				/* Flags */
	DepthWindow,				/* Call Function */
	NULL,					/* user_data */
	&Scolors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	NULL};				/* *text */

void CloseWidw();
struct NewButton CLOSEbutton = 
	{NULL,				/* *next (gadget) */
	0,0,23,10,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|NOT_IN_CHAIN,				/* Flags */
	CloseWidw,				/* Call Function */
	NULL,					/* user_data */
	&Scolors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	NULL};				/* *text */

void DualSizeWindow();
struct NewButton DSIZEbutton = 
	{NULL,				/* *next (gadget) */
	NULL,0,NULL,10,					/* Box x1,y1,x2,y2 */
	BUTN_COOL|NOT_IN_CHAIN,				/* Flags */
	DualSizeWindow,				/* Call Function */
	NULL,					/* user_data */
	&Scolors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	NULL};				/* *text */

void WindPrefs();
struct NewButton PREFSbutton = 
	{NULL,				/* *next (gadget) */
	NULL,0,NULL,10,					/* Box x1,y1,x2,y2 */
	BUTN_COOL|NOT_IN_CHAIN,				/* Flags */
	WindPrefs,				/* Call Function */
	NULL,					/* user_data */
	&Scolors,				/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	NULL};				/* *text */

/****************************************************************/
/****************************************************************/
/*******************************************************************/
struct window *OpenWind(nwind,mod)
struct NewWindow *nwind;
struct Module	*mod;
{
struct window *OpenNewWind();
return(OpenNewWind(nwind,mod ? mod->state : NULL ,PREFERENCES));
}
/*******************************************************************/
struct window *OpenWindNoPrefs(nwind,mod)
struct NewWindow *nwind;
struct Module	*mod;
{
struct window *OpenNewWind();
return(OpenNewWind(nwind,mod ? mod->state : NULL ,NULL));
}
/*******************************************************************/
/*############################################################
NAME	
	OpenNewWind -
FUNCTION
	This function opens a new Studio 16 window and creates all the gadgets
	that appears in the title bar.
INPUTS
	nwind - a pointer to a intuition NewWindow structure
	state - a pointer to a modules StandardModState structure
	x - flags variable
	 PREFERENCES	You want the window preference gadget in title bar
	 DONTMOVEWINDOW	Ignore the StandardModState default window position
	 HIDEMESSAGE	Send the studio message SE_CMD_HIDE_WINDOW rather than
	            	using the intuituition WINDOWCLOSE message

	head - not implimented, should be NULL.
RESULTS
	NONE
##############################################################*/
struct window *OpenNewWind(nwind,state,x,head)
struct NewWindow *nwind;
struct StandardModState *state;
int x;
struct MenuHead *head;
{
struct Window *wind;
struct WindowParms *wp;
int R=nwind->Width,L=0;

if (!nwind->Screen) nwind->Screen = StudioBase->screen;		/* STUDIOBASE REFERENCE */

nwind->DetailPen=5; /*StudioBase->defaults.colors.gadget_window;*/
nwind->BlockPen=5; /*StudioBase->defaults.colors.gadget_window;*/

SYScolors.button=StudioBase->defaults.colors.gadget_window; /* NEED BE DONE ONLY ONCE */

if ((wp=(struct WindowParms *)AllocMem(sizeof(struct WindowParms),MEMF_CLEAR))==NULL) return(NULL);

wp->NWind=nwind;
/*wp->CloseFunc=func;*/
wp->Menu=head;
wp->Flags2=x;
wp->state=state;

if (nwind->LeftEdge==-1)
		{
		wp->Flags2 &= LE_NEGONE;
	if (GlobActivWind)
		{
		nwind->LeftEdge=(GlobActivWind->LeftEdge+GlobActivWind->Width/2)-(nwind->Width/2);
		if (nwind->LeftEdge<0) nwind->LeftEdge=0;
		if ((nwind->LeftEdge+nwind->Width) > StudioBase->screen->Width) nwind->LeftEdge=StudioBase->screen->Width - nwind->Width;
		}
	else
		{
		nwind->LeftEdge=(StudioBase->screen->Width - nwind->Width)/2;
		}
		}

if (nwind->TopEdge==-1)
		{
		wp->Flags2 &= TE_NEGONE;
	if (GlobActivWind)
		{
		nwind->TopEdge=(GlobActivWind->TopEdge+GlobActivWind->Height/2)-(nwind->Height/2);
		if (nwind->TopEdge<0) nwind->TopEdge=0;
		if ((nwind->TopEdge+nwind->Height) > StudioBase->screen->Height) nwind->TopEdge=StudioBase->screen->Height - nwind->Height;
		}
	else
		{
		nwind->TopEdge=(StudioBase->screen->Height - nwind->Height)/2;
		}
		}

if (state)
	{
	if (state->win_h!=NULL && !(x & DONTMOVEWINDOW))
		{
		nwind->LeftEdge=state->win_le;
		nwind->TopEdge=state->win_te;
		nwind->Width=state->win_w;
		nwind->Height=state->win_h;
		}
	state->flags |= (MS_WINDOW_OPEN);
	}
else wp->Flags2 &= ~PREFERENCES;

if (nwind->Width > StudioBase->screen->Width) nwind->Width = StudioBase->screen->Width;
if (nwind->Height > StudioBase->screen->Height) nwind->Height = StudioBase->screen->Height;
if (nwind->LeftEdge+nwind->Width > StudioBase->screen->Width) nwind->LeftEdge = StudioBase->screen->Width - nwind->Width;
if (nwind->Height+nwind->TopEdge > StudioBase->screen->Height) nwind->TopEdge = StudioBase->screen->Height - nwind->Height;
if (nwind->LeftEdge < 0) nwind->LeftEdge = 0;
if (nwind->TopEdge < 0) nwind->TopEdge = 0;

wp->CloseButton=CLOSEbutton;
wp->SizeGad=RESIZEgad;
wp->DSizeButton=DSIZEbutton;
wp->DragGad=DRAGgad;			/* Reentent questionable */
wp->DepthButton=DEPTHbutton;
wp->PrefsButton=PREFSbutton;

wp->Flags=nwind->Flags;
nwind->Flags&= ~(WINDOWCLOSE|WINDOWDEPTH|WINDOWSIZING|WINDOWDRAG);
nwind->Flags|=ACTIVATE|NOCAREREFRESH|BORDERLESS|RMBTRAP;
nwind->IDCMPFlags|=MOUSEBUTTONS|ACTIVEWINDOW|INACTIVEWINDOW;
wp->Title=nwind->Title;

nwind->Title=NULL;

nwind->Width=min(nwind->Width, nwind->MaxWidth==-1 ? 1024:nwind->MaxWidth);
nwind->Height=min(nwind->Height, nwind->MaxHeight==-1 ? 1024:nwind->MaxHeight);

if ((wind=OpenWindow(nwind))==NULL) {FreeMem(wp,sizeof(struct WindowParms));return(NULL);}
if (state) state->window=wind;
wind->UserData=(BYTE *)wp;
nwind->Title=wp->Title;
nwind->Flags=wp->Flags;

if (wp->Flags&WINDOWCLOSE) 
	{
	wp->CloseButton.user_data=(APTR)wind;
	L=24;
	}

if (wp->Flags&WINDOWDEPTH)
	{
	wp->DepthButton.box_x1=wind->Width-23-1;
	wp->DepthButton.box_x2=wind->Width-1;
	wp->DepthButton.user_data=(APTR)wind;
	R=R-24;
	}

if (wp->Flags&WINDOWSIZING) 
	{
	wp->SizeGad.LeftEdge =wind->Width-12;
	wp->SizeGad.TopEdge =wind->Height-6;

	wp->DSizeButton.user_data=(APTR)wind;
	wp->DSizeButton.box_x1=R-24;
	wp->DSizeButton.box_x2=R-1;

	wp->TE=wind->TopEdge;
	wp->LE=wind->LeftEdge;
	wp->H=wind->MinHeight;
	wp->W=wind->MinWidth;
	R=R-24;
	}


if (wp->Flags2&PREFERENCES)
	{
	wp->PrefsButton.user_data=(APTR)wind;
	wp->PrefsButton.box_x1=R-24;
	wp->PrefsButton.box_x2=R-1;
	R=R-24;
	}

if (wp->Flags&WINDOWDRAG)
	{
	wp->DragGad.Width=R-L; 
	wp->DragGad.LeftEdge=L;
	}

if (wp->Flags&WINDOWCLOSE)	create_gadget(wind,&wp->CloseButton);
if (wp->Flags&WINDOWDEPTH)	create_gadget(wind,&wp->DepthButton);
if (wp->Flags&WINDOWSIZING)	AddGadget(wind,&wp->SizeGad,-1);
if (wp->Flags&WINDOWSIZING)	create_gadget(wind,&wp->DSizeButton);
if (wp->Flags2&PREFERENCES)	create_gadget(wind,&wp->PrefsButton);
if (wp->Flags&WINDOWDRAG)	AddGadget(wind,&wp->DragGad,-1);

/*AddTitBarGads(wind);*/
if (GlobActivWind) RefreshBar(GlobActivWind,0);
RefreshWind(wind);
RefreshGList(wind->FirstGadget,wind,NULL,-1);
return(wind);
}
/*******************************************************************/
/*############################################################
NAME	
	CloseWind -
FUNCTION
	This function closes a window, frees the title bar gadget and frees
	 any memory used while opening the window.
INPUTS
	wind - a pointer to the window structer of the window to close.
RESULTS
	NONE
##############################################################*/
struct window *CloseWind(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct StandardModState *state=wp->state;

if(GlobActivWind==wind) GlobActivWind=NULL;

if (wind)
	{
	if (wp->state!=NULL)
		{
		if (wp->Flags2 & LE_NEGONE) state->win_le=-1;
		else state->win_le=wind->LeftEdge;
		if (wp->Flags2 & TE_NEGONE) state->win_te=-1;
		else state->win_te=wind->TopEdge;
		wp->Flags2&=~(TE_NEGONE+LE_NEGONE);
		state->win_w=wind->Width;
		state->win_h=wind->Height;		/* SAVE WINDOW STATES */
		state->flags &= (~MS_WINDOW_OPEN);
		state->window=NULL;
		}


	if (wp->Flags&WINDOWSIZING)	RemoveGadget(wind,&wp->SizeGad);
	if (wp->Flags&WINDOWDRAG)	RemoveGadget(wind,&wp->DragGad);
	if (wp->Flags&WINDOWCLOSE)	{delete_gadget(wp->CloseButton.gad_ptr);}
	if (wp->Flags&WINDOWDEPTH)	{delete_gadget(wp->DepthButton.gad_ptr);}
	if (wp->Flags&WINDOWSIZING)	{delete_gadget(wp->DSizeButton.gad_ptr);}
	if (wp->Flags2&PREFERENCES)	{delete_gadget(wp->PrefsButton.gad_ptr);}
/*DelTitBarGads(wind);*/

	wind->UserData=NULL;
	FreeMem(wp,sizeof(struct WindowParms));

	wind->FirstGadget=NULL;
	CloseWindow(wind);
	}
return(NULL);
}
/*############################################################
NAME	
	RefreshWind -
FUNCTION
	This function refresh the entire window including the title bar,
	the border and all of the window gadgets.  This function will
	overwrite any information displayed on the window including the
	gadgets, so you must refresh all gadgets and other wind elements
	after this function is called.
INPUTS
	wind - Pointer to the window you wish to refresh;
RESULTS
	NONE
##############################################################*/
void RefreshWind(wind)
struct Window *wind;
{
/*struct WindowParms *wp=(struct WindowParms *)wind->UserData;*/

LockGadLibRender();
SetDrMd(wind->RPort,JAM1);
SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);

RectFill(wind->RPort, 2,1, wind->Width-3, wind->Height-2);

SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
RectFill(wind->RPort, 0 ,0, 1, wind->Height);
RectFill(wind->RPort, 0 ,0, wind->Width,0);
RectFill(wind->RPort, 1 ,11, wind->Width-1,11);

SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);
UnLockGadLibRender();

RefreshBar(wind,0);
}
/*############################################################
NAME	
	RefreshBar -
FUNCTION
	Refreshes the title bar of a wind using the curent value of
	the wind flag 'WINDOWACTIVE' to determine the color the bar
	should be rendered.
INPUTS
	wind - a pointer to the window stucture of the window to be refreshed
	x - a flag variable. 
	if x==NULL always refresh
	if x!=NULL only refresh if the color should be changed
RESULTS
	NONE
##############################################################*/
struct GadgetColors svclrs;
void RefreshBar(wind,x)
struct Window *wind;
short x;
{
int R=wind->Width,L=0;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
static struct HitAreaColor clrs;
struct GadgetColors *gclrs=&svclrs;
USHORT FLAG=0,chars,tempclr,textclr;

if (x==0 || (wind->Flags&WINDOWACTIVE)!=(wp->Flags2&WINDOWACTIVE))
	{
/*	WaitTOF();
	WaitTOF();*/

clrs=StudioBase->defaults.colors.gadget_window;

	LockGadLibRender();
	if (wind->Flags&WINDOWACTIVE) 
		{
		tempclr=clrs.hitbox_unselected;
		clrs.hitbox_unselected=clrs.hitbox_selected;
		clrs.hitbox_selected=tempclr;
		textclr=clrs.draw_selected;
		wp->Flags2|=WINDOWACTIVE;
		GlobActivWind=wind;
		if (StudioBase->defaults.flags&WIND_POPTOFRONT) WindowToFront(wind);
		}
	else 	{
		FLAG=1;
		wp->Flags2&=~WINDOWACTIVE;
		if(GlobActivWind==wind) GlobActivWind=NULL;
		textclr=clrs.draw_unselected;
		}

gclrs->slider_box=clrs;
gclrs->slider_knob=clrs;
gclrs->slider_button=clrs;
gclrs->button=clrs;
gclrs->list=clrs;
gclrs->droplist=clrs;
gclrs->gadget_window=clrs;


	SetDrMd(wind->RPort,JAM1);
	SetAPen(wind->RPort,clrs.hitbox_unselected);
	RectFill(wind->RPort, 2,1, wind->Width-3,9);
	draw_indent(wind->RPort,0,0,wind->Width-1,10,clrs.topleft_3d,clrs.bottomright_3d);

	if (wp->Flags&WINDOWCLOSE) 
		{
		if (StudioBase->defaults.flags&WIND_SHORTTITLEBAR && FLAG==1 && !(wp->Flags2&IL_NOSIMPLE))
			{
			hide_gadget(wp->CloseButton.gad_ptr);
			}
		else
			{
			wp->CloseButton.gad_ptr->c=gclrs;
			L=24;
			show_gadget(wp->CloseButton.gad_ptr);
			refresh_gadget(wp->CloseButton.gad_ptr);
			if (wp->CloseButton.gad_ptr->flags&BUTN_HIT) 
				{
				draw_indent(wind->RPort,4,2,19,8,clrs.bottomright_3d^0xFF,clrs.topleft_3d^0xFF);
				draw_indent(wind->RPort,9,4,14,6,clrs.topleft_3d^0xFF,clrs.bottomright_3d^0xFF);
				}
			else
				{
				draw_indent(wind->RPort,4,2,19,8,clrs.bottomright_3d,clrs.topleft_3d);
				draw_indent(wind->RPort,9,4,14,6,clrs.topleft_3d,clrs.bottomright_3d);
				}
			}
		}

	/* Depth Gadget */
	if (wp->Flags&WINDOWDEPTH)
		{
		if ((StudioBase->defaults.flags&WIND_SHORTTITLEBAR && FLAG==1 && !(wp->Flags2&IL_NOSIMPLE)) || (StudioBase->defaults.flags&WIND_POPTOFRONT))
			{
			hide_gadget(wp->DepthButton.gad_ptr);
			}
		else
			{
			wp->DepthButton.gad_ptr->c=gclrs;
			wp->DepthButton.gad_ptr->box_x1=wind->Width-23-1;
			wp->DepthButton.gad_ptr->box_x2=wind->Width-1;
			show_gadget(wp->DepthButton.gad_ptr);
			refresh_gadget(wp->DepthButton.gad_ptr);

			if (wp->DepthButton.gad_ptr->flags&BUTN_HIT) 
				{
				draw_indent(wind->RPort,wind->Width-20,2,wind->Width-9,6,(clrs.bottomright_3d)^0xFF,(clrs.topleft_3d)^0xFF);
				draw_indent(wind->RPort,wind->Width-16,4,wind->Width-5,8,(clrs.bottomright_3d)^0xFF,(clrs.topleft_3d)^0xFF);
				SetAPen(wind->RPort,(gclrs->button.hitbox_unselected)^0xFF);
				}
			else 
				{
				draw_indent(wind->RPort,wind->Width-20,2,wind->Width-9,6,clrs.bottomright_3d,clrs.topleft_3d);
				draw_indent(wind->RPort,wind->Width-16,4,wind->Width-5,8,clrs.bottomright_3d,clrs.topleft_3d);
				SetAPen(wind->RPort,gclrs->button.hitbox_unselected);
				}
			RectFill(wind->RPort,wind->Width-14,5,wind->Width-7,7);
			R=R-24;
			}
		}

	/* Size Gadget */
	if (wp->Flags&WINDOWSIZING)
		{
		if (StudioBase->defaults.flags&WIND_SHORTTITLEBAR && FLAG==1 && !(wp->Flags2&IL_NOSIMPLE))
			{
			hide_gadget(wp->DSizeButton.gad_ptr);
			}
		else
			{
			wp->SizeGad.LeftEdge =wind->Width-12;
			wp->SizeGad.TopEdge =wind->Height-6;
			SetAPen(wind->RPort,clrs.bottomright_3d);
			Move(wind->RPort,wp->SizeGad.LeftEdge,wp->SizeGad.TopEdge+wp->SizeGad.Height-1);
			Draw(wind->RPort,wp->SizeGad.LeftEdge+wp->SizeGad.Width-1,wp->SizeGad.TopEdge);
			SetAPen(wind->RPort,clrs.topleft_3d);
			Move(wind->RPort,wp->SizeGad.LeftEdge+2,wp->SizeGad.TopEdge+wp->SizeGad.Height-1);
			Draw(wind->RPort,wp->SizeGad.LeftEdge+wp->SizeGad.Width-1,wp->SizeGad.TopEdge+1);

			wp->DSizeButton.gad_ptr->box_x1=R-24;
			wp->DSizeButton.gad_ptr->box_x2=R-1;
			wp->DSizeButton.gad_ptr->c=gclrs;
			show_gadget(wp->DSizeButton.gad_ptr);
			refresh_gadget(wp->DSizeButton.gad_ptr);

			if (wp->DSizeButton.gad_ptr->flags&BUTN_HIT) 
				{
				draw_indent(wind->RPort,R-24+4,2,R-24+19,8,clrs.bottomright_3d^0xff,clrs.topleft_3d^0xff);
				draw_indent(wind->RPort,R-24+12,5,R-24+17,7,clrs.topleft_3d^0xff,clrs.bottomright_3d^0xff);
				}
			else 
				{
				draw_indent(wind->RPort,R-24+4,2,R-24+19,8,clrs.bottomright_3d,clrs.topleft_3d);
				draw_indent(wind->RPort,R-24+12,5,R-24+17,7,clrs.topleft_3d,clrs.bottomright_3d);
				}
			R=R-24;
			}
		}

	if (wp->Flags2&PREFERENCES)
		{
		if (StudioBase->defaults.flags&WIND_SHORTTITLEBAR && FLAG==1 && !(wp->Flags2&IL_NOSIMPLE))
			{
			hide_gadget(wp->PrefsButton.gad_ptr);
			}
		else
			{
			wp->PrefsButton.gad_ptr->c=gclrs;
			wp->PrefsButton.gad_ptr->box_x1=R-24;
			wp->PrefsButton.gad_ptr->box_x2=R-1;
			show_gadget(wp->PrefsButton.gad_ptr);
			refresh_gadget(wp->PrefsButton.gad_ptr);

			if (wp->PrefsButton.gad_ptr->flags&BUTN_HIT) 
				SetAPen(wind->RPort,clrs.bottomright_3d^0xFF);
			else	SetAPen(wind->RPort,clrs.bottomright_3d);
			SetDrMd(wind->RPort, JAM1);
			Move(wind->RPort,R-24+7 ,9);
			Text(wind->RPort,&"?",1);
			if (wp->PrefsButton.gad_ptr->flags&BUTN_HIT) 
				SetAPen(wind->RPort,clrs.topleft_3d^0xFF);
			else	SetAPen(wind->RPort,clrs.topleft_3d);
			SetDrMd(wind->RPort, JAM1);
			Move(wind->RPort,R-24+8 ,8);
			Text(wind->RPort,&"?",1);
			R=R-24;
			}
		}

	/* Drag Gadget */
	if (wp->Flags&WINDOWDRAG)
		{
		draw_indent(wind->RPort,L,0,R-1,10,clrs.topleft_3d,clrs.bottomright_3d);
		SetAPen(wind->RPort,textclr);
		SetDrMd(wind->RPort, JAM1);
		chars=MIN(MAX((R-L-12),0)>>3, strlen(wp->Title) );
		Move(wind->RPort,(L+((R-L-(chars<<3))>>1)),8);
		if (wp->Title) Text(wind->RPort,wp->Title,chars);
		wp->DragGad.Width=R-L; 
		wp->DragGad.LeftEdge=L;
		}
	UnLockGadLibRender();
	}
}
/*############################################################
NAME	
	WindTitleChange -
FUNCTION
	Changes the text displayed in the title bar of a studio16 window
INPUTS
	wind - pointer to the window that needs its title bar altered
	cs - a pointer to the new title bar text.
RESULTS
	NONE
##############################################################*/
void WindTitleChange(wind,cs)
struct Window *wind;
char *cs;
{
int R=wind->Width,L=0;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
static struct HitAreaColor clrs;
USHORT FLAG,chars,tempclr,textclr;

clrs=StudioBase->defaults.colors.gadget_window;

if (wind->Flags&WINDOWACTIVE) 
	{
	FLAG=0;
	tempclr=clrs.hitbox_unselected;
	clrs.hitbox_unselected=clrs.hitbox_selected;
	clrs.hitbox_selected=tempclr;
	textclr=clrs.draw_selected;
	}
else 
	{
	FLAG=1;
	textclr=clrs.draw_unselected;
	}

wp->Title=cs;
if (!(StudioBase->defaults.flags&WIND_SHORTTITLEBAR) || FLAG==0)
	{
	if (wp->Flags&WINDOWCLOSE) 	L=24;
	if (wp->Flags&WINDOWDEPTH && !(StudioBase->defaults.flags&WIND_POPTOFRONT))	R=R-24;
	if (wp->Flags&WINDOWSIZING)	R=R-24;
	if (wp->Flags2&PREFERENCES)	R=R-24;
	}

if (wp->Flags&WINDOWDRAG)
	{
	LockGadLibRender();
	SetDrMd(wind->RPort,JAM1);
	SetAPen(wind->RPort,clrs.hitbox_unselected);
	RectFill(wind->RPort, L+2,1, R-3,9);

	draw_indent(wind->RPort,L,0,R-1,10,clrs.topleft_3d,clrs.bottomright_3d);
	SetAPen(wind->RPort,textclr);
	SetDrMd(wind->RPort, JAM1);
	chars=MIN(MAX((R-L-12),0)>>3, strlen(wp->Title) );
	Move(wind->RPort,(L+((R-L-(chars<<3))>>1)),8);
	if (cs) Text(wind->RPort,wp->Title,MIN((MAX(R,33)-33)>>3,strlen(wp->Title)));
	wp->DragGad.Width=R-24; 
	wp->DragGad.LeftEdge=L;
	UnLockGadLibRender();
	}
}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
void SaveGadgetPlacement(ngh)
struct NewGadgetHeader *ngh;
{
struct GadgetHeader *g;
while (ngh)
	{
	if (ngh->gad_ptr!=NULL)
		{
		g=(struct GadgetHeader *)ngh->gad_ptr;
		if (g!=NULL)
			{
			ngh->box_x1=g->box_x1;
			ngh->box_y1=g->box_y1;
			ngh->box_x2=g->box_x2;
			ngh->box_y2=g->box_y2;
			}
		}
	ngh=ngh->next;
	}
}

/*******************************************************************/
/*******************************************************************/
void SaveGadgetState(ngh)
struct NewGadgetHeader *ngh;
{
struct GadgetHeader *g;
while (ngh)
	{
	if (ngh->gad_ptr!=NULL)
		{
		g=(struct GadgetHeader *)ngh->gad_ptr;
		if (ngh->flags & GAD_SLIDER)
			{
			((struct NewSlider *)ngh)->top_element=((struct slider *)g)->top_element;
			ngh->gad_ptr=NULL;
			}
		if (ngh->flags & GAD_DROPLIST)
			{
			((struct NewDropList *)ngh)->initial_value=((struct droplist *)g)->l->current;
			ngh->gad_ptr=NULL;
			}
		if (ngh->flags & GAD_BUTTON)
			{
			((struct NewButton *)ngh)->flags=(((struct NewButton *)ngh)->flags & (~BUTN_HIT)) + (((struct button *)g)->flags & BUTN_HIT);
			ngh->gad_ptr=NULL;
			}
		}
	ngh=ngh->next;
	}
}
/*############################################################
NAME	
	ColorGadList -
FUNCTION
	To set the colors used to render a chain of gadgets to the 
	StudioBase->defaults.colors. Since this module requires the
	Studio library and uses gadlibrary structures it should not
	be implimented in either library.
INPUTS
	ngh - Pointer to a chain of NewGadgetHeader structures
RESULTS
	NONE
##############################################################*/
void ColorGadList(ngh)		/* CHANGE GADGET COLORS */
struct NewGadgetHeader *ngh;
{
while (ngh)
	{
	ngh->c=&StudioBase->defaults.colors;
	ngh=ngh->next;
	}
}
/*******************************************************************/
void lock_knob(k)
	struct slider *k;
{
void move_knob();
k->flags=k->flags|RENDER_ONLY;
move_knob(k);
k->flags=k->flags&(0xFFFFFFFF^RENDER_ONLY);
}
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
void DepthWindow(g)
struct button *g;
{
struct Window *wind=(struct Window *)g->user_data;

if (!(StudioBase->defaults.flags&WIND_POPTOFRONT))
	{
	if(wind->RPort->Layer==wind->RPort->Layer->LayerInfo->top_layer)
		WindowToBack(wind);
	else
		WindowToFront(wind);
	}
else	{
	WindowToBack(wind);
	}
}
/*******************************************************************/
void DualSizeWindow(g)
struct button *g;
{
struct Window *wind=(struct Window *)g->user_data;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
USHORT le,te,w,h;

te=wind->TopEdge;
le=wind->LeftEdge;
w=wind->Width;
h=wind->Height;

if (wp->W > StudioBase->screen->Width) wp->W = StudioBase->screen->Width;
if (wp->H > StudioBase->screen->Height) wp->H = StudioBase->screen->Height;
if (wp->W+wp->LE > StudioBase->screen->Width) wp->LE = StudioBase->screen->Width - wp->W;
if (wp->H+wp->TE > StudioBase->screen->Height) wp->TE = StudioBase->screen->Height - wp->H;

if (wp->W+wp->LE <= StudioBase->screen->Width && wp->H+wp->TE <= StudioBase->screen->Height)
	{
	if (w+wp->LE <= StudioBase->screen->Width && h+wp->TE <= StudioBase->screen->Height)
		{
		MoveWindow(wind,wp->LE-le,wp->TE-te);	/* (0,0) */
		SizeWindow(wind,wp->W-w,wp->H-h);
		}
	else if (wp->W+le <= StudioBase->screen->Width && wp->H+te <= StudioBase->screen->Height)
		{
		SizeWindow(wind,wp->W-w,wp->H-h);
		MoveWindow(wind,wp->LE-le,wp->TE-te);	/* (0,0) */
		}
	else
		{
		MoveWindow(wind,-le,-te);	/* (0,0) */
		SizeWindow(wind,wp->W-w,wp->H-h);
		MoveWindow(wind,wp->LE,wp->TE);
		}
	}
else
	{
	MoveWindow(wind,-le,-te);	/* (0,0) */
	SizeWindow(wind,wp->W-w,wp->H-h);
	MoveWindow(wind,wp->LE,wp->TE);
	}

wp->TE=te;
wp->LE=le;
wp->W=w;
wp->H=h;
}
/*******************************************************************/
void WindPrefs(g)
struct button *g;
{
struct Window *wind=(struct Window *)g->user_data;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct StandardModState *state=wp->state;

BroadcastEventParms(GetEventID("SE_CMD_EDIT_PREFS"),state,wind->LeftEdge+wind->Width,wind->TopEdge,0,0,0);
}
/*******************************************************************/
void CloseWidw(g)
struct button *g;
{
ULONG task;
struct Window *wind=(struct Window *)g->user_data;
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct IntuiMessage *MESS;
struct MsgPort *tport;
void CloseWindTask();
static char Name[40];

if ((wp->Flags2&NOMESSAGE)==0)
	{
	wp->Flags2 |= NOMESSAGE;

/** !!!!!!!!!! The launched task is 60 so it will finish
	executing before the module it is linked into unloads.  This
	is very important.  When pri was -1 modlist would crash **/

	task=(ULONG)CreateTask("I_FIX_A_DEADLOCK",60,CloseWindTask,4000);
	sprintf(Name,"PORT%d",task);
	while ((tport=(struct MsgPort *)FindPort(Name))==0) WaitTOF();

	MESS=(struct IntuiMessage *)AllocMem(sizeof(struct IntuiMessage),MEMF_CLEAR);
	MESS->Class=NULL;
	MESS->IDCMPWindow=wind;
	MESS->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
	MESS->ExecMessage.mn_ReplyPort = NULL;
	MESS->ExecMessage.mn_Length = sizeof(struct IntuiMessage);

	PutMsg(tport,MESS);
	}
}
/*******************************************************************/
void CloseWindTask()
{
ULONG task;
struct Window *wind;
struct MsgPort *Port;
struct MsgPort *tport;
struct MsgPort *tempport;
struct IntuiMessage *MESS;
struct WindowParms *wp;
static char Name[40];

task=(ULONG)FindTask(NULL);
sprintf(Name,"PORT%d",task);
tport=(struct MsgPort *)CreatePort(Name,0);
WaitPort(tport);
MESS=(struct IntuiMessage *)GetMsg(tport);
wind=MESS->IDCMPWindow;
Port=wind->UserPort;
wp=(struct WindowParms *)wind->UserData;
FreeMem(MESS,sizeof(struct IntuiMessage));
DeletePort(tport);

if (wp->Flags2 & HIDEMESSAGE)
	{
	BroadcastEventParms(GetEventID("SE_CMD_HIDE_WINDOW"),wind,NULL,NULL,0,0,0);
	}
else
	{
	/* Create a close window IMDCP message */
	tempport=(struct MsgPort *)CreatePort(0,0);
	MESS=(struct IntuiMessage *)AllocMem(sizeof(struct IntuiMessage),MEMF_CLEAR);
	if (MESS) {
		MESS->Class=CLOSEWINDOW;
		MESS->IDCMPWindow=wind;
		MESS->ExecMessage.mn_Node.ln_Type = NT_MESSAGE;
		MESS->ExecMessage.mn_ReplyPort = tempport;
		MESS->ExecMessage.mn_Length = sizeof(struct IntuiMessage);
		PutMsg(Port,MESS);
	
		WaitPort(tempport);
		GetMsg(tempport);
		FreeMem(MESS,sizeof(struct IntuiMessage));
		}
	else {/**kprintf("Alloc failed!!\n");**/}
	DeletePort(tempport);
	}
}
/*############################################################
NAME	
	FindFileName -
FUNCTION
	This function parces the string returned by the _main(cmdline)
	funtion and derives the file name used to run the new module. If you
	choose to use this function, the name that appears in the instance
	list will always match the name in the ModList.
INPUTS
	name - a pointer to the command line used to launch the new module
		void _main(name)
RESULTS
	char pointer into the same input string, offset to remove any path
	used to launch the module.
##############################################################*/
char *FindFileName(name)
char *name;
{
int x=0,y=1;
static char *pname="Don'T Run Me From WBench";

if (stclen(name))
	{
	while (*(name+x)!=NULL)
		{
		if (*(name+x)=='/' || *(name+x)==':') y=x+1;
		x++;
		if (*(name+x)=='"') *(name+x)=0;
		}
	return(name+y);
	}
else 
	{
	return(pname);
	}
}
/*******************************************************************/
void SaveWindState(wind)
struct Window *wind;
{
struct WindowParms *wp=NULL;
struct StandardModState *state=NULL;

if (wind) wp=(struct WindowParms *)wind->UserData;
if (wp) state=wp->state;

if (state!=NULL)
	{
	state->win_le=wind->LeftEdge;
	state->win_te=wind->TopEdge;
	state->win_w=wind->Width;
	state->win_h=wind->Height;
	}
}
/****************************************************************/
/****************************************************************/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
