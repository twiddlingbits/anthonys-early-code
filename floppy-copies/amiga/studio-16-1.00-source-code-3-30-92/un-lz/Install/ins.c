#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "ins.data"
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/filehandler.h>
#include <graphics/text.h>

struct TextAttr MyFont = {"topaz.font",TOPAZ_EIGHTY,FS_NORMAL,FPF_ROMFONT };
struct TextFont *fnt;
void Resize();

void CloseHelp();
int ABORT=0;
/**********************************/

struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
/**********************************/
/**********************************/
extern struct DosLibrary *DOSBase;

struct GfxBase *GfxBase=NULL;
struct IntuitionBase *IntuitionBase=NULL;
struct Library *GadLibBase=NULL;
void closelibraries();
void openlibraries();
void print();
/**********************************/
long Handler_sigbit=-1,Help_sigbit=-1;
/**********************************/
/**********************************/
void Quit();
/**********************************/
/**********************************/
struct Window *wind,*Hwind=NULL;
/**********************************/
/**********************************/
/**************************************************************/
struct list *l1;
/**************************************************************/
/**************************************************************/
int task;
char DiskName[120];
/**************************************************************/
void main()
{
int mask;
struct IntuiMessage *message;
ULONG class;
struct Window *OpenWindow();
void InstallDisk();
void GetDevsNames();
void GetDiskName();
void OpenHelp();

task=FindTask(NULL);
GetDiskName();
openlibraries();

if ((wind=OpenWindow(&NewWind))==NULL)  {printf("Can not open window!");Quit();}
fnt=(struct TextFont *)OpenFont(&MyFont);
SetFont(wind->RPort,fnt);
SetAPen(wind->RPort, 0);
RectFill(wind->RPort, 2,11, wind->Width-3, wind->Height-2);

SetAPen(wind->RPort, 2);
RectFill(wind->RPort, 0 ,0, 1, wind->Height);

SetAPen(wind->RPort, 1);
RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

draw_indent(wind->RPort,10,105,390,115,1,2);

if (create_gadget_chain(wind,&INSTALLbutton)==0) Quit();

l1=(struct list *)DEVlist.gad_ptr;	/** filled in by create_gadget_chain **/
GetDevsNames();

if ( (Handler_sigbit=AllocSignal(-1))==-1) {telluser("FATAL ERROR","",0);Quit();}
if ( (Help_sigbit=AllocSignal(-1))==-1) {telluser("FATAL ERROR","",0);Quit();}

print("SELECT A DEVICE NAME OR HELP");

while (TRUE)
{
mask=(1<<Handler_sigbit)|(1<<Help_sigbit)|(1<<wind->UserPort->mp_SigBit);
if (Hwind) mask |= (1<<Hwind->UserPort->mp_SigBit);
mask=Wait(mask);

if(mask&(1<<wind->UserPort->mp_SigBit))
	{
	while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=0)
		{
		class=message->Class;
		ReplyMsg(message);

		if (class == CLOSEWINDOW)
			Quit();
		}
	}


if (Hwind) if(mask&(1<<Hwind->UserPort->mp_SigBit))
	{
	while (Hwind && ((message=(struct IntuiMessage *)GetMsg(Hwind->UserPort))!=0))
		{
		class=message->Class;
		ReplyMsg(message);

		if (class == CLOSEWINDOW)
			CloseHelp();
		if (class == NEWSIZE)
			Resize();
		}
	}
if(mask&(1<<Help_sigbit))
	{
	OpenHelp();
	}
if(mask&(1<<Handler_sigbit))
	{
	InstallDisk();
	}
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
void Quit()
{
delete_gadget_chain(wind);

if (Hwind) CloseHelp();
if (fnt) CloseFont(fnt);
if (Handler_sigbit!=-1) FreeSignal(Handler_sigbit);
if (Help_sigbit!=-1) FreeSignal(Help_sigbit);
if (wind) CloseWindow(wind);
closelibraries();
exit(0);
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
void openlibraries()
{
void closelibraries();
char bufr[180];
sprintf(bufr,"%s:libs/gadlib.library",DiskName);
/*sprintf(bufr,"libs:gadlib.library");*/
if ((GadLibBase=(struct Library *)OpenLibrary(bufr,0))==NULL)
	{printf("YOU MUST HAVE THE ORIGINAL DISK INSERTED!!\n");closelibraries();exit(10);}

if ((IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33))==NULL)
	{telluser("Use Workbench 1.2 or greater.","",0);Quit();}

if ((GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0))==NULL)
	{telluser("Can't open graphics library!","",0);Quit();}

}

void closelibraries()
{
if (IntuitionBase) CloseLibrary(IntuitionBase);

if (GfxBase)	CloseLibrary(GfxBase);

if (GadLibBase)	CloseLibrary(GadLibBase);

}
/*****************************************************************/
void GetDiskName()
{
LONG blen;
struct DeviceNode *dn;
struct DosInfo *di;
struct RootNode *root;
UBYTE *bstring;
char *xxx={"Studio16"};
int Flag=1;
char *uu;
root = (struct RootNode *)DOSBase->dl_Root;
di = (struct DosInfo *)BADDR(root->rn_Info);
dn = (struct DeviceNode *) BADDR(di->di_DevInfo);

Forbid();
for (; dn != 0 && Flag ; dn = (struct DeviceNode *)BADDR(dn->dn_Next))
	{
	if ((dn->dn_Type == 2))
		{
		bstring = (UBYTE *)BADDR(dn->dn_Name);
		blen = (LONG)bstring[0];
		if(stcpm(bstring,&"tudio16",&uu) && (Flag)) 
			{
			CopyMem((BYTE *)(bstring + 1),DiskName, (LONG)blen);
			Flag=0;
			}
		}
 	}
 Permit();

if (Flag) strcpy(DiskName,xxx);
}
/*****************************************************************/
void GetDevsNames()
{
LONG blen;
struct DeviceNode *dn;
struct DosInfo *di;
struct RootNode *root;
UBYTE *bstring;
char buf[120];
int i=0;
char *uu;

root = (struct RootNode *)DOSBase->dl_Root;
di = (struct DosInfo *)BADDR(root->rn_Info);
dn = (struct DeviceNode *) BADDR(di->di_DevInfo);

Forbid();
for (; dn != 0 ; dn = (struct DeviceNode *)BADDR(dn->dn_Next))
	{
	if ((dn->dn_Type == 2))
		{
		bstring = (UBYTE *)BADDR(dn->dn_Name);
		blen = (LONG)bstring[0];
		if(!stcpm(bstring,&"Studio16",&uu))
			{
			CopyMem((BYTE *)(bstring + 1),buf, (LONG)blen);
			buf[blen]=':'; buf[blen+1]=NULL;
			add_list_entry(l1, buf, i++);
			}
		}
 	}
 Permit();
}
/*****************************************************************/
/*****************************************************************/
void InstallFunc()
{
Signal(task,(1<<Handler_sigbit));
}
/*****************************************************************/
void print(str)
char *str;
{
char buq[120];
sprintf(buq,"%s                                               ",str);
SetAPen(wind->RPort, 3);
SetBPen(wind->RPort, 0);
SetDrMd(wind->RPort, JAM2);
Move(wind->RPort,15,113);
Text(wind->RPort, buq, 46);
}
/*****************************************************************/
void AbortFunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	ABORT=1;
else
	ABORT=0;
}
/*****************************************************************/
void InstallDisk()
{
int q,fp,x;
char *text;
char bufr[300];

ABORTbutton.gad_ptr->flags=ABORTbutton.gad_ptr->flags&(~BUTN_HIT);
refresh_gadget(ABORTbutton.gad_ptr);
ABORT=0;

if (((struct list *)DEVlist.gad_ptr)->active_entry!=NULL)
	{
	text=((struct list *)DEVlist.gad_ptr)->active_entry->text;
	sprintf(bufr,"on Device '%s'",text);
	if (askuser("Are you sure you want to Install Studio 16?",bufr,0))
		{
		q=((struct list *)DEVlist.gad_ptr)->current;
		sprintf(bufr,"Making Directory %c%sStudio16%c",34,text,34);
		telluser(bufr,"",NULL);

		if (!ABORT) 
			{
			if((fp=Open("s:Studio16Path.config",MODE_NEWFILE))==NULL) {telluser("ERROR OPENING FILE","   Open Error",NOTICE_RETURN_NOW);return;}
			sprintf(bufr,"%sStudio16",text);
			x=strlen(bufr);
			Write(fp,bufr,x);
			Close(fp);
			}
		print("INSTALLING Studio16   ");
		if (!ABORT) {sprintf(bufr,"MakeDir %c%sStudio16%c",34,text,34);		Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Assign OUT_: %c%sStudio16%c",34,text,34);	Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Assign IN_: %c%s:%c",34,DiskName,34);	Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:icon/Studio16.info %c%s%c",34,text,34);Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:runtime/About OUT_:Start");		Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:icon/Start.info OUT_:");		Execute(bufr,NULL,NULL);}

		if (!ABORT) print("INSTALLING RUNTIME    ");
		if (!ABORT) {sprintf(bufr,"MakeDir OUT_:RunTime");			Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:RUNTIME OUT_:Runtime ALL");		Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:icon/RunTime.info OUT_:");		Execute(bufr,NULL,NULL);}

		if (!ABORT) print("INSTALLING Cue List");
		if (!ABORT) {sprintf(bufr,"MakeDir OUT_:CueLists");			Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:lists OUT_:CueLists");	Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:icon/CueLists.info OUT_:");	Execute(bufr,NULL,NULL);}

		if (!ABORT) print("INSTALLING LIBRARY");
		if (!ABORT) {sprintf(bufr,"Copy IN_:libs libs: ALL");			Execute(bufr,NULL,NULL);}

		if (!ABORT) print("INSTALLING S");
		if (!ABORT) {sprintf(bufr,"Delete s:Studio16Base.config");				Execute(bufr,NULL,NULL);}
		if (!ABORT) {sprintf(bufr,"Copy IN_:s s: ALL");				Execute(bufr,NULL,NULL);}

		if (ABORT) print("ABORTED!!!");
		else {print("DONE.");telluser("Finished Installing Studio16","",NULL);}
		}
	}
else	telluser("NO DEVICE SELECTED!","Please select a device (volume) from the list above.",0);
}
/*****************************************************************/
/*****************************************************************/
void HelpFunc(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
	Signal(task,(1<<Help_sigbit));
else
	CloseHelp();
}
/*****************************************************************/
void CloseHelp()
{
if (Hwind)
	{
HELPbutton.gad_ptr->flags &= (~BUTN_HIT);
refresh_gadget(HELPbutton.gad_ptr);

	hide_gadget(HelpList.gad_ptr);
	delete_gadget_chain(Hwind);
	Hwind=CloseWind(Hwind);
	}
}
/*****************************************************************/
void OpenHelp()
{
if(!Hwind)
	{
	if ((Hwind=OpenWind(&HelpWind))==NULL)  {telluser("Can not open window!","",NOTICE_RETURN_NOW);return;}

	create_gadget_chain(Hwind,&HelpList);
	show_gadget(HelpList.gad_ptr);
	refresh_gadget(HelpList.gad_ptr);
	}
}
/*****************************************************************/
struct Window *OpenWindow();

static struct WowParms
	{
	LONG	Flags;
	struct NewWindow *NWind;
	struct Gadget *SizeGad;
	};

struct Gadget RESIZEgad = { NULL,NULL,NULL,NULL,NULL,GADGHCOMP,GADGIMMEDIATE|RELVERIFY,SIZING,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL };

/*******************************************************************/
struct window *OpenWind(nwind)
struct NewWindow *nwind;
{
struct Window *wind;
struct WowParms *wp;

if ((wp=(struct WowParms *)AllocMem(sizeof(struct WowParms),MEMF_CLEAR))==NULL) return(NULL);
wp->NWind=nwind;

if ((nwind->Flags&WINDOWSIZING)==WINDOWSIZING)
	{
	if ((wp->SizeGad=(struct Gadget *)AllocMem(sizeof(struct Gadget),NULL))==NULL) {FreeMem(wp,sizeof(struct WowParms));return(NULL);}
	CopyMem(&RESIZEgad,wp->SizeGad,sizeof(struct Gadget));

	nwind->Flags ^= WINDOWSIZING;
	if ((wind=OpenWindow(nwind))==NULL) {FreeMem(wp,sizeof(struct WowParms));FreeMem(wp->SizeGad,sizeof(struct Gadget));return(NULL);}
	nwind->Flags ^= WINDOWSIZING;

	wind->UserData=(BYTE *)wp;

	RefreshWind(wind);
	AddGadget(wind,wp->SizeGad,-1);
	}
else
	{
	if ((wind=OpenWindow(nwind))==NULL) {FreeMem(wp,sizeof(struct WowParms));return(NULL);}
	wind->UserData=(BYTE *)wp;
	RefreshWind(wind);
	}
SetFont(wind->RPort,fnt);

return(wind);
}
/*******************************************************************/
struct window *CloseWind(wind)
struct Window *wind;
{
struct WowParms *wp=(struct WowParms *)wind->UserData;
struct IntuiMessage *message;

if (wind)
	{
	wp->NWind->LeftEdge=wind->LeftEdge;
	wp->NWind->TopEdge=wind->TopEdge;
	wp->NWind->Width=wind->Width;
	wp->NWind->Height=wind->Height;

	if (wp->SizeGad)
		{
		RemoveGadget(wind,wp->SizeGad);
		FreeMem(wp->SizeGad,sizeof(struct Gadget));
		}
	if (wp) FreeMem(wp,sizeof(struct WowParms));

	while ((message=(struct IntuiMessage *)GetMsg(wind->UserPort))!=NULL)
		ReplyMsg(message);

	wind->UserData=NULL;
	CloseWindow(wind);
	}
return(NULL);
}
/*******************************************************************/
void RefreshWind(wind)
struct Window *wind;
{
struct WowParms *wp=(struct WowParms *)wind->UserData;
struct Gadget *RSG=wp->SizeGad;

SetAPen(wind->RPort, 0);
RectFill(wind->RPort, 2,11, wind->Width-3, wind->Height-2);

SetAPen(wind->RPort, 2);
RectFill(wind->RPort, 0 ,0, 1, wind->Height);

SetAPen(wind->RPort, 1);
RectFill(wind->RPort, wind->Width-2 ,0, wind->Width, wind->Height);
RectFill(wind->RPort, 1 , wind->Height-1, wind->Width, wind->Height);

if (RSG)
	{
	RSG->LeftEdge =wind->Width-12;
	RSG->TopEdge =wind->Height-6;
	RSG->Width =10;
	RSG->Height =5;

	SetAPen(wind->RPort,DARK_BROWN);
	Move(wind->RPort,RSG->LeftEdge,RSG->TopEdge+RSG->Height-1);
	Draw(wind->RPort,RSG->LeftEdge+RSG->Width-1,RSG->TopEdge);
	SetAPen(wind->RPort,WHITE);
	Move(wind->RPort,RSG->LeftEdge+2,RSG->TopEdge+RSG->Height-1);
	Draw(wind->RPort,RSG->LeftEdge+RSG->Width-1,RSG->TopEdge+1);
	}
}
/*******************************************************************/
void Resize()
{
RefreshWind(Hwind);

hide_gadget(HelpList.gad_ptr);
delete_gadget_chain(Hwind);
HelpList.box_x2=Hwind->Width-10;
HelpList.box_y2=Hwind->Height-5;
create_gadget_chain(Hwind,&HelpList);
show_gadget(HelpList.gad_ptr);
refresh_gadget(HelpList.gad_ptr);
}
/*******************************************************************/
