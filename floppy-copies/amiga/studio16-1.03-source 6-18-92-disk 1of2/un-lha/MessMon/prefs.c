#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
/********************************************************************/
/********************************************************************/
extern struct Module *mod;
extern struct StudioBase *StudioBase;
extern int DisplayFlags;
extern struct Task *MessMonTask;
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
   "Mess Mon Prefs", /** TITLE **/
   0,0,
   104,37,1024,1024,
   CUSTOMSCREEN
   };
struct Window *Pwind=NULL;

void SmpteErrorSel();

void NumPreFunc();

#define LE 40
static struct NewButton ton5 = 
	{NULL,					/* *next (gadget) */
	LE+121,51,LE+150,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)75,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"75"};				/* *text */

static struct NewButton ton4 = 
	{&ton5,					/* *next (gadget) */
	LE+91,51,LE+120,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)45,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"45"};				/* *text */

static struct NewButton ton3 = 
	{&ton4,					/* *next (gadget) */
	LE+61,51,LE+90,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,				/* Call Function */
	(APTR)0,				/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"0"};					/* *text */

static struct NewButton ton2 = 
	{&ton3,					/* *next (gadget) */
	LE+31,51,LE+60,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)-5,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"-5"};				/* *text */

static struct NewButton button3 = 
	{&ton2,					/* *next (gadget) */
	12,39,30,47,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR,	/* Flags */
	SmpteErrorSel,				/* Call Function */
	(APTR)4,				/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Display Window"};					/* *text */

static struct NewButton button2 = 
	{&button3,					/* *next (gadget) */
	12,27,30,35,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR,	/* Flags */
	SmpteErrorSel,					/* Call Function */
	(APTR)2,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Display Serial Short"};				/* *text */

static struct NewButton button1 = 
	{&button2,					/* *next (gadget) */
	12,15,30,23,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR,	/* Flags */
	SmpteErrorSel,					/* Call Function */
	(APTR)1,					/* user_data */
	NULL,					/* COLORS */
	NULL,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"Display Serial Full"};				/* *text */

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

	ColorGadList(&button1);
	button1.flags &=~BUTN_HIT;
	button2.flags &=~BUTN_HIT;
	button3.flags |=BUTN_HIT;

	ton2.flags |=BUTN_HIT;
	ton3.flags &=~BUTN_HIT;
	ton4.flags &=~BUTN_HIT;
	ton5.flags &=~BUTN_HIT;
	SetTaskPri(MessMonTask,-5);
	DisplayFlags=4;

	SetAPen(Pwind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(Pwind->RPort,10,59);
	Text(Pwind->RPort,"Priorty", 7);
	
	if (create_gadget_chain(Pwind,&button1)==0) return(Pwind);
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

void SmpteErrorSel(g)
struct button *g;
{
if (g->flags & BUTN_HIT)
	DisplayFlags|=(int)g->user_data;
else
	DisplayFlags&=~(int)g->user_data;

}
/*******************************************************************/
void NumPreFunc(g)
struct button *g;
{
if (g->flags & BUTN_HIT)
	{
	SetTaskPri(MessMonTask,(int)g->user_data);
	}
}
