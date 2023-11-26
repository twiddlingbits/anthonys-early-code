#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "mix.data"
int temp;
#define sp 5.0
#define Xsz 15
#define Ysz 100
#define Xs 20
#define Ys 20
#define COULD_NOT_OPEN_WINDOW 50
#define COULD_NOT_CREATE_GADGETS 51

/**********************************/
/*** Test program for gadlib.c ****/
/**********************************/

extern struct studioBase *studioBase;

static struct Window *MIX_window;

static void WinGadSize();

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/**************************************************************/
int do_MIX_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
	if (class == NEWSIZE || class==ACTIVEWINDOW)
		{
		WinGadSize();
		refresh_gadget_chain(MIX_window,&MIXmongad)==0;
		}

return(NO_ERROR);
}

/*****************************************************************/

int open_MIX_window(m)
struct Module *m;
{
static void InterlaceAdjust();
struct Window *OpenWindow();

MIX_NewWindow.Screen = studioBase->screen;

if ((MIX_window = OpenWindow(&MIX_NewWindow))==0)  return(COULD_NOT_OPEN_WINDOW);
m->window = MIX_window;

WinGadSize();
AddGadget(MIX_window,&SIZEgad,-1);


if (create_gadget_chain(MIX_window,&MIXmongad)==0) return(COULD_NOT_CREATE_GADGETS);

return(NO_ERROR);
}
/*****************************************************************/

int close_MIX_window(m)
struct Module *m;

{
struct NewSlider *ns=&MIXmongad;
struct slider *s;
int end;

end=1;
while (end)
	{
	if (ns->gad_ptr!=NULL)
		{
		s=(struct slider *)ns->gad_ptr;
		ns->top_element=s->top_element;
		ns->gad_ptr=NULL;
		}
	if (ns->next!=NULL) ns=ns->next;
	else end=0;
	}

if (MIX_window)
	{
	delete_gadget_chain(MIX_window);
	MIX_NewWindow.LeftEdge=MIX_window->LeftEdge;
	MIX_NewWindow.TopEdge=MIX_window->TopEdge;
	MIX_NewWindow.Width=MIX_window->Width;
	MIX_NewWindow.Height=MIX_window->Height;
	CloseWindow(MIX_window);
	}

return(NO_ERROR);
}

/**************************************************************/

static void WinGadSize()
{
char frq[] = {"MON","CH3","CH2","CH1","CH0"};
double Y,Yz,x,xz,xs,z;
int c,end;
struct NewSlider *ns=&MIXmongad;
struct slider *s;

SetAPen(MIX_window->RPort, BROWN);
RectFill(MIX_window->RPort, 2,10, MIX_window->Width-3, MIX_window->Height-2);

SetAPen(MIX_window->RPort, DARK_BROWN);
RectFill(MIX_window->RPort, MIX_window->Width-2 ,0, MIX_window->Width, MIX_window->Height);
RectFill(MIX_window->RPort, 1 , MIX_window->Height-1, MIX_window->Width, MIX_window->Height);

Y=4+sp*2;
Yz=MIX_window->Height-(sp*2)-3;

xs=(MIX_window->Width-(sp*10))/5;
xz=MIX_window->Width-(sp*2);


SetAPen(MIX_window->RPort, DARK_BROWN);
SetDrMd(MIX_window->RPort,JAM1);

x=xz-xs-sp+((xs-23)/2);

if (xs>24)
	for(z=0;z < 5; z++)
		{
		c=z;
		SetAPen(MIX_window->RPort, DARK_BROWN);
		Move(MIX_window->RPort,(int)x,(int)(MIX_window->Height-(sp)));
		Text(MIX_window->RPort,&frq[c<<2], 3);
		SetAPen(MIX_window->RPort, WHITE);
		Move(MIX_window->RPort,(int)x+1,(int)(MIX_window->Height-(sp)+1));
		Text(MIX_window->RPort,&frq[c<<2], 3);
		x=x-xs-sp;
		}
else Yz=Yz+7;

end=1;
while (end)
	{
	if (ns->gad_ptr==NULL)
		{
		ns->box_y1=Y;
		ns->box_y2=Yz;

		x=xz-sp;
		xz=x-xs;

		ns->box_x1=xz;
		ns->box_x2=x;
		}
	else
		{
		s=(struct slider *)ns->gad_ptr;
		ns->box_y1=Y;
		ns->box_y2=Yz;
		s->box_y1=Y;
		s->box_y2=Yz;

		x=xz-sp;
		xz=x-xs;

		ns->box_x1=xz;
		ns->box_x2=x;
		s->box_x1=xz;
		s->box_x2=x;

if (s->flags&BORDER_3D) {
   s->box_x1 += 2;
   s->box_x2 -= 2;
   s->box_y1 += 1;
   s->box_y2 -= 1;
   }

calc_knob_size(s);

if (!(s->flags&AUTO_KNOB)) {
   if (ns->knob_width!=-1) 
      s->knob_width = ns->knob_width;
	if (ns->knob_height!=-1)
		s->knob_height = ns->knob_height;
	}
calc_knob_xy(s);


		}

	if (ns->next!=NULL) ns=ns->next;
	else end=0;
	}

SIZEgad.LeftEdge =MIX_window->Width-12;
SIZEgad.TopEdge =MIX_window->Height-6;
SIZEgad.Width =10;
SIZEgad.Height =5;

SetAPen(MIX_window->RPort,DARK_BROWN);
Move(MIX_window->RPort,SIZEgad.LeftEdge,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(MIX_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge);
SetAPen(MIX_window->RPort,WHITE);
Move(MIX_window->RPort,SIZEgad.LeftEdge+2,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(MIX_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge+1);

}
/**************************************************************/
static void MixKnobAdj();

static void MIX0func(k)
struct slider *k;
{
MixKnobAdj(k,0);
}
/**************************************************************/

static void MIX1func(k)
struct slider *k;
{
MixKnobAdj(k,1);
}
/**************************************************************/
static void MIX2func(k)
struct slider *k;
{
MixKnobAdj(k,2);
}
/**************************************************************/
static void MIX3func(k)
struct slider *k;
{

MixKnobAdj(k,3);
}
/**************************************************************/
static void MIXmonfunc(k)
struct slider *k;
{
MixKnobAdj(k,4);
}
/**************************************************************/

static void MixKnobAdj(k,m)
struct slider *k;
int m;
{
char buf[20];
int x,w,y=1;
float z,v;
int c,n;
n=150-k->top_element;
if (n>100) n=100+((n-100)<<1);
z=65535.0/200.0;
v=n;
v=v*z;
c=v;
temp=c;
send_msg(CMD_SET_CHANNEL_VOLUME,m, c, 0, 0, 0,0);

x=n;

w=(k->knob_width-4)>>3;

y=0;
if (w>4) w=4;
if ((w==2 || w==1))
	if (x<101) x=n*10;
	else	y=4;

sprintf(buf,"%03d%%¤¤",x);
SetAPen(k->kw->RPort, BLACK);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);

Move(k->kw->RPort, k->knob_x+((k->knob_width-w*8)>>1), k->knob_y+8);
Text(k->kw->RPort, &buf[y],w);

}
/**************************************************************/
