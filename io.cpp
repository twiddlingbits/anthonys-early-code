//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "basic.h"

/*
 * io.cpp
 *
 * Console I/O routines.
 *
 * These functions take a pointer to a struct IoConsole, which must be created via a call
 * to a device dependent console creation call.
 * 
 * Two types of consoles are supported:  TTY (serial, stdin/out) and Windowed. 
 * The basic TTY functions (e.g. putc) will work on TTY or Windowed consoles.
 * The Windowed functions (eg. setreset, set_io->cursor) only work on windowed consoles.
 * 
 * 	io->io_width and io->io_height are set to zero for TTY devices (stream devices)
 *
 * struct IoConsole contains pointers to device dependent driver functions, as well
 * as variables used by this code.
 *
 * For Example:
 *    struct IoConsole* io = IoWinConOpen();
 *    io_putstr(io, "hello world\n");
 *
 */


//*************************************************

void _erase_line(struct IoConsole* io)
{
	for (int i=io->cursor; i < (io->cursor/io->io_width)*io->io_width+io->io_width; i++)
		io_set_c(io, i, ' ');
}
		
void io_putc(struct IoConsole* io, char cp)
{
	unsigned char c = *((unsigned char*)&cp);

	if (io->io_putc)
	{
		/*
		 * If a stream device (should always be if here), then maintain the cursor
		 */
		if (io->io_width==0 || io->io_height==0)	// Stream
		{
			if (c==13 || c==10)	// return
			{
				io->cursor = 0;
			}
			else if (c==8)	// backspace
			{
				if (io->cursor > 0)
					io->cursor--;
			}
			else if (c >=192)	// 192 to 255 are shortcuts for 0 to 63 spaces, respectively
			{
				for (int i=0; i < (c-192); i++)
					io_putc(io, ' ');

				return;
			}
			else if (c==24)	/* backspace cursor*/
			{
				if (io->cursor > 0)
					io->cursor--;
			}
			else if (c==25)	/* advance cursor*/
			{
				io->cursor++;
			}
			else
			{
				io->cursor++;
			}
		}
		(*io->io_putc)(io, c);
		return;
	}

	if (c==13 || c==10)	// return
	{
		if (io->cursor_visable)
			io_set_c(io, io->cursor,' ');
		
		io->cursor = io->cursor/io->io_width;
		io->cursor = io->cursor*io->io_width;
		io->cursor = io->cursor + io->io_width;
		
		/* if return put us on a new line that isn't a scroll, erase the line */
		if (io->cursor < io->io_width*io->io_height)	
			_erase_line(io);
	}
	else if (c==8)	// backspace
	{
		if (io->cursor > 0)
		{
			if (io->cursor_visable)
				io_set_c(io, io->cursor,' ');
			io->cursor--;
			io_set_c(io, io->cursor,' ');
		}
	}
	else if (c==0xE)	// Turn on cursor
	{
		io->cursor_visable = TRUE;
	}
	else if (c==0xF)	// Turn off cursor
	{
		io_set_c(io, io->cursor,' ');
		io->cursor_visable = FALSE;
	}
	else if (c >=192)	// 192 to 255 are shortcuts for 0 to 63 spaces, respectively
	{
		for (int i=0; i < (c-192); i++)
			io_putc(io, ' ');
	}
	else if (c==24)	/* backspace cursor*/
	{
		if (io->cursor > 0)
			io->cursor--;
	}
	else if (c==25)	/* advance cursor*/
	{
		if (io->cursor < (io->io_width*io->io_height-1))
			io->cursor++;
	}
	else if (c==26)	/* cursor down one line */
	{
		if (io->cursor < io->io_width*(io->io_height-1))
			io->cursor+=io->io_width;
	}
	else if (c==27)	/* cursor up one line */
	{
		if (io->cursor >= io->io_width)
			io->cursor-=io->io_width;
	}
	else if (c==28)	/* home */
	{
		io->cursor=0;
	}
	else if (c==29)	/* beginning of line */
	{
		io->cursor=(io->cursor/io->io_width)*io->io_width;
	}
	else if (c==30)	/* erase to end of line */
	{
		_erase_line(io);
	}
	else if (c==31)	/* erase to end of frame */
	{
		for (int i=io->cursor; i < io->io_width*io->io_height; i++)
			io_set_c(io, i, ' ');
	}
	else
	{
		io_set_c(io, io->cursor, c);
		io->cursor++;
	}

	// Do we need to scroll?
	if (io->cursor == io->io_width*io->io_height)	
	{
		io->cursor = io->io_width*(io->io_height-1);
		for (int i=0; i < (io->io_width*(io->io_height-1)); i++)
			io->video_mem[i] = io->video_mem[i+io->io_width];

		for (i=0; i < io->io_width; i++)
			io->video_mem[io->io_width*io->io_height-i-1] = ' ';

		io_draw_range(io, 0, io->io_width*io->io_height-1);
	}

	if (io->cursor_visable)
		io_set_c(io, io->cursor, '_');

	if (io->cursor >=io->io_width*io->io_height)
	{
		io->cursor=0;		// SHOULD NEVER HAPPEN
		io_putstr(io, "INTERR");
	}
}

//*************************************************

char io_inkey(struct IoConsole* io)
{
	return (*io->io_inkey)(io);
}

//*************************************************

char io_getc(struct IoConsole* io)
{
	return (*io->io_getc)(io);
}

//*************************************************

int io_chk_brk(struct IoConsole* io)
{
	return (*io->io_chk_brk)(io);
}

//*************************************************

void io_close(struct IoConsole* io)
{
	if (io->io_close)
		io->io_close(io);
}

//*************************************************
void io_cls(struct IoConsole* io)
{
	if (io->io_width==0 || io->io_height==0)
		return;

	for (int i=0; i < io->io_width*io->io_height; i++)
		io->video_mem[i]=' ';

	io->cursor = 0;
	io->cursor_visable = FALSE;

	io_draw_range(io, 0, io->io_width*io->io_height-1);
}


//*************************************************

void io_set_c(struct IoConsole* io, unsigned short loc, unsigned char c)
{
	if (io->io_width==0 || io->io_height==0)
		return;

	io->video_mem[loc]=c;
	io->io_draw_range(io, io->video_mem, loc, loc);
}

//*************************************************


unsigned char io_peek(struct IoConsole* io, short loc)
{
	if (io->io_width==0 || io->io_height==0)
		return 0;

	/** Video Ram? **/
	if (loc>=0x3c00 && loc<=0x3fff)
		return io->video_mem[loc-0x3c00];
	else if (loc>=0x3801 && loc<=0x3880)
		return io->io_peek_keyboard(io, loc);
	else
		return 0;
}

//*************************************************

bool io_setreset(struct IoConsole* io, short x, short y, bool isset)
{
	unsigned short loc = x/2+io->io_width*(y/3);
	unsigned char cellx = x%2;
	unsigned char celly = y%3;

	if (loc>=io->io_width*io->io_height)
		return FALSE;

	if (!(io->video_mem[loc]&128))
		io->video_mem[loc]= 128;	/* set to a cleared graphics value */

	if (isset)
		io->video_mem[loc]|= (1<<(celly*2+cellx));
	else
		io->video_mem[loc]&= ~(1<<(celly*2+cellx));


	io_draw_range(io, loc, loc);

	return TRUE;
}

//*************************************************

short io_point(struct IoConsole* io, short x, short y)
{
	unsigned short loc = x/2+io->io_width*(y/3);
	unsigned char cellx = x%2;
	unsigned char celly = y%3;

	if (loc>=io->io_width*io->io_height)
		return 0;		/* -1 = TRUE, 0 = FALSE */

	if (!(io->video_mem[loc]&128))
		return 0;	/* not a graphic cell, so false */

	if (io->video_mem[loc]&(1<<(celly*2+cellx)))
		return -1;	/* -1 - TRS-80 TRUE */
	else 
		return 0;
}


//*************************************************

void io_set_cursor(struct IoConsole* io, short i)
{
	if (io->io_width==0 || io->io_height==0)
		return;

	if (i <0) i =0;
	else if (i >= io->io_width*io->io_height) 
		i=io->io_width*io->io_height-1;
	io->cursor = i;
}

//*************************************************

short io_get_cursor(struct IoConsole* io)
{
	return io->cursor;
}

//*************************************************


void io_putstr(struct IoConsole* io, char* str)
{
	for (int i=0; str[i]; i++)		// Otherwise, implement using putc
		io_putc(io, str[i]);
}

//*************************************************

void io_draw_range(struct IoConsole* io, int start, int end)
{
	if (io->io_width==0 || io->io_height==0)
		return;

	io->io_draw_range(io, io->video_mem, start, end);
}

//*************************************************

char *io_gets(struct IoConsole* io, char *buffer )
{
	short i=0;
	int c;

	io_putc(io, 0xE);		/* io->cursor on */

	while (TRUE)
	{
		c=io_getc(io);
		if (c==	0x1b)		// ESC key
			return NULL;

		if (!(c==0x08 && i == 0))
			io_putc(io, c);

		if (c=='\n' || c=='\r')
		{
			buffer[i]=0;
			io_putc(io, 0xF);		/* io->cursor off */
			return buffer;
		}

		else if (c==0x08 && i > 0)
		{
			i--;
		}
		else if (isgraph(c) || c==' ')
		{
			buffer[i++] = c;
		}
	}
}


//*************************************************

int io_printf1i(struct IoConsole *io, const char *format, int arg1)
{
    char buffer[256];
    int retval;

    retval = _snprintf(buffer, sizeof(buffer), format, arg1);

	buffer[sizeof(buffer)-1]=0;

	io_putstr(io, buffer);

	return retval;
}

//*************************************************

int io_printf2i(struct IoConsole *io, const char *format,  int arg1, int arg2)
{
    char buffer[256];
    int retval;

    retval = _snprintf(buffer, sizeof(buffer), format, arg1, arg2);

	buffer[sizeof(buffer)-1]=0;

	io_putstr(io, buffer);

	return retval;
}

//*************************************************

/*
 * PRINT USING format
 *
 * supports all format specifiers except ^ (E&D)
 *
 */

void io_printusingnum(struct IoConsole *io, char* string, double value)
{
	int i=0,adp,ads,numcommas;
	bool dollar=FALSE, astrix=FALSE, postplus=FALSE,postneg=FALSE, preplus=FALSE, comma=FALSE;
	int predec=0, postdec=0, pad;
	char * ad;

	/*
	 * scan up until the first # 
	 */
	while (TRUE)
	{
		if (string[i]=='*' && string[i+1]=='*')
		{
			astrix = TRUE;
			predec+=2;
			i=i+2;
			if (string[i]=='$')
			{
				i++;
				dollar=TRUE;
			}
			break;
		}
		else if (string[i]=='$' && string[i+1]=='$')
		{
			dollar = TRUE;
			i=i+2;
			break;
		}
		else if (string[i]=='+' && string[i+1]=='#')
		{
			preplus=TRUE;
			i++;
			break;
		}
		else if (string[i]=='.' && string[i+1]=='#')
			break;

		else if (string[i]=='#')
			break;

		else
			io_putc(io, string[i++]);
	}

	/*
	 * scan the # field 
	 */

	while (string[i]==',' || string[i]=='#')
	{
		predec++;
		if (string[i]==',')
			comma = TRUE;
		i++;
	}
	if (string[i]=='.')
	{
		i++;

		while (string[i]=='#')
		{
			postdec++;
			i++;
		}
	}
	/*
	 * any post + o - ?
	 */

	if (string[i]=='-')
		postneg=TRUE;
	else if (string[i]=='+')
		postplus=TRUE;

	/* 
	 * convert the number to ASCII
	 */

	ad=_fcvt(value, postdec, &adp, &ads);

	/*
	 * take all the preceding requirments and generate the output formated string
	 */

	if (adp < 0) adp = 0;
	
	if (comma)
		numcommas=adp/3;
	else
		numcommas=0;

	pad = predec - adp - numcommas;
	if (preplus || ads!=0)
		pad--;	/* the sign will take one space */

	if (pad < 0)
		io_putc(io, '%'); /* error */

	while (pad-- > 0)
	{
		if (astrix)
			io_putc(io, '*');
		else
			io_putc(io, ' ');
	}

	if (predec > 0)
	{
		if (dollar)
			io_putc(io, '$');

		if (ads!=0)	
			io_putc(io, '-');

		if (ads==0 && preplus)
			io_putc(io, '+');

		if (numcommas == 0)
			for (i=0; i < adp; i++)
				io_putc(io, ad[i]);
		else
		{
			int commacount=adp%3;

			for (i=0; i < adp; i++)
			{
				io_putc(io, ad[i]);
				commacount--;
				if (commacount%3==0 && i!=(adp-1))
					io_putc(io, ',');
			}
		}
	}

	if (postdec > 0)
	{
		io_putc(io, '.');
		io_putstr(io, ad+adp);
	}
	if (postneg && ads!=0)
		io_putc(io, '-');

	if (postplus)
	{
		if (ads==0) io_putc(io, '+'); 
		else io_putc(io, '-');
	}
}

//*************************************************
#if 0  // CANT GET THIS TO WORK. arglist is a struct, not a representation of a variable arg list
int __cdecl trs80_printf(const char *format,  ... )
{
        va_list arglist;
        char buffer[256];
        int retval;

        va_start(arglist, format);

        retval = _snprintf(buffer, sizeof(buffer), format, (char*)arglist);
	//	retval = sprintf(buffer, format, arglist);
		buffer[sizeof(buffer)-1]=0;

		

		for (int i=0; buffer[i]; i++)
			trs80_putchar(buffer[i]);

		return retval=0;
}
#endif 