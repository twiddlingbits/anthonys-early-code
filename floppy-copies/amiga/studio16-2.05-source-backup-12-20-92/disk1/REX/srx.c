#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "srx.data"
#include "clib/macros.h"
/**********************************/
#define RX_ER_NO_HANDLER 21
#define RX_ER_INVALID_CHAN_TYPE 22
#define RX_ER_COULD_NOT_OPEN_SAMPLE 23
#define RX_ER_UNSUPORTED_PARAMETER 11
#define RX_ER_COULD_NOT_ALLOCATE_KEY 12
#define RX_ER_COULD_NOT_FIND_SAMPLE 13
#define RX_ER_INVALID_PARAMETER 14
#define RX_ER_TIME_CODE_PAST 6
/**********************************/
int DisplayFlags;
struct Task *MessMonTask;
/**********************************/
/* Declarations for CBACK */
long _stack = 8000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_AREXX";	/* The name of the task to create         */
long _priority = 1;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
/**********************************/
short se_info_module_opened;
short se_info_module_closed;
short se_cmd_select_module;
short se_cmd_kill_module;
short se_cmd_set_channel_volume;
short se_cmd_set_input_gain;
short se_cmd_set_cutoff_freq;
short se_cmd_set_sampling_rate;
short se_cmd_set_channel_pan;
/**********************************/
/**********************************/
struct window *OpenNewWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
void InitModule();
void DisplayWind();
void HideWind();
void handle_studioevents();
void handle_intuievents();
void Quit();
void WindGadSize();
/**********************************/
/**********************************/
struct Module *mod;
extern struct StudioBase *StudioBase;
struct Window *wind;
/**********************************/
/**********************************/
int Ypos=0;
void prnt();
/**************************************************************/
struct AudioHandler 	*hand,*FindAudioHandler();
/**************************************************************/
#include "minrexx.h"
int RexxS16ToFront();
int RexxS16Version();
int RexxS16QuerySmpteTime();
int RexxS16ChanVolume();
int RexxS16Notify();
int RexxS16Rate();
int RexxS16Filter();
int RexxS16Gain();
int RexxS16Pan();
int RexxS16Play();
int RexxS16ToBack();
int RexxS16Exit();
int RexxS16WaitTimeCode();
int RexxS16Open();
int RexxS16Trigger();
int RexxS16Close();
int RexxS16OpenWithAutoClose();
int RexxS16StopAllPlayback();
int disp() ;
struct rexxCommandList rcl[] = {
	{ "s16openwithautoclose" ,RexxS16OpenWithAutoClose},
	{ "s16open"	,RexxS16Open },
	{ "s16trigger"	,RexxS16Trigger },
	{ "s16close"	,RexxS16Close },
	{ "s16tofront"	,RexxS16ToFront },
	{ "s16toback"	,RexxS16ToBack },
	{ "s16exit"	,RexxS16Exit },
	{ "s16version"	,RexxS16Version },
	{ "s16querysmptetime",RexxS16QuerySmpteTime },
	{ "s16chanvol"	,RexxS16ChanVolume },
	{ "s16notify"	,RexxS16Notify },
	{ "s16rate"	,RexxS16Rate },
	{ "s16gain"	,RexxS16Gain },
	{ "s16pan"	,RexxS16Pan },
	{ "s16filter"	,RexxS16Filter },
	{ "s16play"	,RexxS16Play },
	{ "s16waittimecode"	,RexxS16WaitTimeCode },
	{ "s16stopallplayback" ,RexxS16StopAllPlayback},
	{ NULL, NULL } } ;

char *VERSION[100];
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**/long rexxbit ;
/**/char firstcommand[256] ;
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
extern struct IntuitionBase *IntuitionBase;
/**************************************************************/
BOOL OPEN_SCREEN=TRUE;
/**************************************************************/
void _main(argv)
char *argv;
{
int mask,len,x;

Forbid();
if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{closelibraries();Permit();exit(10);}
if (FindPort("Studio16.1")) {CloseLibrary(IntuitionBase);Permit();return;}
/**/rexxbit = upRexxPort("Studio16.1", rcl, "fd", &disp) ;
/**/firstcommand[0] = 0 ;

Permit();

openlibraries();

len=strlen(argv);
for (x=0;x<len && (strnicmp(&"no screen",&argv[x],9)) ;x++) ;
if (x<len) OPEN_SCREEN=FALSE;

sprintf(VERSION,"Studio 16 Vers %s, Arexx Port V1.00",VERSION_STRING);

if (OPEN_SCREEN) StandardEnviroment();			/** opens studio16 customscreen **/
else InitStudio16UserEnv(ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16BASE|ISUE_LOAD_DRIVERS,0,0);
InitModule(argv);				/** initalize module **/

while (TRUE)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask  | rexxbit);
	dispRexxPort() ;
	if (wind) if(mask&(1<<wind->UserPort->mp_SigBit))  handle_intuievents();
	if (mod)  if(mask & (1<<mod->notifyme->mp_SigBit)) handle_studioevents();
	}
}
/**************************************************************/
/**************************************************************/
void InitModule(name)
char *name;
{
char *FindFileName();
void HidePrefWind();
struct Window *DisplayPrefWind();

static struct NewModule ModData ={"Studio16REXX",MS_UTILITY_MODULE|MS_KEEP_RESIDENT,sizeof(struct StandardModState),0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Event Monitor:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

se_cmd_select_module 		= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module 		= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened		= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed		= GetEventID("SE_INFO_MODULE_CLOSED");
se_cmd_set_channel_volume 	= GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_cmd_set_input_gain		= GetEventID("SE_CMD_SET_INPUT_GAIN");
se_cmd_set_cutoff_freq		= GetEventID("SE_CMD_SET_CUTOFF_FREQ");
se_cmd_set_sampling_rate	= GetEventID("SE_CMD_SET_SAMPLING_RATE");
se_cmd_set_channel_pan		= GetEventID("SE_CMD_SET_CHANNEL_PAN");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
if (!(hand=FindAudioHandler(NULL,-1,0,NULL,-1))) telluser("Studio Arexx","No handlers loaded!",NULL);
}
/**************************************************************/
/**************************************************************/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_cmd_select_module)
		{
		if ((struct Module *)event->arg1==mod)
			if (wind==NULL)
				DisplayWind();
		}

	/*********************************/
	if(event->type == se_cmd_kill_module)
		{
		if ((struct Module *)event->arg1 == mod)
			{
			ReplyMsg(event);
			Quit();
			}
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

	if (class == NEWSIZE)
		WindGadSize();
	if (class == CLOSEWINDOW)
		{HideWind(); return;}
	}
}
/**************************************************************/
/**************************************************************/
SHORT NumOfID;

void DisplayWind()
{
static void InterlaceAdjust();

Ypos=0;

NewWind.Screen = StudioBase->screen;

if ((wind = OpenNewWind(&NewWind,mod->state,NULL,NULL,NULL))==0)  {telluser("Event Monitor:","Can not open window!",0);Quit();}

WindGadSize();

BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);

NumOfID=StudioBase->max_registered_ids;

DisplayFlags=4;
MessMonTask=(struct Task *)FindTask(NULL);
SetTaskPri(MessMonTask,-5);

}
/**************************************************************/
/**************************************************************/
void HideWind()
{
struct Window *w;

if (wind)
	{
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
BroadcastEventParms(se_info_module_closed, mod, 0,0,0,0,0);

}
/**************************************************************/
/**************************************************************/
void WindGadSize()
{
Ypos=0;
if (wind)
	{
	RefreshWind(wind);
	}
}
/**************************************************************/
/**************************************************************/
void Quit()
{

IgnoreEvent(se_cmd_select_module,mod);
IgnoreEvent(se_cmd_kill_module,mod);
if (wind) HideWind();
/**/dnRexxPort() ;
DeleteModule(mod);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void prnt(b)
char *b;
{
char e[200];
static BYTE CL=0;
if (wind)
	{
	sprintf(e,"%s                                                                             ",b);
	if ((Ypos*8+22)>wind->Height) {Ypos=0; CL++;}
		{
		if (CL&0x01) SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
		else SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
		SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
		SetDrMd(wind->RPort, JAM2);
		Move(wind->RPort,8,Ypos*8+19);
		Text(wind->RPort,e,MIN(strlen(e),((wind->Width)>>3)-2));
		Ypos++;
		}
	}
}
/**************************************************************/
/**************************************************************/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/*
 *	Now we get into the actual code necessary for our REXX port; functions
 *	that do the real work.  Note that this program was not structured
 *	particularly nicely for Rexx; I had to write each of these functions.
 *	Many programs have these subroutines already in place; they are called
 *	as part of the event loop.  This progam, however, just has one big
 *	switch statement with different actions . . .
 *
 *	First, our locals.
 */
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int args[4] ;		  /* what args did we see to this function? */
char *strarg[4] ;		  /* what args did we see to this function? */

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int getnm(where)
char **where ;
{
register char *p = *where ;
register int val = 0 ;
int gotone = 0 ;

while (*p <= ' ' && *p)	p++ ;

while ('0' <= *p && *p <= '9') 
	{
	gotone = 1 ;
	val = 10 * val + *p++ - '0' ;
	}

*where = p ;
return(val) ;
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

void parseargs(p, n)
char *p ;
int n ;
{
register int i ;

for (i=0; i<n; i++) {args[i] = getnm(&p) ; }
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
int parsestrargs(p)
char *p ;
{
int i=0, flag=0;

while (flag!=2 && i<4)
	{
	while (*p) {p++;flag=0;}
	p++;
	strarg[i++]=p;
	flag++;
	}
return(i-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int disp(msg, dat, p)
register struct RexxMsg *msg ;
register struct rexxCommandList *dat ;
char *p ;
{
int x=0;

x=(int)(*(dat->func))(msg, p) ;

if (wind)
	{
	prnt(msg->rm_Args[0]);
	}

if (x!=-1) {replyRexxCmd(msg, x, NULL, NULL) ; return (0);}
return(1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16Version(msg, p)
struct RexxMsg *msg ;
char *p ;
{
replyRexxCmd(msg, NULL, NULL, VERSION) ;
return(-1);
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16ToFront(msg, p)
struct RexxMsg *msg ;
char *p ;
{
if (StudioBase->screen) ScreenToFront(StudioBase->screen) ;
return(0);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char Rrate[20];
int RexxS16Rate(msg, p)
struct RexxMsg *msg ;
char *p ;
{
int rate;
parseargs(p, 1);

if (!(hand)) return(RX_ER_NO_HANDLER);
if (!(hand->flags & AH_RATE_ADJ)) return(RX_ER_UNSUPORTED_PARAMETER);

rate=(hand->RateValueFunc)((*hand->FindBestRate)(args[0]));
if (hand->current_sampling_rate!=rate) BroadcastEventParms(se_cmd_set_sampling_rate,hand,rate,0,0,0);

sprintf(Rrate,"%d",rate);
replyRexxCmd(msg, NULL, NULL, Rrate) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char Rfilter[20];
int RexxS16Filter(msg, p)
struct RexxMsg *msg ;
char *p ;
{
int freq;

parseargs(p, 1) ;
if (!(hand)) return(RX_ER_NO_HANDLER);
if (!(hand->flags & AH_FILTER_ADJ)) return(RX_ER_UNSUPORTED_PARAMETER);

freq=(hand->FilterValueFunc)((*hand->FindBestFilter)(args[0]));
if (hand->current_filter!=freq) BroadcastEventParms(se_cmd_set_cutoff_freq,hand,freq,0,0,0);

sprintf(Rfilter,"%d",freq);
replyRexxCmd(msg, NULL, NULL, Rfilter) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char Rgain[20];
int RexxS16Gain(msg, p)
struct RexxMsg *msg ;
char *p ;
{
int gain;

if (!(hand)) return(RX_ER_NO_HANDLER);
if (!(hand->flags & AH_GAIN_ADJ)) return(RX_ER_UNSUPORTED_PARAMETER);

parseargs(p, 1) ;

gain=(hand->GainValueFunc)((*hand->FindBestGain)(args[0]<<5));
	if (hand->current_input_gain!=gain) BroadcastEventParms(se_cmd_set_input_gain,hand,gain,0,0,0);

sprintf(Rgain,"%d",gain>>5);
replyRexxCmd(msg, NULL, NULL, Rgain) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char Rpan[20];
int RexxS16Pan(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ChanKey *KEY,*AllocChan();
int flags=CK_ID_ONLY;

parseargs(p, 3) ;

if (args[0]!=2) return(RX_ER_UNSUPORTED_PARAMETER);

if (args[0]==1) flags|=CK_CHAN_INPUT;
else if (args[0]==2) flags|=CK_CHAN_PLAY;
else if (args[0]==3) flags|=CK_CHAN_OUTPUT;
else return(RX_ER_INVALID_CHAN_TYPE);

if (!(hand->flags & AH_STEREO)) return(RX_ER_UNSUPORTED_PARAMETER);

if (!args[1]) return(RX_ER_COULD_NOT_ALLOCATE_KEY);
if((KEY=AllocChan(0,args[1]-1, flags))==NULL) return(RX_ER_COULD_NOT_ALLOCATE_KEY);
if (args[2]>=0 && args[2]<=200)
	BroadcastEventParms(se_cmd_set_channel_pan,KEY,(args[2])<<5,0,0,0);

sprintf(Rpan,"%d",args[2]);
replyRexxCmd(msg, NULL, NULL, Rpan) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char RCvol[20];
int RexxS16ChanVolume(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ChanKey *KEY,*AllocChan();
int flags=CK_ID_ONLY;

parseargs(p, 3) ;

if (args[0]==1) flags|=CK_CHAN_INPUT;
else if (args[0]==2) flags|=CK_CHAN_PLAY;
else if (args[0]==3) flags|=CK_CHAN_OUTPUT;
else return(RX_ER_INVALID_CHAN_TYPE);

if (!args[1]) return(RX_ER_COULD_NOT_ALLOCATE_KEY);
if((KEY=AllocChan(0,args[1]-1, flags))==NULL) return(RX_ER_COULD_NOT_ALLOCATE_KEY);
if (args[2]>=0)
	BroadcastEventParms(se_cmd_set_channel_volume,KEY,(args[2])<<5,0,0,0);

sprintf(RCvol,"%d",args[2]);
replyRexxCmd(msg, NULL, NULL, RCvol) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

char Qtc[20];
int RexxS16QuerySmpteTime(msg, p)
struct RexxMsg *msg ;
char *p ;
{
int tc;
tc=QuerySmpteTime() ;
sprintf(Qtc,"%02d:%02d:%02d:%02d",(tc>>24)&0x000ff,(tc>>16)&0x000ff,(tc>>8)&0x000ff,tc&0x000ff);
replyRexxCmd(msg, NULL, NULL,Qtc) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16ToBack(msg, p)
struct RexxMsg *msg ;
char *p ;
{
if (StudioBase->screen) ScreenToBack(StudioBase->screen) ;
return(0);
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
char *Rexit="Studio Exit Disabled";
int RexxS16Exit(msg, p)
struct RexxMsg *msg ;
char *p ;
{
replyRexxCmd(msg, NULL, NULL,Rexit) ;
return(0);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16Play(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ChanKey *key;
struct ActiveSample *as;
static struct Region myregion;

strcpy(myregion.samp_name,p);
myregion.start=0;
myregion.user_data=NULL;

if (FindRegion(p,&myregion) != NO_ERROR)
	{
	return(RX_ER_COULD_NOT_FIND_SAMPLE);
	}
as=(struct ActiveSample *)ASOpen(&myregion, AS_PLAY|AS_AUTO_DATA);
if (!as)
	{
	return(RX_ER_COULD_NOT_OPEN_SAMPLE);
	}
as->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
key=as->key;
if (!key) {ASClose(as);return(RX_ER_COULD_NOT_ALLOCATE_KEY);}

ASTrigger(as);

ASClose(as);
FreeChan(key);

return(0);
}


/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16WaitTimeCode(msg, p)
struct RexxMsg *msg ;
char *p ;
{
int tc,wtc;

wtc=TCstr2int(p);

while ((tc=QuerySmpteTime())<wtc) WaitTOF();
if (tc==wtc) return(0);
if (tc>wtc) return(RX_ER_TIME_CODE_PAST);
return(100);
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
char RAOpen[40];
int RexxS16Open(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ActiveSample *as;
static struct Region myregion;

strcpy(myregion.samp_name,p);
myregion.start=0;
myregion.user_data=NULL;

if (FindRegion(p,&myregion) != NO_ERROR)
	{
	return(RX_ER_COULD_NOT_FIND_SAMPLE);
	}
as=(struct ActiveSample *)ASOpen(&myregion, AS_PLAY|AS_AUTO_DATA|AS_AUTO_FREECHAN);
if (!as)
	{
	return(RX_ER_COULD_NOT_OPEN_SAMPLE);
	}

sprintf(RAOpen,"%d",as);
replyRexxCmd(msg, NULL, NULL, RAOpen) ;
return(-1);
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
char RAOpenwac[40];
int RexxS16OpenWithAutoClose(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ActiveSample *as;
static struct Region myregion;

strcpy(myregion.samp_name,p);
myregion.start=0;
myregion.user_data=NULL;

if (FindRegion(p,&myregion) != NO_ERROR)
	{
	return(RX_ER_COULD_NOT_FIND_SAMPLE);
	}
as=(struct ActiveSample *)ASOpen(&myregion, AS_PLAY|AS_AUTO_DATA|AS_AUTO_FREECHAN|AS_AUTO_CLOSE);
if (!as)
	{
	return(RX_ER_COULD_NOT_OPEN_SAMPLE);
	}

sprintf(RAOpenwac,"%d",as);
replyRexxCmd(msg, NULL, NULL, RAOpenwac) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
char RATrigger[40];

int RexxS16Trigger(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ActiveSample *as;

parseargs(p, 1) ;
as=(struct ActiveSample *)args[0];
if (!as) return(RX_ER_INVALID_PARAMETER);

as->key=(struct ChanKey *)AllocChan(NULL,-1,CK_CHAN_PLAY);
if (!as->key) {ASClose(as);return(RX_ER_COULD_NOT_ALLOCATE_KEY);}

ASTrigger(as);

sprintf(RATrigger,"%d",as);
replyRexxCmd(msg, NULL, NULL, RATrigger) ;
return(-1);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/

int RexxS16Close(msg, p)
struct RexxMsg *msg ;
char *p ;
{
struct ActiveSample *as;

parseargs(p, 1) ;
as=(struct ActiveSample *)args[0];
if (!as) return(RX_ER_INVALID_PARAMETER);

ASClose(as);

return(0);
}

/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
int RexxS16StopAllPlayback()
{
BroadcastEventParms(GetEventID("SE_CMD_STOP_ALL_PLAYBACK"),0,0,0,0,0,0);
return(0);
}
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
/**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**==**/
int RexxS16Notify(msg, p)
struct RexxMsg *msg ;
char *p ;
{
parsestrargs(p);

telluser(strarg[0],strarg[1],0);

return(0);
}

