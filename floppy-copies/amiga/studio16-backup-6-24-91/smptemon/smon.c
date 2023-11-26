#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "Smon.data"

/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
UBYTE bin2bcd[] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59};

/**********************************/
short se_cmd_select_module;
short se_cmd_kill_module;
short se_info_smpte;
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
extern struct studioBase *studioBase;
static struct Window *wind;
/**********************************/
/**********************************/

void wdraw_digits();

extern struct studioBase *studioBase;


static void WindGadSize();

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void main()
{
int mask;

openlibraries();
StandardEnviroment();			/** opens studio16 customscreen **/
InitModule();				/** initalize module **/
DisplayWind();				/** open window **/

while (TRUE)
	{
	mask=0;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void InitModule()
{
static struct NewModule ModData ={"SMPTE Monitor",0};

if ((mod=(struct Module *)AddModule(&ModData))==0)
	{
	telluser("Could Not Add Module","",0);
	closelibraries();
	exit(10);
	}

se_info_smpte  = GetEventID("SE_INFO_SMPTE");
se_cmd_select_module = GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module = GetEventID("SE_CMD_KILL_MODULE");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_studioevents()
{
struct StudioEvent *event;

while ((event=(struct StudioEvent *)GetMsg(mod->notifyme))!=0)
	{
	/*********************************/
	if(event->type == se_info_smpte)
		{
		SmpteData(event->arg1);
		}

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
			DeleteModule(mod);
			if (wind) HideWind();
			closelibraries();
			exit(0);
			}
		}
	ReplyMsg(event);
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
static void InterlaceAdjust();
struct Window *OpenWindow();

SMON_NewWindow.Screen = studioBase->screen;

if ((wind = OpenWind(&SMON_NewWindow))==0)  {telluser("Can not open SMPTE window!","",0);exit(10);}

WindGadSize();

NotifyMeOnEvent(se_info_smpte, mod);

}
/*****************************************************************/

void HideWind()
{
struct Window *w;

IgnoreEvent(se_info_smpte, mod);

if (wind)
	{
	w=wind;
	wind=NULL;
	CloseWind(w);
	}
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

if (wind)
	{
	cellh = wind->Height-25;
	cw = (wind->Width-32)/12.0;
	cellw=cw;

	l = 16;
	for(x=0;x<12;x++)
		{
		DigX[x]=l;
		l=l+cw;
		if (x==0 || x==3 || x==6 || x==9)
			l=l+(cw/4);
		}

	RefreshWind(wind);

	draw_indent(wind->RPort,10,14,wind->Width-11,wind->Height-6, DARK_BROWN, WHITE);

	new_big_digit_size(cellh,cellw);
	wdraw_digits();
	}
}

/**************************************************************/
/**************************************************************/
 UBYTE ho,mo,so,fo;

void SmpteData(x)
struct TimeCode *x;
{
short tens();
short ones();

if (wind)
	{
	if (ho!=x->hours)
		{
		big_digit(wind->RPort,tens(x->hours),tens(ho), DigX[0], 17);
		big_digit(wind->RPort,ones(x->hours),ones(ho), DigX[1], 17);
		ho=x->hours;
		}
	if (mo!=x->minutes)
		{
		big_digit(wind->RPort,tens(x->minutes),tens(mo), DigX[3], 17);
		big_digit(wind->RPort,ones(x->minutes),ones(mo), DigX[4], 17);
		mo=x->minutes;
		}
	if (so!=x->seconds)
		{
		big_digit(wind->RPort,tens(x->seconds),tens(so), DigX[6], 17);
		big_digit(wind->RPort,ones(x->seconds),ones(so), DigX[7], 17);
		so=x->seconds;
		}
	if (fo!=x->frames)
		{
		big_digit(wind->RPort,tens(x->frames),tens(fo), DigX[9], 17);
		big_digit(wind->RPort,ones(x->frames),ones(fo), DigX[10], 17);
		fo=x->frames;
		}
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/

void wdraw_digits()
{
big_digit(wind->RPort,tens(ho),10,DigX[0], 17);
big_digit(wind->RPort,ones(ho),10,DigX[1], 17);
big_digit(wind->RPort,10,0,DigX[2], 17);

big_digit(wind->RPort,tens(mo),10,DigX[3], 17);
big_digit(wind->RPort,ones(mo),10,DigX[4], 17);
big_digit(wind->RPort,10,0,DigX[5],17);

big_digit(wind->RPort,tens(so),10,DigX[6], 17);
big_digit(wind->RPort,ones(so),10,DigX[7], 17);
big_digit(wind->RPort,10,0,DigX[8],17);

big_digit(wind->RPort,tens(fo),10,DigX[9], 17);
big_digit(wind->RPort,ones(fo),10,DigX[10],17);
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
