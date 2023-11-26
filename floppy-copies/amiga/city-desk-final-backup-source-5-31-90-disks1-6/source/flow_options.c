/** Use -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

flow_options(screen)

struct Screen *screen;

{
static USHORT ImageData1[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image1 = {
   0,0,
   13,7,
   2,
   ImageData1,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData2[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image2 = {
   0,0,
   13,7,
   2,
   ImageData2,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText1 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Irregular Flow",
   NULL
};

static struct Gadget g_irregular = {
   NULL,
   20,65,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image1,
   (APTR)&Image2,
   &IText1,
   NULL,
   NULL,
   6,
   NULL
};

static USHORT ImageData3[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image3 = {
   0,0,
   13,7,
   2,
   ImageData3,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData4[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image4 = {
   0,0,
   13,7,
   2,
   ImageData4,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText2 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Auto Hyphenate",
   NULL
};

static struct Gadget g_autohyphenate = {
   &g_irregular,
   20,55,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image3,
   (APTR)&Image4,
   &IText2,
   NULL,
   NULL,
   5,
   NULL
};

static USHORT ImageData5[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image5 = {
   0,0,
   13,7,
   2,
   ImageData5,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData6[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image6 = {
   0,0,
   13,7,
   2,
   ImageData6,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText3 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Warnings",
   NULL
};

static struct Gadget g_warnings = {
   &g_autohyphenate,
   20,35,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image5,
   (APTR)&Image6,
   &IText3,
   NULL,
   NULL,
   4,
   NULL
};

static USHORT ImageData7[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image7 = {
   0,0,
   13,7,
   2,
   ImageData7,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData8[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image8 = {
   0,0,
   13,7,
   2,
   ImageData8,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText4 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Auto Flow",
   NULL
};

static struct Gadget g_autoflow = {
   &g_warnings,
   20,25,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image7,
   (APTR)&Image8,
   &IText4,
   NULL,
   NULL,
   3,
   NULL
};

static USHORT ImageData9[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image9 = {
   0,0,
   13,7,
   2,
   ImageData9,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData10[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image10 = {
   0,0,
   13,7,
   2,
   ImageData10,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText5 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Snap to Top",
   NULL
};

static struct Gadget g_snaptotop = {
   &g_autoflow,
   20,15,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image9,
   (APTR)&Image10,
   &IText5,
   NULL,
   NULL,
   2,
   NULL
};

static USHORT ImageData11[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image11 = {
   0,0,
   13,7,
   2,
   ImageData11,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData12[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image12 = {
   0,0,
   13,7,
   2,
   ImageData12,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText6 = {
   3,0,JAM2,
   19,0,
   NULL,
   "Use Default",
   NULL
};

static struct Gadget g_usedefault = {
   &g_snaptotop,
   20,45,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image11,
   (APTR)&Image12,
   &IText6,
   NULL,
   NULL,
   5,
   NULL
};

#define GadgetList1 g_usedefault

static struct NewWindow NewWindowStructure1 = {
   229,55,
   165,77,
   0,3,
   GADGETDOWN+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,
   &g_usedefault,
   NULL,
   "Flow Options",
   NULL,
   NULL,
   165,77,
   165,77,
   CUSTOMSCREEN
};

extern SHORT autoflow;
extern SHORT snaptotop;
extern SHORT usedefault;
extern SHORT flow_warnings;
extern SHORT autohyphenate;
extern SHORT irregular;
struct Window *window;
struct IntuiMessage *message,*GetMsg();
ULONG class;
USHORT code;

g_snaptotop.Flags &= (~SELECTED);
g_autoflow.Flags &= (~SELECTED);
g_warnings.Flags &= (~SELECTED);
g_usedefault.Flags &= (~SELECTED);
g_autohyphenate.Flags &= (~SELECTED);
g_irregular.Flags &= (~SELECTED);

if (snaptotop)
   g_snaptotop.Flags |= SELECTED;

if (autoflow)
   g_autoflow.Flags |= SELECTED;

if (usedefault)
   g_usedefault.Flags |= SELECTED;

if (flow_warnings)
   g_warnings.Flags |= SELECTED;

if (autohyphenate)
   g_autohyphenate.Flags |= SELECTED;

if (irregular)
   g_irregular.Flags |= SELECTED;

NewWindowStructure1.Screen = screen;
pop_to_cursor(screen, &NewWindowStructure1);

window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      ReplyMsg(message);

      if (class == CLOSEWINDOW) {
         CloseWindow(window);

         if (g_snaptotop.Flags & SELECTED)
            snaptotop = TRUE;
         else
            snaptotop = FALSE;

         if (g_usedefault.Flags & SELECTED)
            usedefault = TRUE;
         else
            usedefault = FALSE;

         if (g_autoflow.Flags & SELECTED)
            autoflow = TRUE;
         else
            autoflow = FALSE;

         if (g_warnings.Flags & SELECTED)
            flow_warnings = TRUE;
         else
            flow_warnings = FALSE;

         if (g_autohyphenate.Flags & SELECTED)
            autohyphenate = TRUE;
         else
            autohyphenate = FALSE;

         if (g_irregular.Flags & SELECTED)
            irregular = TRUE;
         else
            irregular = FALSE;

         return(AOK);
         }
      }
   }
}

