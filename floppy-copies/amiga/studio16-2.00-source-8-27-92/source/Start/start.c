#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuitionbase.h"
#include "/include/studio16.h"
/**********************************/
/**********************************/
/* Declarations for CBACK */
long _stack = 6000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_STUDIO16";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
/**********************************/
struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct StudioBase *StudioBase=NULL;
struct Library *GadLibBase=NULL;
struct Library *MathBase=NULL;
struct Library *InterfaceBase=NULL;
/**********************************/
/**********************************/
struct Window *wind=NULL,*bdwind=NULL,*MUwind=NULL;
struct Screen *screen=NULL;
BOOL NOT_MY_SCREEN=FALSE;
/**************************************************************/

static struct TextAttr MyFont = {"topaz.font",TOPAZ_EIGHTY,FS_NORMAL,FPF_ROMFONT };
/**************************************************************/

/**************************************************************/

/**************************************************************/

/**************************************************************/
/**************************************************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/**************
#define BLK	0
#define LITE	1
#define MEAD	2
#define DARK	3
#define BLUE	4
#define PURP	5
#define RED	6
#define YELL	7

#define LISTCOLORS {BLK,DARK,LITE,MEAD,MEAD,BLUE,RED}
#define KNOBCOLORS {BLK,LITE,DARK,MEAD,MEAD,BLUE,BLUE}
#define BUTNCOLORS {BLK,LITE,DARK,MEAD,MEAD,BLUE,RED}
#define DROPCOLORS {BLK,LITE,DARK,MEAD,MEAD,BLUE,RED}
#define WINDCOLORS {BLK,LITE,DARK,MEAD,BLUE,LITE,YELL}

static struct GadgetColors defcolors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	DROPCOLORS,

	WINDCOLORS};*************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void OpenStartupWindow();
struct Screen *OpenStudio16Screen();
void OpenStudio16BackdropWindow();
void OpenStudio16MenuWindow();
void InitStudio16Prgm();
void Quit();
/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(name)
char *name;
{
void openlibraries();
void closelibraries();
struct IntuiMessage *message;
int mask;
BOOL flg=TRUE;
openlibraries();

if (StudioBase->LibNode.lib_OpenCnt==2)  /** about+interface.library **/
	{
	OpenStartupWindow();
	screen=OpenStudio16Screen(name);
	if(NOT_MY_SCREEN==FALSE) OpenStudio16BackdropWindow();
	else OpenStudio16MenuWindow();
	InitStudio16Prgm();

	SetTaskPri(FindTask(NULL),-3);
	new_pointer(PTR_NORMAL);
	if (wind) {CloseWindow(wind);wind=NULL;}

	StudioBase->quit_sigbit=AllocSignal(-1);
	if (-1!=StudioBase->quit_sigbit) StudioBase->quit_task=(struct Task *)FindTask(NULL);

	if(NOT_MY_SCREEN==FALSE) 
		while (flg==TRUE)
			{
			mask=Wait(1<<bdwind->UserPort->mp_SigBit|1<<StudioBase->quit_sigbit);
			if (mask & (1<<bdwind->UserPort->mp_SigBit))
				{
				while ((message=(struct IntuiMessage *)GetMsg(bdwind->UserPort))!=0)
					{
					HandleIntuitMess(bdwind,message);
					ReplyMsg(message);
					}
				}
			else flg=FALSE;
			}
	else
		while (flg==TRUE)
			{
			mask=Wait(1<<MUwind->UserPort->mp_SigBit|1<<StudioBase->quit_sigbit);
			if (mask & (1<<MUwind->UserPort->mp_SigBit))
				{
				while ((message=(struct IntuiMessage *)GetMsg(MUwind->UserPort))!=0)
					{
					HandleIntuitMess(MUwind,message);
					ReplyMsg(message);
					}
				}
			else flg=FALSE;
			}


	while (StudioBase->LibNode.lib_OpenCnt!=2) WaitTOF();
	FreeSignal(StudioBase->quit_sigbit);
	}
else
	{
	ScreenToFront(StudioBase->screen);
	MoveScreen(StudioBase->screen,0,-1000);
	}

/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* MAKE AN INTERFACELIB CALL TO SET DEFAULT COLORS THAT CALLS **/
/** THE APPROPRIATE GADLIB CALL (BELOW) + ADD A SIMILAR **/
/** STUDIO16LIB CALL.  OR JUST TELL DAVE TO MAKE THIS SAME **/
/** CALL WHEN HE CHANGES STUDIODEFAUT COLORS **/
/*StudioBase->defaults.colors = defcolors;*/
/*set_default_colors(&defcolors);	*//** GADLIB CALL **/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
Quit();
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void openlibraries()
{
void closelibraries();
int error;

if ((GadLibBase=(struct Library *)OpenLibrary("gadlib.library",0))==NULL)
	{exit(10);}

if ((StudioBase=(struct StudioBase *)OpenLibrary("studio.library",0))==NULL)
	{closelibraries();exit(10);}

if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{closelibraries();exit(10);}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{closelibraries();exit(10);}

if ((error=InitStudio16UserEnv(ISUE_DO_ASSIGNS))!=NO_ERROR)
	{
	if (error==STDERR_NO_STUDIO16_ASSIGN)
		telluser("Studio16:","No Studio16: assign exists!",0);
	else if (error==STDERR_BAD_STUDIO16_ASSIGN)
		telluser("Studio16:","Studio16: does not point to correct Studio16 directory!",0);
	else
		telluser("Studio16:","Unknown error trying to define Studio16:",0);
	closelibraries();
	exit(10);
	}

if ((InterfaceBase=(struct Library *)OpenLibrary("Interface.library",0))==NULL)
	{closelibraries();exit(10);}

if ((MathBase=(struct Library *)OpenLibrary("mathieeedoubtrans.library",0))==NULL)
	telluser("File 'libs:mathieeedoubtrans.library' does not exist","Reinstall WorkBench onto your hard drive.",NULL);
else CloseLibrary(MathBase);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void closelibraries()
{
if (IntuitionBase) CloseLibrary(IntuitionBase);
if (GfxBase)		CloseLibrary(GfxBase);
if (InterfaceBase)	CloseLibrary(InterfaceBase);
if (StudioBase)	CloseLibrary(StudioBase);
if (GadLibBase)	CloseLibrary(GadLibBase);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void OpenStartupWindow()
{
char str[80];
struct NewWindow NewWind = {
  (640-440)>>1,
   (200-70)>>1,
   440,
   70,
   BLACK,
   WHITE,
   NULL,
   ACTIVATE|NOCAREREFRESH|BORDERLESS,
   0,0,
   NULL, /** TITLE **/
   0,0,
   100,34,1024,1024,
   WBENCHSCREEN
   };
int Line=0,len,height;

wind=(struct Window *)OpenWindow(&NewWind);
if (wind)
	{
	SetDrMd(wind->RPort,JAM1);
	SetAPen(wind->RPort,3);
	RectFill(wind->RPort, 2,1, wind->Width-3, wind->Height-2);

	SetAPen(wind->RPort,1);
	RectFill(wind->RPort, 0 ,0, 1, wind->Height);
	RectFill(wind->RPort, 0 ,0, wind->Width,0);

	SetAPen(wind->RPort,2);
	RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
	RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

	draw_indent(wind->RPort,0,0,wind->Width-1,wind->Height-1,2,1);
	draw_indent(wind->RPort,2,1,wind->Width-3,wind->Height-2,1,2);

	SetAPen(wind->RPort,2);

	height=wind->IFont->tf_YSize+2;

	Line++;
	sprintf(str,"Studio 16 %s",VERSION_STRING);
	len=TextLength(wind->RPort,str,strlen(str));
	Move(wind->RPort,(wind->Width-len)>>1,Line*height+2);
	Text(wind->RPort,str,strlen(str));

	Line++;
	sprintf(str,"Copyright 1991,1992 SunRize Industries");
	len=TextLength(wind->RPort,str,strlen(str));
	Move(wind->RPort,(wind->Width-len)>>1,Line*height+2);
	Text(wind->RPort,str,strlen(str));

	sprintf(str,"Loading...");
	len=TextLength(wind->RPort,str,strlen(str));
	Move(wind->RPort,(wind->Width-len)>>1,wind->Height-8);
	Text(wind->RPort,str,strlen(str));
	}
else
	{
	telluser("Studio16 Startup Error:","Could Not Open the Startup Window!",NULL);
	Quit();
	}
}
/**************************************************************/
/**************************************************************/
char sstr[80];
/**************************************************************/
struct Screen *OpenStudio16Screen(name)
char *name;
{
struct NewScreen NewScreenStructure = {
   0,0,   /* screen XY origin relative to View */
   0,STDSCREENHEIGHT,   /* screen width and height */
   3,   /* screen depth (number of bitplanes) */
   6,1,   /* detail and block pens */
   HIRES,   /* display modes for this screen */
   CUSTOMSCREEN|SCREENBEHIND,   /* screen type */
   &MyFont,   /* pointer to default screen font */
   NULL,
   NULL,   /* first in list of custom screen gadgets */
   NULL   /* pointer to custom BitMap structure */
};
struct Screen *scrn;
char scrn_name[100];
int len,x;

LoadDefaults(NULL);

len=strlen(name);
for (x=0;x<len && (strnicmp(&"screen",&name[x],6)) ;x++) ;

if (x+8<len)
	{ 
	strcpy(scrn_name,&name[x+7]);
	len=strlen(scrn_name);
	if (len>1)
		{
		scrn_name[len-1]=NULL;
		len=strlen(scrn_name);
		scrn=IntuitionBase->FirstScreen;
		while (scrn)
			{
			if (!strnicmp(scrn_name,scrn->DefaultTitle,len)) {NOT_MY_SCREEN=TRUE;return(scrn);}
			scrn=scrn->NextScreen;
			}
		}
	}

NewScreenStructure.Width = StudioBase->defaults.screen_width;
NewScreenStructure.ViewModes = StudioBase->defaults.screen_viewmodes;

sprintf(sstr,"Studio 16 © 1991,1992 SunRize Industries %s",VERSION_STRING);
NewScreenStructure.DefaultTitle=sstr;

if ((scrn=(struct Screen *)OpenScreen(&NewScreenStructure)))
	{
	LoadRGB4(&scrn->ViewPort,StudioBase->defaults.palette,8);
	}
else
	{
	telluser("Studio16 Startup Error:","Could Not Open the Screen!",NULL);
	Quit();
	}
return(scrn);
}
/**************************************************************/
/**************************************************************/
struct Menu BlankProj[] = 
		{
		{ &BlankProj[1], 3, 0, 80, 0, MENUENABLED, " Project ", NULL/*&Items[0]*/ },
		{ NULL, 81, 0, 15*8, 0, MENUENABLED, " Applications ", NULL },
		};
struct WindowParms wp;
/**************************************************************/
/**************************************************************/
/**************************************************************/
void OpenStudio16MenuWindow()
{
struct NewWindow NewWind = {
   15,
   15,
   250,
   20,
   6,/*CLR*/
   1,/*CLR*/
   MENUPICK|RAWKEY,
   WINDOWSIZING|ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH,
   0,0,
   "Click Here for Menus", /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };
NewWind.Screen = screen;

MUwind = (struct Window *)OpenWindow(&NewWind);
MUwind->UserData=(BYTE *)&wp;

}
/**************************************************************/
/**************************************************************/
void OpenStudio16BackdropWindow()
{
struct NewWindow NewWindow = {
   0,
   11,
   0,
   0,
   6,/*CLR*/
   1,/*CLR*/
   MENUPICK|RAWKEY,
   ACTIVATE|BORDERLESS|BACKDROP|NOCAREREFRESH,/* REPORTMOUSE|ACTIVATE|NOCAREREFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,*/
   0,0,
   0, /** TITLE **/
   0,0,
   40,20,640,200,
   CUSTOMSCREEN
   };

NewWindow.Screen = screen;
NewWindow.Width = screen->Width;
NewWindow.Height = screen->Height - NewWindow.TopEdge;

if ((bdwind =(struct Window *)OpenWindow(&NewWindow)))
	{
	SetAPen(bdwind->RPort, 0);
	RectFill(bdwind->RPort, 0, 0, bdwind->Width-1, bdwind->Height-1);

	bdwind->UserData=(BYTE *)&wp;
	}
else
	{
	telluser("Studio16 Startup Error:","Could Not Open the Backdrop Window!",NULL);
	Quit();
	}
}
/**************************************************************/
/**************************************************************/
struct MenuItem *BuildProjectMenu();
struct MenuItem *BuildApplicationMenu();

#define A_FLAGS (ISUE_USE_MY_SCREEN|ISUE_FORCE)
#define B_FLAGS (ISUE_LOAD_DRIVERS|ISUE_LOAD_STUDIO16INSTANCE|ISUE_FORCE)
/**************************************************************/
void InitStudio16Prgm()
{
int error;
struct Module *mod;

if ((error=InitStudio16UserEnv(A_FLAGS, screen,bdwind))==NO_ERROR)
	{
	WaitTOF();
	WaitTOF();
	WaitTOF();
	WaitTOF();
	set_default_screen(screen);
	new_pointer(PTR_BUSY);

	if(NOT_MY_SCREEN==FALSE) LoadRGB4(&screen->ViewPort,StudioBase->defaults.palette,8);

	error=InitStudio16UserEnv(B_FLAGS);
	if (error!=NO_ERROR) 
		{
		show_error(error);
		Quit();
		}

	if ((mod=(struct Module *)FindModule("About"))==NULL)
		{
		mod=(struct Module *)LoadModule("ProjectMenu/About");
		if (mod==NULL) ScreenToFront(screen);
		}
	if (mod)
		{
		BroadcastEventParms(GetEventID("SE_CMD_SELECT_MODULE"),mod,0,0,0,0,0);
		}

	BlankProj[0].FirstItem=BuildProjectMenu();
	BlankProj[1].FirstItem=BuildApplicationMenu();
	wp.Project[0]=BlankProj[0];
	wp.Project[1]=BlankProj[1];


	if(NOT_MY_SCREEN==FALSE) 
		{
		SetMenuStrip(bdwind,&BlankProj);
		}
	else
		{
		SetMenuStrip(MUwind,&BlankProj);
		}
	}
else
	{
	show_error(error);
	Quit();
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) CloseWindow(wind);
if (MUwind) CloseWindow(MUwind);
if (bdwind) CloseWindow(bdwind);
if (MUwind || bdwind)
	{
	FreeApplicationMenu(wp.Project[1].FirstItem);
	FreeProjectMenu(wp.Project[0].FirstItem);
	}

if (screen && NOT_MY_SCREEN==FALSE) CloseScreen(screen);
closelibraries();
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
