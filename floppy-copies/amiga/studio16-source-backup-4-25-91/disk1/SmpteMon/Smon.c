#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "Smon.data"

#define COULD_NOT_OPEN_WINDOW 50
#define COULD_NOT_CREATE_GADGETS 51

/**********************************/
/*** Test program for gadlib.c ****/
/**********************************/
void wdraw_digits();

extern struct studioBase *studioBase;


struct Window *SMON_window;

static void WindGadSize();


/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/**************************************************************/
int do_SMON_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
	if (class == NEWSIZE || class==ACTIVEWINDOW)
		{
		WindGadSize();
		}

return(NO_ERROR);
}

/*****************************************************************/

int open_SMON_window(m)
struct Module *m;
{
static void InterlaceAdjust();
struct Window *OpenWindow();

SMON_NewWindow.Screen = studioBase->screen;

if ((SMON_window = OpenWindow(&SMON_NewWindow))==0)  return(COULD_NOT_OPEN_WINDOW);
m->window = SMON_window;

WindGadSize();
AddGadget(SMON_window,&SIZEgad,-1);
/*
if (create_gadget_chain(SMON_window,&SMON16000gad)==0) return(COULD_NOT_CREATE_GADGETS);
*/

send_msg(CMD_SMPTE_ON, 0, 0, 0, 0,0,0);

return(NO_ERROR);
}
/*****************************************************************/

int close_SMON_window(m)
struct Module *m;

{
/*delete_gadget_chain(SMON_window);*/
send_msg(CMD_SMPTE_OFF, 0, 0, 0, 0,0,0);

if (SMON_window)
	{
	Disable();

	SMON_NewWindow.LeftEdge=SMON_window->LeftEdge;
	SMON_NewWindow.TopEdge=SMON_window->TopEdge;
	SMON_NewWindow.Height=SMON_window->Height;
	SMON_NewWindow.Width=SMON_window->Width;

	CloseWindow(SMON_window);

	SMON_window=0;
	Enable();
	}

return(NO_ERROR);
}

/**************************************************************/
/**************************************************************/

/**************************************************************/
/**************************************************************/
int DigX[12];

static void WindGadSize()
{
short x;
float l,cw;
int cellw,cellh;

cellh = SMON_window->Height-25;
cw = (SMON_window->Width-32)/12.0;
cellw=cw;

l = 16;
for(x=0;x<12;x++)
	{
	DigX[x]=l;
	l=l+cw;
	if (x==0 || x==3 || x==6 || x==9)
		l=l+(cw/4);
	}

SetAPen(SMON_window->RPort, BROWN);
RectFill(SMON_window->RPort, 2,10, SMON_window->Width-3, SMON_window->Height-2);

SetAPen(SMON_window->RPort, DARK_BROWN);
RectFill(SMON_window->RPort, SMON_window->Width-2 ,0, SMON_window->Width, SMON_window->Height);
RectFill(SMON_window->RPort, 1 , SMON_window->Height-1, SMON_window->Width, SMON_window->Height);

/*draw_indent(SMON_window->RPort,SMON_window->Width-9,SMON_window->Height-5,SMON_window->Width-5,SMON_window->Height-3, WHITE, DARK_BROWN);*/
draw_indent(SMON_window->RPort,10,14,SMON_window->Width-11,SMON_window->Height-6, DARK_BROWN, WHITE);

new_big_digit_size(cellh,cellw);
if (SMON_window!=0) wdraw_digits();

SIZEgad.LeftEdge =SMON_window->Width-12;
SIZEgad.TopEdge =SMON_window->Height-6;
SIZEgad.Width =10;
SIZEgad.Height =5;

SetAPen(SMON_window->RPort,DARK_BROWN);
Move(SMON_window->RPort,SIZEgad.LeftEdge,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(SMON_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge);
SetAPen(SMON_window->RPort,WHITE);
Move(SMON_window->RPort,SIZEgad.LeftEdge+2,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(SMON_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge+1);

}

/**************************************************************/
/**************************************************************/
 int ots,oos,otf,oof;
 int oth,ooh,otm,oom;

void SmpteDataHM(x)
USHORT x;
{
int th,oh,tm,om;
if (SMON_window!=0)
	{
	if (oth!=(th=(x & 0xC000)>>14))
		{
		big_digit(SMON_window->RPort,th,oth, DigX[0], 17);
		oth=th;
		}
	if (ooh!=(oh=(x & 0x2C00)>>10))
		{
		big_digit(SMON_window->RPort,oh,ooh, DigX[1], 17);
		ooh=oh;
		}
	if (otm!=(tm=(x & 0x0280)>>7))
		{
		big_digit(SMON_window->RPort,tm,otm, DigX[3], 17);
		otm=tm;
		}
	if (oom!=(om=(x & 0x0078)>>3))
		{
		big_digit(SMON_window->RPort,om,oom, DigX[4], 17);
		oom=om;
		}
	}
}

/**************************************************************/
void SmpteDataSF(x)
USHORT x;
{
int ts,os,tf,of;
if (SMON_window!=0)
	{
	if (ots!=(ts=(x & 0xE000)>>13))
		{
		big_digit(SMON_window->RPort,ts,ots, DigX[6], 17);
		ots=ts;
		}
	if (oos!=(os=(x & 0x1E00)>>9))
		{
		big_digit(SMON_window->RPort,os,oos, DigX[7], 17);
		oos=os;
		}
	if (otf!=(tf=(x & 0x0180)>>7))
		{
		big_digit(SMON_window->RPort,tf,otf, DigX[9], 17);
		otf=tf;
		}
	if (oof!=(of=(x & 0x0078)>>3))
		{
		big_digit(SMON_window->RPort,of,oof, DigX[10], 17);
		oof=of;
		}
	}
}

/**************************************************************/
/**************************************************************/

void wdraw_digits()
{
int l;
l = 15;

big_digit(SMON_window->RPort,oth,10,DigX[0], 17);
big_digit(SMON_window->RPort,ooh,10,DigX[1], 17);
big_digit(SMON_window->RPort,10,0,DigX[2], 17);

big_digit(SMON_window->RPort,otm,10,DigX[3], 17);
big_digit(SMON_window->RPort,oom,10,DigX[4], 17);
big_digit(SMON_window->RPort,10,0,DigX[5],17);

big_digit(SMON_window->RPort,ots,10,DigX[6], 17);
big_digit(SMON_window->RPort,oos,10,DigX[7], 17);
big_digit(SMON_window->RPort,10,0,DigX[8],17);

big_digit(SMON_window->RPort,otf,10,DigX[9], 17);
big_digit(SMON_window->RPort,oof,10,DigX[10],17);
}
/**************************************************************/
/**************************************************************/


/**************************************************************/
/**************************************************************/


/**************************************************************/
/**************************************************************/
