/** USE -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

gray_convert_options(screen)

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
   "Over Saturate Contrast",
   NULL
};

static struct Gadget over_saturate = {
   NULL,
   20,60,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image1,
   (APTR)&Image2,
   &IText1,
   NULL,
   NULL,
   3,
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
   "Saturate Contrast",
   NULL
};

static struct Gadget saturate = {
   &over_saturate,
   20,50,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image3,
   (APTR)&Image4,
   &IText2,
   NULL,
   NULL,
   3,
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
   "Don't Use Pure White",
   NULL
};

static struct Gadget no_white = {
   &saturate,
   20,70,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image5,
   (APTR)&Image6,
   &IText3,
   NULL,
   NULL,
   5,
   NULL
};

#define GadgetList1 no_white

static struct IntuiText IText6 = {
   3,0,JAM2,
   12,33,
   NULL,
   "close box to convert to gray.",
   NULL
};

static struct IntuiText IText5 = {
   3,0,JAM2,
   12,24,
   NULL,
   "16 gray scales.  Click the",
   &IText6
};

static struct IntuiText IText4 = {
   3,0,JAM2,
   12,15,
   NULL,
   "This picture is color, not",
   &IText5
};

#define IntuiTextList1 IText4

static struct NewWindow NewWindowStructure1 = {
   177,51,
   254,85,
   0,3,
   GADGETDOWN+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,
   &no_white,
   NULL,
   "Color Conversion Options",
   NULL,
   NULL,
   5,5,
   640,200,
   CUSTOMSCREEN
};

struct Window *window;
struct IntuiMessage *message,*GetMsg();
struct Gadget *g;
ULONG class;
USHORT code;
USHORT return_flags;

saturate.Flags &= (~SELECTED);
over_saturate.Flags &= (~SELECTED);
no_white.Flags &= (~SELECTED);

NewWindowStructure1.Screen = screen;

window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(GRAY_OUTOFMEM);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g = message->IAddress;
      ReplyMsg(message);

      if (class == GADGETDOWN) {
         toggle_mutexc(window, g);
         }

      if (class == CLOSEWINDOW) {
/** Reset pointers & skip system CLOSE and DRAG gadgets **/
         NewWindowStructure1.FirstGadget=window->FirstGadget->NextGadget->NextGadget;
         CloseWindow(window);
         return_flags = 0;

         if (saturate.Flags & SELECTED)
            return_flags |= GRAY_SAT;

         if (over_saturate.Flags & SELECTED)
            return_flags |= GRAY_OVERSAT;

         if (no_white.Flags & SELECTED)
            return_flags |= GRAY_NOWHITE;

         return(return_flags);
         }
      }
   }
}

