#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

get_new_mag (screen, oldmag)

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
   "5",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget five = {
   NULL,   /* next gadget */
   70,25,   /* origin XY of hit box relative to window TopLeft */
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
   "6",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget six = {
   &five,   /* next gadget */
   70,35,   /* origin XY of hit box relative to window TopLeft */
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
   "4",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget four = {
   &six,   /* next gadget */
   70,15,   /* origin XY of hit box relative to window TopLeft */
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

static USHORT ImageData7[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image7 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData7,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData8[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image8 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData8,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "3",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget three = {
   &four,   /* next gadget */
   15,35,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   (APTR)&Image8,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData9[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image9 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData9,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData10[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image10 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData10,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "2",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget two = {
   &three,   /* next gadget */
   15,25,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image9,   /* gadget border or image to be rendered */
   (APTR)&Image10,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData11[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image11 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData11,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData12[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image12 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData12,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "1",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget one = {
   &two,   /* next gadget */
   15,15,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image11,   /* gadget border or image to be rendered */
   (APTR)&Image12,   /* alternate imagery for selection */
   &IText6,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 one

static struct NewWindow NewWindowStructure1 = {
   258,75,   /* window XY origin relative to TopLeft of screen */
   112,46,   /* window width and height */
   0,3,   /* detail and block pens */
   GADGETDOWN+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &one,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "New Mag",   /* window title */
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
int newmag;

one.Flags &= (~SELECTED);
two.Flags &= (~SELECTED);
three.Flags &= (~SELECTED);
four.Flags &= (~SELECTED);
five.Flags &= (~SELECTED);
six.Flags &= (~SELECTED);

switch(oldmag) {
   case 1:  one.Flags |= SELECTED;
            break;
   case 2:  two.Flags |= SELECTED;
            break;
   case 3:  three.Flags |= SELECTED;
            break;
   case 4:  four.Flags |= SELECTED;
            break;
   case 5:  five.Flags |= SELECTED;
            break;
   case 6:  six.Flags |= SELECTED;
            break;

   }

NewWindowStructure1.Screen=screen;
pop_to_cursor(screen, &NewWindowStructure1);

w=(struct Window *)OpenWindow(&NewWindowStructure1);
if (w==NULL) return(oldmag);

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

         if (one.Flags & SELECTED) newmag = 1;
         if (two.Flags & SELECTED) newmag = 2;
         if (three.Flags & SELECTED) newmag = 3;
         if (four.Flags & SELECTED) newmag = 4;
         if (five.Flags & SELECTED) newmag = 5;
         if (six.Flags & SELECTED)  newmag = 6;

/** Reset pointers & skip system CLOSE and DRAG gadgets **/

         NewWindowStructure1.FirstGadget=w->FirstGadget->NextGadget->NextGadget;
         CloseWindow(w);
         return(newmag);
         }
      }
   }
}


