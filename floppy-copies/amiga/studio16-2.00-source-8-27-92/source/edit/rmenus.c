/*****************************************************
 *                  Menu Definitions                 *
 *                                                   *
 *             Created with Menu Creator             *
 *                        by                         *
 *                   David Pehrson                   *
 *                                                   *
 *         Copyright (C) 1986  David Pehrson         *
 *                 Mantis Development                *
 *                                                   *
 *****************************************************/

#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/gadlib.h"
#include "/include/interface.h"

extern struct TextAttr taPlain;

struct IntuiText RItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, " Add", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Delete", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Update ", NULL },
};

struct MenuItem RItems[] =
{
	{
		&RItems[1], 0, 0, 107, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&RItemText[0], NULL, 'A', NULL, NULL 
	},
	{
		&RItems[2], 0, 10, 107, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&RItemText[1], NULL, 'D', NULL, NULL 
	},
	{
		NULL, 0, 20, 107, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&RItemText[2], NULL, 'U', NULL, NULL 
	},
};

struct Menu RTitles[] =
{
	{ NULL, MENU_OFFSET+3, 0, 70, 0, MENUENABLED, "Regions", &RItems[0] },
};

