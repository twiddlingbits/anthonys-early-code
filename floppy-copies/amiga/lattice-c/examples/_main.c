#include "lattice/stdio.h"
#include "lattice/ctype.h"
#include "lattice/ios1.h"

#include "workbench/startup.h"
#include "libraries/dos.h"

#define MAXARG 32              /* maximum command line arguments */

#ifndef TINY
extern int _stack,_fmode,_iomode;
#endif
extern int LoadAddress;

extern struct UFB _ufbs[];
int argc;			/* arg count */
char *argv[MAXARG];		/* arg pointers */

#define MAXWINDOW 40
extern struct WBStartup *WBenchMsg;
static char window[MAXWINDOW] = "con:10/10/320/80/";

/**
*
* name         _main - process command line, open files, and call "main"
*
* synopsis     _main(line);
*              char *line;     ptr to command line that caused execution
*
* description	This function performs the standard pre-processing for
*		the main module of a C program.  It accepts a command
*		line of the form
*
*			pgmname arg1 arg2 ...
*
*		and builds a list of pointers to each argument.  The first
*		pointer is to the program name.  For some environments, the
*		standard I/O files are also opened, using file names that
*		were set up by the OS interface module XCMAIN.
*
**/
_main(line)
char *line;
{
char c;
int x;

/*
*
* Build argument pointer list
*
*/
for(argc = 0; argc < MAXARG; )
	{
	while(isspace(*line)) line++;
	if(*line == '\0') break;
	argv[argc++] = line;
	while((*line != '\0') && (isspace(*line) == 0)) line++;
	c = *line;
	*line++ = '\0';
	if(c == '\0') break;
	}
/*
*
* Open standard files
*
*/
#ifndef TINY

stdin->_file = 0;
stdin->_flag = _IOREAD;
stdout->_file = 1;
stdout->_flag = _IOWRT;
stderr->_file = 2;
stderr->_flag = _IOWRT;

if (argc == 0)		/* running under workbench	*/
	{
	strncat(window, WBenchMsg->sm_ArgList->wa_Name,MAXWINDOW);
	_ufbs[0].ufbfh = Open(window,MODE_NEWFILE);
	_ufbs[1].ufbfh = _ufbs[0].ufbfh;
	_ufbs[1].ufbflg = UFB_NC;
	_ufbs[2].ufbfh = _ufbs[0].ufbfh;
	_ufbs[2].ufbflg = UFB_NC;
	x = 0;
	}
else			/* running under CLI		*/
#endif
	{
	_ufbs[0].ufbfh = Input();
	_ufbs[1].ufbfh = Output();
	_ufbs[2].ufbfh = Open("*", MODE_OLDFILE);
	x = UFB_NC;			/* do not close CLI defaults	*/
	}

_ufbs[0].ufbflg |= UFB_OP | UFB_RA | x;
_ufbs[1].ufbflg |= UFB_OP | UFB_WA | x;
_ufbs[2].ufbflg |= UFB_OP | UFB_WA ;

/*
*
* Call user's main program
*
*/
#ifdef DEBUG
printf("load address = %lx\n",LoadAddress);
Debug(0);
#endif

main(argc,argv);              /* call main function */
#ifndef TINY
exit(0);
#else
_exit(0);
#endif
}

