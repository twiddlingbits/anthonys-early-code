/** compile with -ad **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"

extern struct pageinfo page[MAXPAGES];
extern struct printerprefs printerprefs;

void print_doc(screen,first_page,last_page)

struct Screen *screen;
long first_page,last_page;

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

static struct Gadget strip_print = {
   NULL,   /* next gadget */
   20,72,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   71,   /* user-definable data */
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

static struct Gadget dither_lighter = {
   &strip_print,   /* next gadget */
   20,102,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   70,   /* user-definable data */
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

static struct Gadget dither_ordered = {
   &dither_lighter,   /* next gadget */
   20,92,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   50,   /* user-definable data */
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

static struct Gadget dither_halftone = {
   &dither_ordered,   /* next gadget */
   20,82,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   (APTR)&Image8,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   50,   /* user-definable data */
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

static struct Gadget use_file = {
   &dither_halftone,   /* next gadget */
   20,62,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image9,   /* gadget border or image to be rendered */
   (APTR)&Image10,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   72,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE copiesSIBuff[4] =
   "1";
static struct StringInfo copiesSInfo = {
   copiesSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   4,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors1[] = {
   0,0,
   28,0,
   28,9,
   0,9,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget copies = {
   &use_file,   /* next gadget */
   15,38,   /* origin XY of hit box relative to window TopLeft */
   25,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&copiesSInfo,   /* SpecialInfo structure */
   74,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   50,0,
   50,14,
   0,14,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   3,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "PRINT",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget print = {
   &copies,   /* next gadget */
   20,118,   /* origin XY of hit box relative to window TopLeft */
   47,13,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
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

static struct Gadget proof = {
   &print,   /* next gadget */
   20,52,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image11,   /* gadget border or image to be rendered */
   (APTR)&Image12,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   73,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE end_pageSIBuff[3];
static struct StringInfo end_pageSInfo = {
   end_pageSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   3,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors3[] = {
   0,0,
   28,0,
   28,9,
   0,9,
   0,0
};
static struct Border Border3 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget end_page = {
   &proof,   /* next gadget */
   15,27,   /* origin XY of hit box relative to window TopLeft */
   25,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&end_pageSInfo,   /* SpecialInfo structure */
   75,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE start_pageSIBuff[3];
static struct StringInfo start_pageSInfo = {
   start_pageSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   3,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors4[] = {
   0,0,
   28,0,
   28,9,
   0,9,
   0,0
};
static struct Border Border4 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget start_page = {
   &end_page,   /* next gadget */
   15,16,   /* origin XY of hit box relative to window TopLeft */
   25,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&start_pageSInfo,   /* SpecialInfo structure */
   76,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors5[] = {
   0,0,
   50,0,
   50,14,
   0,14,
   0,0
};
static struct Border Border5 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   8,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "STOP",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Stop = {
   &start_page,   /* next gadget */
   108,118,   /* origin XY of hit box relative to window TopLeft */
   47,13,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 Stop

static struct IntuiText IText11 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,72,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Strip Printing",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText10 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,102,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Dither Lighter",   /* pointer to text */
   &IText11   /* next IntuiText structure */
};

static struct IntuiText IText9 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,92,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Dither Ordered",   /* pointer to text */
   &IText10   /* next IntuiText structure */
};

static struct IntuiText IText8 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,82,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Dither Halftone",   /* pointer to text */
   &IText9   /* next IntuiText structure */
};

static struct IntuiText IText7 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,62,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Print To File",   /* pointer to text */
   &IText8   /* next IntuiText structure */
};

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   50,38,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Copies",   /* pointer to text */
   &IText7   /* next IntuiText structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   46,52,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Proof Mode",   /* pointer to text */
   &IText6   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   50,27,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Ending Page",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   50,17,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Starting Page",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

#define IntuiTextList1 IText3

static struct NewWindow NewWindowStructure1 = {
   220,38,   /* window XY origin relative to TopLeft of screen */
   175,150,   /* window width and height */
   0,3,   /* detail and block pens */
   GADGETDOWN+GADGETUP+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &Stop,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Print Document",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   175,150,   /* minimum width and height */
   175,150,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

extern SHORT greeking;
extern struct Window *canwin;
struct Window *window;
struct IntuiMessage *message,*GetMsg();
struct Gadget *g;
ULONG class;
USHORT code;
SHORT save_greek;
SHORT ck,ck_val;
int i,j;

sprintf(end_pageSIBuff,"%d",last_page);
sprintf(start_pageSIBuff,"%d",first_page);

dither_halftone.Flags &= (~SELECTED);
dither_ordered.Flags &= (~SELECTED);
strip_print.Flags &= (~SELECTED);

if (printerprefs.flags&DITHER_HALFTONE)
   dither_halftone.Flags |= SELECTED;
else
   dither_ordered.Flags |= SELECTED;

if (printerprefs.flags&STRIP_PRINT)
   strip_print.Flags |= SELECTED;

NewWindowStructure1.Screen = screen;
window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(0);

canwin = window;

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g=message->IAddress;
      ReplyMsg(message);


      if (class == CLOSEWINDOW) {

         if (strip_print.Flags&SELECTED)
            printerprefs.flags |= STRIP_PRINT;
         else
            printerprefs.flags &= (~STRIP_PRINT);

         NewWindowStructure1.FirstGadget=window->FirstGadget->NextGadget->NextGadget;
         CloseWindow(window);
         canwin = 0;
         return(0);
         }

      if (class==GADGETDOWN) {

         printerprefs.flags &= (~DITHER_HALFTONE);
         printerprefs.flags &= (~DITHER_LIGHT);
         if (dither_halftone.Flags&SELECTED)
            printerprefs.flags |= DITHER_HALFTONE;
         if (dither_lighter.Flags&SELECTED)
            printerprefs.flags |= DITHER_LIGHT;

         if (g->GadgetID==50)
            toggle_mutexc(window, g);
         }

      if (class == GADGETUP && g==&print) {
         setdefport();   /** sets ser: or par: flag in printerprefs **/
         stcd_i(end_pageSIBuff,&last_page);
         stcd_i(start_pageSIBuff,&first_page);

         if (!(first_page==last_page &&
              (first_page==DEFAULTEVEN || first_page==DEFAULTODD))) {

            if (first_page < 1 || first_page >= MAXPAGES-2)
               first_page = 1;

            if (last_page >= MAXPAGES-2 || last_page < first_page)
               last_page = MAXPAGES-3;
            }

         if (use_file.Flags&SELECTED)
            printerprefs.flags |= USE_FILE;
         else
            printerprefs.flags &= (~USE_FILE);

         stcd_i(copiesSIBuff,&i);
         printerprefs.copies = i;

         if (proof.Flags&SELECTED)
            printerprefs.flags |= PROOF;
         else
            printerprefs.flags &= (~PROOF);

         if (strip_print.Flags&SELECTED)
            printerprefs.flags |= STRIP_PRINT;
         else
            printerprefs.flags &= (~STRIP_PRINT);

         save_greek = greeking;
         greeking = FALSE;     /** bobby's stuff needs this flag false **/

         i = print_doc_init();   /** Open's files to print to, etc. **/
         showstat(screen,i);

         if (printerprefs.printertype==PREFERENCES)
            ck_val = printerprefs.copies;
         else
            ck_val = 1;

         if (i==AOK) {
            newpointer(screen, BUSY);
            for (ck=0; ck < ck_val; ck++) {
               for (j=first_page; j <= last_page; j++) {
                  if (page[j].flags&PAGEINUSE) {
                     i=print_page(&page[j]);
                     if (i!=AOK) {
                        showstat(screen,i);
                        goto print_error;
                        }
                     }
                  }
               }
print_error:   print_doc_end();
               newpointer(screen, RESTORE);
            }
         greeking = save_greek;
         }
      }
   }
}

