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

struct IntuiText SubText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, " RAW", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " IFF", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " RAW", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " IFF", NULL },
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "Open", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save As...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "About...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Start Scan", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Restart Scan", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Edit Picture", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Show Raw Data ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Set Screen Type", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Set Scan Area", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 111, 0, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 10, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[3], 111, 0, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 10, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
	},
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, &SubItems[0], NULL 
	},
	{
		&Items[2], 0, 10, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, NULL, &SubItems[2], NULL 
	},
	{
		&Items[3], 0, 20, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 123, 10, ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 40, 123, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'Q', NULL, NULL 
	},
	{
		&Items[6], 0, 0, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, 'S', NULL, NULL 
	},
	{
		&Items[7], 0, 10, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, 'R', NULL, NULL 
	},
	{
		&Items[8], 0, 20, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, 'E', NULL, NULL 
	},
	{
		NULL, 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, 'D', NULL, NULL 
	},
	{
		&Items[10], 0, 0, 136, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 10, 136, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, NULL, NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 78, 10, MENUENABLED, " Project", &Items[0] },
	{ &Titles[2], 80, 0, 54, 10, MENUENABLED, " Scan", &Items[5] },
	{ NULL, 133, 0, 78, 10, MENUENABLED, " Options", &Items[9] },
};

