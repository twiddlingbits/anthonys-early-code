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
extern struct Disk_Samp samp[MAX_SAMPLES]; /** MAKE LIKED LIST!!!!*/
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */

/*##
   NAME
      setup - open libraries, screens, and initilize structures

   ARGS
      none

   FUNCTION

      sets up studioBase, opens the screen and backdrop window.
      A call to setdown() will deallocate all everything allocated
      by setup().
##*/

void setup()
{
void setup_defaults();
void setdown();
char *AllocMem();
struct Screen *OpenScreen();
struct Window *OpenWindow();
int i,x,y;

for(i=0;i<MAX_SAMPLES;i++) {
   samp[i].length=0;
   samp[i].rate=0;
   samp[i].name=0;
   samp[i].type=0;
   samp[i].version=0;
   samp[i].lmem=0;
   samp[i].rmem=0;
   }

setmem(studioBase,sizeof(struct studioBase),0);

openlibraries();
setup_defaults(0);
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
link_standard_modules();   /** link edit, smpte, etc. to studioBase **/
}

void setdown()
{
int i;

/*   mproc = (struct Process *)FindTask(0L);
   mproc->pr_WindowPtr = temp_ptr; */       /** restore original pointer **/

while (studioBase->first_module)
   delete_module(studioBase->first_module);
unload_play();          /** unload play handler **/
if (studioBase->back_window)
   CloseWindow(studioBase->back_window);
if (studioBase->screen)
   CloseScreen(studioBase->screen);

for(i=0;i<MAX_SAMPLES;i++)
   delete_sample(i);

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
strcpy(studioBase->defaults.temp_files_path,"ram:");

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

