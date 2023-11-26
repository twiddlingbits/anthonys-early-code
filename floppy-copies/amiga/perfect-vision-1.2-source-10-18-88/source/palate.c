#include "exec/types.h"
#include "intuition/intuition.h"

alter_palate(screen)

struct Screen *screen;

{
int i;
Structure Window *w;


static SHORT BorderVectors1[] = {
   0,0,
   23,0,
   23,9,
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

static struct Gadget Gadget21 = {
   NULL,   /* next gadget */
   186,67,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget20 = {
   &Gadget21,   /* next gadget */
   155,67,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   23,0,
   23,9,
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

static struct Gadget Gadget19 = {
   &Gadget20,   /* next gadget */
   122,67,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors4[] = {
   0,0,
   23,0,
   23,9,
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

static struct Gadget Gadget18 = {
   &Gadget19,   /* next gadget */
   90,67,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors5[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border5 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget17 = {
   &Gadget18,   /* next gadget */
   186,54,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors6[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border6 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget16 = {
   &Gadget17,   /* next gadget */
   155,54,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors7[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border7 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors7,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget15 = {
   &Gadget16,   /* next gadget */
   121,54,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors8[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border8 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors8,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget14 = {
   &Gadget15,   /* next gadget */
   89,54,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border8,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors9[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border9 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors9,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget13 = {
   &Gadget14,   /* next gadget */
   185,41,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border9,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors10[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border10 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors10,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget12 = {
   &Gadget13,   /* next gadget */
   154,41,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border10,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors11[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border11 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors11,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget11 = {
   &Gadget12,   /* next gadget */
   121,41,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors12[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border12 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors12,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget10 = {
   &Gadget11,   /* next gadget */
   89,41,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border12,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors13[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border13 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors13,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget9 = {
   &Gadget10,   /* next gadget */
   185,28,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border13,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors14[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border14 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors14,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget8 = {
   &Gadget9,   /* next gadget */
   154,28,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border14,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors15[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border15 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors15,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget7 = {
   &Gadget8,   /* next gadget */
   121,28,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border15,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors16[] = {
   0,0,
   23,0,
   23,9,
   0,9,
   0,0
};
static struct Border Border16 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors16,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget6 = {
   &Gadget7,   /* next gadget */
   89,28,   /* origin XY of hit box relative to window TopLeft */
   20,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border16,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors17[] = {
   0,0,
   58,0,
   58,12,
   0,12,
   0,0
};
static struct Border Border17 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors17,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   3,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "CANCEL",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget5 = {
   &Gadget6,   /* next gadget */
   148,88,   /* origin XY of hit box relative to window TopLeft */
   55,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border17,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors18[] = {
   0,0,
   34,0,
   34,12,
   0,12,
   0,0
};
static struct Border Border18 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors18,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   7,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget4 = {
   &Gadget5,   /* next gadget */
   94,88,   /* origin XY of hit box relative to window TopLeft */
   31,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border18,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo Gadget3SInfo = {
   FREEVERT,   /* PROPINFO flags */
   0,17472,   /* horizontal and vertical pot values */
   4095,4095,   /* horizontal and vertical body values */
};

static USHORT ImageData1[] = {
   0x1000,0x3000,0x7000,0xF000,0x7000,0x3000,0x1000
};

static struct Image Image1 = {
   0,16,   /* XY origin relative to container TopLeft */
   4,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget Gadget3 = {
   &Gadget4,   /* next gadget */
   63,26,   /* origin XY of hit box relative to window TopLeft */
   13,74,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget3SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo Gadget2SInfo = {
   FREEVERT,   /* PROPINFO flags */
   0,21840,   /* horizontal and vertical pot values */
   4095,4095,   /* horizontal and vertical body values */
};

static USHORT ImageData2[] = {
   0x1000,0x3000,0x7000,0xF000,0x7000,0x3000,0x1000
};

static struct Image Image2 = {
   0,20,   /* XY origin relative to container TopLeft */
   4,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget Gadget2 = {
   &Gadget3,   /* next gadget */
   42,26,   /* origin XY of hit box relative to window TopLeft */
   13,74,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget2SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo Gadget1SInfo = {
   FREEVERT,   /* PROPINFO flags */
   0,30576,   /* horizontal and vertical pot values */
   4095,4095,   /* horizontal and vertical body values */
};

static USHORT ImageData3[] = {
   0x1000,0x3000,0x7000,0xF000,0x7000,0x3000,0x1000
};

static struct Image Image3 = {
   0,29,   /* XY origin relative to container TopLeft */
   4,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget Gadget1 = {
   &Gadget2,   /* next gadget */
   21,26,   /* origin XY of hit box relative to window TopLeft */
   13,74,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget1SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 Gadget1

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   67,17,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "B",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   44,17,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "G",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   23,17,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "R",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

#define IntuiTextList1 IText3

static struct NewWindow NewWindowStructure1 = {
   144,41,   /* window XY origin relative to TopLeft of screen */
   218,114,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   WINDOWDRAG,   /* other window flags */
   &Gadget1,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Select A color",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   218,114,   /* minimum width and height */
   218,114,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

NewWindowStructure1.Screen = screen;

w = OpenWindow(&NewWindowStructure1);

for (i=0; i< 1000000; i++) ;

CloseWindow(w);
}

