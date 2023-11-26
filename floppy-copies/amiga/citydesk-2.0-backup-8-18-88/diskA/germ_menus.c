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
	{ 0, 1, JAM1, 0, 1, &taPlain, "   640 X 200", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   640 X 400", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Sichtbar", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Unsichtbar", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Preferences", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   HP LaserJet+", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Postscript", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   75 dpi", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   100 dpi", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   150 dpi", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   300 dpi", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   1 CR", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   2 CR", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Amiga", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   ASCII", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Scribble!", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   WordPerfect", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   1 pt", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   6 pt", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   12 pt =  1 pica", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   18 pt = .25 inch", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   36 pt = .50 inch", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Andere", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Ein", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Aus", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Standard", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Selber", NULL },
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "Neu", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Öffnen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Sichern", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Sichern als...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Grafik einlesen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Text einlesen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Objekt Sichern", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Seite drucken", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Dokument drucken", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Von...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Ende", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Zeichnung erstellen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Grafik invertieren", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Seitenformat editieren", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Textattribute editieren", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Textkettenfluß", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Textkette verbinden", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Textkette unterbrechen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Sichern", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Einlesen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Druck-Optionen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Bildschirmauflösung", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Spaltenanzeige sind...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Ausdruck über...", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "LaserJet Auflösung", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Drucker-Info zeigen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Absatzende", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Leerzeichen entfernen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Text Format", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Spaltenrasterung", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Zeige Raster", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Raster-Aufteilung ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Nach vorne ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Nach hinten ", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Oberste Zeile", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Auto-Textfluß", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Warnungen", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "Textfluß-Attribute", NULL },
};

struct MenuItem SubItems[] =
{
	{
		&SubItems[1], 180, 0, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 10, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[3], 180, 0, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 10, 120, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[5], 180, 0, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		6, (APTR)&SubText[4], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[6], 180, 10, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		5, (APTR)&SubText[5], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 20, 136, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		3, (APTR)&SubText[6], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[8], 180, 0, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		14, (APTR)&SubText[7], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[9], 180, 10, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		13, (APTR)&SubText[8], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[10], 180, 20, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		11, (APTR)&SubText[9], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 30, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		7, (APTR)&SubText[10], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[12], 180, 0, 72, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[11], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 10, 72, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[12], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[14], 180, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[13], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[14], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[16], 180, 0, 128, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		14, (APTR)&SubText[15], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[17], 180, 10, 128, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		13, (APTR)&SubText[16], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[18], 180, 20, 128, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		11, (APTR)&SubText[17], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 180, 30, 128, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		7, (APTR)&SubText[18], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[20], 148, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[19], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[20], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[22], 148, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[21], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[22], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[24], 148, 0, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		62, (APTR)&SubText[23], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[25], 148, 10, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		61, (APTR)&SubText[24], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[26], 148, 20, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		59, (APTR)&SubText[25], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[27], 148, 30, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		55, (APTR)&SubText[26], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[28], 148, 40, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		47, (APTR)&SubText[27], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 50, 168, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		31, (APTR)&SubText[28], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[30], 148, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[29], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[30], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[32], 148, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[31], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[32], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[34], 148, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[33], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[34], NULL, NULL, NULL, NULL 
	},
	{
		&SubItems[36], 148, 0, 104, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		2, (APTR)&SubText[35], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 148, 10, 104, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
		1, (APTR)&SubText[36], NULL, NULL, NULL, NULL 
	},
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 171, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[0], NULL, ']', NULL, NULL 
	},
	{
		&Items[2], 0, 10, 171, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[1], NULL, '[', NULL, NULL 
	},
	{
		&Items[3], 0, 20, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
	{
		&Items[4], 0, 30, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
	},
	{
		&Items[5], 0, 40, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
	},
	{
		&Items[6], 0, 50, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
	},
	{
		&Items[7], 0, 60, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
	},
	{
		&Items[8], 0, 70, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
	},
	{
		&Items[9], 0, 80, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
	},
	{
		&Items[10], 0, 90, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[9], NULL, NULL, NULL, NULL 
	},
	{
		&Items[11], 0, 100, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[10], NULL, NULL, NULL, NULL 
	},
	{
		&Items[12], 0, 110, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
	},
	{
		&Items[13], 0, 120, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[12], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 130, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[13], NULL, NULL, NULL, NULL 
	},
	{
		&Items[15], 0, 0, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[14], NULL, NULL, NULL, NULL 
	},
	{
		&Items[16], 0, 10, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[15], NULL, NULL, NULL, NULL 
	},
	{
		&Items[17], 0, 20, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[16], NULL, NULL, NULL, NULL 
	},
	{
		&Items[18], 0, 30, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[17], NULL, NULL, NULL, NULL 
	},
	{
		&Items[19], 0, 40, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[18], NULL, NULL, NULL, NULL 
	},
	{
		&Items[20], 0, 50, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[19], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 60, 200, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[20], NULL, NULL, NULL, NULL 
	},
	{
		&Items[22], 0, 0, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[21], NULL, NULL, NULL, NULL 
	},
	{
		&Items[23], 0, 10, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[22], NULL, NULL, NULL, NULL 
	},
	{
		&Items[24], 0, 20, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[23], NULL, NULL, NULL, NULL 
	},
	{
		&Items[25], 0, 30, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[24], NULL, NULL, &SubItems[0], NULL 
	},
	{
		&Items[26], 0, 40, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[25], NULL, NULL, &SubItems[2], NULL 
	},
	{
		&Items[27], 0, 50, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[26], NULL, NULL, &SubItems[4], NULL 
	},
	{
		&Items[28], 0, 60, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[27], NULL, NULL, &SubItems[7], NULL 
	},
	{
		&Items[29], 0, 70, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[28], NULL, NULL, NULL, NULL 
	},
	{
		&Items[30], 0, 80, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[29], NULL, NULL, &SubItems[11], NULL 
	},
	{
		&Items[31], 0, 90, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[30], NULL, NULL, &SubItems[13], NULL 
	},
	{
		NULL, 0, 100, 192, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[31], NULL, NULL, &SubItems[15], NULL 
	},
	{
		&Items[33], 0, 0, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[32], NULL, NULL, &SubItems[19], NULL 
	},
	{
		&Items[34], 0, 10, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[33], NULL, NULL, &SubItems[21], NULL 
	},
	{
		NULL, 0, 20, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[34], NULL, NULL, &SubItems[23], NULL 
	},
	{
		&Items[36], 0, 0, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[35], NULL, ']', NULL, NULL 
	},
	{
		NULL, 0, 10, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
		0, (APTR)&ItemText[36], NULL, '[', NULL, NULL 
	},
	{
		&Items[38], 0, 0, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[37], NULL, NULL, &SubItems[29], NULL 
	},
	{
		&Items[39], 0, 10, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[38], NULL, NULL, &SubItems[31], NULL 
	},
	{
		&Items[40], 0, 20, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[39], NULL, NULL, &SubItems[33], NULL 
	},
	{
		NULL, 0, 30, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
		0, (APTR)&ItemText[40], NULL, NULL, &SubItems[35], NULL 
	},
};

struct Menu Titles[] =
{
	{ &Titles[1], 3, 0, 70, 10, MENUENABLED, "Projekt", &Items[0] },
	{ &Titles[2], 72, 0, 86, 10, MENUENABLED, "Editieren", &Items[14] },
	{ &Titles[3], 157, 0, 102, 10, MENUENABLED, "Preferences", &Items[21] },
	{ &Titles[4], 258, 0, 62, 10, MENUENABLED, "Raster", &Items[32] },
	{ &Titles[5], 319, 0, 54, 10, MENUENABLED, "Tools", &Items[35] },
	{ NULL, 372, 0, 150, 10, MENUENABLED, "Textfluß-Optionen", &Items[37] },
};

