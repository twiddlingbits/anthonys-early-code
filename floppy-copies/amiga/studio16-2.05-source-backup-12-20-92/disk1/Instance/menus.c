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

struct IntuiText SubText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Applications", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Utilities", NULL },
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, " Edit", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Duplicate", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Remove", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Show", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 84, 0, 136, 10, CHECKED|MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 84, 10, 136, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
	},
	{
		&Items[3], 0, 20, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 30, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, &SubItems[0], NULL 
	},
};

struct Menu Titles[] =
{
	{ NULL, MENU_OFFSET+3, 0, 86, 0, MENUENABLED, " Instance", &Items[0] },
};

