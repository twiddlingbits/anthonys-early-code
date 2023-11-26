/** USE -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

/******************************/
/*** get resize preferences ***/
/******************************/

sizemode(screen,sizedir,nice)

struct Screen *screen;
SHORT *sizedir,*nice;

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
   "Size Top to Bottom Only",
   NULL
};

static struct Gadget dir_y = {
   NULL,
   20,35,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image1,
   (APTR)&Image2,
   &IText1,
   NULL,
   NULL,
   2,
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
   "Size Left to Right Only",
   NULL
};

static struct Gadget dir_x = {
   &dir_y,
   20,25,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image3,
   (APTR)&Image4,
   &IText2,
   NULL,
   NULL,
   2,
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
   "Size Any Direction",
   NULL
};

static struct Gadget dir_any = {
   &dir_x,
   20,15,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image5,
   (APTR)&Image6,
   &IText3,
   NULL,
   NULL,
   2,
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
   "Use Aspect Ratio",
   NULL
};

static struct Gadget aspect = {
   &dir_any,
   20,45,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image7,
   (APTR)&Image8,
   &IText4,
   NULL,
   NULL,
   2,
   NULL
};

#define GadgetList1 aspect

static struct NewWindow NewWindowStructure1 = {
   163,59,
   249,62,
   0,1,
   GADGETDOWN+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,
   &aspect,
   NULL,
   "Size Options",
   NULL,
   NULL,
   5,5,
   640,200,
   CUSTOMSCREEN
};

struct Window *w;
ULONG class;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();

dir_any.Flags &= (~SELECTED);
dir_x.Flags &= (~SELECTED);
dir_y.Flags &= (~SELECTED);
aspect.Flags &= (~SELECTED);


if (*nice) {
   aspect.Flags |= SELECTED;
   }
else {
   if (*sizedir==ANYWAY) dir_any.Flags |= SELECTED;
   if (*sizedir==XONLY) dir_x.Flags |= SELECTED;
   if (*sizedir==YONLY) dir_y.Flags |= SELECTED;
   }

NewWindowStructure1.Screen=screen;
pop_to_cursor(screen, &NewWindowStructure1);
w=(struct Window *)OpenWindow(&NewWindowStructure1);
if (w==NULL) return(NULL);

loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);


   if (class == GADGETDOWN) {
      *nice=FALSE;
      toggle_mutexc(w, address);
      if (dir_any.Flags & SELECTED) *sizedir=ANYWAY;
      if (dir_x.Flags & SELECTED) *sizedir=XONLY;
      if (dir_y.Flags & SELECTED) *sizedir=YONLY;
      if (aspect.Flags & SELECTED) *nice=TRUE;
      goto loop;
      }

   if (class==CLOSEWINDOW) {
      NewWindowStructure1.FirstGadget=w->FirstGadget->NextGadget->NextGadget;
      CloseWindow(w);
      return(TRUE);
      }
   }
goto loop;
}
