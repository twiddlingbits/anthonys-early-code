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
void new_pointer();
int DoDefaultAssigns();
void InitModState();
void LoadModDir();

/*############################################################
NAME	
	StandardEnviroment -
FUNCTION
		InitStudio16UserEnv(FLAGS, 0, 0);
		where FLAGS is:
		#define	FLAGS ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16INSTANCE|
					ISUE_LOAD_STUDIO16BASE|
					ISUE_OPEN_DEF_SCREEN|ISUE_LOAD_DRIVERS
INPUTS
	NONE
RESULTS
	standard error code
##############################################################*/

int StandardEnviroment()
{
#define FLAGS (ISUE_DO_ASSIGNS|ISUE_LOAD_STUDIO16INSTANCE|ISUE_LOAD_STUDIO16BASE|ISUE_OPEN_DEF_SCREEN|ISUE_LOAD_DRIVERS)
return(InitStudio16UserEnv(FLAGS, 0, 0));
}

/*############################################################
NAME	
	InitStudio16UserEnv
FUNCTION
	Only the first call to this function does anything.  All
	subsequent calles are ignored (unless you set the
	ISUE_FORCE flag.  However, this flag should be used
	carefully, and never from within a Studio 16 module.)
	The first call to this function after opening Studio.library
	allows you to setup the users enviroment to various defaults.

	If the standard studio16 screen isn't open, this will
	open it, provided ISUE_OPEN_DEF_SCREEN is set.	All modules
	that expect a screen pointer in StudioBase may call this
	before opening their windows.

	Set any of the following flags:
	
	ISUE_DO_ASSIGNS					** Assign Studio16: **
	ISUE_LOAD_STUDIO16INSTANCE		** LoadModState(0) **
	ISUE_LOAD_STUDIO16BASE			** LoadDefaults(0) ***
	ISUE_OPEN_DEF_SCREEN				** OpenScreen() 3 BitPlanes **
	ISUE_USE_MY_SCREEN				** use passed params **
	ISUE_LOAD_DRIVERS					** load drivers & utilites **

	ISUE_FORCE							** Allow a call after first **

INPUTS
	flags			See above
	myscreen		Screen pointer used if ISUE_USE_MY_SCREEN is passed
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
long error;

/** Only allow first call to run, unless ISUE_FORCE set **/

if (!(flags&ISUE_FORCE)) {
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
	}

/** assign Studio16: **/

if (flags&ISUE_DO_ASSIGNS) {
	error=DoDefaultAssigns();
	if (error!=NO_ERROR) {
		init_error=error;
		init_done=TRUE;
		return(error);
		}
	}

/** Load base defaults 1st so we know screen size, etc. **/

if (flags&ISUE_LOAD_STUDIO16BASE)
	LoadDefaults(0);		/** load s:studio16base.config **/

/** Open the studio 16 screen if needed **/

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
	StudioBase->flags &= (~SB_APP_SCREEN);
	}

if (flags&ISUE_USE_MY_SCREEN) {
	StudioBase->screen=myscreen;
	StudioBase->back_window=mybackwin;
	StudioBase->flags |= SB_APP_SCREEN;
	}

/** THERE is a "bug" here.	 I can't put init_done=TRUE after
		LoadModState.	If i do, all calls to LoadModule() time
		out because the loaded modules call StandardEnviroment()
		which hangs up until the the inital LoadModState() returns. **/

init_error=NO_ERROR;
init_done=TRUE;

if (flags&ISUE_LOAD_DRIVERS)
	{
	LoadModDir("Drivers");
	LoadModDir("Utilities");
	}

if (flags&ISUE_LOAD_STUDIO16INSTANCE)
	{
	LoadModState(0);

	if (FindModule("Instance")==NULL) {
			LoadModule("Instance");	/** Needed becase for some dumb reason it sends "se_cmd_kill" msgs **/
/**		LoadModDir("Apps");		 **/
/**		LoadModDir("ProjectMenu");**/
		}
	}

return(NO_ERROR);
}

/*************************************************************/

/** If the user doesn't have any assigns, pick some for him **/

int DoDefaultAssigns()
{
long fp;
char path[STUDIO16_MAX_PATH_SIZE];
char cmd[sizeof(path)+18];
int len;
long nil;
char *p;
int error;
long lock;

error=NO_ERROR;
nil = Open("NIL:",MODE_NEWFILE);

if (!CheckAssign("Studio16")) {
	fp=Open("s:Studio16Path.config",MODE_OLDFILE);
	if (fp) {
		len=Seek(fp,0,OFFSET_END);
		len=Seek(fp,0,OFFSET_BEGINNING);
		if (len > 0 && len < sizeof(path)) {
			Read(fp,path,len);
			path[len]=0;
			p=(char *)strchr(path,10);	/* Null any LF's in file */
			if (p)
				*p=0;
			p=(char *)strchr(path,13);	/* Null any LF's in file */
			if (p)
				*p=0;
			sprintf(cmd,"assign Studio16: %c%s%c",34,path,34);
			Execute(cmd,0L,nil);
			}
		Close(fp);
		}
	else {
		error=STDERR_NO_STUDIO16_ASSIGN;
		}	
	}

if (nil)
	Close(nil);

if (error==NO_ERROR) { /** check assign's validity **/
	lock=Lock("Studio16:Apps",ACCESS_READ);
	if (lock) 
		UnLock(lock);
	else
		error=STDERR_BAD_STUDIO16_ASSIGN;
	}
return(error);
}


/*************************************************************/

void LoadModDir(dir)
char *dir;
{
char path[STUDIO16_MAX_PATH_SIZE];
struct FileLock *fkey;
static struct FileInfoBlock XXX,*dinfo=&XXX;

sprintf(path,"Studio16:%s",dir);

if (!(fkey=(struct FileLock *)Lock(path,ACCESS_READ))) 
	{
	return;
	}

if (!Examine(fkey, dinfo)) 
	{
	UnLock(fkey);
	return;
	}

if ( dinfo->fib_DirEntryType < 0L ) 
	{
	UnLock(fkey);
	return;
	}

while(ExNext(fkey, dinfo))
	{
	if (dinfo->fib_EntryType<0) 
		{
		if (IsValidModName(dinfo->fib_FileName))
			{
			if (FindModule(dinfo->fib_FileName)==0)
				{
				sprintf(path,"%s/%s",dir,dinfo->fib_FileName);
				LoadModule(path);
				}
			}
		}
	}

UnLock(fkey);
}

IsValidModName(str)
char *str;
{
int len;

len=strlen(str);
if (len < 5)
	return(TRUE);
return(stricmp(&str[len-5],".info"));
}

/*############################################################
NAME	
	CloseAllModules
FUNCTION
	Send's "SE_CMD_KILL_MOUDLE" messages to all resident
	studio16 modules, in the appropriate order for a clean
	shut down.

	After all modules are close, the following code is 
	executed:
		if (StudioBase->quit_task)
			Signal(StudioBase->quit_task,1<<StudioBase->quit_sigbit);
INPUTS
	callerismoudle		set to TRUE if the caller is a currently
							resident module (like "Quit"), otherwise
							FALSE (like shell cmd "StudioClose")
RESULTS
	None
##############################################################*/

void CloseAllModules(callerismodule)
int callerismodule;
{
short se_cmd_kill_module;
struct MinNode *node;
struct Module *mod;
int AnyLeft;
int waitcount;

se_cmd_kill_module	=GetEventID("SE_CMD_KILL_MODULE");
if (callerismodule)
	waitcount=1;
else
	waitcount=0;

/**** KILL ALL NON UTILITIES ****/

ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	{
	mod=(struct Module *)node;
	if(!(mod->state->flags&MS_UTILITY_MODULE)) 
	SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
	}
ReleaseSharedSemaphore(&StudioBase->lockstudio);

/**** WAIT ****/

AnyLeft=2;
while (AnyLeft>waitcount)		/* 1 if caller is a moudle, because **/
	{									/* it can't respond to kill messages **/
	AnyLeft=0;
	WaitTOF();WaitTOF();
	ObtainSharedSemaphore(&StudioBase->lockstudio);
	for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
		{
		mod=(struct Module *)node;
		if(!(mod->state->flags&MS_UTILITY_MODULE)) {AnyLeft++;}
		}
	ReleaseSharedSemaphore(&StudioBase->lockstudio);
	}

/** HALT ANY PLAYING SOUND ****/

BroadcastEventParms(GetEventID("SE_CMD_STOP_ALL_PLAYBACK"),0,0,0,0,0,0);

/**** KILL ALL UTILITIES ****/
ObtainSharedSemaphore(&StudioBase->lockstudio);
for (node=StudioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	{
	mod=(struct Module *)node;
	SendMsg(mod,se_cmd_kill_module,mod,0,0,0,0,0);
	}
if (StudioBase->quit_task)
	Signal(StudioBase->quit_task,1<<StudioBase->quit_sigbit);

ReleaseSharedSemaphore(&StudioBase->lockstudio);
}
