#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "cl.h"
/********************************************************************/
/********************************************************************/
extern struct Module *mod;
extern struct StudioBase *StudioBase;
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
   "Cue List Prefs", /** TITLE **/
   0,0,
   104,37,1024,1024,
   CUSTOMSCREEN
   };
struct Window *Pwind=NULL;

void SmpteErrorSel();

void NumPreFunc();

#define LE 40
static struct NewButton ton7 = 
	{NULL,					/* *next (gadget) */
	LE+131,51,LE+150,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)9,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"9"};				/* *text */


static struct NewButton ton6 = 
	{&ton7,					/* *next (gadget) */
	LE+111,51,LE+130,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)8,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"8"};				/* *text */

static struct NewButton ton5 = 
	{&ton6,					/* *next (gadget) */
	LE+91,51,LE+110,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)7,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"7"};				/* *text */

static struct NewButton ton4 = 
	{&ton5,					/* *next (gadget) */
	LE+71,51,LE+90,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)6,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"6"};				/* *text */

static struct NewButton ton3 = 
	{&ton4,					/* *next (gadget) */
	LE+51,51,LE+70,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,				/* Call Function */
	(APTR)5,				/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"5"};					/* *text */

static struct NewButton ton2 = 
	{&ton3,					/* *next (gadget) */
	LE+31,51,LE+50,61,				/* Box x1,y1,x2,y2 */
	BUTN_COOL|BUTN_TOGGLE|MANUAL_XOR|KEEP_ONE_ACTIVE,	/* Flags */
	NumPreFunc,					/* Call Function */
	(APTR)4,					/* user_data */
	NULL,					/* COLORS */
	1,					/* id */
	NULL,					/* MsgPort *notifyme */
	NULL,					/* *gad_ptr */
	"4"};				/* *text */

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
	"SMPTE Timecode Jump"};					/* *text */

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
	"SMPTE Timecode Error"};				/* *text */

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
	"SMPTE Time Out"};				/* *text */

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
	if (((struct CueModState *)(state))->PlayFlags&1) button1.flags |=BUTN_HIT;
	else button1.flags &=~BUTN_HIT;
	if (((struct CueModState *)(state))->PlayFlags&2) button2.flags |=BUTN_HIT;
	else button2.flags &=~BUTN_HIT;
	if (((struct CueModState *)(state))->PlayFlags&4) button3.flags |=BUTN_HIT;
	else button3.flags &=~BUTN_HIT;

	ton2.flags &=~BUTN_HIT;
	ton3.flags &=~BUTN_HIT;
	ton4.flags &=~BUTN_HIT;
	ton5.flags &=~BUTN_HIT;
	ton6.flags &=~BUTN_HIT;
	ton7.flags &=~BUTN_HIT;

	switch (((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded)
		{
		case 4:
			ton2.flags |=BUTN_HIT;
		break;
		case 5:
			ton3.flags |=BUTN_HIT;
		break;
		case 6:
			ton4.flags |=BUTN_HIT;
		break;
		case 7:
			ton5.flags |=BUTN_HIT;
		break;
		case 8:
			ton6.flags |=BUTN_HIT;
		break;
		case 9:
			ton7.flags |=BUTN_HIT;
		break;
		}

	SetAPen(Pwind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(Pwind->RPort,10,59);
	Text(Pwind->RPort,"PreLoad", 7);
	
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
	((struct CueModState *)(mod->state))->PlayFlags|=(int)g->user_data;
else
	((struct CueModState *)(mod->state))->PlayFlags&=~(int)g->user_data;

}
/*******************************************************************/
void NumPreFunc(g)
struct button *g;
{
if (g->flags & BUTN_HIT)
	{
	((struct CueModState *)(mod->state))->MaxNumOfSndsPreloaded=(int)g->user_data;
	}
}
