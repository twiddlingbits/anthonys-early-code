#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "WallPaper.img"

#include "setup.pwc"

/** open custom screen, open libraries, launch modules **/

extern struct studioBase *studioBase;
short first_call=TRUE;

/*############################################################
NAME	
	StandardEnviroment -
FUNCTION
	If the standard studio16 screen isn't open, this will
	open it.  All modules that expect a screen pointer
	in studioBase should call this before opening their
	windows.
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

int StandardEnviroment()
{
void setdown();
char *AllocMem();
struct Screen *OpenScreen();
struct Window *OpenWindow();
int x,y;

Forbid();
if (!first_call) {
	Permit();
	while (studioBase->screen==0)
		WaitTOF();
	return(NO_ERROR);
	}
else {
	first_call=FALSE;
	Permit();
	}

NewScreenStructure.Width = studioBase->defaults.screen_width;
NewScreenStructure.ViewModes = studioBase->defaults.screen_viewmodes;

studioBase->screen=OpenScreen(&NewScreenStructure);
if (studioBase->screen==0) {
   return(STDERR_CANT_OPEN_SCREEN);
   }

LoadRGB4(&studioBase->screen->ViewPort,Palette,PaletteColorCount);

NewWindow.Screen = studioBase->screen;
NewWindow.Width = studioBase->screen->Width;
NewWindow.Height = studioBase->screen->Height - NewWindow.TopEdge;

studioBase->back_window = OpenWindow(&NewWindow);
if (studioBase->back_window==0) {
   return(STDERR_CANT_OPEN_WINDOW);
   }

SetAPen(studioBase->back_window->RPort, LIGHT_BROWN);
RectFill(studioBase->back_window->RPort, 0, 0, studioBase->back_window->Width-1, studioBase->back_window->Height-1);

for(y=-9;y<studioBase->back_window->Height;y+=35) {
   for(x=-107;x<studioBase->back_window->Width;x+=172) {
      if (y&1) DrawImage(studioBase->back_window->RPort,&WallPaper,x,y);
       else DrawImage(studioBase->back_window->RPort,&WallPaper,x+83,y);
      }
  	}
return(NO_ERROR);
}

