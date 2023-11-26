#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "/include/gadlib.h"
#include "/include/studio16.h"
#include "/include/clr.h"
#include <hardware/intbits.h>
#include <hardware/cia.h>
#include "graphics/view.h"
#include <graphics/gfxmacros.h>
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include "VU.data"
#include "/include/CardCmds.h"

#define COULD_NOT_OPEN_WINDOW 50
#define COULD_NOT_CREATE_GADGETS 51
static void WinGadSize();

extern struct studioBase *studioBase;
struct Window *VU_window;


static struct VUmeter  {
		struct RastPort *RPort;	/* Windows Raster Port */
		USHORT	x,y;		/* X / Y Coordinates of VU */
		USHORT	x2,y2;		/* X / Y Coordinates of VU */
		USHORT	p,lp;		/* Last Position */
		};

static struct VUmeter VU = {NULL,14,17,383,17+31,0,0};

static void SetupVU();
static void UpdateVU();

//********************************************************************/
/********************************************************************/
/********************************************************************/

int do_VU_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
	if (class == NEWSIZE || class==ACTIVEWINDOW)
		{
		WinGadSize();
		}

return(NO_ERROR);
}

/*****************************************************************/

int open_VU_window(m)
struct Module *m;
{
static void InterlaceAdjust();
struct Window *OpenWindow();

VU_NewWindow.Screen = studioBase->screen;

if ((VU_window = OpenWindow(&VU_NewWindow))==0)  return(COULD_NOT_OPEN_WINDOW);
m->window = VU_window;

VU.RPort=VU_window->RPort;

WinGadSize();
AddGadget(VU_window,&SIZEgad,-1);

/*if (create_gadget_chain(VU_window,&MONITORbutton)==0) return(COULD_NOT_CREATE_GADGETS);*/

send_msg(CMD_HILO_ON, 0, 0, 0, 0,0,0);

return(NO_ERROR);
}
/*****************************************************************/

int close_VU_window(m)
struct Module *m;

{
send_msg(CMD_HILO_OFF, 0, 0, 0, 0,0,0);

/*delete_gadget_chain(VU_window);*/

if (VU_window) 
	{
	VU_NewWindow.LeftEdge=VU_window->LeftEdge;
	VU_NewWindow.TopEdge=VU_window->TopEdge;
	VU_NewWindow.Height=VU_window->Height;
	VU_NewWindow.Width=VU_window->Width;


	CloseWindow(VU_window);
	Disable();
	VU_window=0;
	Enable();
	}
return(NO_ERROR);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/*****************************************************************/

static USHORT	TopX[] = 
	{12,13,14,15,
	16,17,18,19,
	20,21,22,23,24,
	25,26,27,28,29,
	30,31,32,33,34,35,
	36,37,38,39,40,41,42,
	43,44,45,46,47,48,49,50,51,52,53,
	54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
	85,86,87,88,89,90,91,92,93,94,95,
	96,97,98,99,100,101,102,
	103,104,105,106,107,108,
	109,110,111,112,113,
	114,115,116,117,118,
	119,120,121,122,
	123,124,125,126,
	127,128,129,
	130};

static USHORT	TopY[] =
	{14,14,14,14,
	13,13,13,13,
	12,12,12,12,12,
	11,11,11,11,11,
	10,10,10,10,10,10,
	9,9,9,9,9,9,9,
	8,8,8,8,8,8,8,8,8,8,8,
	7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
	8,8,8,8,8,8,8,8,8,8,8,
	9,9,9,9,9,9,9,
	10,10,10,10,10,10,
	11,11,11,11,11,
	12,12,12,12,12,
	13,13,13,13,
	14,14,14,14,
	15,15,15,
	16};

static int lp=0;

static void SetupVU(VU)
struct VUmeter *VU;
{
USHORT z;

SetDrMd(VU->RPort,COMPLEMENT);
VU->lp=0;
if (studioBase->screen->Height > 390) 
	{
	DrawImage(VU->RPort,&VUintIMG,VU->x-1,VU->y);
	Move(VU->RPort,VU->x+TopX[0],VU->y+(TopY[VU->lp])+(TopY[VU->lp]>>1));
	Draw(VU->RPort,VU->x+(TopX[0]>>2)+52,VU->y+57);
	}

else
	{
	DrawImage(VU->RPort,&VUstdIMG,VU->x,VU->y);
	Move(VU->RPort,VU->x+TopX[0],VU->y+TopY[0]);
	Draw(VU->RPort,VU->x+(TopX[0]>>2)+52,VU->y+28);
	}

for( z=0;z<110;z++)
	UpdateVU(VU,z);
for( z=109;z>0;z--)
	UpdateVU(VU,z);
}

/*****************************************************************/
static void UpdateVU(VU,p)
struct VUmeter *VU;
USHORT p;
{
SHORT pp,ad,sr[]= {0,1,1,1,2,2,2,2,2,3,3};

SetDrMd(VU->RPort,COMPLEMENT);
pp=VU->lp;
ad=0;
if (p>pp)
	{
	if (p>pp+8)	ad=((p-pp)>>3);
	if (ad>10)	pp=pp+sr[10];
	else		pp=pp+sr[ad];
	if (pp>118) pp=118;
	}
else
	{
	if (p<pp-8)	ad=((pp-p)>>3);
	if (ad>10)	pp=pp-sr[10];
	else		pp=pp-sr[ad];
	if (pp<0) pp=0;
	}

if (pp!=VU->lp)
	if (studioBase->screen->Height > 390) 
		{
		Move(VU->RPort,VU->x+TopX[VU->lp],VU->y+(TopY[VU->lp])+(TopY[VU->lp]>>1));
		Draw(VU->RPort,VU->x+(TopX[VU->lp]>>2)+52,VU->y+57);
		Move(VU->RPort,VU->x+TopX[pp],VU->y+(TopY[pp])+(TopY[pp]>>1));
		Draw(VU->RPort,VU->x+(TopX[pp]>>2)+52,VU->y+57);
		}
	else 
		{
		Move(VU->RPort,VU->x+TopX[VU->lp],VU->y+TopY[VU->lp]);
		Draw(VU->RPort,VU->x+(TopX[VU->lp]>>2)+52,VU->y+28);
		Move(VU->RPort,VU->x+TopX[pp],VU->y+TopY[pp]);
		Draw(VU->RPort,VU->x+(TopX[pp]>>2)+52,VU->y+28);
		}
SetDrMd(VU->RPort,JAM2);
VU->lp=pp;
}
/********************************************************************/
static void Graph(h,l)
BYTE h,l;
{
struct RastPort r;
CopyMem(VU_window->RPort,&r,sizeof(struct RastPort));
SetAPen(&r, 7);
SetBPen(&r, LIGHT_BROWN);
SetDrMd(&r, JAM2);
Move(&r,VU.x2 ,(h>>1)+VU.y);
Draw(&r,VU.x2 ,(l>>1)+VU.y);
r.Mask=4;
ScrollRaster(&r,1,0,VU.x+149,VU.y,VU.x2,VU.y2);
}
/********************************************************************/

/*****************************************************************/

void UpdateGraph(msg)
struct play_msg *msg;
{
if (VU_window)
	{
	Graph(msg->arg1,msg->arg2);
	UpdateVU(&VU,msg->arg3);
	ReplyMsg(msg);
	}
}

/*****************************************************************/

static void WinGadSize()
{
VU.x2=VU_window->Width-13;
SetAPen(VU_window->RPort, BROWN);
RectFill(VU_window->RPort, 2,10, VU_window->Width-3, VU_window->Height-2);

SetAPen(VU_window->RPort, DARK_BROWN);
RectFill(VU_window->RPort, VU_window->Width-2 ,0, VU_window->Width, VU_window->Height);
RectFill(VU_window->RPort, 1 , VU_window->Height-1, VU_window->Width, VU_window->Height);

draw_indent(VU_window->RPort,VU.x+149-2,VU.y-1,VU.x2+2,VU.y2+1, DARK_BROWN, WHITE);
SetAPen(VU_window->RPort, DARK_BROWN);
Move(VU_window->RPort,VU.x+149,(VU.y+VU.y2)>>1);
Draw(VU_window->RPort,VU.x2,(VU.y+VU.y2)>>1);

SetupVU(&VU);
SIZEgad.LeftEdge =VU_window->Width-12;
SIZEgad.TopEdge =VU_window->Height-6;
SIZEgad.Width =10;
SIZEgad.Height =5;

SetAPen(VU_window->RPort,DARK_BROWN);
Move(VU_window->RPort,SIZEgad.LeftEdge,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(VU_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge);
SetAPen(VU_window->RPort,WHITE);
Move(VU_window->RPort,SIZEgad.LeftEdge+2,SIZEgad.TopEdge+SIZEgad.Height-1);
Draw(VU_window->RPort,SIZEgad.LeftEdge+SIZEgad.Width-1,SIZEgad.TopEdge+1);

}
