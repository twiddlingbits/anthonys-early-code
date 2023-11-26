#include "exec/types.h"
#include <exec/lists.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <exec/semaphores.h>
#include "intuition/intuition.h"
#include "exec/memory.h"
#include "/include/psound.h"

extern struct studioBase *studioBase;

struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();

struct Window *OpenWindow();

static struct WindowParms
	{
	LONG	Flags;
	struct NewWindow *NWind;
	struct Gadget *SizeGad;
	};

struct Gadget RESIZEgad = { NULL,NULL,NULL,NULL,NULL,GADGHCOMP,GADGIMMEDIATE|RELVERIFY,SIZING,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL };

/*******************************************************************/
struct window *OpenWind(nwind)
struct NewWindow *nwind;
{
struct Window *wind;
struct WindowParms *wp;

if ((wp=(struct WindowParms *)AllocMem(sizeof(struct WindowParms),MEMF_CLEAR))==NULL) return(NULL);
wp->NWind=nwind;

if ((nwind->Flags&WINDOWSIZING)==WINDOWSIZING)
	{
	if ((wp->SizeGad=(struct Gadget *)AllocMem(sizeof(struct Gadget),NULL))==NULL) {FreeMem(wp,sizeof(struct WindowParms));return(NULL);}
	CopyMem(&RESIZEgad,wp->SizeGad,sizeof(struct Gadget));

	nwind->Flags ^= WINDOWSIZING;
	if ((wind=OpenWindow(nwind))==NULL) {FreeMem(wp,sizeof(struct WindowParms));FreeMem(wp->SizeGad,sizeof(struct Gadget));return(NULL);}
	nwind->Flags ^= WINDOWSIZING;

	wind->UserData=(BYTE *)wp;

	RefreshWind(wind);
	AddGadget(wind,wp->SizeGad,-1);
	}
else
	{
	if ((wind=OpenWindow(nwind))==NULL) {FreeMem(wp,sizeof(struct WindowParms));return(NULL);}
	wind->UserData=(BYTE *)wp;
	RefreshWind(wind);
	}
return(wind);
}
/*******************************************************************/
struct window *CloseWind(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct IntuiMessage *message;

if (wind)
	{
	wp->NWind->LeftEdge=wind->LeftEdge;
	wp->NWind->TopEdge=wind->TopEdge;
	wp->NWind->Width=wind->Width;
	wp->NWind->Height=wind->Height;

	if (wp->SizeGad)
		{
		RemoveGadget(wind,wp->SizeGad);
		 FreeMem(wp->SizeGad,sizeof(struct Gadget));
		}
	if (wp) FreeMem(wp,sizeof(struct WindowParms));

	while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=NULL)
		ReplyMsg(message);

	wind->UserData=NULL;
	CloseWindow(wind);
	}
return(NULL);
}
/*******************************************************************/
void RefreshWind(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
struct Gadget *RSG=wp->SizeGad;

SetAPen(wind->RPort, BROWN);
RectFill(wind->RPort, 2,11, wind->Width-3, wind->Height-2);

SetAPen(wind->RPort, WHITE);
RectFill(wind->RPort, 0 ,0, 1, wind->Height);

SetAPen(wind->RPort, DARK_BROWN);
RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

if (RSG)
	{
	RSG->LeftEdge =wind->Width-12;
	RSG->TopEdge =wind->Height-6;
	RSG->Width =10;
	RSG->Height =5;

	SetAPen(wind->RPort,DARK_BROWN);
	Move(wind->RPort,RSG->LeftEdge,RSG->TopEdge+RSG->Height-1);
	Draw(wind->RPort,RSG->LeftEdge+RSG->Width-1,RSG->TopEdge);
	SetAPen(wind->RPort,WHITE);
	Move(wind->RPort,RSG->LeftEdge+2,RSG->TopEdge+RSG->Height-1);
	Draw(wind->RPort,RSG->LeftEdge+RSG->Width-1,RSG->TopEdge+1);
	}
}
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
			((struct NewButton *)ngh)->flags=(((struct NewButton *)ngh)->flags & (0xFFFFFFFF ^ BUTN_HIT)) + (((struct button *)g)->flags & BUTN_HIT);
			ngh->gad_ptr=NULL;
			}
		}
	ngh=ngh->next;
	}
}
/*******************************************************************/
void BroadcastEvent(type, arg1, arg2, arg3, arg4, arg5, flags)
short type;
unsigned long arg1;
unsigned long arg2;
unsigned long arg3;
unsigned long arg4;
unsigned long arg5;
unsigned long flags;
{
void BEP();
BEP(studioBase->notifyarray,type, arg1, arg2, arg3, arg4, arg5, flags);
}

void BEP(NotifyArray,type, arg1, arg2, arg3, arg4, arg5, flags)
struct MinList NotifyArray[];
short type;
unsigned long arg1;
unsigned long arg2;
unsigned long arg3;
unsigned long arg4;
unsigned long arg5;
unsigned long flags;

{
struct MinNode *node;



ObtainSemaphore(&studioBase->lock);
for (node=NotifyArray[type].mlh_Head; node->mln_Succ; node = node->mln_Succ)
	{
	SendMsg(((struct NotifyLink *)node)->notifyme, type, arg1, arg2, arg3, arg4, arg5, flags);
	}
ReleaseSemaphore(&studioBase->lock);
}
/*******************************************************************/

void lock_knob(k)
	struct slider *k;
{
void move_knob();
k->flags=k->flags|RENDER_ONLY
move_knob(k)
k->flags=k->flags&(0xFFFFFFFF^RENDER_ONLY);
}

/*
GAD_SLIDER
GAD_BUTTON
GAD_LIST
GAD_DROPLIST
GAD_LOOPLIST
*/
