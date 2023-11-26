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

struct MenuItem *jackandbean2[11];

getmenu2()  {

static char stTopaz[] = "topaz.font";

static struct TextAttr taPlain =
{
   stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

static struct TextAttr taBIU =
{
   stTopaz, 8, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

static struct TextAttr taBU =
{
   stTopaz, 8, FSF_BOLD | FSF_UNDERLINED, FPF_ROMFONT
};

static struct TextAttr taBI =
{
   stTopaz, 8, FSF_BOLD | FSF_ITALIC, FPF_ROMFONT
};

static struct TextAttr taB =
{
   stTopaz, 8, FSF_BOLD, FPF_ROMFONT
};

static struct TextAttr taIU =
{
   stTopaz, 8, FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

static struct TextAttr taI =
{
   stTopaz, 8, FSF_ITALIC, FPF_ROMFONT
};

static struct TextAttr taU =
{
   stTopaz, 8, FSF_UNDERLINED, FPF_ROMFONT
};

static struct IntuiText SubText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "   320 X 200", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   640 X 200", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   320 X 400", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   640 X 400", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Quick", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Slow ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "90 Degrees", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "180 Degrees", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "270 degrees", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Vertically", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Horizont.", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Fine", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Med", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Thick", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Smll Squr", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   Big Squr", NULL },
};

static struct IntuiText ItemText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "Clear Screen", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Start Over", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Screen Size", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Quit With Save", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Just Quit", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Cut", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Paste (normal)", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Paste (resize)", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Erase Area", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Draw", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Line", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Box", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Fill", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Rot", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Flip", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Mag", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Pens", NULL },
};

static struct MenuItem SubItems[] =
{
   {
      &SubItems[1], 143, 0, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      14, (APTR)&SubText[0], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[2], 143, 10, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      13, (APTR)&SubText[1], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[3], 143, 20, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      11, (APTR)&SubText[2], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 143, 30, 112, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      7, (APTR)&SubText[3], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[5], 63, 0, 107, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      2, (APTR)&SubText[4], NULL, 'D', NULL, NULL 
   },
   {
      NULL, 63, 10, 107, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      1, (APTR)&SubText[5], NULL, 'S', NULL, NULL 
   },
   {
      &SubItems[7], 63, 0, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[6], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[8], 63, 10, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[7], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 63, 20, 104, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[8], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[10], 63, 0, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[9], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 63, 10, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[10], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[12], 63, 0, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      30, (APTR)&SubText[11], NULL, '1', NULL, NULL 
   },
   {
      &SubItems[13], 63, 10, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      29, (APTR)&SubText[12], NULL, '2', NULL, NULL 
   },
   {
      &SubItems[14], 63, 20, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      27, (APTR)&SubText[13], NULL, '3', NULL, NULL 
   },
   {
      &SubItems[15], 63, 30, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      23, (APTR)&SubText[14], NULL, '4', NULL, NULL 
   },
   {
      NULL, 63, 40, 139, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | CHECKIT | HIGHCOMP,
      15, (APTR)&SubText[15], NULL, '5', NULL, NULL 
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
      0, (APTR)&ItemText[2], NULL, NULL, &SubItems[0], NULL 
   },
   {
      &Items[4], 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[3], NULL, 'Q', NULL, NULL 
   },
   {
      NULL, 0, 40, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[4], NULL, 'X', NULL, NULL 
   },
   {
      &Items[6], 0, 0, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[5], NULL, 'C', NULL, NULL 
   },
   {
      &Items[7], 0, 10, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[6], NULL, 'P', NULL, NULL 
   },
   {
      &Items[8], 0, 20, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[7], NULL, 'R', NULL, NULL 
   },
   {
      NULL, 0, 30, 155, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[8], NULL, 'E', NULL, NULL 
   },
   {
      &Items[10], 0, 0, 75, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[9], NULL, NULL, &SubItems[4], NULL 
   },
   {
      &Items[11], 0, 10, 75, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[10], NULL, 'L', NULL, NULL 
   },
   {
      &Items[12], 0, 20, 75, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[11], NULL, 'B', NULL, NULL 
   },
   {
      &Items[13], 0, 30, 75, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[12], NULL, 'F', NULL, NULL 
   },
   {
      &Items[14], 0, 40, 75, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[13], NULL, NULL, &SubItems[6], NULL 
   },
   {
      &Items[15], 0, 50, 75, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[14], NULL, NULL, &SubItems[9], NULL 
   },
   {
      &Items[16], 0, 60, 75, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[15], NULL, 'M', NULL, NULL 
   },
   {
      NULL, 0, 70, 75, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[16], NULL, NULL, &SubItems[11], NULL 
   },
};

static struct Menu Titles[] =
{
   { &Titles[1], 3, 0, 62, 10, MENUENABLED, " Proj.", &Items[0] },
   { &Titles[2], 64, 0, 54, 10, MENUENABLED, " Edit", &Items[5] },
   { NULL, 117, 0, 94, 10, MENUENABLED, " Functions", &Items[9] },
};

jackandbean2[0] = &SubItems[0];
jackandbean2[1] = &SubItems[1];
jackandbean2[2] = &SubItems[2];
jackandbean2[3] = &SubItems[3];
jackandbean2[4] = &SubItems[4];
jackandbean2[5] = &SubItems[5];
jackandbean2[6] = &SubItems[11];
jackandbean2[7] = &SubItems[12];
jackandbean2[8] = &SubItems[13];
jackandbean2[9] = &SubItems[14];
jackandbean2[10] = &SubItems[15];
return(&Titles[0]);

}

