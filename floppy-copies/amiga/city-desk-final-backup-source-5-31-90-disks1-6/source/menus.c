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
	{ 0, 1, JAM1, 0, 1, &taPlain, "New", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Open...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Append", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save As...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Load Graphic", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Load Text", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save Object...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Document", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "About", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Cut Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Copy Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Paste Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Erase Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Page Format  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save City Desk Prefs", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Load City Desk Prefs", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Set Options", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Show Status   ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Assign Fonts:", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Load Macro File", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Set Grid", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create Graphic", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Invert Graphic", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Connect Text Chain", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Break Text Chain", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create Object Links   ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Unlink Entire Group", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Coordinate Display", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, 'N', NULL, NULL 
	},
	{
		&Items[2], 0, 10, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'O', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, 'S', NULL, NULL 
	},
	{
		&Items[5], 0, 40, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'V', NULL, NULL 
	},
	{
		&Items[6], 0, 50, 155, 10, ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		&Items[7], 0, 60, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, 'G', NULL, NULL 
	},
	{
		&Items[8], 0, 70, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, 'T', NULL, NULL 
	},
	{
		&Items[9], 0, 80, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
	},
	{
		&Items[10], 0, 90, 155, 10, ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, NULL, NULL, NULL 
	},
	{
		&Items[11], 0, 100, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, 'P', NULL, NULL 
	},
	{
		&Items[12], 0, 110, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
	},
	{
		&Items[13], 0, 120, 155, 10, ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[12], NULL, NULL, NULL, NULL 
	},
	{
		&Items[14], 0, 130, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[13], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 140, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[14], NULL, 'Q', NULL, NULL 
	},
	{
		&Items[16], 0, 0, 147, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[15], NULL, 'X', NULL, NULL 
	},
	{
		&Items[17], 0, 10, 147, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[16], NULL, 'C', NULL, NULL 
	},
	{
		&Items[18], 0, 20, 147, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[17], NULL, 'J', NULL, NULL 
	},
	{
		&Items[19], 0, 30, 147, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[18], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 40, 147, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[19], NULL, 'F', NULL, NULL 
	},
	{
		&Items[21], 0, 0, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[20], NULL, NULL, NULL, NULL 
	},
	{
		&Items[22], 0, 10, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[21], NULL, NULL, NULL, NULL 
	},
	{
		&Items[23], 0, 20, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[22], NULL, 'H', NULL, NULL 
	},
	{
		&Items[24], 0, 30, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[23], NULL, 'B', NULL, NULL 
	},
	{
		&Items[25], 0, 40, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[24], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 50, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[25], NULL, NULL, NULL, NULL 
	},
	{
		&Items[27], 0, 0, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[26], NULL, 'A', NULL, NULL 
	},
	{
		&Items[28], 0, 10, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[27], NULL, 'E', NULL, NULL 
	},
	{
		&Items[29], 0, 20, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[28], NULL, 'I', NULL, NULL 
	},
	{
		&Items[30], 0, 30, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[29], NULL, 'Y', NULL, NULL 
	},
	{
		&Items[31], 0, 40, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[30], NULL, 'Z', NULL, NULL 
	},
	{
		&Items[32], 0, 50, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[31], NULL, 'L', NULL, NULL 
	},
	{
		&Items[33], 0, 60, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[32], NULL, 'U', NULL, NULL 
	},
	{
		NULL, 0, 70, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[33], NULL, 'D', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 86, 10, MENUENABLED, "Project  ", &Items[0] },
	{ &Titles[2], 88, 0, 62, 10, MENUENABLED, "Edit  ", &Items[15] },
	{ &Titles[3], 149, 0, 118, 10, MENUENABLED, "Preferences  ", &Items[20] },
	{ NULL, 266, 0, 86, 10, MENUENABLED, "General  ", &Items[26] },
};

