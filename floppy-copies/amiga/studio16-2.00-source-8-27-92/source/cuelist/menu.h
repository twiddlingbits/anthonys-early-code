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

char stTopaz[] = "topaz.font";

struct TextAttr taPlain =
{
	stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

struct TextAttr taBIU =
{
	stTopaz, 8, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taBU =
{
	stTopaz, 8, FSF_BOLD | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taBI =
{
	stTopaz, 8, FSF_BOLD | FSF_ITALIC, FPF_ROMFONT
};

struct TextAttr taB =
{
	stTopaz, 8, FSF_BOLD, FPF_ROMFONT
};

struct TextAttr taIU =
{
	stTopaz, 8, FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taI =
{
	stTopaz, 8, FSF_ITALIC, FPF_ROMFONT
};

struct TextAttr taU =
{
	stTopaz, 8, FSF_UNDERLINED, FPF_ROMFONT
};

struct IntuiText ItemText[] =
{
	{ 6, 1, JAM1, 0, 1, &taPlain, " New", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Load", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Merge", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Save", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Save As", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, "···········", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Offset", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Prefs", NULL },

	{ 6, 1, JAM1, 0, 1, &taPlain, " Add", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Duplicate", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Delete", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, "···············", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " TC Add", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " TC Update", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, "···············", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Play", NULL },
	{ 6, 1, JAM1, 0, 1, &taPlain, " Stop", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
	},
	{
		&Items[3], 0, 20, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[3], NULL, 'V', NULL, NULL 
	},
	{
		&Items[5], 0, 40, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP ,
		0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
	},
	{
		&Items[6], 0, 50, 88, 10, ITEMENABLED | ITEMTEXT ,
		0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		&Items[7], 0, 60, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 70, 88, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
	},
	{
		&Items[9], 0, 0, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[8], NULL, 'A', NULL, NULL 
	},
	{
		&Items[10], 0, 10, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[9], NULL, 'L', NULL, NULL 
	},
	{
		&Items[11], 0, 20, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[10], NULL, 'D', NULL, NULL 
	},
	{
		&Items[12], 0, 30, 15*8, 10, ITEMENABLED | ITEMTEXT,
		0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
	},
	{
		&Items[13], 0, 40, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[12], NULL, 'T', NULL, NULL 
	},
	{
		&Items[14], 0, 50, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[13], NULL, 'U', NULL, NULL 
	},
	{
		&Items[15], 0, 60, 15*8, 10, ITEMENABLED | ITEMTEXT ,
		0, (APTR)&ItemText[14], NULL, NULL, NULL, NULL 
	},
	{
		&Items[16], 0, 70, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[15], NULL, 'P', NULL, NULL 
	},
	{
		NULL, 0, 80, 15*8, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP|COMMSEQ,
		0, (APTR)&ItemText[16], NULL, 'S', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], MENU_OFFSET, 0, 8*10, 0, MENUENABLED, " CueList ", &Items[0] },
	{ NULL, 81 +MENU_OFFSET, 0, 8*8, 0, MENUENABLED, " Entry", &Items[8] },
};

