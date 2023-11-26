#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_ABOUT";	/* The name of the task to create         */
long _priority = -2;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
extern struct Image InterlaceImage;
extern struct Image StandardImage;
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_hide_window;
/**********************************/
/**********************************/
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void Quit();
void handle_studioevents();
void handle_intuievents();
/**********************************/
/**********************************/
extern struct StudioBase *StudioBase;
/*struct Module *hand;*/
struct Module *mod;
struct Window *wind;
struct StandardModState *state;
struct MsgPort *timerport;
/**********************************/
/**********************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
static struct NewWindow NewWind = {
   (640-270)>>1,
   (200-170)>>1,
   270,
   170,
   BLACK,
   WHITE,
   MOUSEBUTTONS,
   NOCAREREFRESH|BORDERLESS,
   /*&OKgad*/NULL,0,
   NULL, /** TITLE **/
   0,0,
   100,34,1024,1024,
   CUSTOMSCREEN
   };

/**************************************************************/
/**************************************************************/
/**************************************************************/
void _main(cmd)
char *cmd;
{
int mask;

InitModule(cmd);			/** initalize module **/

while (TRUE)
	{
	mask=NULL;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/**************************************************************/

/**************************************************************/
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
static struct NewModule ModData ={NULL,NULL,sizeof(struct StandardModState)};
char *FindFileName();

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("About:","Could Not Add Module",0);
	Quit();
	}

state=mod->state;

se_cmd_select_module 	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_hide_window	= GetEventID("SE_CMD_HIDE_WINDOW");

NotifyMeOnEvent(se_cmd_hide_window, mod);
NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/
void Quit()
{
if (wind) HideWind();
if (mod) DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module && (struct Module *)event->arg1==mod)
			if (wind==NULL) DisplayWind();

	/*********************************/
	if(event->type == se_cmd_kill_module && (struct Module *)event->arg1 == mod)
		{
		ReplyMsg(event);
		Quit();
		}
	ReplyMsg(event);
	}
}
/**************************************************************/
/**************************************************************/
void handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	HandleIntuitMess(wind,message);
	class=message->Class;
	ReplyMsg(message);

/*	if (class == CLOSEWINDOW)*/
		{
		HideWind();
		return;
		}
	}
}
/**************************************************************/
/**************************************************************/
void DisplayWind()
{
int DiskSpaceAvail();
char temp[100],temp2[100],temp3[100];
int CON=0;
int INC=0;
int size,edge;
char *Int2Str();
char *Int2MegStr();
struct Window *OpenWindow();
NewWind.Screen = StudioBase->screen;
SetTaskPri(FindTask(NULL),-10);


if (StudioBase->screen->Height>300)
	{
	/* Interlaced */
	CON=-50;
	INC=3;
	NewWind.Width=268;
	NewWind.Height=320;
	NewWind.LeftEdge=(StudioBase->screen->Width-NewWind.Width)>>1;
	NewWind.TopEdge=(StudioBase->screen->Height-NewWind.Height)>>1;
	}
else
	{
	CON=-11;
	INC=0;
	NewWind.Width=268;
	NewWind.Height=180;
	NewWind.LeftEdge=(StudioBase->screen->Width-NewWind.Width)>>1;
	NewWind.TopEdge=(StudioBase->screen->Height-NewWind.Height)>>1;
	}

WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();

if ((wind = OpenWindow(&NewWind))!=0)  
	{
	SetDrMd(wind->RPort,JAM1);
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	RectFill(wind->RPort, 2,1, wind->Width-3, wind->Height-2);

	draw_indent(wind->RPort,0,0,wind->Width-1,wind->Height-1,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);

	if (StudioBase->screen->Height>300)
		{
		/* Interlaced */
		DrawImage(wind->RPort,&InterlaceImage,6,3);

		Move(wind->RPort,5,wind->Height-97+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-97+CON);
		Move(wind->RPort,5,wind->Height-96+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-96+CON);
		Move(wind->RPort,5,wind->Height-95+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-95+CON);
		Move(wind->RPort,5,wind->Height-94+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-94+CON);

		sprintf(temp,"Studio 16");
		Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-81+CON);
		Text(wind->RPort,temp,strlen(temp));
		Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1)+1,wind->Height-81+CON);
		Text(wind->RPort,temp,strlen(temp));

		sprintf(temp,"Version %s",VERSION_STRING);
		Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-71+CON);
		Text(wind->RPort,temp,strlen(temp));
		}
	else
		{
		DrawImage(wind->RPort,&StandardImage,6,3);

		SetAPen(wind->RPort,StudioBase->defaults.colors.button.draw_unselected);
		Move(wind->RPort,5,wind->Height-84+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-84+CON);
		Move(wind->RPort,5,wind->Height-83+CON);
		Draw(wind->RPort,wind->Width-6,wind->Height-83+CON);

		sprintf(temp,"Studio 16 Version %s",VERSION_STRING);
		Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-73+CON);
		Text(wind->RPort,temp,strlen(temp));
		Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1)+1,wind->Height-73+CON);
		Text(wind->RPort,temp,strlen(temp));
		}

CON+=INC;
	sprintf(temp,"©1991,1992 SunRize Industries");
	Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-64+CON);
	Text(wind->RPort,temp,strlen(temp));
CON+=INC;
CON+=INC;
CON+=INC;

	sprintf(temp,"Software by: Todd Modjeski");
	Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-51+CON);
	Text(wind->RPort,temp,strlen(temp));
CON+=INC;

	sprintf(temp,"             Anthony Wood ");
	Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-42+CON);
	Text(wind->RPort,temp,strlen(temp));
CON+=INC;
CON+=INC;
CON+=INC;
CON+=INC;

	Move(wind->RPort,5,wind->Height-38+CON);
	Draw(wind->RPort,wind->Width-6,wind->Height-38+CON);
CON+=INC;

	size=AvailMem(MEMF_CHIP);
	sprintf(temp,"CHIP:%11s",Int2Str(size,temp2));
	Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-28+CON);
	Text(wind->RPort,temp,strlen(temp));
CON+=INC;

	size=AvailMem(MEMF_FAST);
	sprintf(temp,"FAST:%11s",Int2Str(size,temp2));
	Move(wind->RPort,((wind->Width-(strlen(temp)*8))>>1),wind->Height-19+CON);
	Text(wind->RPort,temp,strlen(temp));
	edge=((wind->Width-(strlen(temp)*8))>>1);
	edge-=(8*8);
CON+=INC;

	sprintf(temp3,"%13s",StudioBase->defaults.samp_paths);
	if (strlen(temp3)>13) {temp3[13]=NULL;temp3[12]='*';}

	size=DiskSpaceAvail(StudioBase->defaults.samp_paths);
	if (size>=0) sprintf(temp,"%s%10sM",temp3,Int2MegStr(size,temp2));
	else sprintf(temp,"%sNot Mounted",temp2);
	Move(wind->RPort,edge,wind->Height-10+CON);
	Text(wind->RPort,temp,strlen(temp));
CON+=INC;

	Move(wind->RPort,5,wind->Height-17);
	Draw(wind->RPort,wind->Width-6,wind->Height-17);

	draw_indent(wind->RPort,(wind->Width>>1)-30,wind->Height-14,(wind->Width>>1)+30,wind->Height-4,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(wind->RPort,(wind->Width-(2*8))>>1,wind->Height-6);
	SetAPen(wind->RPort,StudioBase->defaults.colors.button.draw_unselected);
	Text(wind->RPort,"OK",2);
	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	ScreenToFront(StudioBase->screen);
	MoveScreen(StudioBase->screen,0,-1000);
	}
else telluser("About:","Can not open window!",0);
}
/**************************************************************/
/**************************************************************/
char *Int2Str(x,str)
int x;
char *str;
{
if (x<999) sprintf(str,"%d",x);
else if (x<999999) sprintf(str,"%d,%03d",x/1000,x-((x/1000)*1000));
else sprintf(str,"%d,%03d,%03d",x/1000000,(x/1000)-((x/1000000)*1000),x-((x/1000)*1000));
return(str);
}
/**************************************************************/
/**************************************************************/
char *Int2MegStr(x,str)
int x;
char *str;
{
char dec[5];
sprintf(dec,".%03d",((x%1024)*1000)/1024);
x=x/1024;
if (x<999) sprintf(str,"%d",x);
else if (x<999999) sprintf(str,"%d,%03d",x/1000,x-((x/1000)*1000));
else sprintf(str,"%d,%03d,%03d",x/1000000,(x/1000)-((x/1000000)*1000),x-((x/1000)*1000));
strcat(str,dec);
return(str);
}
/**************************************************************/
/**************************************************************/
void HideWind()
{
if (wind)
	{
	CloseWindow(wind);
	wind=NULL;
	BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
	}
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
