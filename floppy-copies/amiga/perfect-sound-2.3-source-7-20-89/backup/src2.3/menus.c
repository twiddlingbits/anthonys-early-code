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
   { 0, 1, JAM1, 0, 1, &taPlain, "   ON", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   OFF", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   ON", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "   OFF", NULL },
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
   { 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "About", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Delete Marked Range  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Discard This Sample  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Insert Marked Range  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Change Playback Speed  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Copy Range to New Slot  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Append Slot to Slot  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Create Stereo  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Break Up Stereo  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Flip This Sample  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Graph Marked Range  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Create Instrument ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Freq=Freq*2  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Freq=Freq/2", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Zero Marked Range", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Auto Graph", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Output Filter", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Alter Record Speed  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "View Signal Levels", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Monitor Digitizer  ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Record a Sample", NULL },
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
      &SubItems[7], 191, 0, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      2, (APTR)&SubText[6], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 191, 10, 64, 10, CHECKED|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      1, (APTR)&SubText[7], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[9], 191, 0, 64, 10, CHECKED|ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      2, (APTR)&SubText[8], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 191, 10, 64, 10, ITEMENABLED | ITEMTEXT | CHECKIT | HIGHCOMP,
      1, (APTR)&SubText[9], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[11], 191, 0, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[10], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[12], 191, 10, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[11], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 191, 20, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[12], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[14], 191, 0, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[13], NULL, NULL, NULL, NULL 
   },
   {
      &SubItems[15], 191, 10, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[14], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 191, 20, 64, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&SubText[15], NULL, NULL, NULL, NULL 
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
      &Items[5], 0, 40, 123, 10, ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[4], NULL, NULL, NULL, NULL 
   },
   {
      &Items[6], 0, 50, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[5], NULL, NULL, NULL, NULL 
   },
   {
      NULL, 0, 60, 123, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[6], NULL, NULL, NULL, NULL 
   },
   {
      &Items[8], 0, 0, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[7], NULL, 'R', NULL, NULL 
   },
   {
      &Items[9], 0, 10, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[8], NULL, 'D', NULL, NULL 
   },
   {
      &Items[10], 0, 20, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[9], NULL, 'I', NULL, NULL 
   },
   {
      &Items[11], 0, 30, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[10], NULL, 'P', NULL, NULL 
   },
   {
      &Items[12], 0, 40, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[11], NULL, 'C', NULL, NULL 
   },
   {
      &Items[13], 0, 50, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[12], NULL, 'A', NULL, NULL 
   },
   {
      &Items[14], 0, 60, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[13], NULL, 'Y', NULL, NULL 
   },
   {
      NULL, 0, 70, 235, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[14], NULL, 'B', NULL, NULL 
   },
   {
      &Items[16], 0, 0, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[15], NULL, 'F', NULL, NULL 
   },
   {
      &Items[17], 0, 10, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[16], NULL, 'G', NULL, NULL 
   },
   {
      &Items[18], 0, 20, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[17], NULL, 'N', NULL, NULL 
   },
   {
      &Items[19], 0, 30, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[18], NULL, 'X', NULL, NULL 
   },
   {
      &Items[20], 0, 40, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[19], NULL, '/', NULL, NULL 
   },
   {
      &Items[21], 0, 50, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[20], NULL, 'Z', NULL, NULL 
   },
   {
      &Items[22], 0, 60, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[21], NULL, NULL, &SubItems[6], NULL 
   },
   {
      NULL, 0, 70, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[22], NULL, NULL, &SubItems[8], NULL 
   },
   {
      &Items[24], 0, 0, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[23], NULL, 'S', NULL, NULL 
   },
   {
      &Items[25], 0, 10, 203, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[24], NULL, 'V', NULL, NULL 
   },
   {
      &Items[26], 0, 20, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[25], NULL, NULL, &SubItems[10], NULL 
   },
   {
      NULL, 0, 30, 203, 10, ITEMENABLED | ITEMTEXT | HIGHCOMP,
      0, (APTR)&ItemText[26], NULL, NULL, &SubItems[13], NULL 
   },
};

struct Menu Titles[] =
{
   { &Titles[1], 3, 0, 94, 10, MENUENABLED, " Project  ", &Items[0] },
   { &Titles[2], 96, 0, 62, 10, MENUENABLED, " Edit ", &Items[7] },
   { &Titles[3], 157, 0, 86, 10, MENUENABLED, " Special ", &Items[15] },
   { NULL, 242, 0, 94, 10, MENUENABLED, " Digitize ", &Items[23] },
};

