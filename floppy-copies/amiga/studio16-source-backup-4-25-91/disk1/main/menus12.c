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


static struct IntuiText ItemText[] =
{
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "New", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Open", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Save", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Save As...", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "About...", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Quit", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Cut", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Copy", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Paste", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Show Sample List", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Record/Monitor", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Delay", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Equalizer", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Smpte Monitor", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "Mixer", NULL },
   { BLACK, WHITE, JAM1, 0, 1, &taPlain, "VU Meters", NULL },
};

static struct MenuItem Items[] =
{
   {
      &Items[1], 0, 0, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[0], NULL, NULL, NULL, NULL 
   },
   {
      &Items[2], 0, 10, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[1], NULL, NULL, NULL, NULL 
   },
   {
      &Items[3], 0, 20, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[2], NULL, NULL, NULL, NULL 
   },
   {
      &Items[4], 0, 30, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[3], NULL, NULL, NULL, NULL 
   },
   {
      &Items[5], 0, 40, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 0, 50, 96, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
   },
   {
      &Items[7], 0, 0, 56, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
   },
   {
      &Items[8], 0, 10, 56, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[7], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 0, 20, 56, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[8], NULL, NULL, NULL, NULL 
   },
   {
      &Items[10], 0, 0, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[9], NULL, NULL, NULL, NULL 
   },
   {
      &Items[11], 0, 10, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[10], NULL, NULL, NULL, NULL 
   },
   {
      &Items[12], 0, 20, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[11], NULL, NULL, NULL, NULL 
   },
   {
      &Items[13], 0, 30, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[12], NULL, NULL, NULL, NULL 
   },
   {
      &Items[14], 0, 40, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[13], NULL, NULL, NULL, NULL 
   },
   {
      &Items[15], 0, 50, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[14], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 0, 60, 160, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[15], NULL, NULL, NULL, NULL 
   },
};

struct Menu Titles12[] =
{
   { &Titles12[1], 3, 0, 78, 0, MENUENABLED, " Project", &Items[0] },
   { &Titles12[2], 80, 0, 54, 0, MENUENABLED, " Edit", &Items[6] },
   { NULL, 133, 0, 78, 0, MENUENABLED, " Windows", &Items[9] },
};

