#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <libraries/dos.h>
#include <dos.h>
#include <stdio.h>
#include "/include/psound.h"
#include "WallPaper.img"

#include "setup.pwc"

/** open custom screen, open libraries, launch modules **/

int _oserr,curdir;	/** stop dfind() link errors **/

static short init_done=FALSE;
static short first_call=TRUE;
static int init_error=NO_ERROR;

extern struct IntuitionBase *IntuitionBase;

extern struct StudioBase *StudioBase;
extern short se_cmd_select_module;
void AttachWorkingFiles();
void new_pointer();
void DoDefaultAssigns();
void InitModState();

/*############################################################
NAME	
	StandardEnviroment -
FUNCTION
		InitStudio16UserEnv(FLAGS, 0, 0);
		where FLAGS is:
		#define 	FLAGS ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16INSTANCE|
					ISUE_LOAD_STUDIO16BASE|ISUE_ATTACH_WORKING_FILES|
					ISUE_OPEN_DEF_SCREEN
INPUTS
	NONE
RESULTS
	standard error code
##############################################################*/

int StandardEnviroment()
{
#define FLAGS (ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16INSTANCE|ISUE_LOAD_STUDIO16BASE|ISUE_ATTACH_WORKING_FILES|ISUE_OPEN_DEF_SCREEN)
return(InitStudio16UserEnv(FLAGS, 0, 0));
}

/*############################################################
NAME	
	InitStudio16UserEnv
FUNCTION
	Only the first call to this function does anything.  All
	subsequent calles are ignored.  The first call to this 
	function after opening Studio.library allows you to
	setup the users enviroment to various defaults.

	If the standard studio16 screen isn't open, this will
	open it, provided ISUE_OPEN_DEF_SCREEN is set.  All modules
	that expect a screen pointer in StudioBase may call this
	before opening their windows.

	Set any of the following flags:
	
	ISUE_DO_ASSIGNS					** Assign Studio16: **
	ISUE_LOAD_STUDIO16INSTANCE		** LoadModState(0) **
	ISUE_LOAD_STUDIO16BASE			** LoadDefaults(0) ***
	ISUE_ATTACH_WORKING_FILES		** AttachWorkingFiles() **
	ISUE_OPEN_DEF_SCREEN				** OpenScreen() 3 BitPlanes **
	ISUE_USE_MY_SCREEN				** use passed params **

INPUTS
	flags			See above
	myscreen 	Screen pointer used if ISUE_USE_MY_SCREEN is passed
	mybackwin	Backdrop window used if ISUE_USE_MY_SCREEN is passed
RESULTS
	standard error code
##############################################################*/

int InitStudio16UserEnv(flags, myscreen, mybackwin)
unsigned long flags;
struct Screen *myscreen;
struct Window *mybackwin;
{
void setdown();
char *AllocMem();
struct Screen *OpenScreen();
struct Window *OpenWindow();
int x,y;

if (init_done)
	return(init_error);

Forbid();
if (!first_call) {
	Permit();
	while (!init_done)
		WaitTOF();
	return(init_error);
	}
else {
	first_call=FALSE;
	Permit();
	}

if (flags&ISUE_DO_ASSIGNS)
	DoDefaultAssigns();

/** point studio screen to active screen in case no screen is **/
/** to be opened **/

StudioBase->screen=IntuitionBase->ActiveScreen;

if (flags&ISUE_LOAD_STUDIO16BASE)
	LoadDefaults(0);		/** load s:studio16base.config **/

if (flags&ISUE_OPEN_DEF_SCREEN) {
	NewScreenStructure.Width = StudioBase->defaults.screen_width;
	NewScreenStructure.ViewModes = StudioBase->defaults.screen_viewmodes;

	StudioBase->screen=OpenScreen(&NewScreenStructure);
	if (StudioBase->screen==0) {
		init_error=STDERR_CANT_OPEN_SCREEN;
		init_done=TRUE;
	   return(STDERR_CANT_OPEN_SCREEN);
	   }

	LoadRGB4(&StudioBase->screen->ViewPort,StudioBase->defaults.palette,PaletteColorCount);

	NewWindow.Screen = StudioBase->screen;
	NewWindow.Width = StudioBase->screen->Width;
	NewWindow.Height = StudioBase->screen->Height - NewWindow.TopEdge;

	StudioBase->back_window = OpenWindow(&NewWindow);
	if (StudioBase->back_window==0) {
		init_error=STDERR_CANT_OPEN_WINDOW;
		init_done=TRUE;
	   return(STDERR_CANT_OPEN_WINDOW);
	   }


	SetAPen(StudioBase->back_window->RPort, LIGHT_BROWN);
	RectFill(StudioBase->back_window->RPort, 0, 0, StudioBase->back_window->Width-1, StudioBase->back_window->Height-1);

	if (StudioBase->defaults.flags&SBD_WALLPAPER_ON) {
		for(y=-9;y<StudioBase->back_window->Height;y+=35) {
		   for(x=-107;x<StudioBase->back_window->Width;x+=172) {
	  	    if (y&1) DrawImage(StudioBase->back_window->RPort,&WallPaper,x,y);
	   	    else DrawImage(StudioBase->back_window->RPort,&WallPaper,x+83,y);
		      }
		  	}
		}
	}

if (flags&ISUE_USE_MY_SCREEN) {
	StudioBase->screen=myscreen;
	StudioBase->back_window=mybackwin;
	}

new_pointer(PTR_BUSY);

if (flags&ISUE_ATTACH_WORKING_FILES)
	AttachWorkingFiles();

/** THERE is a "bug" here.  I can't put init_done=TRUE after
		LoadModState.  If i do, all calls to LoadModule() time
		out because the loaded modules call StandardEnviroment()
		which hangs up until the the inital LoadModState() returns. **/

init_error=NO_ERROR;
init_done=TRUE;

if (flags&ISUE_LOAD_STUDIO16INSTANCE)
	{
	LoadModState(0);

	if (FindModule("Instance")==NULL) {
		LoadModule("ModList");
		LoadModule("Instance");
		}
	}

new_pointer(PTR_NORMAL);
return(NO_ERROR);
}

/***********************************************************/

struct dir_entry {
	struct MinNode node;
	char workname[50];
	char sampname[50];
	int version;
	};


/*############################################################
NAME	
	AttachWorkingFiles
FUNCTION
	Scans the Working Directory (audio: etc.) and generate	
	Disk_Samp structs for all the valid samp files.  Called
	automaticly by GetStandardEnviroment().
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void AttachWorkingFiles()
{
/*static short awf_first_call=TRUE;*/
struct FILEINFO __aligned info;
int error;
char *workname;
char searchpath[150];
struct MinList dir;
struct MinNode *node;
struct dir_entry *de;

/*Forbid();
if (!awf_first_call) {
	Permit();
	return;
	}
awf_first_call=FALSE;
Permit();
*/

NewList(&dir);

strcpy(searchpath,StudioBase->defaults.temp_files_path);
add(searchpath,"#?.???L");

error=dfind(&info,searchpath,0);
while (error==0) {
	workname=info.fib_FileName;
	if (AddDirEntry(&dir, workname)==NO_ERROR)
		error=dnext(&info);	/** This guy is loosing memory!! **/
	else							/** I have no idea why **/
		error=1;
	}

/** scan dir entries and pick each one with highest version # */

node=dir.mlh_Head;
while(node->mln_Succ) {
   de=(struct dir_entry *)node;
	node = node->mln_Succ;
	QuickLoadSamp(StudioBase->defaults.temp_files_path,de->workname,de->sampname);
	Remove(de);
	FreeMem(de, sizeof(struct dir_entry));
	}
}

/***********************************************************/

AddDirEntry(l, workname)
struct MinList *l;
char *workname;
{
struct dir_entry *de;
struct MinNode *node;
int version;
char sampname[50];

strcpy(sampname,workname);
sampname[strlen(sampname)-5]=0;
sscanf(workname+strlen(workname)-4,"%d",&version);

for (node=l->mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   de=(struct dir_entry *)node;
	if (stricmp(de->sampname,sampname)==0) {
		if (de->version >= version) {
			return(NO_ERROR);
			}
		else {
			strcpy(de->sampname,sampname);
			strcpy(de->workname,workname);
			de->version=version;
			return(NO_ERROR);
			}
		}
	}
	
de = (struct dir_entry *)AllocMem(sizeof(struct dir_entry),MEMF_CLEAR);
if (de==NULL)
	return(OUT_OF_MEMORY);
strcpy(de->sampname,sampname);
strcpy(de->workname,workname);
de->version=version;
AddTail(l, de);
return(NO_ERROR);
}

/***********************************************************/

void new_pointer(mode)

SHORT mode;
{
static short lastmode = PTR_NORMAL;
static short thismode = PTR_NORMAL;
struct Window *w;
struct Screen *screen;
void setall();


static unsigned short chip busy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x380, 0x380,
   0x1FE0, 0x1FE0,
   0x3FFC, 0x3FFC,
   0x3FFE, 0x31FE,
   0x3FFE, 0x3DFE,
   0x7FFE, 0x7B1E,
   0x7FFE, 0x77DE,
   0x7FFC, 0x71BC,
   0x7FFC, 0x7F7C,
   0x3FFC, 0x3F1C,
   0x7FF8, 0x7FF8,
   0x7F00, 0x7F00,
   0x3BC0, 0x3BC0,
   0x1C0, 0x1C0,
   0x80, 0x80,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -1 */


if (mode==PTR_RESTORE) mode=lastmode;     /* restore previous mode */
lastmode=thismode;    /* need to remember last 2 modes for RESTORE */
thismode=mode;

screen = StudioBase->screen;

switch (mode) {

   case PTR_NORMAL:
      for (w=screen->FirstWindow;w!=0;w=w->NextWindow) {
         ClearPointer(w);
         }
      return;

   case PTR_BUSY:

      setall(screen,busy, 16, 16, -2, -1);
/*      setmenus(screen, FALSE);
      menus_on = FALSE; */
      break;

   }  /** end of switch **/
}

/************************************/
/** change pointers for all windows */
/************************************/

void setall(s,data,a,b,c,d)

struct Screen *s;
char *data;
int a,b,c,d;
{
struct Window *w;

for (w=s->FirstWindow; w!=0; w=w->NextWindow) {
   SetPointer(w,data,a,b,c,d-1);
   }
}



/** If the user doesn't have any assigns, pick some for him **/

void DoDefaultAssigns()
{
long fp;
char path[160];
char cmd[sizeof(path)+18];
int len;
long nil;

nil = Open("NIL:",MODE_NEWFILE);

if (!CheckAssign("Audio")) {			/** Don't need this any more! **/
	Execute("assign audio: ram:",0L,nil);
	}

if (!CheckAssign("Studio16")) {
	fp=Open("s:Studio16Path.config",MODE_OLDFILE);
	if (fp) {
		len=Seek(fp,0,OFFSET_END);
		len=Seek(fp,0,OFFSET_BEGINNING);
		if (len > 0) {
			Read(fp,path,len);
			path[len]=0;
			sprintf(cmd,"assign Studio16: %c%s%c",34,path,34);
			Execute(cmd,0L,nil);
			}
		Close(fp);
		}
	}

if (nil)
	Close(nil);
}
