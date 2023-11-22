//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <windows.h>
#include <wincon.h>

#include "basic.h"

/*
 * iowincon.cpp
 *
 * This file contains MS Windows console implementations for struct IoConsole.
 */


void IoWinConClose(struct IoConsole* io)
{
	FreeConsole();
}

char IoWinConInkey(struct IoConsole* io)
{
	HANDLE hin;
	unsigned long dummy;
	DWORD numevents;
	INPUT_RECORD record;


	hin=GetStdHandle(STD_INPUT_HANDLE);

	if (INVALID_HANDLE_VALUE!=hin)
		if(GetNumberOfConsoleInputEvents(hin, &numevents))
			while (numevents>0)
			{
				if (ReadConsoleInput(hin, &record, 1, &dummy))
					if (record.EventType==KEY_EVENT)
						if (record.Event.KeyEvent.bKeyDown)
							return record.Event.KeyEvent.uChar.AsciiChar;

				GetNumberOfConsoleInputEvents(hin, &numevents);
			}

	return 0;
}

char IoWinConGetC(struct IoConsole* io)
{
	HANDLE hin;
	unsigned long dummy;
	INPUT_RECORD record;

	hin=GetStdHandle(STD_INPUT_HANDLE);

	if (INVALID_HANDLE_VALUE!=hin)
		while (TRUE)
		{
			if (ReadConsoleInput(hin, &record, 1, &dummy))
				if (record.EventType==KEY_EVENT)
					if (record.Event.KeyEvent.bKeyDown)
						return record.Event.KeyEvent.uChar.AsciiChar;
		}

	return 0x1b;	// ESC
}

void IoWinConPutc(struct IoConsole* io, char c)
{
	unsigned long dummy;

	HANDLE hout;

	hout=GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE!=hout)
		WriteConsole(hout, &c, 1, &dummy, NULL); 
}

/*
void IoWinConPutstr(struct IoConsole* io, char * string)
{
	unsigned long dummy;
	HANDLE hout;

	hout=GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE!=hout)
		WriteConsole(hout, string, strlen(string), &dummy, NULL); 
}
*/

int IoWinConChkBrk(struct IoConsole* io)
{
	char c;
	while (c=IoWinConInkey(io))
	{
		if (c==0x1b)	// esc
			return ERR_BREAK;
	}

	return ERR_OKAY;
}

void IoWinConDrawRange(struct IoConsole* io, unsigned char* vm, int start, int end)
{
}

unsigned char IoWinConTrsPeekKeyboard(struct IoConsole* io, unsigned short addr)
{
	return 0;
}

struct IoConsole* IoWinConOpen(void)
{
	static struct IoConsole io;

	HANDLE hout;
	
	AllocConsole();

	hout=GetStdHandle(STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE!=hout)
		SetConsoleTitle("awBASIC Debug Console"); 

	io.io_inkey		= IoWinConInkey;
	io.io_putc		= IoWinConPutc;
	io.io_chk_brk	= IoWinConChkBrk;
	io.io_getc		= IoWinConGetC;
	io.io_close		= IoWinConClose;

	io.io_width = 0; /* zero means no window handeling by io_ functions */
	io.io_height = 0;
	

	/* The following functions are setup so that BASIC statements that use windowed console functions will 
	   not cause an error. */

	io.io_peek_keyboard=IoWinConTrsPeekKeyboard;

	return &io;
}