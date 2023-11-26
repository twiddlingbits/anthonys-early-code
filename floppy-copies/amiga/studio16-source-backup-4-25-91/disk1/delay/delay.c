#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/gadlib.h"
#include "/include/studio16.h"
#include "/include/clr.h"
#include "graphics/view.h"
#include <graphics/gfxmacros.h>
#include "Delay.data"
#include "/include/CardCmds.h"

#define Clock 10000000.0


extern struct studioBase *studioBase;

/*****************************************************************/
int Cutoff[28] =
	{ 33333,22222,20833,16667,13889,13333,11111,10417,9524,8333,
	7407,6944,6667,6068,5952,5556,5208,5128,4762,4630,4444,4167,
	3718,3472,3205,2976,2778,2604};

/*****************************************************************/

struct Window *Delay_window;
static UBYTE	AutoFilt=0;
static int	BuffLen=1,SampR=1;

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
int Delay_do_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
return(NO_ERROR);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/

int Delay_open_window(this_module)
struct Module *this_module;
{
struct Window *OpenWindow();

Delay_NewWindow.Screen = studioBase->screen;
if ((Delay_window = OpenWindow(&Delay_NewWindow))==0) return(/*COULD_NOT_OPEN_WINDOW*/50);
this_module->window=Delay_window;

SetAPen(Delay_window->RPort, BROWN);
RectFill(Delay_window->RPort, 2,10, Delay_window->Width-3, Delay_window->Height-2);
SetAPen(Delay_window->RPort, DARK_BROWN);
RectFill(Delay_window->RPort, Delay_window->Width-2 ,0, Delay_window->Width, Delay_window->Height);
RectFill(Delay_window->RPort, 1 , Delay_window->Height-1, Delay_window->Width, Delay_window->Height);

if ((create_gadget_chain(Delay_window,&RATEgad))==0) 			return(/*COULD_NOT_CREATE_GADGETS*/50);
FREQknob=(struct slider *) FREQgad.gad_ptr;
RATEknob = (struct slider *) RATEgad.gad_ptr;

return(NO_ERROR);
}
/*****************************************************************/

int Delay_close_window(this_module)
struct Module *this_module;
{
/*send_msg(CMD_DELAY_OFF,0, 0, 0, 0, 0,0);*/

if (this_module->window)
	{
	delete_gadget_chain(this_module->window);  /** delete all gadgets in this window */
	ClearMenuStrip(this_module->window);
	CloseWindow(this_module->window);
	}

return(NO_ERROR);
}
/*****************************************************************/


/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
static void GAINfunc(k)
struct slider *k;
{
int x;
char buf[20];
sprintf(buf,"Gain %2d",k->top_element);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

x=(k->top_element)<<4;
/*send_msg(CMD_SET_INPUT_GAIN,x, 0, 0, 0, 0,0);*/
}

/*****************************************************************/
static void MIXfunc(k)
struct slider *k;
{
char buf[20];
sprintf(buf,"Mix%3d%%",(k->top_element*100)/127);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 7);

/*send_msg(CMD_SET_VAR2,(k->top_element)<<8,0,0,0,0,0);
send_msg(CMD_SET_VAR1,(128-k->top_element)<<8,0,0,0,0,0);*/

}

/*****************************************************************/
static void FEEDBACKfunc(k)
struct slider *k;
{
char buf[20];
sprintf(buf,"Feedback%3d%%",(k->top_element*100)/128);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 12);

/*send_msg(CMD_SET_VAR3,(k->top_element)<<8,0,0,0,0,0);*/
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

/*send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[x], 0, 0, 0, 0,0);*/

}
/*****************************************************************/
static void RATEfunc(k)
struct slider *k;
{
static void PrintDtime();
char buf[20];
float	x,y;
int	c;
USHORT	z,v,v2;

x=600-(k->top_element)+60;
y=(Clock/2)/x;
c=y;
SampR=c;
sprintf(buf,"Rate %5d",c);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

z=x;
v=z & 0x00FF;
v2=(z & 0xFF00)>>8;

/*send_msg(CMD_SET_SAMPLING_RATE,c,0,0, 0, 0,0);*/

if (AutoFilt) 
	{
	z=0;
	while(((Cutoff[z]*1.5) > c>>1)&& z!=27)  z++;
	if(FREQknob->top_element!=(27-z))
		{
		FREQknob->top_element=27-z;
/*		send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[27-z], 0, 0, 0, 0,0);*/
		move_knob(FREQknob);
		}
	}
PrintDtime();
}
/*****************************************************************/
static void MONITORfunc(b)
struct button *b;
{
/*
if (b->flags&BUTN_HIT)
	send_msg(CMD_DELAY_ON, 0, 0, 0, 0, 0,0);
else
	send_msg(CMD_DELAY_OFF,0, 0, 0, 0, 0,0);*/
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
/*		send_msg(CMD_SET_CUTOFF_FREQ,Cutoff[27-z], 0, 0, 0, 0,0);*/
		move_knob(FREQknob);
		}
	AutoFilt=1;
	}

else
	AutoFilt=0;

}
/********************************************************************/
static void Bufffunc(k)
struct slider *k;
{
static void PrintDtime();
char buf[20];
sprintf(buf,"Buffer %3d",k->top_element);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+4, k->knob_y+8);
Text(k->kw->RPort, buf, 10);

BuffLen=((k->top_element+1)*0x0020)-1;

/*send_msg(CMD_SET_VAR0,BuffLen,0,0,0,0,0);
send_msg(CMD_SENDCMD,DelayBuff,0,0,0,0,0);*/

PrintDtime();
}

/********************************************************************/
static void PrintDtime()
{
char buf[30];
int	dt,dy;
float f,f1,f2;
f1=BuffLen;
f2=SampR;

f=(f1/f2)*1000.0;
dt=f;
f=f-dt;
f=f*1000;
dy=f;
sprintf(buf,"Delay Time %4d.%03d mS",dt,dy);
SetAPen(Delay_window->RPort, BLACK);
SetBPen(Delay_window->RPort, BROWN);
SetDrMd(Delay_window->RPort, JAM2);
Move(Delay_window->RPort,212,134);
Text(Delay_window->RPort, buf, 22);
}

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
