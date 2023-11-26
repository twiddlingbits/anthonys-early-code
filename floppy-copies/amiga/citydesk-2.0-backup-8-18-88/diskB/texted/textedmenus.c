
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>

textedmenus(p)
struct MenuItem **p;
{


static char stTopaz[] = "topaz.font";

static struct TextAttr taPlain =
{
   stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

static struct IntuiText ItemText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "New", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Open", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Save as..", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Print", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "----------------", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Quit/save/reflow", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Quit and save", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Just quit", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Get Search String", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Search Forward", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Search Back", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Get Replacement", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Replace", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Replace w/Query", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "---------------", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Cut", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Copy", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Paste", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Delete", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Overwrite", NULL },
};

static struct MenuItem Items[] =
{
   {
      &Items[1], 0, 0, 171, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[0], NULL, 'N', NULL, NULL 
   },
   {
      &Items[2], 0, 10, 171, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[1], NULL, 'O', NULL, NULL 
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
      &Items[5], 0, 40, 171, 10, ITEMTEXT | HIGHCOMP,
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
      NULL, 0, 70, 171, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
   },
   {
      &Items[9], 0, 0, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[8], NULL, 'G', NULL, NULL 
   },
   {
      &Items[10], 0, 10, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[9], NULL, 'S', NULL, NULL 
   },
   {
      &Items[11], 0, 20, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[10], NULL, 'B', NULL, NULL 
   },
   {
      &Items[12], 0, 30, 179, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
   },
   {
      &Items[13], 0, 40, 179, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[12], NULL, NULL, NULL, NULL 
   },
   {
      &Items[14], 0, 50, 179, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[13], NULL, NULL, NULL, NULL 
   },
   {
      &Items[15], 0, 60, 179, 10, ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[14], NULL, NULL, NULL, NULL 
   },
   {
      &Items[16], 0, 70, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[15], NULL, 'K', NULL, NULL 
   },
   {
      &Items[17], 0, 80, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[16], NULL, 'C', NULL, NULL 
   },
   {
      &Items[18], 0, 90, 179, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[17], NULL, 'P', NULL, NULL 
   },
   {
      NULL, 0, 100, 179, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[18], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 0, 0, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      0, (APTR)&ItemText[19], NULL, NULL, NULL, NULL 
   },
};

static struct Menu Titles[] =
{
   { &Titles[1], 3, 0, 78, 10, MENUENABLED, " Project", &Items[0] },
   { &Titles[2], 80, 0, 62, 10, MENUENABLED, " Edits", &Items[8] },
   { NULL, 141, 0, 102, 10, MENUENABLED, " Parameters", &Items[19] },
};


*p = &Items[19];

return(&Titles[0]);
}
