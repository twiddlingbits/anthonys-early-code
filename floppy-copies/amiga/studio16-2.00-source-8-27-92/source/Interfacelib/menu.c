#include "exec/types.h"
#include <exec/lists.h>
#include <exec/libraries.h>
#include <exec/ports.h>
#include <exec/semaphores.h>
#include <stdio.h>
#include "intuition/intuition.h"
#include "exec/memory.h"
#include "clib/macros.h"
#include "/include/studio16.h"
#include "/include/interface.h"

extern struct StudioBase *StudioBase;

extern char stTopaz[];

extern struct TextAttr taPlain;
struct TextAttr taHEAD =
{
	stTopaz, 8, FSF_ITALIC , FPF_ROMFONT
};

char a2k[] =
	{
	'`','1','2','3','4','5','6','7',
	'8','9','0','-','=',000,000,000,
	'Q','W','E','R','T','Y','U','I',
	'O','P','[',']',000,000,000,000,
	'A','S','D','F','G','H','J','K',
	'L',';',000,000,000,000,000,000,
	000,'Z','X','C','V','B','N','M',
	000,'.','/',
	};

#define AWindC {2,1,6,7,7,1,1}
#define IWindC {2,1,6,2,2,1,1}
#define SWindC {0,0,0,0,0,0,0}

static struct HitAreaColor AWindClr = {2,1,6,7,7,5,5};
static struct HitAreaColor IWindClr = {2,1,6,2,2,5,5};

static struct GadgetColors Acolors = {
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	AWindC,
	};

static struct GadgetColors Icolors = {
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	IWindC,
	};

static struct GadgetColors Scolors = {
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	SWindC,
	};

#define SYScolors Icolors

void ViewFile();
void RefreshBar();
void AddTitBarGads();
void DelTitBarGads();
void SaveGadgetPlacement();
void CloseMenuList();

/***********************************************************/
void LaunchMod(file,flag)
char *file,flag;
{
struct Module *mod;
char temp[100];

new_pointer(PTR_BUSY);
mod=(struct Module *)FindModule(file);
if (mod==NULL)
	{
	if (flag)
		{
		sprintf(temp,"Apps/%s",file);
		mod=(struct Module *)LoadModule(temp);
		}
	else
		{ 
		sprintf(temp,"ProjectMenu/%s",file);
		mod=(struct Module *)LoadModule(temp);
		}
	BroadcastEventParms(GetEventID("SE_CMD_SELECT_MODULE"),mod,0,0,0,0,0);
	}
else
	{
	if (mod->state)
		{
		if(mod->state->window)
			{
			if(mod->state->window->RPort->Layer==mod->state->window->RPort->Layer->LayerInfo->top_layer)
				{
				WindowToBack(mod->state->window);
				ActivateWindow(mod->state->window);
				}
			else
				{
				WindowToFront(mod->state->window);
				ActivateWindow(mod->state->window);
				}
			}
		else BroadcastEventParms(GetEventID("SE_CMD_SELECT_MODULE"),mod,0,0,0,0,0);
		}
	}
new_pointer(PTR_NORMAL);
}

/*############################################################
NAME	
	HandleIntuitMess -
FUNCTION
	This function is a wedge for recieving intuition messages.  It should
	always be called right after recieing intuition messages.  It handles
	changing the title bar colors.
INPUTS
	wind - a pointer to the callers window structure
	message - a pointer to the intuition message just recieved.
RESULTS
	NONE
##############################################################*/
void 	HandleIntuitMess(wind,message)
struct Window *wind;
struct IntuiMessage *message;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;
ULONG class=message->Class;
ULONG code=message->Code;
USHORT qual=message->Qualifier;
void MenusOn();
void DelMenuBar();
int *ItemData();
int x;
char *file,chr;
struct MenuItem *MI=NULL;
struct MenuItemPlus *MIP=NULL;

if (class == ACTIVEWINDOW) RefreshBar(wind,1);
if (class == INACTIVEWINDOW) RefreshBar(wind,1);
if (class == RAWKEY) 
	{
/*kprintf("RAWKEY: %x%x  %x%x",code,qual);*/
	if (qual&0x0008)
		{
		if (code<0x3b)
			{
			chr=a2k[code];
			if (chr)
				{
				MI=wp->Project[0].FirstItem;
				while(MI) 
					{
					MIP=(struct MenuItemPlus *)MI;
					if (chr==MIP->Key)
						{
						file=(char *)ItemData(MI);
						if (file) LaunchMod(file,0);
						return;
						}
					MI=MI->NextItem;
					}

				MI=wp->Project[1].FirstItem;
				while(MI) 
					{
					MIP=(struct MenuItemPlus *)MI;
					if (chr==MIP->Key)
						{
						file=(char *)ItemData(MI);
						if (file) LaunchMod(file,1);
						return;
						}
					MI=MI->NextItem;
					}
				}
			}
		}
	}
if (class == MENUPICK)
	{
/*	kprintf("MENU:%x%x   Item:%x%x\n",MENUNUM(code),ITEMNUM(code));*/
	if (MENUNUM(code)==0)
		{
		x=ITEMNUM(code);
		if (x<20)
			{
			MI=wp->Project[0].FirstItem;
			while(x) {MI=MI->NextItem;x--;}
			if(MI) file=(char *)ItemData(MI);

			if (file) LaunchMod(file,0);
			}
		}

	if (MENUNUM(code)==1)
		{
		x=ITEMNUM(code);
		if (x<20)
			{
			MI=wp->Project[1].FirstItem;
			while(x) {MI=MI->NextItem;x--;}
			if(MI) file=(char *)ItemData(MI);

			if (file) LaunchMod(file,1);
			}
		}
	}
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
void SortList(pointer)
char *pointer[];
{
int x=0,flag=1;
char *temp;

while(pointer[x++]);
if (x<3) return;

while (flag)
	{
	flag=0;
	x=1;
	while(pointer[x])
		{
		if (stricmp(pointer[x],pointer[x-1]) <0)
			{
			flag=1;
			temp=pointer[x];
			pointer[x]=pointer[x-1];
			pointer[x-1]=temp;
			}
		x++;
		}
	}
}
/****************************************************************/
/****************************************************************/
struct MenuItem *CreateMenuItemNoSpace(Name,Data,Key,Last)
char *Name;
struct MenuItem *Last;
char Key;
int *Data;
{
char *str;
struct IntuiText *Text,IText =	{ 6, 1, JAM1, 0, 1, &taPlain,NULL, NULL };
struct MenuItemPlus *MenuP,*LastP;
struct MenuItem *Menu,MenuI =
	{
	NULL,0, 0, 104+8*7, 10, ITEMENABLED | ITEMTEXT| HIGHCOMP,
	NULL, NULL, NULL, NULL, NULL, NULL 
	};


Menu=(struct MenuItem *)AllocMem(sizeof(struct MenuItemPlus),MEMF_CLEAR);
if (!Menu) telluser("MENU BUILDER:","Could not allocate MENUITEM",NULL);
*Menu=MenuI;
MenuP=(struct MenuItemPlus *)Menu;
LastP=(struct MenuItemPlus *)Last;
MenuP->Data=Data;
MenuP->Num=0;

if (Last)
	{ 
	Menu->TopEdge=Last->TopEdge+10;
	Menu->LeftEdge=Last->LeftEdge;
	Menu->NextItem=Last;
	MenuP->Num=LastP->Num+1;
	}
else Menu->NextItem=NULL;


Text=(struct IntuiText *)AllocMem(sizeof(struct IntuiText),NULL);
if (!Text) telluser("MENU BUILDER:","Could not allocate INTUITEXT",NULL);
*Text=IText;

str=(char *)AllocMem(21,NULL);
if (!str) telluser("MENU BUILDER:","Could not allocate STRING",NULL);

if (Key)
	{
	sprintf(str," %-16s",Name);
	str[17]=NULL;
	sprintf(str,"%s ^%c",str,Key);
	MenuP->Key=Key;
	}
else sprintf(str," %-19s",Name);

((struct IntuiText *)(Menu->ItemFill))=Text;
((struct IntuiText *)(Menu->ItemFill))->IText=str;
return(Menu);
}
/****************************************************************/
char ProjectData[1024],*ProjectList[21];
char ApplicationData[1024],*ApplicationList[21];
/****************************************************************/
void BuildProjectMenuList(path)
char *path;
{
void ReadDir();
int x=0;

ReadDir(ProjectData,ProjectList,path);
while (ProjectList[x])
	{
	if (stricmp(ProjectList[x],&"Quit")==0) strcpy(ProjectList[x],&"zzzz");
	x++;
	}
SortList(ProjectList);
x=0;
while (ProjectList[x])
	{
	if (stricmp(ProjectList[x],&"zzzz")==0) strcpy(ProjectList[x],&"Quit");
	x++;
	}
}
/****************************************************************/
/****************************************************************/
struct MenuItem *BuildProjectMenu()
{
struct MenuItem *MI=NULL;
int x=0;
struct MenuItem *CreateMenuItem();

while (ProjectList[x]) 
	{
	MI=CreateMenuItemNoSpace(ProjectList[x],ProjectList[x],*(ProjectList[x++]-1),MI);
	}

return(MI);
}
void FreeProjectMenu(MenuI)
struct MenuItem *MenuI;
{
void FreeChannelMenu();
FreeChannelMenu(MenuI);
}
/****************************************************************/
/****************************************************************/
void BuildApplicationMenuList(path)
char *path;
{
void ReadDir();
ReadDir(ApplicationData,ApplicationList,path);
SortList(ApplicationList);
}
/****************************************************************/
/****************************************************************/
struct MenuItem *BuildApplicationMenu()
{
struct MenuItem *MI=NULL;
int x=0;
struct MenuItem *CreateMenuItem();

while (ApplicationList[x]) 
	{
	MI=CreateMenuItemNoSpace(ApplicationList[x],ApplicationList[x],*(ApplicationList[x++]-1),MI);
	}

return(MI);
}
void FreeApplicationMenu(MenuI)
struct MenuItem *MenuI;
{
void FreeChannelMenu();
FreeChannelMenu(MenuI);
}
/****************************************************************/
/****************************************************************/
void ReadDir(data,pointer,path)
char *data;
char *pointer[];
char *path;
{
struct FileLock      *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;
int x=0;

if (! (fkey=(struct FileLock *)Lock(path,ACCESS_READ)) ) 
	{
	telluser("InterfaceLib:","Studio16: directory does not exist.",0);
	return;
	}

if ( ! Examine(fkey, dinfo) ) 
	{
	telluser("InterfaceLib:","Disk Error (reading module names).",0);
	UnLock(fkey);
	return;
	}

if ( dinfo->fib_DirEntryType < 0L ) 
	{
	telluser("InterfaceLib:","Path can not include File Name.",0);
	UnLock(fkey);
	return;
	}

while( ExNext( fkey, dinfo ) )
	{
	if (dinfo->fib_EntryType<0) 
		{
		if (!dotinfo(dinfo->fib_FileName))
			{
			if (x<20)
				{
				if (strlen(dinfo->fib_Comment)==1)
					{
					*data=dinfo->fib_Comment[0];data++;
					}
				else
					{
					*data=NULL;data++;
					}
				pointer[x++]=data;
				data=(char *)stpcpy(data,dinfo->fib_FileName);
				data++;
				}
			}
		}
	}
pointer[x++]=NULL;
UnLock(fkey);
}
/****************************************************************/
/****************************************************************/
int dotinfo(str)
char *str;
{
short len;

len = strlen(str);
if (len < 5)
	return(FALSE);

if (strcmp(str+len-5,".info")==0)
	return(TRUE);
else
	return(FALSE);
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
void AddTitBarGads(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;

if (wp->Flags&WINDOWSIZING)	AddGadget(wind,&wp->SizeGad,-1);
if (wp->Flags&WINDOWCLOSE)	wp->CloseButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWDEPTH)	wp->DepthButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWSIZING)	wp->DSizeButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags2&PREFERENCES)	wp->PrefsButton.gad_ptr->flags&=~HIDE_GADGET;
if (wp->Flags&WINDOWDRAG)	AddGadget(wind,&wp->DragGad,-1);
}
/****************************************************************/
/****************************************************************/
void DelTitBarGads(wind)
struct Window *wind;
{
struct WindowParms *wp=(struct WindowParms *)wind->UserData;

	if (wp->Flags&WINDOWSIZING)	RemoveGadget(wind,&wp->SizeGad);
	if (wp->Flags&WINDOWDRAG)	RemoveGadget(wind,&wp->DragGad);
	if (wp->Flags&WINDOWCLOSE)	wp->CloseButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags&WINDOWDEPTH)	wp->DepthButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags&WINDOWSIZING)	wp->DSizeButton.gad_ptr->flags|=HIDE_GADGET;
	if (wp->Flags2&PREFERENCES)	wp->PrefsButton.gad_ptr->flags|=HIDE_GADGET;
}
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/
/****************************************************************/

/****************************************************************/
/****************************************************************/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct MenuItem *CreateMenuItem(Name,Data,Last)
char *Name;
struct MenuItem *Last;
int *Data;
{
char *str;
struct IntuiText *Text,IText =	{ 6, 1, JAM1, 0, 1, &taPlain,NULL, NULL };
struct MenuItemPlus *MenuP,*LastP;
struct MenuItem *Menu,MenuI =
	{
	NULL,0, 0, 104, 10, ITEMENABLED | ITEMTEXT| CHECKIT | HIGHCOMP,
	NULL, NULL, NULL, NULL, NULL, NULL 
	};

Menu=(struct MenuItem *)AllocMem(sizeof(struct MenuItemPlus),MEMF_CLEAR);
if (!Menu) telluser("MENU BUILDER:","Could not allocate MENUITEM",NULL);
*Menu=MenuI;
MenuP=(struct MenuItemPlus *)Menu;
LastP=(struct MenuItemPlus *)Last;
MenuP->Data=Data;
MenuP->Num=0;

if (Last)
	{ 
	Menu->TopEdge=Last->TopEdge+10;
	Menu->LeftEdge=Last->LeftEdge;
	Menu->NextItem=Last;
	MenuP->Num=LastP->Num+1;
	}
else Menu->NextItem=NULL;

Text=(struct IntuiText *)AllocMem(sizeof(struct IntuiText),NULL);
if (!Text) telluser("MENU BUILDER:","Could not allocate INTUITEXT",NULL);
*Text=IText;

str=(char *)AllocMem(21,NULL);
if (!str) telluser("MENU BUILDER:","Could not allocate STRING",NULL);
sprintf(str,"   %-10s",Name);


((struct IntuiText *)(Menu->ItemFill))=Text;
((struct IntuiText *)(Menu->ItemFill))->IText=str;
return(Menu);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct MenuItem *CreateMenuHeader(Name,Last)
char *Name;
struct MenuItem *Last;
{
char *str;
struct IntuiText *Text,IText =	{ 7, 1, JAM1, 0, 1, &taHEAD,NULL, NULL };
struct MenuItemPlus *MenuP,*LastP;
struct MenuItem *Menu,MenuI =
	{
	NULL,0, 0, 102, 10, ITEMENABLED | ITEMTEXT,
	NULL, NULL, NULL, NULL, NULL, NULL 
	};

Menu=(struct MenuItem *
)AllocMem(sizeof(struct MenuItemPlus),MEMF_CLEAR);
if (!Menu) telluser("MENU BUILDER:","Could not allocate MENUITEM",NULL);
*Menu=MenuI;
MenuP=(struct MenuItemPlus *)Menu;
LastP=(struct MenuItemPlus *)Last;
MenuP->Data=NULL;
MenuP->Num=0;

if (Last)
	{
	Menu->TopEdge=0;
	Menu->LeftEdge=Last->LeftEdge+105;
	Menu->NextItem=Last;
	MenuP->Num=LastP->Num+1;
	}
else Menu->NextItem=NULL;

Text=(struct IntuiText *)AllocMem(sizeof(struct IntuiText),NULL);
if (!Text) telluser("MENU BUILDER:","Could not allocate INTUITEXT",NULL);
*Text=IText;

str=(char *)AllocMem(21,NULL);
if (!str) telluser("MENU BUILDER:","Could not allocate STRING",NULL);
sprintf(str,"   %s",Name);


((struct IntuiText *)(Menu->ItemFill))=Text;
((struct IntuiText *)(Menu->ItemFill))->IText=str;
return(Menu);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void ToggleMode(MenuI)
struct MenuItem *MenuI;
{
while(MenuI)
	{
	MenuI->Flags|=MENUTOGGLE;
	MenuI=MenuI->NextItem;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void SetMutual(MenuI)
struct MenuItem *MenuI;
{
struct MenuItem *NextI;

int x=0;

NextI=MenuI;

while(MenuI)
	{
	NextI=MenuI->NextItem;
	MenuI->MutualExclude=0xFFFFFFFF-(1<<x++);
	MenuI=NextI;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*############################################################
NAME	BuildChannelMenu
FUNCTION
	Builds a linked list of MenuItems each containing a chankey
	the list may be built for specific needs by using the search
	parameter. For Instance if search=CK_CHAN_HILO|CH_CHAN_PLAY,
	a list of play channels that could display HILO info would
	be built.
	The other parameter 'flags' it used to determine what kind
	of menu you wish, if NULL a single select list is created
	if MENU_MULTI_SELECT multiple items may be selected
INPUTS
	search - ChanKey flags that describe the type of channels
		you want
	flags - internal use flags used to build different types
		of menus.
RESULTS
	(struct MenuItem *) of the first item of the menu.
##############################################################*/
struct MenuItem *BuildChannelMenu(search,flags)
int search,flags;
{
void SetMutual();
void ToggleMode();
int x;
struct AudioHandler *hand=NULL;
struct MenuItem *MI=NULL;

hand = (struct AudioHandler *) FindAudioHandler(NULL,-1,0,NULL,-1);
while(hand)
	{
	MI=CreateMenuHeader(hand->name,MI);
	for(x=0;x<hand->number_of_channels;x++)
		{
		if (search & hand->channel[x].flags) MI=CreateMenuItem(hand->channel[x].name,&hand->channel[x],MI);
		}
	hand = (struct AudioHandler *) FindAudioHandler(hand,-1,0,NULL,-1);
	}
if (flags&MENU_MULTI_SELECT) ToggleMode(MI);
else SetMutual(MI);

return(MI);
}
/*############################################################
NAME	FreeChannelMenu
FUNCTION
	Undos all memory allocation of BuildChannelMenu
INPUTS
	(struct MenuItem *) of the first item of a menu
RESULTS
	NONE
##############################################################*/
void FreeChannelMenu(MenuI)
struct MenuItem *MenuI;
{
struct MenuItem *NextI;

NextI=MenuI;

while(MenuI)
	{
	NextI=MenuI->NextItem;

	FreeMem(((struct IntuiText *)(MenuI->ItemFill))->IText,21);
	FreeMem(MenuI->ItemFill,sizeof(struct IntuiText));
	FreeMem(MenuI,sizeof(struct MenuItemPlus));
	MenuI=NextI;
	}
}
/*############################################################
NAME	MenuCheckSum
FUNCTION
	Returns a checksum for a menu. This is useful for 
	determining whether a new handler has been added
	to the system
INPUTS
	(struct MenuItem *) to the first MenuItem of the list
RESULTS
	(int) check sum number
##############################################################*/
int MenuCheckSum(FirstMI)
struct MenuItem *FirstMI;
{
struct MenuItem *MenuI=FirstMI;
int x,y=1,sum=0;
byte *text;

while(MenuI)
	{
	text=((struct IntuiText *)(MenuI->ItemFill))->IText;
	if (text)
		{
		x=0;
		while (text[x])
			{
			sum=sum+text[x]*y;
			x++;
			}
		}
	y++;
	MenuI=MenuI->NextItem;
	}
return(sum);
}
/*############################################################
NAME	FindCheckedItem
FUNCTION
	Finds the next occurance of a checked item;
	to find all checked items;

	t=0;
	mi=Titles.FirstItem;
	while ((mi=FindCheckedItem(mi)))
		{
		Chan[t]=ItemChan(mi);
		t++;
		mi=mi->NextItem;
		}
	NumOfItemsChecked=t;

INPUTS
	struct MenuItem *
RESULTS
	(struct MenuItem *) if an item was checked or NULL
##############################################################*/
struct MenuItem *FindCheckedItem(MenuI)
struct MenuItem *MenuI;
{
while(MenuI && !(MenuI->Flags&CHECKED))
	{
	MenuI=MenuI->NextItem;
	}

if (MenuI)
	if (MenuI->Flags&CHECKED)
		{
		return(MenuI);
		}

return(NULL);
}
/*############################################################
NAME	ItemData
FUNCTION
	Returns the Data for a particular MenuItem
INPUTS
	(struct MenuItem *) of the item that you want the Data
RESULTS
	(int *) of the data of the menu item or NULL
##############################################################*/
int *ItemData(MenuI)
struct MenuItem *MenuI;
{
struct MenuItemPlus *MenuP;

if (MenuI)
	{
	MenuP=(struct MenuItemPlus *)MenuI;
	return(MenuP->Data);
	}

return(NULL);
}
/*############################################################
NAME	ItemNum
FUNCTION
	Return the Item number for the MenuItem Passed,
	this number should not be used as a discrete value
	since it changes as handlers are added/deleted from a
	system. Its primary use is for saving checked items
	in ModStates
INPUTS
	(struct MenuItem *) 
RESULTS
	(int) menu item number;
##############################################################*/
int ItemNum(MenuI)
struct MenuItem *MenuI;
{
struct MenuItemPlus *MenuP;

if (MenuI)
	{
	MenuP=(struct MenuItemPlus *)MenuI;
	return(MenuP->Num);
	}

return(-1);
}
/*############################################################
NAME	CheckItem
FUNCTION
	Sets the CHECKED flag for the partcular menu item
	(same as MenuI|=CHECKED;
INPUTS
	(struct MenuItem *) of the MenuItem to check
RESULTS
	(int *) of the data of the MenuItem or NULL
##############################################################*/
int *CheckItem(MenuI)
struct MenuItem *MenuI;
{
struct MenuItemPlus *MenuP;

if (MenuI)
	{
	MenuI->Flags|=CHECKED;
	MenuP=(struct MenuItemPlus *)MenuI;
	return(MenuP->Data);
	}

return(NULL);
}
/*############################################################
NAME	CheckNum
FUNCTION
	Checks the num MenuItem, This function is used with
	the function ItemNum()
INPUTS
	First MenuItem and Int.
RESULTS
	(int *) of the Data of the MenuItem or NULL if not found
##############################################################*/
int *CheckNum(MenuI,num)
struct MenuItem *MenuI;
int num;
{
struct MenuItemPlus *MenuP;

while(MenuI)
	{
	MenuP=(struct MenuItemPlus *)MenuI;
	if (MenuP->Num==num) 
		{
		MenuI->Flags|=CHECKED;
		return(MenuP->Data);
		}
	MenuI=MenuI->NextItem;
	}

return(NULL);
}
/*############################################################
NAME	CheckData
FUNCTION
	Checks the item number of the menu with this particular
	Data.
INPUTS
	FirstMenuItem and a (int *) Data
RESULTS
	(int *) of the Data of the MenuItem or NULL if not found
##############################################################*/
int *CheckData(MenuI,Data)
struct MenuItem *MenuI;
int *Data;
{
struct MenuItemPlus *MenuP;

while(MenuI)
	{
	MenuP=(struct MenuItemPlus *)MenuI;
	if(MenuP->Data==Data) 
		{
		MenuI->Flags|=CHECKED;
		return(MenuP->Data);
		}
	MenuI=MenuI->NextItem;
	}

return(NULL);
}
/*############################################################
NAME	S16Text;
FUNCTION
	Alternative to Move() SetDrMd() SetAPen Text() sequence
INPUTS
	*rport - of where to draw text
	*text - the text to be rendered
	len - length of string or NULL for whole string
RESULTS
	NONE
##############################################################*/
void S16Text(rport,text,len,x,y)
struct RastPort *rport;
char *text;
short len,x,y;
{
if (len==NULL) len=strlen(text);

SetBPen(rport,StudioBase->defaults.colors.gadget_window.background);

SetDrMd(rport,JAM2);
SetAPen(rport,StudioBase->defaults.colors.gadget_window.topleft_3d);
Move(rport,x,y);
Text(rport,text,len);

SetDrMd(rport,JAM2);
SetAPen(rport,StudioBase->defaults.colors.gadget_window.bottomright_3d);
Move(rport,x+2,y+1);
Text(rport,text,len);

SetDrMd(rport,JAM1);
SetAPen(rport,StudioBase->defaults.colors.gadget_window.topleft_3d);
Move(rport,x,y);
Text(rport,text,len);
}
/*############################################################
NAME	MakeMenusFit
FUNCTION
	Thes function makes sure a Menu will not overwrap accross
	the edge of a screen, this function is called automatically
	when you open a window with OpenNewWind() with a menu pointer.
	Use this function if you decide to change the menus while
	a window is open.
INPUTS
RESULTS
##############################################################*/
void MakeMenuesFit(menu)
struct Menu *menu;
{
struct MenuItem *MenuI;

int size,offset;

while(menu)
	{
	size=0;
	MenuI=menu->FirstItem;
	while(MenuI)
		{
		if (size<(MenuI->LeftEdge+MenuI->Width)) size=MenuI->LeftEdge+MenuI->Width;
		MenuI=MenuI->NextItem;
		}
	size+=menu->LeftEdge;
	size+=30;

	if (size>StudioBase->screen->Width)
		{
		offset=StudioBase->screen->Width-size;
		MenuI=menu->FirstItem;
		while(MenuI)
			{
			MenuI->LeftEdge+=offset;
			MenuI=MenuI->NextItem;
			}
		}

	menu=menu->NextMenu;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void ChangeMenuColor(menu)
struct Menu *menu;
{
struct MenuItem *item,*sitem;
struct IntuiText *itext;
while (menu)
	{
	item=menu->FirstItem;
	while (item)
		{
		itext=(struct IntuiText *)item->ItemFill;
		if (item->Flags&HIGHCOMP) itext->FrontPen=StudioBase->defaults.colors.gadget_window.bottomright_3d;
		else itext->FrontPen=StudioBase->defaults.colors.gadget_window.hitbox_selected;
		itext->BackPen=StudioBase->defaults.colors.gadget_window.topleft_3d;
		if (item->SubItem)
			{
			sitem=item->SubItem;
			while (sitem)
				{
				itext=(struct IntuiText *)sitem->ItemFill;
				if (sitem->Flags&HIGHCOMP) itext->FrontPen=StudioBase->defaults.colors.gadget_window.bottomright_3d;
				else itext->FrontPen=StudioBase->defaults.colors.gadget_window.hitbox_selected;
				itext->BackPen=StudioBase->defaults.colors.gadget_window.topleft_3d;
				sitem=sitem->NextItem;
				}
			}
		item=item->NextItem;
		}
	menu=menu->NextMenu;
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*############################################################
NAME	BuildHandlerMenu
FUNCTION
	Builds a linked list of MenuItems each containing a Data
	the list may be built for specific needs by using the search
	parameter. For Instance if search=CK_CHAN_HILO|CH_CHAN_PLAY,
	a list of play channels that could display HILO info would
	be built.
	The other parameter 'flags' it used to determine what kind
	of menu you wish, if NULL a single select list is created
	if MENU_MULTI_SELECT multiple items may be selected
INPUTS
	search - ChanKey flags that describe the type of channels
		you want
	flags - internal use flags used to build different types
		of menus.
RESULTS
	(struct MenuItem *) of the first item of the menu.
##############################################################*/
struct MenuItem *BuildHandlerMenu(search,flags)
int search,flags;
{
void SetMutual();
void ToggleMode();
struct AudioHandler *hand=NULL;
struct MenuItem *MI=NULL;

hand = (struct AudioHandler *) FindAudioHandler(NULL,-1,0,NULL,-1);
while(hand)
	{
/*	if (search & hand.flags)*/ MI=CreateMenuItem(hand->name,hand,MI);
	hand = (struct AudioHandler *) FindAudioHandler(hand,-1,0,NULL,-1);
	}
if (flags&MENU_MULTI_SELECT) ToggleMode(MI);
else SetMutual(MI);

return(MI);
}
/*############################################################
NAME	FreeHandlerMenu
FUNCTION
	Undos all memory allocation of BuildHandlerMenu
INPUTS
	(struct MenuItem *) of the first item of a menu
RESULTS
	NONE
##############################################################*/
void FreeHandlerMenu(MenuI)
struct MenuItem *MenuI;
{
FreeChannelMenu(MenuI);
}

