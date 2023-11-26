#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"
/**********************************/
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_ABOUT";	/* The name of the task to create         */
long _priority = -10;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct StudioBase *StudioBase=NULL;
struct Library *GadLibBase=NULL;
struct Library *MathBase=NULL;
/**********************************/
/**********************************/
struct Window *wind;
/**************************************************************/
/**************************************************************/
char *name;
void _main(name)
{
void DisplayWind();
void HideWind();
void openlibraries();
void closelibraries();

openlibraries();

StandardEnviroment();

DisplayWind();

Wait(1<<wind->UserPort->mp_SigBit);

HideWind();

closelibraries();
}
/**************************************************************/
/**************************************************************/
struct Gadget OKgad = { NULL,(370>>1)-30,100-26,62,11,GADGHCOMP,GADGIMMEDIATE,BOOLGADGET,
	NULL,NULL,NULL,0,NULL,NULL,NULL };
/**************************************************************/
/**************************************************************/
static struct NewWindow NewWind = {
   (640-370)>>1,
   (200-111)>>1,
   370,
   111,
   BLACK,
   WHITE,
   ACTIVEWINDOW,
   NOCAREREFRESH|BORDERLESS,
   &OKgad,0,
   NULL, /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };
/**************************************************************/
/**************************************************************/
USHORT chip StuClipData[] =
{
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,

0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFF80,0x3FFF,0xFFFF,0x88FF,0xFE3F,0x87FC,
0xFE3F,0x8FFF,0xFFFF,0x88FF,0xFE3C,0x7FFC,
0xFC7F,0xCE3F,0xFFFF,0x8FFF,0xFC38,0xFFFC,
0xFC7F,0xFE3F,0xFFFF,0x8FFF,0xF831,0xFFFC,
0xFC7F,0xF80F,0xFFFF,0x8FFF,0xFE31,0xFFFC,
0xFE1F,0xFE3F,0xFFFF,0x8FFF,0xFE30,0x03FC,
0xFFC3,0xFE38,0xF8F0,0x08F0,0x7E30,0xF1FC,
0xFFF0,0x7E38,0xF8C7,0x88C7,0x1E31,0xF8FC,
0xFFFE,0x1E38,0xF88F,0x888F,0x8E31,0xF8FC,
0xFFFF,0x8E38,0xF88F,0x888F,0x8E31,0xF8FC,
0xFFFF,0x8E38,0xF88F,0x888F,0x8E31,0xF8FC,
0xFCFF,0x8E38,0xF88F,0x888F,0x8E31,0xF8FC,
0xFC7F,0x1E3C,0x78C7,0x88C7,0x1E38,0xF1FC,
0xFF00,0x7F8F,0x00F0,0x08F0,0x7E3C,0x03FC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,
0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFF,0xFFFC,

0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x007F,0xC000,0x0000,0x7700,0x01C0,0x7800,
0x01C0,0x7000,0x0000,0x7700,0x01C3,0x8000,
0x0380,0x31C0,0x0000,0x7000,0x03C7,0x0000,
0x0380,0x01C0,0x0000,0x7000,0x07CE,0x0000,
0x0380,0x07F0,0x0000,0x7000,0x01CE,0x0000,
0x01E0,0x01C0,0x0000,0x7000,0x01CF,0xFC00,
0x003C,0x01C7,0x070F,0xF70F,0x81CF,0x0E00,
0x000F,0x81C7,0x0738,0x7738,0xE1CE,0x0700,
0x0001,0xE1C7,0x0770,0x7770,0x71CE,0x0700,
0x0000,0x71C7,0x0770,0x7770,0x71CE,0x0700,
0x0000,0x71C7,0x0770,0x7770,0x71CE,0x0700,
0x0300,0x71C7,0x0770,0x7770,0x71CE,0x0700,
0x0380,0xE1C3,0x8738,0x7738,0xE1C7,0x0E00,
0x00FF,0x8070,0xFF0F,0xF70F,0x81C3,0xFC00,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
};
struct Image StuClip =
{
0,0,          /* LeftEdge, TopEdge */
94,20,3,      /* Width, Height, Depth */
&StuClipData[0],
31,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};
/**************************************************************/
/**************************************************************/
void openlibraries()
{
void closelibraries();

if ((StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0))==NULL)
	{exit(10);}

if ((GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0))==NULL)
	{closelibraries();exit(10);}

if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{closelibraries();exit(10);}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{closelibraries();exit(10);}

if ((MathBase=(struct Library *)OpenLibrary("mathieeedoubtrans.library",0))==NULL)
	telluser("File 'libs:mathieeedoubtrans.library' does not exist","Reinstall WorkBench onto your hard drive.",NULL);
else CloseLibrary(MathBase);
}
/**************************************************************/
/**************************************************************/
void closelibraries()
{
if (IntuitionBase) CloseLibrary(IntuitionBase);
if (GadLibBase)	CloseLibrary(GadLibBase);
if (GfxBase)	CloseLibrary(GfxBase);
if (StudioBase)	CloseLibrary(StudioBase);
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
struct Window *OpenWindow();
NewWind.Screen = StudioBase->screen;
ScreenToFront(StudioBase->screen);
MoveScreen(StudioBase->screen,0,-1000);
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
if ((wind = OpenWindow(&NewWind))!=0)  
	{
	SetDrMd(wind->RPort,JAM1);
	SetAPen(wind->RPort,2);
	RectFill(wind->RPort, 2,1, wind->Width-3, wind->Height-2);

	SetAPen(wind->RPort,1);
	RectFill(wind->RPort, 0 ,0, 1, wind->Height);
	RectFill(wind->RPort, 0 ,0, wind->Width,0);

	SetAPen(wind->RPort,6);
	RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
	RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

	DrawImage(wind->RPort,&StuClip,(wind->Width-94)>>1,2);
	SetAPen(wind->RPort,7);
	RectFill(wind->RPort, 4,2,((wind->Width-94)>>1)-1,10);
	RectFill(wind->RPort, ((wind->Width-94)>>1)+94,2,wind->Width-5-16-3,10);
	RectFill(wind->RPort, 23,wind->Height-11,wind->Width-5,wind->Height-3);

	RectFill(wind->RPort, 4,12,20,wind->Height-3);
	RectFill(wind->RPort, wind->Width-5-16,2,wind->Width-5,wind->Height-13);

	SetAPen(wind->RPort,5);
	Move(wind->RPort,((wind->Width-(9*8))>>1),30);
	Text(wind->RPort,"Vers 1.00", 9);
	Move(wind->RPort,(wind->Width-(40*8))>>1,45);
	Text(wind->RPort,"� Copyright 1991,1992 SunRize Industries", 40);
	Move(wind->RPort,(wind->Width-(12*8))>>1,wind->Height-51);
	Text(wind->RPort,"Software By:",12);
	Move(wind->RPort,(wind->Width-(13*8))>>1,wind->Height-42);
	Text(wind->RPort,"Todd Modjeski",13);
	Move(wind->RPort,(wind->Width-(12*8))>>1,wind->Height-33);
	Text(wind->RPort,"Anthony Wood",12);

	draw_indent(wind->RPort,(wind->Width>>1)-30,wind->Height-25,(wind->Width>>1)+30,wind->Height-15,1,6);
	Move(wind->RPort,(wind->Width-(2*8))>>1,wind->Height-17);
	Text(wind->RPort,"OK",2);
	}
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
if (wind)
	{
	CloseWindow(wind);
	}
}
/**************************************************************/
/**************************************************************/
