//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <string.h>		// strlen
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "basic.h"

/*
 * Debug.cpp
 *
 * This file contains the implementation of the BASIC debug console.  The debug console is command line based,
 * and can be used to issue comands like "tron" (Trace On).  The debug console io is performed
 * using BasicCode.debug_con console handle.  This pointer can be set by an initial call to DebugOpenConsole().
 */

void DebugShowCmdPrompt(IoConsole* io)
{
	io_putstr(io, "Command> ");
}


struct IoConsole* DebugAttachConsole(struct BasicCode* bc, IoConsole* io)
{
	bc->debug_con = io;

	if (io)	// NULL console allowed
	{
		io_putstr(io, "Welcome to the awBASIC debug console.\nEnter any BASIC statement, debug commands, or HELP.\n\n");
		DebugShowCmdPrompt(io);
	}

	return io;
}

void DebugExecuteCmd(IoConsole* io, struct BasicCode* code, char* cmd)
{

	if (stricmp(cmd, "tron")==0)
	{
		io_putstr(io, "Trace On (each statement)\n\n");
		code->tron = TRACE_STMT;
	}
	else if (stricmp(cmd, "tron line")==0)
	{
		io_putstr(io, "Trace On (each line)\n\n");
		code->tron = TRACE_LINE;
	}
	else if (stricmp(cmd, "help")==0)
	{
		io_putstr(io, "Debug Commands:\n   tron       Trace on (each statement)\n   tron line  Trace on (each line)\n   tokens     Show unknown tokens\n   list       List program\n");
	}
	else if (stricmp(cmd, "tokens")==0)
	{ 
		DebugShowUnknownTokens(code, io);
	}
	else if (stricmp(cmd, "list")==0)
	{ 
		DebugShowList(code, io);
	}
	else if (strlen(cmd)>0)
	{
		if (code->run_mode!=RM_IDLE)
		{
			io_putstr(io, "Can not execute a BASIC statement while code is running.\n");
		}
		else
		{
			int err, len, dummy;
			struct IoConsole *temp;

			TokenizeLine((unsigned char*)cmd, cmd, &len, &dummy);
			cmd[len]=0;
			code->pc=(unsigned char*)cmd;
			temp = code->std_con;
			code->std_con = code->debug_con;
			while ((err = ExecuteStatement(code)) == ERR_OKAY) ;
			code->std_con = temp;
			io_putstr(io, "BASIC engine return code: ");
			io_putstr(io, LookupErrorString(err));
			io_putc(io, '\n');
		}
	}

	DebugShowCmdPrompt(io);
}

void DebugTimeSlice(struct BasicCode* code)
{
	static char inbuffer[80];
	static int inpos=0;
	
	char c;

	if (code==NULL)
		return;

	IoConsole* io = code->debug_con;

	if (io==NULL)
		return;

	while (c=io_inkey(io))
	{
		if (code->tron > 0)
		{
			io_putstr(io, "\nTrace Off\n");
			code->tron = TRACE_NONE;
		}
		else
		{
			if (c==8)	/* backspace? */
			{
				if (inpos>0)
				{
					io_putc(io, c);
					io_putc(io, ' ');
					io_putc(io, c);
					inpos--;
				}
			}
			else
			{
				io_putc(io, c);
				if (c=='\r')
				{
					io_putc(io, '\n');
					inbuffer[inpos]=0;
					DebugExecuteCmd(io, code, inbuffer);
					inpos=0;
				}
				else
					inbuffer[inpos++]=c;
			}
		}
	}
}

void DebugTraceOn(struct BasicCode* code)
{
	code->tron=TRACE_STMT;
}

bool DebugIsToken(unsigned char c)
{
	return (c&128)==128;
}

#define DP_LIST_STATEMENT	0
#define DP_LIST_LINE		1
#define DP_SHOW_UNKNOWN_TOK	2

void DebugListPrimative(struct IoConsole* io, unsigned char *p,int mode)
{
	char* tokenstr;
	bool inquote=FALSE;

	while(TRUE)
	{
		if (*p=='\"')
		{
			if (mode!=DP_SHOW_UNKNOWN_TOK)
				io_putc(io, *p);
			inquote = !inquote;
		}
		else if (!inquote && DebugIsToken(*p))
		{
			tokenstr = TokenToAscii(*p);
			if (mode!=DP_SHOW_UNKNOWN_TOK)
			{
				if (tokenstr)
					io_putstr(io, tokenstr);
				else
					io_printf1i(io, "<UNKNOWN TOKEN %2xh>",*p);
			}
			else
			{
				if (tokenstr==NULL)
					io_printf1i(io, "token: %02xh\n",*p);
			}
		}
		else if (*p==NULL)
		{
			inquote = FALSE;
			return;
		}
		else if (IsEosToken(*p) && mode==DP_LIST_STATEMENT)
		{	
			return;
		}
		else if (isgraph(*p) || isspace(*p))
		{
			if (mode!=DP_SHOW_UNKNOWN_TOK)
				io_putc(io, *p);
		}
		else
		{
			if (mode!=DP_SHOW_UNKNOWN_TOK)
				io_printf1i(io,"<%d>", *p);
			else
				if (!inquote)
					io_printf1i(io,"unprintable: %02xh", *p);
		}
		p++;
	}
}

void DebugListCurrentLine(struct BasicCode* code, struct IoConsole* io)
{
	if (io==NULL)
		return;

	DebugListPrimative(io, 4+code->current_line_start_pc, DP_LIST_LINE);
	io_putc(io, '\n');
}

void DebugListCurrentStatement(struct BasicCode* code, struct IoConsole* io)
{
	if (io==NULL)
		return;

	DebugListPrimative(io, code->current_statement_start_pc, DP_LIST_STATEMENT);
}

void DebugErrorDump(struct BasicCode* code, struct IoConsole* io, int err)
{
	int current_line = code->current_line_start_pc[2] + 
					  (code->current_line_start_pc[3]<<8);

	io_putstr(io, "\n?");
	io_putstr(io, LookupErrorString(err));
	io_printf1i(io, " Error in %d\n", current_line);

	io_putstr(io, "\nLine with error:\n");
	DebugListCurrentLine(code, io);

	io_putstr(io, "\n\nStatement with error:\n: ");
	DebugListCurrentStatement(code, io);
	io_putstr(io, "\n");
	DebugShowCmdPrompt(io);
}


void DebugShowTrace(struct BasicCode* code)
{
	unsigned int current_line;
	struct IoConsole* io = code->debug_con;

	if (io==NULL)
		return;
	
	if (code->tron==TRACE_LINE)
	{
		current_line = code->current_line_start_pc[2] + (code->current_line_start_pc[3]<<8);

		if (code->last_tron_line != current_line)
		{
			io_printf1i(io, "%d ",current_line);
			DebugListCurrentLine(code, code->debug_con);
		}

		code->last_tron_line = current_line;
	}
	else
	{
		io_putc(io,'<');
		DebugListCurrentStatement(code, io);
		io_putc(io,'>');
	}
}

void DebugShowUnknownTokens(struct BasicCode* code, struct IoConsole* io)
{
	io_putstr(io, "Unknown Tokens (in hex):\n");
	int i=0;
	while (!(code->image[i] == 0 && code->image[i+1]==0))
	{
		DebugListPrimative(io, 4+&code->image[i], DP_SHOW_UNKNOWN_TOK);
		i = code->image[i] + (code->image[i+1]<<8);
	}
}

void DebugShowList(struct BasicCode* code, struct IoConsole* io)
{
	int i=0;
	while (!(code->image[i] == 0 && code->image[i+1]==0))
	{
		io_printf1i(io,"%d ", code->image[i+2] + (code->image[i+3]<<8));
		DebugListPrimative(io, 4+&code->image[i], DP_LIST_LINE);
		io_putc(io, '\n');
		i = code->image[i] + (code->image[i+1]<<8);
	}
}

char* LookupErrorString(unsigned char e)
{
	switch (e)
	{
	case ERR_INTERNAL:
		return "UNEXPECTED INTERNAL";

	case ERR_OKAY:
		return "OKAY";

	case ERR_UNDEFINED_TOKEN:
		return "UNDEFINED TOKEN";

	case ERR_NORMAL_END:
		return "NORMAL_END";

	case ERR_UNDEFINED_OP:
		return "UNDEFINED OPERATION";

	case ERR_MISSING_PARN:
		return "MISSING PARENTHESES";

	case ERR_NORMAL_EOL:
		return "NORMAL EOL";

	case ERR_NORMAL_CONT:
		return "NORMAL_CONT";

	case ERR_STACK_UNDER:
		return "STACK UNDERFLOW";

	case ERR_BREAK:
		return "BREAK";

	case ERR_WINDOWCLOSED:
		return "WINDOW CLOSED";

	case ERR_SYNTAX:
		return "SN";

	case ERR_DIV_ZERO:
		return "/0";

	case ERR_MISSING_LN:
		return "MISSING LINE NUMBER";

	case ERR_OUTOFMEM:
		return "OM";

	case ERR_STRINGTOLONG:
		return "STRING TO LONG";

	case ERR_TM:
		return "TM";

	case ERR_OS:
		return "OS";

	case ERR_RG:
		return "RG";

	case ERR_NF:
		return "NF";

	case ERR_FC:
		return "FC";

	case ERR_DD:
		return "DD";

	case ERR_BS:
		return "BS";

	case ERR_OD:
		return "OD";

	default:
		return "UNKNOWN ERROR";
	}
}


#if 0

char *debug_gets(char *buffer)
{
	HANDLE hin;
	BOOL suc;
	unsigned long dummy;

	tron_putstr("DEBUG_GETS\n");
	hin=GetStdHandle(STD_INPUT_HANDLE);

	if (INVALID_HANDLE_VALUE!=hin)
		suc=ReadConsole(hin, buffer, 79, &dummy,0);

	if (suc)
		return buffer;
	else
		return 0;
}
#endif
