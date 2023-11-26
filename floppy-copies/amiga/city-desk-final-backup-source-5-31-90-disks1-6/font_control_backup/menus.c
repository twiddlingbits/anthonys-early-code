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
	{ 0, 1, JAM1, 0, 1, &taPlain, "Download List", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Download Font", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Delete All Fonts", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Reset Printer", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Test Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Select Font by ID", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "About...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "QUIT", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create .hp File", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create .ps File", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
	},
	{
		&Items[3], 0, 20, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
	},
	{
		&Items[5], 0, 40, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
	},
	{
		&Items[6], 0, 50, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		&Items[7], 0, 60, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 70, 152, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
	},
	{
		&Items[9], 0, 0, 136, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 10, 136, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, NULL, NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 118, 10, MENUENABLED, " LaserJet+   ", &Items[0] },
	{ NULL, 120, 0, 86, 10, MENUENABLED, "City Desk", &Items[8] },
};

