#include <stdio.h>
#include <ctype.h>
#include "basic.h"

static unsigned short cursor;
static bool cursor_visable = FALSE;
static unsigned char* video_mem;

void IoTrsSetChar(unsigned short loc, unsigned char c)
{
	video_mem[loc]=c;
	WinDrawRange(video_mem, loc, loc);
}

unsigned char IoTrsPeek(unsigned short p1)
{
	/** Video Ram? **/
	if (p1>=0x3c00 && p1<=0x3fff)
		return video_mem[p1-0x3c00];
	else if (p1>=0x3801 && p1<=0x3880)
		return WinTrsPeekKeyboard(p1);
	else
		return 0;
}


bool IoTrsSetReset(short x, short y, bool isset)
{
	unsigned short loc = x/2+64*(y/3);
	unsigned char cellx = x%2;
	unsigned char celly = y%3;

	if (loc>1023)
		return FALSE;

	if (!(video_mem[loc]&128))
		video_mem[loc]= 128;	/* set to a cleared graphics value */

	if (isset)
		video_mem[loc]|= (1<<(celly*2+cellx));
	else
		video_mem[loc]&= ~(1<<(celly*2+cellx));


	WinDrawRange(video_mem, loc, loc);

	return TRUE;
}

short IoTrsPoint(short x, short y)
{
	unsigned short loc = x/2+64*(y/3);
	unsigned char cellx = x%2;
	unsigned char celly = y%3;

	if (loc>1023)
		return -1;		/* -1 = TRUE, 0 = FALSE */

	if (!(video_mem[loc]&128))
		return 0;	/* not a graphic cell, so false */

	if (video_mem[loc]&(1<<(celly*2+cellx)))
		return -1;	/* -1 - TRS-80 TRUE */
	else 
		return 0;
}

void IoTrsSetCursor(short i)
{
	if (i <0) i =0;
	else if (i > 1023) i=1023;
	cursor = i;
}

short IoTrsGetCursor()
{
	return cursor;
}

void IoTrsPutC(char cp)
{
	unsigned char c = *((unsigned char*)&cp);

	if (c==13 || c==10)	// return
	{
		if (cursor_visable)
			IoTrsSetChar(cursor,' ');
		
		cursor = cursor/64;
		cursor = cursor*64;
		cursor = cursor + 64;
	}
	else if (c==8)	// backspace
	{
		if (cursor > 0)
		{
			if (cursor_visable)
				IoTrsSetChar(cursor,' ');
			cursor--;
				IoTrsSetChar(cursor,' ');
		}
	}
	else if (c==0xE)	// Turn on cursor
	{
		cursor_visable = TRUE;
	}
	else if (c==0xF)	// Turn off cursor
	{
		IoTrsSetChar(cursor,' ');
		cursor_visable = FALSE;
	}
	else if (c >=192)	// 192 to 255 are tabs for 0 to 63 spaces, respectively
	{
		for (int i=0; i < (c-192); i++)
			IoTrsPutC(' ');
	}
	else if (c==24)	/* backspace cursor*/
	{
		if (cursor > 0)
			cursor--;
	}
	else if (c==25)	/* advance cursor*/
	{
		if (cursor < 1023)
			cursor++;
	}
	else if (c==26)	/* cursor down one line */
	{
		if (cursor <= 1023-64)
			cursor+=64;
	}
	else if (c==27)	/* acursor up one line */
	{
		if (cursor >= 64)
			cursor-=64;
	}
	else if (c==28)	/* home */
	{
		cursor=0;
	}
	else if (c==29)	/* beginning of line */
	{
		cursor=(cursor/64)*64;
	}
	else if (c==30)	/* erase to end of line */
	{
		for (int i=cursor; i < (cursor/64)*64+64; i++);
			IoTrsSetChar(i, ' ');
	}
	else if (c==31)	/* erase to end of frame */
	{
		for (int i=cursor; i < 1024; i++);
			IoTrsSetChar(i, ' ');
	}
	else
	{
		IoTrsSetChar(cursor,c);
		cursor++;
	}

	// Do we need to scroll?
	if (cursor == 1024)	
	{
		cursor = 1024 - 64;
		for (int i=0; i < (1024 - 64); i++)
			video_mem[i] = video_mem[i+64];

		for (i=0; i < 64; i++)
			video_mem[1024-i-1] = ' ';

		WinDrawRange(video_mem, 0, 1023);
	}

	if (cursor_visable)
		IoTrsSetChar(cursor, '_');

	if (cursor >=1024)
	{
		cursor=0;		// SHOULD NEVER HAPPEN
		IoTrsPutC('E');IoTrsPutC('R');
	}
}

//*************************************************

void IoTrsCls(void)
{
	for (int i=0; i < 1024; i++)
		video_mem[i]=' ';

	cursor = 0;
	cursor_visable = FALSE;

	WinDrawRange(video_mem, 0, 1023);
}


