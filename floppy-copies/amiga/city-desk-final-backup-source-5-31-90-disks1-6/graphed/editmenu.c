
#include <exec/types.h>
#include <intuition/intuition.h>

struct MenuItem *jackandbean1[6];

getmenu1 () {

static char stTopaz[] = "topaz.font";

static struct TextAttr taPlain =
{
   stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

static struct IntuiText SubText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "   Raw", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Adjusted", NULL },
};

static struct IntuiText ItemText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "Clear Screen", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Start Over", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Change Screen", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Load Brush", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Save Brush", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Return w/Brush", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Just Return", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "UNDO", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Circle", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Erase Area", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Rotate Mode", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Resize", NULL },
};

static struct MenuItem SubItems[] =
{
   {
      &SubItems[1], 121, 0, 104, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      2, (APTR)&SubText[0], NULL, NULL, NULL, NULL
   },
   {
      NULL, 121, 10, 104, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      1, (APTR)&SubText[1], NULL, NULL, NULL, NULL
   },
};

static struct MenuItem Items[] =
{
   {
      &Items[1], 0, 0, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[0], NULL, 'W', NULL, NULL 
   },
   {
      &Items[2], 0, 10, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[1], NULL, 'Z', NULL, NULL 
   },
   {
      &Items[3], 0, 20, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL
   },
   {
      &Items[4], 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
   },
   {
      &Items[5], 0, 40, 155, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
   },
   {
      &Items[6], 0, 50, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[5], NULL, 'Q', NULL, NULL 
   },
   {
      NULL, 0, 60, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[6], NULL, 'X', NULL, NULL 
   },
   {
      &Items[8], 0, 0, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[7], NULL, 'U', NULL, NULL 
   },
   {
      &Items[9], 0, 10, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
   },
   {
      &Items[10], 0, 20, 163, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[9], NULL, 'E', NULL, NULL 
   },
   {
      &Items[11], 0, 30, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[10], NULL, NULL, &SubItems[0], NULL
   },
   {
      NULL, 0, 40, 163, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
   },
};

static struct Menu Titles[] =
{
   { &Titles[1], 3, 0, 70, 10, MENUENABLED, "Project", &Items[0] },
   { NULL, 72, 0, 54, 10, MENUENABLED, "Edits", &Items[7] },
};

jackandbean1[4] = &SubItems[0];
jackandbean1[5] = &SubItems[1];

return(&Titles[0]);

}


