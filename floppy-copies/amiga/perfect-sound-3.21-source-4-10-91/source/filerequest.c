#include <exec/types.h>
#include "intuition/intuition.h"
#include "libraries/arpbase.h"   /* Standard ARP header file */
/* #include "arpfunctions.h"  */ /* Predeclared functions */

/***************************************************
*  get_fname(window,screen,hail,ddef,ddir);
*
*   Displays a window/requester that
* gets a file name for device,directory,default file, extension
*
*   Calling args:
* window:   Window making the request
* screen:   Screen, if NULL assummed workbench
* hail:   Text prompt at top of requester
* ddef:   Input default file-name. Has NO DIRECTORY OR EXTENSION.
* ddir:   Directory of file, may be null **/

get_fname(window, screen, hail, Filename, Directory)

struct Screen *screen;
struct Window *window;
char *Filename, *Directory, *hail;

{
void alter_newwindow();
extern struct ArpBase *ArpBase;
struct FileRequester FR;
char *Selection;
/*int old_flag;

old_flag = window->Flags&REPORTMOUSE;*/
setmem(&FR, sizeof (FR), 0);

FR.fr_Hail = hail;
FR.fr_File = Filename;
FR.fr_Dir  = Directory;
FR.fr_Window = window;

FR.fr_FuncFlags = FRF_NewWindFunc|FRF_DoColor;
FR.fr_Function  = alter_newwindow;

Selection = FileRequest( &FR );

if (Selection)
   return(1);
else
   return(0);     /** canceled **/
}


void alter_newwindow(mask, new_window)
struct NewWindow *new_window;
{
new_window->LeftEdge = 160;
new_window->TopEdge = 35;
}
