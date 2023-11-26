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
	{ 0, 1, JAM1, 0, 1, &taPlain, " Play Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Stop Playback", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Rename Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Delete Sample   ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Convert Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Edit Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "······················", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Add New Path", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Remove Path", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Update Paths", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "······················", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Show Regions", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, 'P', NULL, NULL 
	},
	{
		&Items[2], 0, 10, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'S', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, 'R', NULL, NULL 
	},
	{
		&Items[4], 0, 30, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, 'D', NULL, NULL 
	},
	{
		&Items[5], 0, 40, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'C', NULL, NULL 
	},
	{
		&Items[6], 0, 50, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, 'E', NULL, NULL 
	},
	{
		&Items[7], 0, 60, 179, 10, ITEMTEXT | ITEMENABLED,
		0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
	{
		&Items[8], 0, 70, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, 'A', NULL, NULL 
	},
	{
		&Items[9], 0, 80, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, 'M', NULL, NULL 
	},
	{
		&Items[10], 0, 90, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, 'U', NULL, NULL 
	},
	{
		&Items[11], 0, 100, 179, 10, ITEMTEXT | ITEMENABLED,
		0, (APTR)&ItemText[10], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 110, 179, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, 'G', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ NULL, MENU_OFFSET+3, 0, 94, 10, MENUENABLED, " Open List", &Items[0] },
};

