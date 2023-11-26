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
#include "/include/psound.h"

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
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "AIFF", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "IFF-8SVX", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "16 Bit Dump", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "AIFF", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "IFF-8SVX", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "16 Bit Dump", NULL },
};

struct IntuiText ItemText[] =
{
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "New", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Open", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Save", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Save As...", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Delete", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "About", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Quit", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Cut", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Copy", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Paste", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Erase", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Insert", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Create New Sample", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Append Sample to Sample", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Set Playback Speed", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Flip Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Zero Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Ramp Up Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Ramp Down Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Scale Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Mix Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Zoom Range", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Mark Sample", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "LP Filter Sample  ", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Discard Sample", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Create Stereo  ", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Break Up Stereo  ", NULL },
	{ BLACK, WHITE, JAM1, 0, 1, &taPlain, "Resample", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 111, 0, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[2], 111, 10, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 20, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[4], 111, 0, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[5], 111, 10, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[4], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 20, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[5], NULL, NULL, NULL, NULL 
	},
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 123, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, 'O', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, &SubItems[0], NULL 
	},
	{
		&Items[4], 0, 30, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, &SubItems[3], NULL 
	},
	{
		&Items[5], 0, 40, 123, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, 'K', NULL, NULL 
	},
	{
		&Items[6], 0, 50, 123, 10, ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		&Items[7], 0, 60, 123, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, '?', NULL, NULL 
	},
	{
		NULL, 0, 70, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
	},
	{
		&Items[9], 0, 0, 227, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, 'X', NULL, NULL 
	},
	{
		&Items[10], 0, 10, 227, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, 'C', NULL, NULL 
	},
	{
		&Items[11], 0, 20, 227, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, 'V', NULL, NULL 
	},
	{
		&Items[12], 0, 30, 227, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
	},
	{
		&Items[13], 0, 40, 227, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[12], NULL, NULL, NULL, NULL 
	},
	{
		&Items[14], 0, 50, 227, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[13], NULL, NULL, NULL, NULL 
	},
	{
		&Items[15], 0, 60, 227, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[14], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 70, 227, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[15], NULL, NULL, NULL, NULL 
	},
	{
		&Items[17], 0, 0, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[16], NULL, 'F', NULL, NULL 
	},
	{
		&Items[18], 0, 10, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[17], NULL, 'Z', NULL, NULL 
	},
	{
		&Items[19], 0, 20, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[18], NULL, 'U', NULL, NULL 
	},
	{
		&Items[20], 0, 30, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[19], NULL, 'R', NULL, NULL 
	},
	{
		&Items[21], 0, 40, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[20], NULL, 'A', NULL, NULL 
	},
	{
		&Items[22], 0, 50, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[21], NULL, 'E', NULL, NULL 
	},
	{
		&Items[23], 0, 60, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[22], NULL, 'Z', NULL, NULL 
	},
	{
		&Items[24], 0, 70, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[23], NULL, 'M', NULL, NULL 
	},
	{
		&Items[25], 0, 80, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[24], NULL, 'L', NULL, NULL 
	},
	{
		&Items[26], 0, 90, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[25], NULL, 'D', NULL, NULL 
	},
	{
		&Items[27], 0, 100, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[26], NULL, 'Y', NULL, NULL 
	},
	{
		&Items[28], 0, 110, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[27], NULL, 'B', NULL, NULL 
	},
	{
		NULL, 0, 120, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[28], NULL, 'H', NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 86, 10, MENUENABLED, " Project ", &Items[0] },
	{ &Titles[2], 88, 0, 62, 10, MENUENABLED, " Edit ", &Items[8] },
	{ NULL, 149, 0, 70, 10, MENUENABLED, " Tools ", &Items[16] },
};

