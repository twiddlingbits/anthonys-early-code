#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/gadlib.h"
#include "/include/studio16.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "Record.data"
#include "/include/CardCmds.h"

#define Clock 10000000.0

#define COULD_NOT_OPEN_WINDOW 50
#define COULD_NOT_CREATE_GADGETS 51

char TempSampName[120] =  {"NoName",0};

int GlobSampRate;

extern struct studioBase *studioBase;

static UBYTE	AutoFilt=0;

struct Window *Record_window;

/*****************************************************************/
extern int Cutoff[];
/*н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н╖н*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/

int Record_do_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
return(NO_ERROR);
}

/*****************************************************************/

int Record_open_window(m)
struct Module *m;
{
struct Window *OpenWindow();

Record_NewWindow.Screen = studioBase->screen;

if ((Record_window = OpenWindow(&Record_NewWindow))==0)  return(COULD_NOT_OPEN_WINDOW);
m->window = Record_window;

SetAPen(Record_window->RPort, BROWN);

RectFill(Record_window->RPort, 2,10, Record_window->Width-3, Record_window->Height-2);
SetAPen(Record_window->RPort, DARK_BROWN);
RectFill(Record_window->RPort, Record_window->Width-2 ,0, Record_window->Width, Record_window->Height);
RectFill(Record_window->RPort, 1 , Record_window->Height-1, Record_window->Width, Record_window->Height);

if (create_gadget_chain(Record_window,&MONITORbutton)==0) return(COULD_NOT_CREATE_GADGETS);

FREQknob = (struct slider *)FREQgad.gad_ptr;
RATEknob = (struct slider *)RATEgad.gad_ptr;

return(NO_ERROR);
}
/*****************************************************************/

int Record_close_window(m)
struct Module *m;
{
struct NewSlider *ns=&MONITORbutton;
struct slider *s;
int end;

end=1;
while (end)
	{
	if (ns->gad_ptr!=NULL && (ns->flags&GAD_SLIDER))
		{
		s=(struct slider *)ns->gad_ptr;
		ns->top_element=s->top_element;
		ns->gad_ptr=NULL;
		}
	if (ns->next!=NULL) ns=ns->next;
	else end=0;
	}

if (Record_window)
	{
	delete_gadget_chain(Record_window);

	Record_NewWindow.LeftEdge=Record_window->LeftEdge;
	Record_NewWindow.TopEdge=Record_window->TopEdge;
	Record_NewWindow.Height=Record_window->Height;
	Record_NewWindow.Width=Record_window->Width;

	CloseWindow(Record_window);

	Record_window=0;
	}

return(NO_ERROR);
}

/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
/*оооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооооо*/
static void GAINfunc(k)
struct slider *k;
{
char buf[20];
int x;

sprintf(buf,"Gain %2d",k->top_element);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

x=(k->top_element)<<4;

send_msg(CMD_SET_INPUT_GAIN,x, 0, 0, 0, 0,0);
}

/*****************************************************************/
static void FREQfunc(k)
struct slider *k;
{

char buf[20];
short	x;

x=27-k->top_element;

sprintf(buf,"Filter %5d",Cutoff[x]);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);

send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[x], 0, 0, 0, 0,0);

}
/*****************************************************************/
static void RATEfunc(k)
struct slider *k;
{
char buf[20];
float	x,y;
int	c;
USHORT	z;

x=600-(k->top_element)+60;
y=(Clock/2)/x;
c=y;
GlobSampRate=c;
sprintf(buf,"Rate %5d",c);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

send_msg(CMD_SET_SAMPLING_RATE,c,0,0, 0, 0,0);

if (AutoFilt) 
	{
	z=0;
	while(((Cutoff[z]*1.5) > c>>1)&& z!=27)  z++;
	if(FREQknob->top_element!=(27-z))
		{
		FREQknob->top_element=27-z;
		send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[27-z], 0, 0, 0, 0,0);
		move_knob(FREQknob);
		}
	}
}
/*****************************************************************/
static void MONITORfunc(b)
struct button *b;
{

if (b->flags&BUTN_HIT)
	send_msg(CMD_MONITOR_ON,0,0,0,0,0,0);
else
	send_msg(CMD_MONITOR_OFF,0,0,0,0,0,0);
}
/*****************************************************************/
static void RECORDfunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	{
	record_sound(TempSampName, GlobSampRate);
	}
else
	{
	record_sound(TempSampName, GlobSampRate);
	}
}
/*****************************************************************/

static void FILEfunc(b)
struct button *b;
{
FILEbut = b;
newgetstr(TempSampName,"Sample Name?",8*40);
xor_button(b);
}
/*****************************************************************/
static void AUTOFILTfunc(b)
struct button *b;
{
float	x,y;
int	c;
USHORT	z;


if (b->flags&BUTN_HIT)
	{
	x=600-(RATEknob->top_element)+60;
	y=(Clock/2)/x;
	c=y;

	z=0;
	while(((Cutoff[z]*1.5) > c>>1) && z!=27)  z++;
	if(FREQknob->top_element!=(27-z))
		{
		FREQknob->top_element=27-z;
		send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[27-z], 0, 0, 0, 0,0);
		move_knob(FREQknob);
		}
	AutoFilt=1;
	}

else
	AutoFilt=0;

}
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
