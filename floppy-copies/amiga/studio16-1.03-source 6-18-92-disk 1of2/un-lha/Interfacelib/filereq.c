#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "filereq.data"
#include "clib/macros.h"
#include <libraries/dos.h>
#include <libraries/dosextens.h>
#include <libraries/filehandler.h>
#include "stdio.h"
#include <dos.h>
#include "clib/macros.h"
/**********************************/
/**********************************/
#define NUMGADS 2
/**********************************/
/**********************************/
extern struct DosLibrary *DOSBase;
extern struct StudioBase *StudioBase;
char *curdir;
/**********************************/
/**********************************/
struct FileReqGlobes
	{
	struct Window *wind;
	long sigbit;
	struct Task *task;
	UBYTE FileSt[31];
	UBYTE PathSt[201];
	UBYTE UNDOBUFFER[201];
	UBYTE UNDOBUFFER2[31];
	struct StringInfo PathSInfo;
	struct Gadget PathStGad;
	struct StringInfo FileSInfo;
	struct Gadget FileStGad;
	struct NewWindow NewWind;
	struct NewList PATHlist;
	struct NewList FILElist;
	struct NewButton but[2];
	int flag;
	};
/**********************************/
/**********************************/
struct window *OpenWind();
struct window *CloseWind();
void RefreshWind();
struct Window *DisplayWind();
void HideWind();
void handle_intuievents();
void WindGadSize();
void GetFileNames();
void WindTitleChange();
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**************************************************************/
/*############################################################
NAME	
	FileRequest -
FUNCTION
	Opens up a file requestor window running under the calling process.
	Since file requester uses DOS functions it is illegal to call it
	from a task. Note that the path and the file must be combined to
	get the complete filename. The path will always end with a '/' or ':'
INPUTS
	title - Pointer to the string to be displayed in the Title Bar of
		the requester.
	path  - Pointer to the default path, this string is altered.
	name  - Pointer to the default filename, this string is altered.
	flags - should always be set to 'FR_SINGLE_SELECT' for future
		compatablity.
RESULTS
	char *path - a pointer to the new path string, or NULL if aborted.
##############################################################*/
char *FileRequest(title,path,name,flags)
char *title,*path,*name;
int flags;
{
int mask,x;
struct FileReqGlobes FRG,*frg=&FRG;/*AllocMem(sizeof(struct FileReqGlobes),MEMF_CLEAR);*/
/********************** Move in Gadgets *******************/
frg->PathSInfo=PathSInfo;
frg->FileSInfo=FileSInfo;
frg->PathSInfo.Buffer=(UBYTE *)&frg->PathSt[0];
frg->FileSInfo.Buffer=(UBYTE *)&frg->FileSt[0];
frg->PathSInfo.UndoBuffer=(UBYTE *)&frg->UNDOBUFFER[0];
frg->FileSInfo.UndoBuffer=(UBYTE *)&frg->UNDOBUFFER2[0];
/*****************************/
frg->FileStGad=FileStGad;
frg->PathStGad=PathStGad;
frg->FileStGad.NextGadget=&frg->PathStGad;
frg->FileStGad.SpecialInfo=(APTR)&frg->FileSInfo;
frg->PathStGad.SpecialInfo=(APTR)&frg->PathSInfo;
/*****************************/
frg->PATHlist=PATHlist;
frg->PATHlist.next=(struct NewGadgetHeader *)&frg->FILElist;
frg->FILElist=FILElist;
frg->FILElist.next=(struct NewGadgetHeader *)&frg->but[1];
/*****************************/
frg->but[0]=but[0];
frg->but[1]=but[1];
frg->but[1].next=(struct NewGadgetHeader *)&frg->but[0];
/*****************************/
/**********************************************************/
frg->NewWind=NewWind;
if (title) frg->NewWind.Title=title;
/**********************************************************/
strcpy(frg->FileSt,name);
strcpy(frg->PathSt,path);

frg->task=(struct Task *)FindTask(NULL);

DisplayWind(frg); if (frg->wind==0) return(0);

GetFileNames(frg->PathSt,frg);

if ( (frg->sigbit=AllocSignal(-1))==-1) {telluser("File Request:","Cant get SigBit",0);return(0);}

frg->flag=0;
while (frg->flag==0)
	{
	mask=(1<<frg->sigbit);
	if (frg->wind) mask |= (1<<frg->wind->UserPort->mp_SigBit);
	mask=Wait(mask);
	if (frg->wind) if(mask&(1<<frg->wind->UserPort->mp_SigBit)) handle_intuievents(frg);
	if (mask & (1<<frg->sigbit) && (frg->flag==0)) GetFileNames(frg->PathSt,frg);
	}

if (frg->sigbit!=-1) FreeSignal(frg->sigbit);

if (frg->flag==1) {HideWind(frg);return(NULL);}
else 
	{
	strcpy(name,frg->FileSt);
	strcpy(path,frg->PathSt);
	x=stclen(path);
	if (path[x-1]!='/' && path[x-1]!=':') strcat(path,&"/");
	HideWind(frg);
	return(path);
	}
}
/**************************************************************/
/**************************************************************/
void handle_intuievents(frg)
struct FileReqGlobes *frg;
{
struct IntuiMessage *message;
ULONG class;
struct Gadget *gad;

while ((message=(struct IntuiMessage *)GetMsg(frg->wind->UserPort))!=0)
	{
	HandleIntuitMess(frg->wind,message);
	class=message->Class;
	gad=(struct Gadget *)message->IAddress;
	ReplyMsg(message);

	if (class == NEWSIZE)
		{
		WindGadSize(frg);
		}

	if (class == CLOSEWINDOW)
		{
		frg->flag=1;
		}

	if (class == GADGETUP)
		{
		if (gad==&frg->FileStGad) ActivateGadget(&frg->PathStGad,frg->wind,NULL);
		if (gad==&frg->PathStGad) {ActivateGadget(&frg->FileStGad,frg->wind,NULL);
		Signal(frg->task,(1<<frg->sigbit));}
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
struct Window *DisplayWind(frg)
struct FileReqGlobes *frg;
{
struct Window *OpenNewWind();
void DisplayHandlerWind();
void GetDevsNames();
int x,w,i,s;
struct NewWindow NW=frg->NewWind;

if (frg->wind = OpenNewWind(&NW,NULL,NULL,NULL))
	{
	ColorGadList(&frg->PATHlist);

	s=((frg->wind->Width-20)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		frg->but[i].box_x1=(x>>8);
		frg->but[i].box_x2=(x+w)>>8; x=x+w+(5<<8);
		frg->but[i].box_y1=frg->wind->Height-16;
		frg->but[i].box_y2=frg->wind->Height-6;
		}

	frg->FILElist.box_x2=((frg->wind->Width-30)*3)/4-5;
	frg->FILElist.box_y2=frg->wind->Height-47;

	frg->PATHlist.box_x1=((frg->wind->Width-30)*3)/4+4;
	frg->PATHlist.box_x2=frg->wind->Width-11;
	frg->PATHlist.box_y2=frg->wind->Height-47;

	if (create_gadget_chain(frg->wind,&frg->PATHlist)==0) {telluser("File Request:","Eminant Disaster",0);return(0);}   /* FIX XIF */

	frg->PATHlist.gad_ptr->user_data=(APTR)frg;
	frg->FILElist.gad_ptr->user_data=(APTR)frg;
	frg->but[0].gad_ptr->user_data=(APTR)frg;
	frg->but[1].gad_ptr->user_data=(APTR)frg;

	GetDevsNames(frg);

	AddGList(frg->wind,&frg->FileStGad,NULL,2,NULL);

/*	WindGadSize(frg);*/
	frg->FileStGad.TopEdge=frg->wind->Height-29;
	frg->PathStGad.TopEdge=frg->wind->Height-41;
	frg->FileStGad.Width=MIN( ((frg->wind->Width-70)>>3)<<3,29*8 );
	frg->PathStGad.Width=((frg->wind->Width-70)>>3)<<3;
	RefreshGList(&frg->FileStGad,frg->wind,NULL,2);

	draw_indent(frg->wind->RPort,frg->FileStGad.LeftEdge-2,frg->FileStGad.TopEdge-1,MIN(frg->FileStGad.Width+frg->FileStGad.LeftEdge+1,276+15),frg->FileStGad.TopEdge+frg->FileStGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(frg->wind->RPort,frg->PathStGad.LeftEdge-2,frg->PathStGad.TopEdge-1,frg->PathStGad.Width+frg->PathStGad.LeftEdge+1,frg->PathStGad.TopEdge+frg->PathStGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(frg->wind->RPort,frg->PathStGad.LeftEdge-(8*6),frg->PathStGad.TopEdge+6);
	Text(frg->wind->RPort,&"Path:", 5);
	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(frg->wind->RPort,frg->PathStGad.LeftEdge-(8*6)+1,frg->PathStGad.TopEdge+6+1);
	Text(frg->wind->RPort,&"Path:", 5);

	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(frg->wind->RPort,frg->FileStGad.LeftEdge-(8*6),frg->FileStGad.TopEdge+6);
	Text(frg->wind->RPort,&"File:", 5);
	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(frg->wind->RPort,frg->FileStGad.LeftEdge-(8*6)+1,frg->FileStGad.TopEdge+6+1);
	Text(frg->wind->RPort,&"File:", 5);
/*	WindGadSize(frg);*/


	return(frg->wind);
	}
else telluser("File Request:","Can not open window!",0);
return(0);
}
/**************************************************************/
/**************************************************************/
void HideWind(frg)
struct FileReqGlobes *frg;
{
if (frg->wind)
	{
/*	NewWind.LeftEdge=frg->wind->LeftEdge;
	NewWind.TopEdge=frg->wind->TopEdge;*/
	NewWind.Height=frg->wind->Height;
	NewWind.Width=frg->wind->Width;
	hide_gadget(frg->PATHlist.gad_ptr);
	hide_gadget(frg->FILElist.gad_ptr);
	delete_gadget_chain(frg->wind);
	CloseWind(frg->wind);
	}
}
/**************************************************************/
/**************************************************************/
void PathFunc(g)
struct	list *g;
{
void DisplayHandlerWind();
struct FileReqGlobes *frg=(struct FileReqGlobes *)g->user_data;

if(g->flags & LIST_DOUBLESELECT)
	{
	strcpy(frg->PathSt,g->active_entry->text);
	RefreshGList(&frg->PathStGad,frg->wind,NULL,1);
	Signal(frg->task,(1<<frg->sigbit));
	}
}
/**************************************************************/
/**************************************************************/
void FileFunc(g)
struct	list *g;
{
struct FileReqGlobes *frg=(struct FileReqGlobes *)g->user_data;
int x,y;
char txt[100];

if(g->flags & LIST_DOUBLESELECT && *g->active_entry->text!='(')
	{
	frg->flag=2;
	}
if(g->flags & LIST_NEWSELECT && g->active_entry)
	{
	if(strcmp(&"(DIR) /Parent",g->active_entry->text)==0)
		{
		y=strlen(frg->PathSt)-2;
		for(x=y;x>0 && frg->PathSt[x]!='/' && frg->PathSt[x]!=':';x--) ;
		if (x!=0) frg->PathSt[x+1]=0;
		RefreshGList(&frg->PathStGad,frg->wind,NULL,1);
		Signal(frg->task,(1<<frg->sigbit));
		}
	else
		{
		if(*g->active_entry->text=='(')
			{
			if(stclen(frg->PathSt)) if(frg->PathSt[stclen(frg->PathSt)-1]!='/' && frg->PathSt[stclen(frg->PathSt)-1]!=':')	strcat(frg->PathSt,&"/");
			strcat(frg->PathSt,g->active_entry->text+6);
			strcat(frg->PathSt,&"/");
			RefreshGList(&frg->PathStGad,frg->wind,NULL,1);
			Signal(frg->task,(1<<frg->sigbit));
			}
		else
			{
			strcpy(txt,g->active_entry->text);
			txt[g->active_entry->user_data]=0;
			strcpy(frg->FileSt,txt);
			RefreshGList(&frg->FileStGad,frg->wind,NULL,1);
			}
		}
	}
}
/**************************************************************/
/**************************************************************/
void GetDevsNames(frg)
struct FileReqGlobes *frg;
{
LONG blen;
struct DeviceNode *dn;
struct DosInfo *di;
struct RootNode *root;
UBYTE *bstring;
char buf[120];
int i=0;
char *NoShow();

root = (struct RootNode *)DOSBase->dl_Root;
di = (struct DosInfo *)BADDR(root->rn_Info);
dn = (struct DeviceNode *) BADDR(di->di_DevInfo);

new_pointer(PTR_BUSY);
hide_gadget(frg->PATHlist.gad_ptr);
Forbid();
for (; dn != 0 ; dn = (struct DeviceNode *)BADDR(dn->dn_Next))
	{
	bstring = (UBYTE *)BADDR(dn->dn_Name);
	blen = (LONG)bstring[0];
	CopyMem((BYTE *)(bstring + 1),buf, (LONG)blen);
	buf[blen]=':'; buf[blen+1]=NULL;
	if (dn->dn_Task!=0) 
		if (!(dn->dn_Type==1 && dn->dn_Handler!=0))
			if (NoShow(buf)) add_list_entry(frg->PATHlist.gad_ptr, buf, i++);
 	}
Permit();
show_gadget(frg->PATHlist.gad_ptr);
refresh_gadget(frg->PATHlist.gad_ptr);
new_pointer(PTR_NORMAL);
}
/************************************************************************/
/************************************************************************/
char *NoShow(dname)
char *dname;
{
int x=0;
char *noshow[]	= {
	"REXX:",
	"S:",
	"C:",
	"CLIPS:",
	"DEVS:",
	"ENV:",
	"FONTS:",
	"L:",
	"LIBS:",
	"",
	};
while (*noshow[x]!=0)
	if (stricmp(noshow[x],dname)) x++;
	else return(0);
return(dname);
}
/**************************************************************/
/**************************************************************/
void GetFileNames(path,frg)
char *path;
struct FileReqGlobes *frg;
{
char text[100],text2[100];
short count=-1,size,x;
struct FileLock      *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;

hide_gadget(frg->FILElist.gad_ptr);
while(((struct list *)(frg->FILElist.gad_ptr))->list_size)
	{
	delete_list_entry_by_index(frg->FILElist.gad_ptr,0);
	}
show_gadget(frg->FILElist.gad_ptr);
refresh_gadget(frg->FILElist.gad_ptr);
new_pointer(PTR_BUSY);

size=(((struct list *)(frg->FILElist.gad_ptr))->box_y2 - ((struct list *)(frg->FILElist.gad_ptr))->box_y)/8 -1;

strcpy(text,path);

hide_gadget(frg->FILElist.gad_ptr);
if (path[stclen(path)-1]!=':') 
	{
	add_list_entry(frg->FILElist.gad_ptr,"(DIR) /Parent", 0);
	count++;
	}

if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) {new_pointer(PTR_NORMAL);telluser("File Request:","Disk directory does not exist.",0);return;}
if ( ! Examine(fkey, dinfo) ) {new_pointer(PTR_NORMAL);telluser("File Request:","Disk Error.",0);UnLock(fkey);return;}
if ( dinfo->fib_DirEntryType < 0L ) {new_pointer(PTR_NORMAL);telluser("File Request:","Path can not include File Name.",0);UnLock(fkey);return;}

while( ExNext( fkey, dinfo ) )
	{
	count++;
	if (count==size)
		{
		show_gadget(frg->FILElist.gad_ptr);
		refresh_gadget(frg->FILElist.gad_ptr);
		hide_gadget(frg->FILElist.gad_ptr);
		}

		if (dinfo->fib_EntryType<0) 
			{
			x=stclen(dinfo->fib_FileName);
			sprintf(text2,"%-31s %8d",dinfo->fib_FileName,dinfo->fib_Size);
			add_list_entry(frg->FILElist.gad_ptr,text2, x);
			}
		else 
			{
			sprintf(text2,"(DIR) %s",dinfo->fib_FileName);
			add_list_entry(frg->FILElist.gad_ptr,text2, -1);
			}
	}

UnLock(fkey);

show_gadget(frg->FILElist.gad_ptr);
refresh_gadget(frg->FILElist.gad_ptr);
new_pointer(PTR_NORMAL);
}
/**************************************************************/
/**************************************************************/
void WindGadSize(frg)
struct FileReqGlobes *frg;
{
int x,w,i,s;

if (frg->wind)
	{
	RefreshWind(frg->wind);
	new_gadget_size(frg->FILElist.gad_ptr,10,16,((frg->wind->Width-30)*3)/4-5,frg->wind->Height-47);
	new_gadget_size(frg->PATHlist.gad_ptr,((frg->wind->Width-30)*3)/4+4,16,frg->wind->Width-11,frg->wind->Height-47);
	refresh_gadget(frg->FILElist.gad_ptr);
	refresh_gadget(frg->PATHlist.gad_ptr);

	frg->FileStGad.TopEdge=frg->wind->Height-29;
	frg->PathStGad.TopEdge=frg->wind->Height-41;
	frg->FileStGad.Width=MIN( ((frg->wind->Width-70)>>3)<<3,29*8 );
	frg->PathStGad.Width=((frg->wind->Width-70)>>3)<<3;
	RefreshGList(&frg->FileStGad,frg->wind,NULL,2);

	draw_indent(frg->wind->RPort,frg->FileStGad.LeftEdge-2,frg->FileStGad.TopEdge-1,MIN(frg->FileStGad.Width+frg->FileStGad.LeftEdge+1,276+15),frg->FileStGad.TopEdge+frg->FileStGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);
	draw_indent(frg->wind->RPort,frg->PathStGad.LeftEdge-2,frg->PathStGad.TopEdge-1,frg->PathStGad.Width+frg->PathStGad.LeftEdge+1,frg->PathStGad.TopEdge+frg->PathStGad.Height,StudioBase->defaults.colors.gadget_window.bottomright_3d,StudioBase->defaults.colors.gadget_window.topleft_3d);

	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(frg->wind->RPort,frg->PathStGad.LeftEdge-(8*6),frg->PathStGad.TopEdge+6);
	Text(frg->wind->RPort,&"Path:", 5);
	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(frg->wind->RPort,frg->PathStGad.LeftEdge-(8*6)+1,frg->PathStGad.TopEdge+6+1);
	Text(frg->wind->RPort,&"Path:", 5);

	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	Move(frg->wind->RPort,frg->FileStGad.LeftEdge-(8*6),frg->FileStGad.TopEdge+6);
	Text(frg->wind->RPort,&"File:", 5);
	SetAPen(frg->wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	Move(frg->wind->RPort,frg->FileStGad.LeftEdge-(8*6)+1,frg->FileStGad.TopEdge+6+1);
	Text(frg->wind->RPort,&"File:", 5);

	s=((frg->wind->Width-20)<<8) - ((5<<8)*(NUMGADS-1));
	w=(s/NUMGADS);
	x=10<<8;

	for(i=0;i<NUMGADS;i++)
		{
		new_gadget_size(frg->but[i].gad_ptr,(x>>8),frg->wind->Height-16,(x+w)>>8,frg->wind->Height-6);
		refresh_gadget(frg->but[i].gad_ptr);
		x=x+w+(5<<8);
		}
	}
}
/**************************************************************/
/**************************************************************/
void GadFunc(g)
struct	button *g;
{
struct FileReqGlobes *frg=(struct FileReqGlobes *)g->user_data;
frg->flag=1;
Signal(frg->task,(1<<frg->sigbit));
}
/**************************************************************/
/**************************************************************/
void OKGadFunc(g)
struct	button *g;
{
struct FileReqGlobes *frg=(struct FileReqGlobes *)g->user_data;
frg->flag=2;
Signal(frg->task,(1<<frg->sigbit));
}
/**************************************************************/
/**************************************************************/
