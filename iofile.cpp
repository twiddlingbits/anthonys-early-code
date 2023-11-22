//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>

#include "basic.h"

struct IoConsoleFile {
	struct IoConsole io;
	FILE *fp;
};


void IoFilePutc(IoConsole* io, char c)
{
	FILE* fp;

	fp=((IoConsoleFile*)io)->fp;

	fputc(c, fp);
}

/*
void IoFilePutstr(IoConsole* io, char* cp)
{
	FILE* fp;

	fp=((IoConsoleFile*)io)->fp;
	if (fp==NULL)
	{
		fp = IoFileOpenFile();
		((IoConsoleFile*)io)->fp = fp;
	}

	if (fp==NULL)
		return;

	fprintf(fp, "%s",cp);
}
*/

void IoFileWriteClose(IoConsole* io)
{
	fclose(((IoConsoleFile*)io)->fp);
}

FILE* IoFileOpenFile(char *in_fname)
{
	char fname[400];
	FILE* fp = NULL;

	if (in_fname)
	{
		fp = fopen(in_fname, "wt");
	}
	else
	{
		if (WinGetFileName(fname, sizeof(fname), 0))
			fp = fopen(fname, "wt");
	}

	return fp;
}

struct IoConsole* IoFileWriteOpen(char *in_fname)
{
	static struct IoConsoleFile iof;

	iof.io.io_inkey		= NULL;
	iof.io.io_putc		= IoFilePutc;
	iof.io.io_chk_brk	= NULL;
	iof.io.io_getc		= NULL;
	iof.io.io_close		= IoFileWriteClose;
	iof.io.io_width = 0;
	iof.io.io_height = 0;
	
	iof.io.cursor=0;

	/* Window functions that are not needed for simple streaming console */

	iof.io.io_draw_range= NULL;
	iof.io.io_peek_keyboard=NULL;
	iof.io.cursor_visable = FALSE;
	iof.io.video_mem = NULL;

	/* values specific to this console type */
	iof.fp = IoFileOpenFile(in_fname);

	if (iof.fp)
		return (IoConsole*)&iof;
	else
		return NULL;
}

/*****************************************************/

void IoNullPutc(IoConsole* io, char c)
{
	return;
}


char IoNullInkey(struct IoConsole* io)
{
	return 0;
}


char IoNullGetc(struct IoConsole* io)
{
	return 0;
}


int IoNullChkBrk(struct IoConsole* io)
{
	return ERR_OKAY;
}


struct IoConsole* IoNullOpen(void)
{
	static struct IoConsole ionull;

	ionull.io_inkey		= IoNullInkey;
	ionull.io_putc		= IoNullPutc;
	ionull.io_chk_brk	= IoNullChkBrk;
	ionull.io_getc		= IoNullGetc;
	ionull.io_close		= NULL;		// null okay, just won't call any close function
	ionull.io_width = 0;
	ionull.io_height = 0;
	
	ionull.cursor=0;

	/* Window functions that are not needed for simple streaming console */

	ionull.io_draw_range= NULL;
	ionull.io_peek_keyboard=NULL;
	ionull.cursor_visable = FALSE;
	ionull.video_mem = NULL;

	return &ionull;
}