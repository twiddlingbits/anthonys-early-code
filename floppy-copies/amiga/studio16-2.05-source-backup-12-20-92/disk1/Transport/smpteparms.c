#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "clib/macros.h"
#include "trans.h"
/**********************************/
int Pflags;
extern struct StudioBase *StudioBase;
/**********************************/
struct Window  *rpwind=NULL;
struct MsgPort *rpport=NULL;
struct Task *rptask,*FindTask();
/**********************************/
extern struct AudioHandler 	*hand;
extern struct Module 		*mod;
extern int GlobSampRate;
extern int GlobFilterCut;
extern int plock;
int Sflags;
extern int StartTC;
extern int EndTC;
/**********************************/
BOOL RPEXIT=FALSE;
#define WINDW 155
#define WINDH 42
/**********************************/
void RPDisplayWindow();
void RPHideWindow();
void rphandle_intuievents();
void rphandle_studioevents();
void RecordParmsMain();
void OpenRecordParmsWindow();
/**********************************/
/**********************************/
/**********************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern struct Gadget StopStrGad;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
static struct NewWindow RPNewWind = {
   -1,
   -1,
   WINDW,
   WINDH,
   BLACK,
   WHITE,
   GADGETUP,
   ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,
   &StopStrGad,0,
   "SMPTE", /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void GadFunc();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
UBYTE UNDOBUF[12];
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
UBYTE StartSIBuff[12] =
	"00:00:01:00";
struct StringInfo StartSInfo = {
	StartSIBuff,	/* buffer where text will be edited */
	UNDOBUF,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

struct Gadget StartStrGad = {
	NULL,	/* next gadget */
	WINDW-(10+8*12),16,	/* origin XY of hit box relative to window TopLeft */
	8*12,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGCENTER,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&StartSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
UBYTE StopSIBuff[12] =
	"00:01:01:00";
struct StringInfo StopSInfo = {
	StopSIBuff,	/* buffer where text will be edited */
	UNDOBUF,	/* optional undo buffer */
	0,	/* character position in buffer */
	12,	/* maximum number of characters to allow */
	0,	/* first displayed character buffer position */
	0,0,0,0,0,	/* Intuition initialized and maintained variables */
	0,	/* Rastport of gadget */
	0,	/* initial value for integer gadgets */
	NULL	/* alternate keymap (fill in if you set the flag) */
};

struct Gadget StopStrGad = {
	&StartStrGad,	/* next gadget */
	WINDW-(10+8*12),29,	/* origin XY of hit box relative to window TopLeft */
	8*12,8,	/* hit box width and height */
	NULL,	/* gadget flags */
	RELVERIFY+STRINGCENTER,	/* activation flags */
	STRGADGET,	/* gadget type flags */
	NULL,	/* gadget border or image to be rendered */
	NULL,	/* alternate imagery for selection */
	NULL,	/* first IntuiText structure */
	NULL,	/* gadget mutual-exclude long word */
	(APTR)&StopSInfo,	/* SpecialInfo structure */
	NULL,	/* user-definable data */
	NULL	/* pointer to user-definable data */
};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void OpenRecordParmsWindow()
{
if (rpwind) {WindowToFront(rpwind);return;}
CreateTask("STUDIO_TRANSPORT_SMPTE_PARMS",0,RecordParmsMain,4000);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RecordParmsMain()
{
int mask;
plock++;
rptask=FindTask(NULL);
RPDisplayWindow();
RPEXIT=FALSE;
while (!RPEXIT)
	{
	mask=NULL;
	if (rpport) mask|=(1<<rpport->mp_SigBit);
	if (rpwind) mask |= (1<<rpwind->UserPort->mp_SigBit);
	mask=Wait(mask);
	if (rpwind) if(mask&(1<<rpwind->UserPort->mp_SigBit))  rphandle_intuievents();
	if (rpport) if (mask & (1<<rpport->mp_SigBit)) rphandle_studioevents();
	}
RPHideWindow();
plock--;
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void rphandle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(rpport))!=0)
	{
	/*********************************/
	/*********************************/
	/*********************************/
	/*********************************/
	ReplyMsg(event);
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void rphandle_intuievents()
{
struct IntuiMessage *message;
struct Gadget *gad;
ULONG class;
char buld[15];

while ((message=(struct IntuiMessage *)GetMsg(rpwind->UserPort))!=0)
	{
	HandleIntuitMess(rpwind,message);
	class=message->Class;
	gad=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class == CLOSEWINDOW)
		{
		RPEXIT=TRUE;
		}

	if (class == GADGETUP)
		{
		if (gad==&StartStrGad) {StartTC=TCstr2int(StartSIBuff);}
		if (gad==&StopStrGad)  {EndTC=TCstr2int(StopSIBuff);}

		sprintf(buld,"%02d:%02d:%02d:%02d",(StartTC>>24)&0x0FF,(StartTC>>16)&0x0FF,(StartTC>>8)&0x0FF,(StartTC)&0x0FF);
		strcpy(StartSIBuff,buld);
		sprintf(buld,"%02d:%02d:%02d:%02d",(EndTC>>24)&0x0FF,(EndTC>>16)&0x0FF,(EndTC>>8)&0x0FF,(EndTC)&0x0FF);
		strcpy(StopSIBuff,buld);
		RefreshGList(&StopStrGad,rpwind,NULL,2);
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RPHideWindow()
{
if (rpwind)
	{
	DeletePort(rpport);	rpport=NULL;
	StartTC=TCstr2int(StartSIBuff);
	EndTC=TCstr2int(StopSIBuff);
	CloseWind(rpwind);
	rpwind=NULL;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RPDisplayWindow()
{
struct Window *OpenNewWind();
char buld[15];

	sprintf(buld,"%02d:%02d:%02d:%02d",(StartTC>>24)&0x0FF,(StartTC>>16)&0x0FF,(StartTC>>8)&0x0FF,(StartTC)&0x0FF);
	strcpy(StartSIBuff,buld);
	sprintf(buld,"%02d:%02d:%02d:%02d",(EndTC>>24)&0x0FF,(EndTC>>16)&0x0FF,(EndTC>>8)&0x0FF,(EndTC)&0x0FF);
	strcpy(StopSIBuff,buld);

if ((rpwind = OpenNewWind(&RPNewWind,NULL,IL_NO_MENUS,NULL))!=0)  
	{
	rpport = (struct MsgPort *)CreatePort(0,0);

	draw_indent(rpwind->RPort,StartStrGad.LeftEdge-2,StartStrGad.TopEdge-1,StartStrGad.Width+StartStrGad.LeftEdge+1,StartStrGad.TopEdge+StartStrGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(rpwind->RPort,StopStrGad.LeftEdge-2,StopStrGad.TopEdge-1,StopStrGad.Width+StopStrGad.LeftEdge+1,StopStrGad.TopEdge+StopStrGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	S16Text(rpwind->RPort,"Start",5,5,StartStrGad.TopEdge+6);
	S16Text(rpwind->RPort,"Stop",4,5,StopStrGad.TopEdge+6);

	}
else telluser("Transport Record Parms:","Can not open window!",0);
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
