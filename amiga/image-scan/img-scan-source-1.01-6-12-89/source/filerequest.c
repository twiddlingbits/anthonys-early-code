#include <exec/types.h>
#include "intuition/intuition.h"
#include "libraries/arpbase.h"   /* Standard ARP header file */
#include "arpfunctions.h"   /* Predeclared functions */

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
struct FileRequester FR;
char *Selection;
void alter_newwindow();

FR.fr_Hail = hail;
FR.fr_File = Filename;
FR.fr_Dir  = Directory;
FR.fr_Window = window;
if (window->Width <= 325)
   FR.fr_FuncFlags = FRF_NewWindFunc;
else
   FR.fr_FuncFlags = 0;
FR.fr_reserved1 = 0;
FR.fr_Function  = alter_newwindow;
FR.fr_reserved2 = 0;

SetRGB4(&screen->ViewPort, 1, 13, 13, 13);
Selection = FileRequest( &FR );
SetRGB4(&screen->ViewPort, 1, 1, 1, 1);
if (Selection)
   return(1);
else
   return(0);     /** canceled **/
}

void alter_newwindow(mask, new_window)
struct NewWindow *new_window;
{
new_window->LeftEdge = 0;
}

