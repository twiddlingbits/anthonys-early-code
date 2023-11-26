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
	{ 0, 1, JAM1, 0, 1, &taPlain, "Open Project      ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Open Preferences", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Project", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Page", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Print Options", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, 'S', NULL, NULL 
	},
	{
		&Items[2], 0, 10, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'U', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, 'N', NULL, NULL 
	},
	{
		&Items[4], 0, 30, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, 'R', NULL, NULL 
	},
	{
		&Items[5], 0, 40, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'I', NULL, NULL 
	},
	{
		&Items[6], 0, 50, 187, 10, ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, 'Z', NULL, NULL 
	},
	{
		NULL, 0, 60, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, 'E', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ NULL, 3, 0, 78, 0, MENUENABLED, "Project ", &Items[0] },
};

