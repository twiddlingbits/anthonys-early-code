#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

extern SHORT pointer_mode;

pointer_options(screen)

struct Screen *screen;

{
static USHORT ImageData1[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData2[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Move Linked Objects",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget linked = {
   NULL,   /* next gadget */
   20,35,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData3[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData4[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData4,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Move Stamped Objects",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget stamped = {
   &linked,   /* next gadget */
   20,25,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData5[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData5,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData6[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData6,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Normal",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget normal = {
   &stamped,   /* next gadget */
   20,15,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 normal

static struct NewWindow NewWindowStructure1 = {
   193,66,   /* window XY origin relative to TopLeft of screen */
   217,51,   /* window width and height */
   0,3,   /* detail and block pens */
   GADGETDOWN+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &normal,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Pointer Options",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *w;
ULONG class;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();

normal.Flags &= (~SELECTED);
stamped.Flags &= (~SELECTED);
linked.Flags &= (~SELECTED);

if (pointer_mode==PTR_NORMAL) normal.Flags |= SELECTED;
if (pointer_mode==PTR_STAMPED) stamped.Flags |= SELECTED;
if (pointer_mode==PTR_LINKED) linked.Flags |= SELECTED;

NewWindowStructure1.Screen=screen;
pop_to_cursor(screen, &NewWindowStructure1);
w=(struct Window *)OpenWindow(&NewWindowStructure1);
if (w==NULL) return(NULL);

while (TRUE) {

   Wait(1<<w->UserPort->mp_SigBit);
   while ((message=GetMsg(w->UserPort))!=NULL) {
      class=message->Class;
      address=(struct Gadget *)message->IAddress;
      ReplyMsg(message);


      if (class==GADGETDOWN) {
         toggle_mutexc(w, address);
         }

      if (class==CLOSEWINDOW) {
         if (normal.Flags & SELECTED) pointer_mode = PTR_NORMAL;
         if (stamped.Flags & SELECTED) pointer_mode = PTR_STAMPED;
         if (linked.Flags & SELECTED) pointer_mode = PTR_LINKED;

/** Reset pointers & skip system CLOSE and DRAG gadgets **/

         NewWindowStructure1.FirstGadget=w->FirstGadget->NextGadget->NextGadget;
         CloseWindow(w);
         return(TRUE);
         }
      }
   }
}


