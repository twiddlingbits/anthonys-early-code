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
* ddir:   Directory of file, may be null
*
*  The basic call is:
*
*   getfile(Window,Screen,Hailing_string,file_name,directory_name);
*
* Where:
*   Window is a pointer to your window
*   Screen is a pointer to your screen ... or (long)NULL if workbench
*   Hailing_string is a prompt displayed in the requester
*   file_name is a text array which will be altered by getfile,
*   it returns the file name.
*   directory_name is a text array altered by getfile, it
*   is the directory.
*
*   The return value is either a pointer to your buffer, file_name,
*   or NULL if the user selected CANCEL.
*
*   You must reserve two text areas for file and directory like so:
*
* #define FNAME_SIZE 33
* TEXT   file_name[FNAME_SIZE + 1];
* TEXT   dir_name[FNAME_SIZE + 2]
**********************************************************************/

#ifndef GET_FNAME

#define GET_FNAME

#define FNAME_SIZE 33

extern char *get_fname();

#endif

