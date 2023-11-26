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
	{ 0, 1, JAM1, 0, 1, &taPlain, "IFF", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "DUMP", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "COMP", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "IFF", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "DUMP", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "COMP", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Stereo", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Right", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Left", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Stereo", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Right", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Left", NULL },
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "New", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Open", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Save As...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Delete", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "About", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Flip Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Zero Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Ramp Up Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Ramp Down Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Scale Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Mix Range", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Mark Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "LP Filter Sample  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Discard Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create Stereo  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Break Up Stereo  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Create Instrument  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Freq=Freq*2  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Freq=Freq/2", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Alter Record Speed  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "View Signal Levels", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Monitor Digitizer  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Record a Sample", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Auto Gain Set", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Toggle Output Filter  ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Turn Off Demo Sounds", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Turn On Demo Sounds", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Help", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 111, 0, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[2], 111, 10, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 20, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[4], 111, 0, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[5], 111, 10, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[4], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 111, 20, 48, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[5], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[7], 191, 0, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[6], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[8], 191, 10, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[7], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 191, 20, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[8], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[10], 191, 0, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[9], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[11], 191, 10, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[10], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 191, 20, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&SubText[11], NULL, NULL, NULL, NULL 
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
		&Items[9], 0, 0, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, 'F', NULL, NULL 
	},
	{
		&Items[10], 0, 10, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, 'Z', NULL, NULL 
	},
	{
		&Items[11], 0, 20, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, 'U', NULL, NULL 
	},
	{
		&Items[12], 0, 30, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, 'R', NULL, NULL 
	},
	{
		&Items[13], 0, 40, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[12], NULL, 'A', NULL, NULL 
	},
	{
		&Items[14], 0, 50, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[13], NULL, 'E', NULL, NULL 
	},
	{
		&Items[15], 0, 60, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[14], NULL, 'M', NULL, NULL 
	},
	{
		&Items[16], 0, 70, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[15], NULL, 'L', NULL, NULL 
	},
	{
		&Items[17], 0, 80, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[16], NULL, 'D', NULL, NULL 
	},
	{
		&Items[18], 0, 90, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[17], NULL, 'Y', NULL, NULL 
	},
	{
		NULL, 0, 100, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[18], NULL, 'B', NULL, NULL 
	},
	{
		&Items[20], 0, 0, 195, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[19], NULL, 'N', NULL, NULL 
	},
	{
		&Items[21], 0, 10, 195, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[20], NULL, 'X', NULL, NULL 
	},
	{
		NULL, 0, 20, 195, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[21], NULL, '/', NULL, NULL 
	},
	{
		&Items[23], 0, 0, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[22], NULL, 'S', NULL, NULL 
	},
	{
		&Items[24], 0, 10, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[23], NULL, 'V', NULL, NULL 
	},
	{
		&Items[25], 0, 20, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[24], NULL, NULL, &SubItems[6], NULL 
	},
	{
		&Items[26], 0, 30, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[25], NULL, NULL, &SubItems[9], NULL 
	},
	{
		NULL, 0, 40, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[26], NULL, 'G', NULL, NULL 
	},
	{
		&Items[28], 0, 0, 219, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[27], NULL, 'T', NULL, NULL 
	},
	{
		&Items[29], 0, 10, 219, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[28], NULL, NULL, NULL, NULL 
	},
	{
		&Items[30], 0, 20, 219, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[29], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 30, 219, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[30], NULL, NULL, NULL, NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 86, 10, MENUENABLED, " Project ", &Items[0] },
	{ &Titles[2], 88, 0, 62, 10, MENUENABLED, " Edit ", &Items[8] },
	{ &Titles[3], 149, 0, 110, 10, MENUENABLED, " Instrument ", &Items[19] },
	{ &Titles[4], 258, 0, 94, 10, MENUENABLED, " Digitize ", &Items[22] },
	{ NULL, 351, 0, 62, 10, MENUENABLED, " Other", &Items[27] },
};

