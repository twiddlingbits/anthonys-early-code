#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

/** I found a bug that causes WB1.3 to do wierd things if your gadget
    width allows strings bigger than MaxChars **/

#define SHI 30

void newgetstr(string,title,width)
int width;
UBYTE *title,*string;

{
extern struct StudioBase *StudioBase;
struct Window *Window;
struct Gadget g;
struct StringInfo s;
struct IntuiMessage *message,*GetMsg();
UBYTE undo[100];
int x;
void CleanString();   
static struct NewWindow NewWindow = {
	113,52,   /* window XY origin relative to TopLeft of screen */
	370,103,	/* window width and height */
	BLACK,WHITE,	/* detail and block pens */
	GADGETUP,	/* IDCMP flags */
	ACTIVATE+NOCAREREFRESH+BORDERLESS+SMART_REFRESH,	/* other window flags */
	NULL,	/* first gadget in gadget list */
	NULL,	/* custom CHECKMARK imagery */
	NULL,	/* window title */
	NULL,	/* custom screen pointer */
	NULL,	/* custom bitmap */
	5,5,	/* minimum width and height */
	640,200,	/* maximum width and height */
	CUSTOMSCREEN	/* destination screen type */
};

/* add gadgets */

g.GadgetType=REQGADGET|STRGADGET;
g.Flags=GADGHCOMP;
g.Activation=ENDGADGET|RELVERIFY;
g.GadgetRender=NULL;
g.SelectRender=NULL;
g.GadgetText=NULL;
g.NextGadget=NULL;
g.TopEdge=SHI-15+2;
g.Width=width-40;
if (g.Width > 312)   /** 39 X 8 = 304 (40 chars max & 8 pix per char) **/
	g.Width = 312;
g.LeftEdge=(width-g.Width)/2;
g.Height=11;
g.SpecialInfo=(APTR)&s;

s.Buffer=string;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=width/8;

NewWindow.LeftEdge=(640-width)/2;
NewWindow.TopEdge=50;
NewWindow.Width=width;
NewWindow.Height=SHI;
NewWindow.Screen=StudioBase->screen;
NewWindow.FirstGadget=&g;
Window = (struct Window *)OpenWindow(&NewWindow);
if (Window) {

	SetDrMd(Window->RPort,JAM1);
	SetAPen(Window->RPort,StudioBase->defaults.colors.gadget_window.background);
	RectFill(Window->RPort, 2,1, Window->Width-3, Window->Height-2);
	draw_indent(Window->RPort,0,0,Window->Width-1,11,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	draw_indent(Window->RPort,0,12,Window->Width-1,Window->Height-1,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	SetAPen(Window->RPort,StudioBase->defaults.colors.gadget_window.draw_selected);
	Move(Window->RPort,(Window->Width/2)-(stclen(title)*4),8);
	Text(Window->RPort,title,stclen(title));
	draw_indent(Window->RPort,g.LeftEdge-2,g.TopEdge-1,g.LeftEdge+g.Width+1,g.TopEdge+8,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	RefreshGList(&g,Window,NULL,1);
	for(x=0;x<20;x++) WaitTOF();
	ActivateGadget(&g,Window,NULL);

	again:
	Wait(1<<Window->UserPort->mp_SigBit);
	while(message=GetMsg(Window->UserPort)) 
		{
		if (message->Class==GADGETUP) 
			{
			ReplyMsg(message);
			CleanString(string);
			string[39]=0;
			CloseWindow(Window);
			return;
			}
		ReplyMsg(message);
		}
	goto again;
	}
}

void CleanString(string)
char *string;
{
int x=strlen(string);
int y,z;

if (x==0) return;

if (string[0]==' ')
	{
	y=0;
	while(string[y++]==' ') ;
	for(z=0;z<x-y+2;z++)
		string[z]=string[z+y-1];
	x=strlen(string);
	}
if (x==0) return;
if (string[x-1]==' ')
	{
	y=x-1;
	while(string[y--]==' ') ;
	string[y+2]=0;
	}
ParseStringForIllegalChars(string,NULL);
}
