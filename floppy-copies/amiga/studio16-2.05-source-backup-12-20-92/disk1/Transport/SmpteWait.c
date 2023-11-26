#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "clib/macros.h"
#include "trans.h"
/**********************************/
extern short se_cmd_kill_module;
/**********************************/
struct Window  *SWwind=NULL;
struct MsgPort *SWport=NULL;
struct Task *SWtask,*FindTask();
/**********************************/
extern struct AudioHandler 	*hand;
extern struct Module 		*mod;
extern int GlobSampRate;
extern int GlobFilterCut;
extern int plock;
extern struct Task *task;
extern long sw_abort_sigbit;
extern long sw_trigger_sigbit;
/**********************************/
static BOOL EXIT=FALSE;
#define WINDW 200
#define WINDH 60
/**********************************/
void ExitSW();
void SWDisplayWindow();
void SWHideWindow();
void SWhandle_intuievents();
void SWhandle_studioevents();
void SmpteWaitMain();
void OpenSmpteWaitWindow();
/**********************************/
/**********************************/
/**********************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct NewWindow SWNewWind = {
   -1,
   -1,
   WINDW,
   WINDH,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   0,0,
   "Transport Smpte", /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void Playfunc()
{
Signal(task,(1<<sw_trigger_sigbit));
}


struct NewButton PBut = {
	NULL,					/* *next (gadget) */
	8,NULL,NULL,NULL,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|MANUAL_XOR,			/* Flags */
	Playfunc,			/* Call Function */
	NULL,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"PLAY"};				/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void Abortfunc()
{
Signal(task,(1<<sw_abort_sigbit));
}

struct NewButton AbortBut = {
	&PBut,					/* *next (gadget) */
	8,NULL,NULL,NULL,			/* Box x1,y1,x2,y2 */
	BUTN_COOL|MANUAL_XOR,			/* Flags */
	Abortfunc,			/* Call Function */
	NULL,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"ABORT"};				/* *text */
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
char SWTCstr[30];
char SWTCstr2[30];
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void OpenSmpteWaitWindow(tc,etc)
int tc;
{
sprintf(SWTCstr, "Start Time %02d:%02d:%02d:%02d",(tc>>24)&0x0FF,(tc>>16)&0x0FF,(tc>>8)&0x0FF,(tc)&0x0FF);
sprintf(SWTCstr2,"End   Time %02d:%02d:%02d:%02d",(etc>>24)&0x0FF,(etc>>16)&0x0FF,(etc>>8)&0x0FF,(etc)&0x0FF);
if (SWwind) {WindowToFront(SWwind);return;}
CreateTask("STUDIO_TRANSPORT_SMPTE_WAIT",0,SmpteWaitMain,4000);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void CloseSmpteWaitWindow()
{
EXIT=TRUE;
if (SWtask) Signal(SWtask,(1<<SWport->mp_SigBit));
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SmpteWaitMain()
{
int mask;
plock++;
SWtask=FindTask(NULL);
SWDisplayWindow();
EXIT=FALSE;
while (!EXIT)
	{
	mask=NULL;
	if (SWport) mask|=(1<<SWport->mp_SigBit);
	if (SWwind) mask |= (1<<SWwind->UserPort->mp_SigBit);
	mask=Wait(mask);
	if (SWwind) if(mask&(1<<SWwind->UserPort->mp_SigBit))  SWhandle_intuievents();
	if (SWport) if (mask & (1<<SWport->mp_SigBit)) SWhandle_studioevents();
	}
SWHideWindow();
plock--;
SWtask=NULL;
return;
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SWhandle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(SWport))!=0)
	{
	/*********************************/
	/*********************************/
	/*********************************/
	/*********************************/
	if(event->type == se_cmd_kill_module && (struct Module *)event->arg1 == mod)
		{
		EXIT=TRUE;
		}
	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SWhandle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(SWwind->UserPort))!=0)
	{
	HandleIntuitMess(SWwind,message);
	class=message->Class;
	ReplyMsg(message);

	if (class == CLOSEWINDOW)
		{
		EXIT=TRUE;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SWHideWindow()
{
if (SWwind)
	{
	IgnoreEventPort(se_cmd_kill_module,SWport);
	DeletePort(SWport);	SWport=NULL;
	delete_gadget_chain(SWwind);
	CloseWind(SWwind);
	SWwind=NULL;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SWDisplayWindow()
{
struct Window *OpenNewWind();

if ((SWwind = OpenNewWind(&SWNewWind,NULL,IL_NO_MENUS,NULL))!=0)  
	{
	SWport = (struct MsgPort *)CreatePort(0,0);

	NotifyMeOnEventPort(se_cmd_kill_module,SWport);

	ColorGadList(&AbortBut);
	PBut.box_x2=(SWwind->Width/2)-4;
	PBut.box_y1=SWwind->Height-25;
	PBut.box_y2=SWwind->Height-5;
	AbortBut.box_x1=(SWwind->Width/2)+4;
	AbortBut.box_x2=SWwind->Width-9;
	AbortBut.box_y1=SWwind->Height-25;
	AbortBut.box_y2=SWwind->Height-5;
	if (create_gadget_chain(SWwind,&AbortBut)==0)  {telluser("Transport Sample Parms:","Could not create gadgets!",0); EXIT=TRUE;}

	if (Pflags & SMPTE_STOP) 
		{
		S16Text(SWwind->RPort,SWTCstr,strlen(SWTCstr),10,20);
		S16Text(SWwind->RPort,SWTCstr2,strlen(SWTCstr2),10,31);
		}
	else S16Text(SWwind->RPort,SWTCstr,strlen(SWTCstr),10,25);

	}
else telluser("Transport Smpte Wait:","Can not open window!",0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
