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
	{ 0, 1, JAM1, 0, 1, &taPlain, "Cut", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Copy", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Paste", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Erase", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Insert", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Flip Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Sample Parameters", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 91, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, 'X', NULL, NULL 
	},
	{
		&Items[2], 0, 10, 91, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'C', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 91, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, 'V', NULL, NULL 
	},
	{
		&Items[4], 0, 30, 91, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 40, 91, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
	},
	{
		&Items[6], 0, 0, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, 'F', NULL, NULL 
	},
	{
		NULL, 0, 10, 179, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 62, 10, MENUENABLED, " Edit ", &Items[0] },
	{ NULL, 64, 0, 70, 10, MENUENABLED, " Tools ", &Items[5] },
};

