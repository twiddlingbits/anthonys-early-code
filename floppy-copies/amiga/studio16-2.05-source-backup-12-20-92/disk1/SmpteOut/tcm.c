#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include <devices/timer.h>
#include "Prog:include/Image.h"

void EditDigit();
int EditMode=-1;
extern int curtc;
#include "tcm.data"

/**********************************/
/* Declarations for CBACK */
long _stack = 4000;			/* Amount of stack space our task needs   */
char *_procname = "STUDIO_SMPTE_OUT";	/* The name of the task to create         */
long _priority = 0;			/* The priority to run us at              */
long _BackGroundIO = 0;			/* Flag to tell it we want to do I/O      */
/**********************************/
UBYTE ho,mo,so,fo;
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
void PauseFunc();
/**********************************/
int tc[4];
/**********************************/
extern short x[9];
extern short y[9];
extern short w[9];
extern short h[9];
int DigX[12];
extern int DigW,DigH;
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
USHORT Dcolor;
BOOL SUSPEND=FALSE;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
ULONG draw_sigbit=-1;
ULONG draw_sigmask;
ULONG SmpteErrorNum;
struct Task *task;

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct SmpteSource *OldSMPTEsrc,*SetSmpteSource(),*AddSmpteSource();
struct SmpteSource *SMPTEsrc;
struct NewSmpteSource NewSMPTEsrc={"SMPTE Output",0,NULL};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void _main(argv)
char *argv;
{
int mask;
int QuerySmpteTime();
void DisplaySmpteError();

InitModule(argv);				/** initalize module **/

Dcolor=StudioBase->defaults.colors.gadget_window.bottomright_3d;

while (TRUE)
	{
	mask = draw_sigmask;
	if (wind) mask |= (1<<wind->UserPort->mp_SigBit);
	if (mod)  mask |= (1<<mod->notifyme->mp_SigBit);
	mask=Wait(mask);
	if (mask&draw_sigmask) SmpteData(curtc);
	if (wind && (mask&(1<<wind->UserPort->mp_SigBit))) handle_intuievents();
	if (mod  && (mask & (1<<mod->notifyme->mp_SigBit))) handle_studioevents();
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

if ((mod=(struct Module *)AddModule(&ModData))==0) 
	{
	BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"),0, 0,0,0,0,0);
	telluser("SMPTE Output:","Could Not Add Module",0);
	closelibraries();
	exit(10);
	}

mod->state->flags|=MS_CHOOSE_WINDOW;


se_cmd_select_module	= GetEventID("SE_CMD_SELECT_MODULE");
se_cmd_kill_module	= GetEventID("SE_CMD_KILL_MODULE");
se_info_module_opened	= GetEventID("SE_INFO_MODULE_OPENED");
se_info_module_closed	= GetEventID("SE_INFO_MODULE_CLOSED");

NotifyMeOnEvent(se_cmd_select_module, mod);
NotifyMeOnEvent(se_cmd_kill_module, mod);

task=(struct Task *)FindTask(NULL);
if ((draw_sigbit=AllocSignal(-1))==-1) {telluser("SMPTE Output:","Error 2, Can't get SigBit",NULL);}
draw_sigmask=1<<draw_sigbit;

SMPTEsrc=AddSmpteSource(&NewSMPTEsrc);
OldSMPTEsrc=SetSmpteSource(SMPTEsrc);
SetSmpteSource(OldSMPTEsrc);


BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_SUCCEDED"), mod, 0,0,0,0,0);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
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
			DeleteModule(mod);
			if (wind) HideWind();
			if (draw_sigbit!=-1) FreeSignal(draw_sigbit);

			DeleteSmpteSource(SMPTEsrc);
			SetSmpteSource(OldSMPTEsrc);

			closelibraries();
			exit(0);
			}
		}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void EditDigit(num)
int num;
{
if (num==2 || num==5 || num==8) return;

SetDrMd(wind->RPort,COMPLEMENT);

if (EditMode!=-1)
	{
	RectFill(wind->RPort,DigX[EditMode]-1,18,DigX[EditMode]+DigW,19+DigH);
	}

if (EditMode==num) {EditMode=-1;return;}
else EditMode=num;

RectFill(wind->RPort,DigX[num]-1,18,DigX[num]+DigW,19+DigH);

SetDrMd(wind->RPort,JAM2);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
int handle_intuievents()
{
struct IntuiMessage *message;
ULONG class;
int x,y,code,z;
void RefreshDigits();
/**int xht,xho,xmt,xmo,xst,xso,xft,xfo;**/

while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
	{
	class=message->Class;
	x=message->MouseX;
	y=message->MouseY;
	code=message->Code;
	HandleIntuitMess(wind,message);
	ReplyMsg(message);


	if (class ==MOUSEBUTTONS && !(code&0x00080) && GENERATING==GOFF)
		{
		if (y>18  && y<(DigH+19))
			for (z=0;z<11;z++)
				if (DigX[z]<x && (DigX[z]+DigW)>x) EditDigit(z);
		}

	if (class == RAWKEY)
		{
		if (code>0 &&code<0x0b) 
			{
			if (code==0x0a) code=0;

			if (EditMode==0) ho=(ho%10)+(code*10);
			if (EditMode==1) ho=((ho/10)*10)+code;
			if (EditMode==3) mo=(mo%10)+(code*10);
			if (EditMode==4) mo=((mo/10)*10)+code;
			if (EditMode==6) so=(so%10)+(code*10);
			if (EditMode==7) so=((so/10)*10)+code;
			if (EditMode==9) fo=(fo%10)+(code*10);
			if (EditMode==10)fo=((fo/10)*10)+code;

			if (ho>23) ho=23;
			if (mo>59) mo=59;
			if (so>59) so=59;
			if (fo>29) fo=29;

			curtc=(ho<<24)+(mo<<16)+(so<<8)+fo;
			wdraw_digits();

			if (EditMode==0 || EditMode==3 || EditMode==6 || EditMode==9) EditDigit(EditMode+1);
			else if (EditMode==1 || EditMode==4 || EditMode==7) EditDigit(EditMode+2);
			else if (EditMode==10) EditDigit(EditMode);
			}
		if (code==0x4E) 
			{
			if (EditMode==0 || EditMode==3 || EditMode==6 || EditMode==9) EditDigit(EditMode+1);
			else if (EditMode==1 || EditMode==4 || EditMode==7) EditDigit(EditMode+2);
			}

		if (code==0x4F)
			{
			if (EditMode==3 || EditMode==6 || EditMode==9) EditDigit(EditMode-2);
			else if (EditMode==1 || EditMode==4 || EditMode==7 || EditMode==10) EditDigit(EditMode-1);
			}

		if (code==0x41)
			{
			if (EditMode==-1) 
				{
				curtc=(curtc&0xFFFFFF00)+(((curtc&0x000000FF)/10)*10);
				RefreshDigits(curtc);
				EditDigit(10);
				}
			else if (EditMode==10) 
				{
				curtc=(curtc&0xFFFFFF00)+(((curtc&0x000000FF)%10));
				RefreshDigits(curtc);
				EditDigit(9);
				}
			else if (EditMode==9) 
				{
				curtc=(curtc&0xFFFF00FF)+(((((curtc&0x0000FF00)>>8)/10)*10)<<8);
				RefreshDigits(curtc);
				EditDigit(7);
				}
			else if (EditMode==7) 
				{
				curtc=(curtc&0xFFFF00FF)+(((((curtc&0x0000FF00)>>8)%10))<<8);
				RefreshDigits(curtc);
				EditDigit(6);
				}
			else if (EditMode==6) 
				{
				curtc=(curtc&0xFF00FFFF)+(((((curtc&0x00FF0000)>>16)/10)*10)<<16);
				RefreshDigits(curtc);
				EditDigit(4);
				}
			else if (EditMode==4) 
				{
				curtc=(curtc&0xFF00FFFF)+(((((curtc&0x00FF0000)>>16)%10))<<16);
				RefreshDigits(curtc);
				EditDigit(3);
				}
			else if (EditMode==3) 
				{
				curtc=(curtc&0x00FFFFFF)+(((((curtc&0xFF000000)>>24)/10)*10)<<24);
				RefreshDigits(curtc);
				EditDigit(1);
				}
			else if (EditMode==1)
				{
				curtc=(curtc&0x00FFFFFF)+(((((curtc&0xFF000000)>>24)%10))<<24);
				RefreshDigits(curtc);
				EditDigit(0);
				}
			}

		if (code==0x46)
			{
/*******************
			xfo=(curtc&0x000000FF);
			xft=xfo/10; xfo=xfo%10;

			xso=(curtc&0x0000FF00)>>8;
			xst=xso/10; xso=xso%10;

			xmo=(curtc&0x00FF0000)>>16;
			xmt=xmo/10; xmo=xmo%10;

			xho=(curtc&0xFF000000)>>24;
			xht=xho/10; xho=xho%10;

			if (EditMode==0) xht=xho;
			if (EditMode<=1) xho=xmt;
			if (EditMode<=3) xmt=xmo;
			if (EditMode<=4) xmo=xst;
			if (EditMode<=6) xst=xso;
			if (EditMode<=7) xso=xft;
			if (EditMode<=9) xft=xfo;
			if (EditMode<=10) xfo=0;

			curtc=((xht*10+xho)<<24)+((xmt*10+xmo)<<16)+((xst*10+xso)<<8)+(xft*10+xfo);
			RefreshDigits(curtc);
****************/
			if (EditMode==10) 
				{
				curtc=(curtc&0xFFFFFF00)+(((curtc&0x000000FF)/10)*10);
				RefreshDigits(curtc);
				}
			else if (EditMode==9) 
				{
				curtc=(curtc&0xFFFFFF00)+(((curtc&0x000000FF)%10));
				RefreshDigits(curtc);
				}
			else if (EditMode==7) 
				{
				curtc=(curtc&0xFFFF00FF)+(((((curtc&0x0000FF00)>>8)/10)*10)<<8);
				RefreshDigits(curtc);
				}
			else if (EditMode==6)
				{
				curtc=(curtc&0xFFFF00FF)+(((((curtc&0x0000FF00)>>8)%10))<<8);
				RefreshDigits(curtc);
				}
			else if (EditMode==4)
				{
				curtc=(curtc&0xFF00FFFF)+(((((curtc&0x00FF0000)>>16)/10)*10)<<16);
				RefreshDigits(curtc);
				}
			else if (EditMode==3)
				{
				curtc=(curtc&0xFF00FFFF)+(((((curtc&0x00FF0000)>>16)%10))<<16);
				RefreshDigits(curtc);
				}
			else if (EditMode==1)
				{
				curtc=(curtc&0x00FFFFFF)+(((((curtc&0xFF000000)>>24)/10)*10)<<24);
				RefreshDigits(curtc);
				}
			else if (EditMode==0)
				{
				curtc=(curtc&0x00FFFFFF)+(((((curtc&0xFF000000)>>24)%10))<<24);
				RefreshDigits(curtc);
				}
			}

		if (code==0x4D || code==0x44) 
			{
			EditDigit(EditMode);
			}


/*		kprintf("CODE %x%x\n",code);*/
		}

	if (class == MENUPICK)
		{
		if (MENUNUM(code)==FIRST_MENU)
			{
			x=ITEMNUM(code);
			if (x>-1 && x<5)
				{
				if (GENERATING!=GOFF) GENERATING=GQUIT;
				while (GENERATING!=GOFF) WaitTOF();
				curtc=tc[x];
				RefreshDigits(curtc);

				StopBut.gad_ptr->flags|=BUTN_HIT;
				refresh_gadget(StopBut.gad_ptr);
				PlayBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(PlayBut.gad_ptr);
				RewBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(RewBut.gad_ptr);
				FFBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(FFBut.gad_ptr);
				}
			if (x>4 && x<9) 
				{
				tc[x-5]=curtc;
				ItemText[x-3].IText[7]='0'+((curtc>>24)&0x000FF)/10;
				ItemText[x-3].IText[8]='0'+((curtc>>24)&0x000FF)%10;

				ItemText[x-3].IText[10]='0'+((curtc>>16)&0x000FF)/10;
				ItemText[x-3].IText[11]='0'+((curtc>>16)&0x000FF)%10;

				ItemText[x-3].IText[13]='0'+((curtc>>8)&0x000FF)/10;
				ItemText[x-3].IText[14]='0'+((curtc>>8)&0x000FF)%10;

				ItemText[x-3].IText[16]='0'+((curtc)&0x000FF)/10;
				ItemText[x-3].IText[17]='0'+((curtc)&0x000FF)%10;
				}
			if (x==10)
				{
				if (GENERATING!=GOFF) GENERATING=GQUIT;
				while (GENERATING!=GOFF) WaitTOF();
				curtc=0;
				RefreshDigits(curtc);

				StopBut.gad_ptr->flags|=BUTN_HIT;
				refresh_gadget(StopBut.gad_ptr);
				PlayBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(PlayBut.gad_ptr);
				RewBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(RewBut.gad_ptr);
				FFBut.gad_ptr->flags&=~BUTN_HIT;
				refresh_gadget(FFBut.gad_ptr);
				}
			}
		}

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
struct Window *OpenNewWind();

ColorGadList(&PlayBut);

if (wind = OpenNewWind(&SMON_NewWindow,mod->state,NULL,Titles))
	{
	WindGadSize();

	BroadcastEventParms(se_info_module_opened, mod, 0,0,0,0,0);
	curtc=QuerySmpteTime();
	SmpteData(curtc);
	}
else telluser("SMPTE Output:","Can not open window!",0);
}
/*****************************************************************/

void HideWind()
{
struct Window *w;

if (GENERATING!=GOFF) GENERATING=GQUIT;
while (GENERATING!=GOFF) WaitTOF();

if (wind)
	{
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
void my_new_gadget_size(ngh,x1,y1,x2,y2)
struct NewGadgetHeader *ngh;
short x1,y1,x2,y2;
{
if (ngh->gad_ptr) ngh->flags=ngh->gad_ptr->flags;

ngh->box_x1=x1;
ngh->box_x2=x2;
ngh->box_y1=y1;
ngh->box_y2=y2;
}
/**************************************************************/
/**************************************************************/

static void WindGadSize()
{
short x;
int l,cw;
int cellw,cellh;
int w,h;
if (wind)
	{
	SUSPEND=TRUE;

	w=wind->Width;
	h=wind->Height;
	l=(w-20)/4;

	my_new_gadget_size(&FFBut ,12+l*3 ,h-17 ,w-13     ,h-5);
	my_new_gadget_size(&RewBut,12+l*2 ,h-17 ,12+l*3-4 ,h-5);
	my_new_gadget_size(&StopBut ,12+l*1 ,h-17 ,12+l*2-4 ,h-5);
	my_new_gadget_size(&PlayBut  ,12+l*0 ,h-17 ,12+l*1-4 ,h-5);

	delete_gadget_chain(wind);
	FFBut.gad_ptr=NULL;
	RewBut.gad_ptr=NULL;
	PlayBut.gad_ptr=NULL;
	StopBut.gad_ptr=NULL;

	cellh = wind->Height-43;
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

	draw_indent(wind->RPort,10,16,wind->Width-11,wind->Height-22,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(wind->RPort,8,15,wind->Width-9,wind->Height-21,StudioBase->defaults.colors.gadget_window.topleft_3d,StudioBase->defaults.colors.gadget_window.bottomright_3d);

	new_big_digit_size(cellh,cellw);

	if (create_gadget_chain(wind,&PlayBut)==0)  {telluser("SMPTE Output:","Could not create gadgets!",0); }



	SUSPEND=FALSE;
	wdraw_digits();
	}
}

/**************************************************************/
/**************************************************************/

void SmpteData(z)
struct TimeCode z;
{
short tens();
short ones();
struct TimeCode *x=&z;
if (wind && !SUSPEND)
	{
	LockGadLibRender();
	SetDrMd(wind->RPort,JAM2);
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
	UnLockGadLibRender();
	}
}

/**************************************************************/
/**************************************************************/
/**************************************************************/

void wdraw_digits()
{
int t;

SetDrMd(wind->RPort,JAM1);
if (EditMode!=-1)
	{
	SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
	RectFill(wind->RPort,DigX[EditMode]-1,18,DigX[EditMode]+DigW,19+DigH);
	}

big_digit(wind->RPort,tens(ho),11,DigX[0], 19);
big_digit(wind->RPort,ones(ho),11,DigX[1], 19);
big_digit(wind->RPort,10,11,DigX[2], 19);

big_digit(wind->RPort,tens(mo),11,DigX[3], 19);
big_digit(wind->RPort,ones(mo),11,DigX[4], 19);
big_digit(wind->RPort,10,11,DigX[5],19);

big_digit(wind->RPort,tens(so),11,DigX[6], 19);
big_digit(wind->RPort,ones(so),11,DigX[7], 19);
big_digit(wind->RPort,10,11,DigX[8],19);

big_digit(wind->RPort,tens(fo),11,DigX[9], 19);
big_digit(wind->RPort,ones(fo),11,DigX[10],19);

if (EditMode!=-1) 
	{
	t=EditMode;
	EditMode=-1;
	EditDigit(t);
	}
}
/**************************************************************/
/**************************************************************/
void RefreshDigits(z)
struct TimeCode z;
{
struct TimeCode *x=&z;

fo=x->frames;
ho=x->hours;
mo=x->minutes;
so=x->seconds;

wdraw_digits();
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
/*void DrawImg(wind,img,x,y,h,w,d,c)
struct Window *wind;
UWORD *img;
int x,y,h,w,d,c;
{
CHIP UWORD imgdata[7*4+1];




}*/
