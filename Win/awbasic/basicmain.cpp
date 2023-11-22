//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>
#include <ctype.h> 		// isdigit
#include <string.h>		// strlen
#include <ctype.h>
#include <memory.h>		// memset
#include <malloc.h>

#include "basic.h"

//
// STATUS 4-19-02:
//
// BUGS
//		A. Need to report error on LPRINT@
//		A. DIM A$,B% not supported (see wheel.bas)
//		b. Add PRINT USING "!";"ABC";"DEF" format.
//		c. Bug? defstr a:a="fred":defint a.  Lose memory?
//		d. BUG:  hold key down, move focus out of window (or click and hold title bar), release key.  Key up lost / bit stuck in map.
//		e. Upon a line change due to a return or next, the trace window doesn't print out the new line number.
//		f. ConWin title is hardcoded to debug window.
//		g. When basic first run, window sometimes opens off screen.
//		h. In Win95, when you close the console with close box, get a system error message.
//		i. PRINT USING"## ,##";L;M;   Not suported (used in strek40a.bas and others)
//		J. IF A < > 5 THEN 10  doesn't work.  (note space < >).  Found one program (wumpus*) that did this.
//		k. for x=0 to 1 step 0:next should be infinte loop, but its not. Also, forx=1to1step0 is not infinte loop in real l2 basic
//		l. Double click on basic window title bar makes it full screen
//		m. 10 if a$="<RETURN GO TO NEXT LINE>" then .....  cases and END (I think the file loader is stopping, no error)
//		n.  2085      ford=1to4:nextd     ' comment   Gives a SYNTAX ERROR
//

//************************

/*
 * Allocate an execution context for basic.
 * This current implementation only allows one basic process, but could
 * be easily modified to use malloc for the creation of multiple process.
 */
struct BasicCode* BasicCreateProcess(struct IoConsole* primary_con, IoConsole* debug_con)
{
	static struct BasicCode code;
	static bool already_called = FALSE;

	if (already_called)
		return NULL;

	already_called = TRUE;

	/*
	* Set up consoles
	*/

	memset(&code, 0, sizeof(code));

	BasicSetDefaultState(&code);

	code.std_con=primary_con;
	code.debug_con=DebugAttachConsole(&code, debug_con);

	return &code;
}

/* Free any process resources */

void BasicDestroyProcess(struct BasicCode* p)
{
	BasicSetDefaultState(p);
}

 
/* Sets numerical variables to 0.  Frees and clears strings */

void BasicClear(struct BasicCode* p)
{
	short i;

	for (i=0; i < MAX_VARIABLES; i++)
		if (p->variables[i].label)
			if (p->variables[i].value.type_flags&VT_STRING && p->variables[i].value.value.string)
				free(p->variables[i].value.value.string);

	memset(p->variables, 0, sizeof(p->variables));
}

/*
 * Reset a struct BasicCode so it is in the state for a new 'Run'
 * Frees and zeros any allocated variables, resets the stack pointer, resets data pointer, and variable default types
 * Sets up code just as if a new Run is about to happen.
 */

void BasicSetDefaultState(struct BasicCode* p)
{
	short i;

	BasicClear(p);

	if (p->print_con)
	{
		io_close(p->print_con);
		p->print_con=NULL;
	}

	for (i=0; i < 26; i++)
		p->deftab[i]=VT_FLOAT;

	p->stack.sp=0;
	p->onerror=0;
	DoRestore(p);
	p->pc = &p->image[0];

	if (p->std_con)
		io_inkey(p->std_con);	// clear the inkey buffer (it was remembering the last key press)
}

/*
 * Given an execution context, run the code in it.
 */

int BasicRun(struct BasicCode* code)
{
	int err;

	code->run_mode=RM_RUNNING;
	err = Interpret(code);
	code->run_mode=RM_IDLE;

	if (code->print_con)
	{
		io_close(code->print_con);
		code->print_con=NULL;
	}

	if (err==ERR_WINDOWCLOSED)
		return err;

	if (err != ERR_NORMAL_END)
	{
		int current_line = code->current_line_start_pc[2] + 
					  (code->current_line_start_pc[3]<<8);
		
		if (io_get_cursor(code->std_con)%code->std_con->io_width!=0)
			io_putc(code->std_con,'\n');

		if (err==ERR_BREAK)
		{
			io_printf1i(code->std_con, "Break in %d\n", current_line);
		}
		else
		{
			io_putc(code->std_con, '?');
			io_putstr(code->std_con, LookupErrorString(err));
			io_printf1i(code->std_con, " Error in %d\n", current_line);

			if (code->debug_con)
				DebugErrorDump(code, code->debug_con, err);
		}
	}

	io_putstr(code->std_con, "*** BASIC Execution Complete\n");

	return err;
}

bool BasicSaveFile(struct BasicCode* code, char* filespec)
{
	struct IoConsole* io;
	
	io = IoFileWriteOpen(filespec);

	if (io)
	{
		DebugShowList(code, io);
		io_close(io);
		return TRUE;
	}

	return FALSE;
}

bool BasicLoadFile(struct BasicCode* process, char* filespec)
{
	unsigned char buffer[128*1024];		// temp storage for read files
	FILE* fp;
	size_t size;

	BasicSetDefaultState(process);  
	
	fp=fopen(filespec, "rb");

	size = fread(buffer, sizeof (buffer[0]), sizeof (buffer), fp);

	if (size == 0 || size == sizeof(buffer))
	{
		return FALSE;
	}

	fclose(fp);

	if (isgraph(buffer[0]) || isspace(buffer[0]))		// ASCII File
	{
		buffer[size] = 0; // mark end
		return Tokenize((char*)buffer, process);
	}
	else
	{
		return LoadAlreadyTokenized(buffer, size, process);
	}
}

bool LoadAlreadyTokenized(unsigned char* source, int size, struct BasicCode* code)
{

	int i, last;
	unsigned char* m_memory;

	m_memory = code->image;
	if (size > MAX_IMAGESIZE)
	{
		io_putstr(code->std_con,"code to large\n");
		return FALSE;
	}

	if (source[0] != 0xFF)	// BASIC disk images always start with 0xFF
	{
		io_putstr(code->std_con, "Not a TRS-80 L2 BASIC tokenized file\n");
		return FALSE;
	}

	i = 0;	// basic program address
	memcpy(m_memory+i, source+1, size-1);

	//
	// A BASIC file is a dump of the RAM image.  Scan and patchup address
	// in case the memory location this BASIC program was placed at has changed.
	// (BASIC RAM images have the format <addr of next line><line number>...<0><addr of next line>...)
	//
	do
	{
		last = i;
		i = i+4;

		while (m_memory[i])
			i++;

		i++;

		m_memory[last] = i&0xFF;
		m_memory[last+1] = i>>8;

	} while (m_memory[i] || m_memory[i+1]);	// two bytes of zero mark the end of a basic program

	i = i + 2;					// point to the first byte past the basic program in memory

	return TRUE;
}

