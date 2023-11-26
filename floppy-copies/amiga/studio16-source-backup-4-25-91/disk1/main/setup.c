#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "WallPaper.img"

#include "setup.pwc"

/** open custom screen, open libraries, launch modules **/

struct studioBase studioBaseMem;
struct studioBase *studioBase = &studioBaseMem;

unsigned char *bigblock[2][4];	/** PUT IN STUDIOBASE!! **/

/*extern struct Disk_Samp samp[MAX_SAMPLES];*/ /** MAKE LIKED LIST!!!!*/
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */

/*##
   NAME
      setup - open libraries, screens, and initilize structures

   ARGS
      none

   FUNCTION

      sets up studioBase, opens the screen and backdrop window.
      A call to setdown() will deallocate everything allocated
      by setup().
##*/

void setup()
{
void setup_defaults();
void setdown();
char *AllocMem();
struct Screen *OpenScreen();
struct Window *OpenWindow();
int x,y;

setmem(studioBase,sizeof(struct studioBase),0);
NewList(&studioBase->modules);
NewList(&studioBase->regions);
NewList(&studioBase->samps);
NewList(&studioBase->cue_events);

openlibraries();
setup_defaults(0);
init_gadlib();
NewScreenStructure.Width = studioBase->defaults.screen_width;
NewScreenStructure.ViewModes = studioBase->defaults.screen_viewmodes;

studioBase->screen=OpenScreen(&NewScreenStructure);
if (studioBase->screen==0) {
   telluser("can't open screen","");
   setdown();
   exit(10);
   }

LoadRGB4(&studioBase->screen->ViewPort,Palette,PaletteColorCount);

NewWindow.Screen = studioBase->screen;
NewWindow.Width = studioBase->screen->Width;
NewWindow.Height = studioBase->screen->Height - NewWindow.TopEdge;

studioBase->back_window = OpenWindow(&NewWindow);
if (studioBase->back_window==0) {
   telluser("can't open back window","");
   setdown();
   exit(10);
   }

if (init_msg()) {    /** load all text msgs **/
   telluser("Can't load ps.txt","");
   setdown();
   exit(10);
   return;
   }

SetMenuStrip(studioBase->back_window,Titles12);   /* turn on menus */

SetAPen(studioBase->back_window->RPort, LIGHT_BROWN);
RectFill(studioBase->back_window->RPort, 0, 0, studioBase->back_window->Width-1, studioBase->back_window->Height-1);

for(y=-9;y<studioBase->back_window->Height;y+=35) {
   for(x=-107;x<studioBase->back_window->Width;x+=172) {
      if (y&1) DrawImage(studioBase->back_window->RPort,&WallPaper,x,y);
       else DrawImage(studioBase->back_window->RPort,&WallPaper,x+83,y);
      }
   }

load_play();      /** load play handler **/
init_send_msg();     /** initilize free struct play_msg's **/
                     /** must be after load_play! **/
link_standard_modules();   /** link edit, smpte, etc. to studioBase **/

for (y=0; y < 4; y++) {
	bigblock[0][y]=(unsigned char *)AllocMem(BIG_BLOCK_SIZE*2, 0);
	if (bigblock[0]==0) {
		setdown();
		exit(10);
		}

	bigblock[1][y]=(unsigned char *)AllocMem(BIG_BLOCK_SIZE*2, 0);
	if (bigblock[1]==0) {
		setdown();
		exit(10);
		}
	}
}

void setdown()
{
struct MinNode *node;
struct Module *mod;
struct Disk_Sample *s;
int i;

/*   mproc = (struct Process *)FindTask(0L);
   mproc->pr_WindowPtr = temp_ptr; */       /** restore original pointer **/


node=studioBase->samps.mlh_Head;
while (node->mln_Succ) {
   s=(struct Disk_Samp *)node;
   node = node->mln_Succ;
   delete_sample(s);
   }

node=studioBase->modules.mlh_Head;
while (node->mln_Succ) {
   mod=(struct Module *)node;
   node = node->mln_Succ;
   delete_module(mod);
   }

unload_play();          /** unload play handler **/
if (studioBase->back_window)
   CloseWindow(studioBase->back_window);
if (studioBase->screen)
   CloseScreen(studioBase->screen);
free_msg();
rem_gadlib();
for (i=0; i < 4; i++) {
	if (bigblock[0][i])
		FreeMem(bigblock[0][i], BIG_BLOCK_SIZE*2);
	if (bigblock[1][i])
		FreeMem(bigblock[1][i], BIG_BLOCK_SIZE*2);
	}
closelibraries();
}

/******************************************************************/
/*##
      NAME
      setup_defaults - fill studioBase->defaults

   ARGS:
      1  pointer to string containing complete path of file
         containing defaults to try and load.
   RETURNS:
      None
   FUNCTION:
      trys to load defaults from the specified file.  If the file
      can't be opened, some defauts are generated.  Defaults are
      kept in studioBase->defaults. If a null is passed for arg1,
      a default string "AutoPres" is generated.
##*/
/******************************************************************/

void setup_defaults(file)
char *file;    /** complete path/file pointing to defaults or NULL **/

{
struct Screen def_screen;

if (file==NULL)
   file = "AutoPrefs";

/*** put in stuff to load file here!!! **/

strcpy(studioBase->defaults.samp_io_path,"aw:sounds");
strcpy(studioBase->defaults.temp_files_path,"audio:");

if (GetScreenData(&def_screen, sizeof(struct Screen), WBENCHSCREEN, 0)) {
   studioBase->defaults.screen_width = def_screen.Width;
   if (def_screen.Height >= 400)
      studioBase->defaults.screen_viewmodes = LACE;
   if (def_screen.Width >= 200)
      studioBase->defaults.screen_viewmodes |= HIRES;
   }
else {
   studioBase->defaults.screen_width = 640;
   studioBase->defaults.screen_viewmodes = HIRES;
   }
}

