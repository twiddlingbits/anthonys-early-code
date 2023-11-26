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
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Serial Full", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Serial Short", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Window", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   -5", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "    0", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   45", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   75", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 20, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE | CHECKED,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 0, 56, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		14, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
	},
	{
		&Items[5], 0, 10, 56, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP|CHECKED,
		13, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
	},
	{
		&Items[6], 0, 20, 56, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		11, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 30, 56, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		7, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], MENU_OFFSET, 0, 80, 0, MENUENABLED, " MessMon ", &Items[0] },
	{ NULL, MENU_OFFSET+81, 0, 88, 0, MENUENABLED, " Priority", &Items[3] },
};

