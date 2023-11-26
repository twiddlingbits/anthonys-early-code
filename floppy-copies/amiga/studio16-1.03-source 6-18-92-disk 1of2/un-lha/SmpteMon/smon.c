#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "Smon.data"
#include <devices/timer.h>
/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_SMPTE_MONITOR";	/* The name of the task to create         */
long _priority = -1;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
struct MsgPort *timerport;
struct timerequest *timermsg;
struct timeval currentval;
struct key_notice *pausekn;
short smptepaused = FALSE;
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void PauseFunc();
/**********************************/
/**********************************/
UBYTE bin2bcd[] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59};
/**********************************/
/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_module_opened;
short se_info_module_closed;
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
int  handle_intuievents();
void SmpteData();
/**********************************/
/**********************************/
struct Module *mod;
extern struct StudioBase *StudioBase;
static struct Window *wind;
/**********************************/
/**********************************/
void wdraw_digits();
static void WindGadSize();
/**********************************/
/**********************************/
int FLAG;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
ULONG sink_sigbit=-1;
ULONG sink_sigmask;
ULONG error_sigbit=-1;
ULONG error_sigmask;
ULONG SmpteErrorNum;
struct Task *task;
void SinkFunc();
struct NewSmpteSink NewSMPTEsnk = {-10,NULL,SinkFunc,NULL};
struct SmpteSink *SMPTEsnk;

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*void SinkFunc2()
{
Signal(task,sink_sigmask);
}*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
int QuerySmpteTime();
void DisplaySmpteError();
InitModule(argv);				/** initalize module **/

/*kprintf("OFFSET:%d %d",(((int)(&StudioBase->defaults.smpte_mode)) - ((int)(StudioBase))));*/

while (TRUE)
	{
	mask=1<<timerport->mp_SigBit | sink_sigmask | error_sigmask;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	if(mask & sink_sigmask) SmpteData(QuerySmpteTime());
	if(mask & error_sigmask) DisplaySmpteError(SmpteErrorNum);
	if(mask & (1<<timerport->mp_SigBit)) 
		{
		GetMsg(timerport);
		if (wind)
			{
			SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
			SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
			SetDrMd(wind->RPort, JAM2);
			RectFill(wind->RPort,2,12,9+10*3,15);
			}
		FLAG=0;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule(name)
char *name;
{
char *FindFileName();
static struct NewModule ModData ={NULL,0,sizeof(struct StandardModState)};
openlibraries();
StandardEnviroment();

ModData.name=FindFileName(name);

if ((mod=(struct Module *)AddModule(&ModData))==0) {telluser("SMPTE Mon:","Could Not Add Module",0);closelibraries();exit(10);}

mod->state->flags|=MS_CHOOSE_WINDOW;


timerport = (struct MsgPort *) CreatePort( 0, 0 );
timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );

se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

task=(struct Task *)FindTask(NULL);
if ((sink_sigbit=AllocSignal(-1))==-1) {telluser("SMPTE Mon:","Error, Can't get SigBit",NULL);}
sink_sigmask=1<<sink_sigbit;
if ((error_sigbit=AllocSignal(-1))==-1) {telluser("SMPTE Mon:","Error, Can't get SigBit",NULL);}
error_sigmask=1<<error_sigbit;
SMPTEsnk=(struct SmpteSink *)AddSmpteSink(&NewSMPTEsnk);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void DisplaySmpteError(x)
ULONG x;
{
ULONG y;
/*kprintf("SMPTE ERROR %d %d  %x%x\n",x,QuerySmpteTime());*/
if(!(FLAG & x))
	{
	if (wind)
		{
		y=1;
		while(!((x>>(y-1))&1)) y++;
		if (y==1/*TIMEOUT*/) SmpteData(QuerySmpteTime());
		if (y==StudioBase->defaults.colors.gadget_window.background) 
			{
			if (StudioBase->defaults.colors.gadget_window.background!=6)
				SetAPen(wind->RPort,6);
			else	SetAPen(wind->RPort,7);
			}
		else SetAPen(wind->RPort,y);
		SetDrMd(wind->RPort, JAM2);
		RectFill(wind->RPort,2+(10*(y-1)),12,9+(10*(y-1)),15);
		}

	if (FLAG==0)
		{
		currentval.tv_secs = 0;
		currentval.tv_micro = 300000;
		timermsg->tr_node.io_Command = TR_ADDREQUEST;
		timermsg->tr_time = currentval;
		SendIO((struct IORequest *) timermsg );
		}
	FLAG|=x;
	}
SmpteData(QuerySmpteTime());
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;
void HandEvnt();

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	HandEvnt(event);
	ReplyMsg(event);
	}
}
void HandEvnt(event)
struct StudioEvent *event;
{
	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==0)
				DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			if (FLAG!=0)
				{
				AbortIO( timermsg );
				WaitIO( timermsg );
				}
			if (timerport) DeletePort(timerport);
			CloseDevice( (struct IORequest *) timermsg );
			DeleteExtIO( (struct IORequest *) timermsg );
			DeleteModule(mod);
			if (wind) HideWind();
			if (sink_sigbit!=-1) FreeSignal(sink_sigbit);
			if (error_sigbit!=-1) FreeSignal(error_sigbit);
			DeleteSmpteSink(SMPTEsnk);
			closelibraries();
			exit(0);
			}
		}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	class=message->Class;
	HandleIntuitMess(wind,message);
	ReplyMsg(message);

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{
		HideWind();
		return(0);
		}
	}
return(0);
}
/*****************************************************************/
void DisplayWind()
{
static struct NewKeyNotice KeyData = {NULL,PauseFunc,0};
static void InterlaceAdjust();
struct Window *OpenNewWind();

if (wind = OpenNewWind(&SMON_NewWindow,mod->state,NULL,NULL))
	{
	WindGadSize();

	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	KeyData.w=wind;
	pausekn=(struct key_notice *)create_key_notice(&KeyData);
	SmpteData(QuerySmpteTime());
	}
else telluser("SMPTE Mon:","Can not open window!",0);
}
/*****************************************************************/

void HideWind()
{
struct Window *w;

if (wind)
	{
	delete_key_notice(pausekn);
	pausekn=NULL;
	w=wind;
	wind=NULL;
	WaitBlit();
	WaitTOF();
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);
}
/**************************************************************/
/**************************************************************/

/**************************************************************/
/**************************************************************/
int DigX[12];

static void WindGadSize()
{
short x;
int l,cw;
int cellw,cellh;

if (wind)
	{
	cellh = wind->Height-27;
	cw = ((wind->Width-32)<<16)/12;
	cellw=(cw>>16);

	l = (16<<16);
	for(x=0;x<12;x++)
		{
		DigX[x]=(l>>16);
		l=l+cw;
		if (x==0 || x==3 || x==6 || x==9)
			l=l+(cw/4);
		}
	RefreshWind(wind);

	draw_indent(wind->RPort,10,16,wind->Width-11,wind->Height-6,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	new_big_digit_size(cellh,cellw);
	wdraw_digits();
	}
}

/**************************************************************/
/**************************************************************/
UBYTE ho,mo,so,fo;

void SmpteData(z)
struct TimeCode z;
{
short tens();
short ones();
struct TimeCode *x=&z;
if (wind && !smptepaused)
	{
	if (ho!=x->hours)
		{
		big_digit(wind->RPort,tens(x->hours),tens(ho), DigX[0], 19);
		big_digit(wind->RPort,ones(x->hours),ones(ho), DigX[1], 19);
		ho=x->hours;
		}
	if (mo!=x->minutes)
		{
		big_digit(wind->RPort,tens(x->minutes),tens(mo), DigX[3], 19);
		big_digit(wind->RPort,ones(x->minutes),ones(mo), DigX[4], 19);
		mo=x->minutes;
		}
	if (so!=x->seconds)
		{
		big_digit(wind->RPort,tens(x->seconds),tens(so), DigX[6], 19);
		big_digit(wind->RPort,ones(x->seconds),ones(so), DigX[7], 19);
		so=x->seconds;
		}
	if (fo!=x->frames)
		{
		big_digit(wind->RPort,tens(x->frames),tens(fo), DigX[9], 19);
		big_digit(wind->RPort,ones(x->frames),ones(fo), DigX[10], 19);
		fo=x->frames;
		}
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/

void wdraw_digits()
{
big_digit(wind->RPort,tens(ho),10,DigX[0], 19);
big_digit(wind->RPort,ones(ho),10,DigX[1], 19);
big_digit(wind->RPort,10,0,DigX[2], 19);

big_digit(wind->RPort,tens(mo),10,DigX[3], 19);
big_digit(wind->RPort,ones(mo),10,DigX[4], 19);
big_digit(wind->RPort,10,0,DigX[5],19);

big_digit(wind->RPort,tens(so),10,DigX[6], 19);
big_digit(wind->RPort,ones(so),10,DigX[7], 19);
big_digit(wind->RPort,10,0,DigX[8],19);

big_digit(wind->RPort,tens(fo),10,DigX[9], 19);
big_digit(wind->RPort,ones(fo),10,DigX[10],19);
}
/**************************************************************/
/**************************************************************/
short tens(x)
UBYTE x;
{
return((bin2bcd[x]&0xF0)>>4);
}
short ones(x)
UBYTE x;
{
return(bin2bcd[x]&0x0F);
}
/**************************************************************/
/**************************************************************/
/** Called every time user hits a key with SmpteWindow active **/

void PauseFunc(kn,code,qualifier)
unsigned short code;
unsigned short qualifier;
struct new_key_notice *kn;
{
if (code==0x40) {	/** space bar down **/
	smptepaused=!smptepaused;
	}
}
