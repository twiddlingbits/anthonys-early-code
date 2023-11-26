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


struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Hide Title Bar", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Freeze Display", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 177, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 10, 177, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE |COMMSEQ,
		0, (APTR)&ItemText[1], NULL, 'F', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ NULL,  MENU_OFFSET, 0, 118, 0, MENUENABLED, " SmpteMonitor", &Items[0] },
};

