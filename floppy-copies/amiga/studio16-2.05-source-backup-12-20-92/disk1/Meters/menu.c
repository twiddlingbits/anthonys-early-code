#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"

char stTopaz[] = "topaz.font";

struct TextAttr taPlain =
{
	stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

struct IntuiText ItemText[] =
{
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Analog", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Digital", NULL },
	{ 0, 1, JAM1, 0, 1, &taPlain, "   Graph", NULL },
};

struct MenuItem Items[] =
{
	{
		&Items[1], 0, 0, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
	},
	{
		&Items[2], 0, 10, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
	},
	{
		NULL, 0, 20, 96, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP | MENUTOGGLE,
		0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
	},
};




struct Menu Titles[] =
{
	{ &Titles[1], MENU_OFFSET, 0, 9*8, 0, MENUENABLED, " Meters ", &Items[0] },
	{ NULL, 9*8+1+MENU_OFFSET, 0, 11*8, 0, MENUENABLED, " Channels ", NULL },
};


