#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "eq.data"

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


static struct Window *EQ_window;

static void WinGadSize();


/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/**************************************************************/
int do_eq_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
	if (class == NEWSIZE || class==ACTIVEWINDOW)
		{
		WinGadSize();
		refresh_gadget_chain(EQ_window,&EQ16000gad);
		}

return(NO_ERROR);
}

/*****************************************************************/

int open_eq_window(m)
struct Module *m;
{
struct Window *OpenWindow();

EQ_NewWindow.Screen = studioBase->screen;

if ((EQ_window = OpenWindow(&EQ_NewWindow))==0)  return(COULD_NOT_OPEN_WINDOW);
m->window = EQ_window;

WinGadSize();
AddGadget(EQ_window,&SIZEgad,-1);

if (create_gadget_chain(EQ_window,&EQ16000gad)==0) return(COULD_NOT_CREATE_GADGETS);

return(NO_ERROR);
}
/*****************************************************************/

int close_eq_window(m)
struct Module *m;
{
struct NewSlider *ns=&EQ16000gad;
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

if (EQ_window)
	{
	delete_gadget_chain(EQ_window);
	EQ_NewWindow.LeftEdge=EQ_window->LeftEdge;
	EQ_NewWindow.TopEdge=EQ_window->TopEdge;
	EQ_NewWindow.Width=EQ_window->Width;
	EQ_NewWindow.Height=EQ_window->Height;
	CloseWindow(EQ_window);
	}

return(NO_ERROR);
}

/**************************************************************/

static void EQ31func()
{
}
static void EQ62func()
{
}
static void EQ125func()
{
}
static void EQ250func()
{
}
static void EQ500func()
{
}
static void EQ1000func()
{
}
static void EQ2000func()
{
}
static void EQ4000func()
{
}
static void EQ8000func()
{
}
static void EQ16000func()
{
}


static void WinGadSize()
{
char frq[] = {"16k","8k ","4k ","2k ","1k ","500","250","125","62 ","31 "};
double Y,Yz,x,xz,xs,z;
int c,end;
struct NewSlider *ns=&EQ16000gad;
struct slider *s;

SetAPen(EQ_window->RPort, BROWN);
RectFill(EQ_window->RPort, 2,10, EQ_window->Width-3, EQ_window->Height-2);

SetAPen(EQ_window->RPort, DARK_BROWN);
RectFill(EQ_window->RPort, EQ_window->Width-2 ,0, EQ_window->Width, EQ_window->Height);
RectFill(EQ_window->RPort, 1 , EQ_window->Height-1, EQ_window->Width, EQ_window->Height);

Y=4+sp*2;
Yz=EQ_window->Height-(sp*2)-3;

xs=(EQ_window->Width-(sp*15))/10;
xz=EQ_window->Width-(sp*2);

x=xz-xs-sp+((xs-23)/2);

SetAPen(EQ_window->RPort, DARK_BROWN);
SetDrMd(EQ_window->RPort,JAM1);
if (xs>24)
	for(z=0;z < 10; z++)
		{
		c=z;
		SetAPen(EQ_window->RPort, DARK_BROWN);
		Move(EQ_window->RPort,(int)x,(int)(EQ_window->Height-(sp)));
		Text(EQ_window->RPort,&frq[c<<2], 3);
		SetAPen(EQ_window->RPort, WHITE);
		Move(EQ_window->RPort,(int)x+1,(int)(EQ_window->Height-(sp)+1));
		Text(EQ_window->RPort,&frq[c<<2], 3);
		x=x-xs-sp;
		}
else Yz=Yz+7;

SetAPen(EQ_window->RPort, DARK_BROWN);
SetDrMd(EQ_window->RPort,JAM1);
for(z=0;z < 17; z++)
	{
	Move(EQ_window->RPort,(int)xz,(int)(Y+((Yz-Y-4)/16.0)*z+2.5));
	Draw(EQ_window->RPort,10,(int)(Y+((Yz-Y-4)/16.0)*z+2.5));
	}

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

SIZEgad.LeftEdge =EQ_window->Width-12;
SIZEgad.TopEdge =EQ_window->Height-6;
SIZEgad.Width =10;
SIZEgad.Height =5;

SetAPen(EQ_window->RPort,DARK_BROWN);
Move(EQ_window->RPort,SIZEgad.LeftEdge,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(EQ_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge);
SetAPen(EQ_window->RPort,WHITE);
Move(EQ_window->RPort,SIZEgad.LeftEdge+2,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(EQ_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge+1);
}

