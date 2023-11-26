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
	{ 0, 1, JAM1, 0, 1, &taPlain, "Insert @ Start", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Insert @ End", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Replace Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Insert @ Start", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Insert @ End", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Replace Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Off", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   1 Frame", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   1 Second", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   10 Seconds", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   1 Minute", NULL },
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, " New", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Open", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "····················", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Play All", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Play Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Play Display", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Play To Start ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Stop Playing", NULL },
	{ 0, 1, JAM1, 0, 1, &taI, "Non Destructive", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Cut", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Copy", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Paste", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Keep", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Delete", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Undo Last", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Undo All", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Make Permanent", NULL },
	{ 0, 1, JAM1, 0, 1, &taI, "Destructive", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Cut", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Copy", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Copy As..", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Paste", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Keep", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Delete", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Echo", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " FFT", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Invert", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Normalize", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Reverse", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Scale", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Zero", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "···················", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Resample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Gen Silence", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Gen Sine Wave", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Set Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Set Start Mark", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Set End Mark", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Set Sample Parms", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, " Set Display Offset", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "························", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Loop", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Freehand Draw", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   OK Requesters", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Units in Samples", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Show Regions", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Grid", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 80, 0, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&SubText[0], NULL, 'V', NULL, NULL 
	},
	{
		&SubItems[2], 80, 10, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 80, 20, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[4], 80, 0, 128, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[5], 80, 10, 128, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[4], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 80, 20, 128, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[5], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[7], 80, 0, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		30, (APTR)&SubText[6], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[8], 80, 10, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		29, (APTR)&SubText[7], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[9], 80, 20, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		27, (APTR)&SubText[8], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[10], 80, 30, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		23, (APTR)&SubText[9], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 80, 40, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		15, (APTR)&SubText[10], NULL, NULL, NULL, NULL 
	},
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'O', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 163, 10, ITEMTEXT | ITEMENABLED,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, 'A', NULL, NULL 
	},
	{
		&Items[5], 0, 40, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'P', NULL, NULL 
	},
	{
		&Items[6], 0, 50, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, 'U', NULL, NULL 
	},
	{
		&Items[7], 0, 60, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, 'T', NULL, NULL 
	},
	{
		NULL, 0, 70, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, 'S', NULL, NULL 
	},
	{
		&Items[9], 0, 0, 163, 10, ITEMTEXT,
		0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
	},
	{
		&Items[10], 0, 10, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, 'X', NULL, NULL 
	},
	{
		&Items[11], 0, 20, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, 'C', NULL, NULL 
	},
	{
		&Items[12], 0, 30, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, NULL, &SubItems[0], NULL 
	},
	{
		&Items[13], 0, 40, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[12], NULL, 'K', NULL, NULL 
	},
	{
		&Items[14], 0, 50, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[13], NULL, 'D', NULL, NULL 
	},
	{
		&Items[15], 0, 60, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[14], NULL, 'Z', NULL, NULL 
	},
	{
		&Items[16], 0, 70, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[15], NULL, NULL, NULL, NULL 
	},
	{
		&Items[17], 0, 80, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[16], NULL, NULL, NULL, NULL 
	},
	{
		&Items[18], 0, 90, 163, 10, ITEMTEXT,
		0, (APTR)&ItemText[17], NULL, NULL, NULL, NULL 
	},
	{
		&Items[19], 0, 100, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[18], NULL, NULL, NULL, NULL 
	},
	{
		&Items[20], 0, 110, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[19], NULL, NULL, NULL, NULL 
	},
	{
		&Items[21], 0, 120, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[20], NULL, NULL, NULL, NULL 
	},
	{
		&Items[22], 0, 130, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[21], NULL, NULL, &SubItems[3], NULL 
	},
	{
		&Items[23], 0, 140, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[22], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 150, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[23], NULL, NULL, NULL, NULL 
	},
	{
		&Items[25], 0, 0, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[24], NULL, NULL, NULL, NULL 
	},
	{
		&Items[26], 0, 10, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[25], NULL, NULL, NULL, NULL 
	},
	{
		&Items[27], 0, 20, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[26], NULL, 'I', NULL, NULL 
	},
	{
		&Items[28], 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[27], NULL, 'R', NULL, NULL 
	},
	{
		&Items[29], 0, 40, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[28], NULL, NULL, NULL, NULL 
	},
	{
		&Items[30], 0, 50, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[29], NULL, 'L', NULL, NULL 
	},
	{
		&Items[31], 0, 60, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[30], NULL, 'Y', NULL, NULL 
	},
	{
		&Items[32], 0, 70, 155, 10, ITEMTEXT | ITEMENABLED,
		0, (APTR)&ItemText[31], NULL, NULL, NULL, NULL 
	},
	{
		&Items[33], 0, 80, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[32], NULL, NULL, NULL, NULL 
	},
	{
		&Items[34], 0, 90, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[33], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 100, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[34], NULL, NULL, NULL, NULL 
	},
	{
		&Items[36], 0, 0, 195, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[35], NULL, NULL, NULL, NULL 
	},
	{
		&Items[37], 0, 10, 195, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[36], NULL, 'M', NULL, NULL 
	},
	{
		&Items[38], 0, 20, 195, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[37], NULL, 'E', NULL, NULL 
	},
	{
		&Items[39], 0, 30, 195, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[38], NULL, NULL, NULL, NULL 
	},
	{
		&Items[40], 0, 40, 195, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[39], NULL, NULL, NULL, NULL 
	},
	{
		&Items[41], 0, 50, 195, 10, ITEMTEXT | ITEMENABLED,
		0, (APTR)&ItemText[40], NULL, NULL, NULL, NULL 
	},
	{
		&Items[42], 0, 60, 195, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[41], NULL, NULL, NULL, NULL 
	},
	{
		&Items[43], 0, 70, 195, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[42], NULL, NULL, NULL, NULL 
	},
	{
		&Items[44], 0, 80, 195, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[43], NULL, NULL, NULL, NULL 
	},
	{
		&Items[45], 0, 90, 195, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[44], NULL, NULL, NULL, NULL 
	},
	{
		&Items[46], 0, 100, 195, 10, MENUTOGGLE|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		0, (APTR)&ItemText[45], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 110, 195, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[46], NULL, NULL, &SubItems[6], NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], MENU_OFFSET+3, 0, 70, 10, MENUENABLED, " Editor ", &Items[0] },
	{ &Titles[2], MENU_OFFSET+72, 0, 54, 10, MENUENABLED, " Edit ", &Items[8] },
	{ &Titles[3], MENU_OFFSET+125, 0, 78, 10, MENUENABLED, " Effects ", &Items[24] },
	{ NULL, MENU_OFFSET+202, 0, 78, 10, MENUENABLED, " Options ", &Items[35] },
};

