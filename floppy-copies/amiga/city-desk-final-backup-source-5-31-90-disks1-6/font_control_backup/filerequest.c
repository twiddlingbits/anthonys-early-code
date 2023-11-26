#include <exec/types.h>
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
{
struct FileRequester FR;
char *Selection;

FR.fr_Hail = hail;
FR.fr_File = Filename;
FR.fr_Dir  = Directory;
FR.fr_Window = window;
FR.fr_FuncFlags = 0;
FR.fr_reserved1 = 0;
FR.fr_Function  = 0;
FR.fr_reserved2 = 0;

if ( Selection = FileRequest( &FR ) )
   return(1);
else
   return(0);     /** canceled **/
}

