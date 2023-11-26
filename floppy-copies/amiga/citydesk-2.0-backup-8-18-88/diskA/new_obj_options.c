#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"


objoptions(screen,obj)

struct Screen *screen;
struct object *obj;

{
static SHORT BorderVectors8[] = {
   0,0,
   117,0,
   117,32,
   0,32,
   0,0
};
static struct Border Border8 = {
   118,90,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors8,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static SHORT BorderVectors7[] = {
   0,0,
   117,0,
   117,19,
   0,19,
   0,0
};
static struct Border Border7 = {
   118,132,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors7,   /* pointer to XY vectors */
   &Border8   /* next border in list */
};

static SHORT BorderVectors6[] = {
   0,0,
   117,0,
   117,61,
   0,61,
   0,0
};
static struct Border Border6 = {
   0,90,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors6,   /* pointer to XY vectors */
   &Border7   /* next border in list */
};

static SHORT BorderVectors5[] = {
   0,0,
   235,0,
   235,59,
   0,59,
   0,0
};
static struct Border Border5 = {
   0,21,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   &Border6   /* next border in list */
};

static SHORT BorderVectors4[] = {
   0,0,
   117,0,
   117,10,
   0,10,
   0,0
};
static struct Border Border4 = {
   0,80,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   &Border5   /* next border in list */
};

static SHORT BorderVectors3[] = {
   0,0,
   117,0,
   117,10,
   0,10,
   0,0
};
static struct Border Border3 = {
   118,80,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   &Border4   /* next border in list */
};

static SHORT BorderVectors2[] = {
   0,0,
   235,0,
   235,11,
   0,11,
   0,0
};
static struct Border Border2 = {
   0,10,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   &Border3   /* next border in list */
};

static SHORT BorderVectors1[] = {
   0,0,
   117,0,
   117,10,
   0,10,
   0,0
};
static struct Border Border1 = {
   118,122,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   &Border2   /* next border in list */
};

static struct Gadget Gadget19 = {
   NULL,   /* next gadget */
   0,0,   /* origin XY of hit box relative to window TopLeft */
   1,1,   /* hit box width and height */
   GADGHBOX+GADGHIMAGE,   /* gadget flags */
   NULL,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

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
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "BOX",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget box_obj = {
   &Gadget19,   /* next gadget */
   20,65,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   20,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData3[] = {
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x001F,0xF801,0xFF80,0x1FF0,0x0000,0x0000,0x0000,0x0000,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x0000,0x0000,0x0000,0x0000,0x001F,0xF801,0xFF80,0x1FF0,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData4[] = {
   0xE000,0x0000,0x0000,0x0000,0xF800,0x0000,0x0000,0x0000,
   0xFE1F,0xF801,0xFF80,0x1FF0,0xF800,0x0000,0x0000,0x0000,
   0xE000,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000,
   0xF800,0x0000,0x0000,0x0000,0xFE1F,0xF801,0xFF80,0x1FF0,
   0xF800,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData4,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget dash_line = {
   &box_obj,   /* next gadget */
   135,93,   /* origin XY of hit box relative to window TopLeft */
   60,5,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   7,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData5[] = {
   0x0000,0x0000,0x0000,0x0000,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x0000,0x0000,0x0000,0x0000
};

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData5,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData6[] = {
   0xE000,0x0000,0x0000,0x0000,0xF81F,0xFFFF,0xFFFF,0xFFF0,
   0xFE1F,0xFFFF,0xFFFF,0xFFF0,0xF81F,0xFFFF,0xFFFF,0xFFF0,
   0xE000,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000,
   0xF81F,0xFFFF,0xFFFF,0xFFF0,0xFE1F,0xFFFF,0xFFFF,0xFFF0,
   0xF81F,0xFFFF,0xFFFF,0xFFF0,0xE000,0x0000,0x0000,0x0000
};

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData6,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget thick_line = {
   &dash_line,   /* next gadget */
   135,114,   /* origin XY of hit box relative to window TopLeft */
   60,5,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   7,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData7[] = {
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x0000,0x0000,0x0000,0x0000,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x0000,0x0000,0x0000,0x0000
};

static struct Image Image7 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData7,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData8[] = {
   0xE000,0x0000,0x0000,0x0000,0xF800,0x0000,0x0000,0x0000,
   0xFE1F,0xFFFF,0xFFFF,0xFFF0,0xF81F,0xFFFF,0xFFFF,0xFFF0,
   0xE000,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000,
   0xF800,0x0000,0x0000,0x0000,0xFE1F,0xFFFF,0xFFFF,0xFFF0,
   0xF81F,0xFFFF,0xFFFF,0xFFF0,0xE000,0x0000,0x0000,0x0000
};

static struct Image Image8 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData8,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget med_line = {
   &thick_line,   /* next gadget */
   135,106,   /* origin XY of hit box relative to window TopLeft */
   60,5,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE|TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   (APTR)&Image8,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   7,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData9[] = {
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x001F,0xFFFF,0xFFFF,0xFFF0,0x0000,0x0000,0x0000,0x0000,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
   0x0000,0x0000,0x0000,0x0000,0x001F,0xFFFF,0xFFFF,0xFFF0,
   0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
};

static struct Image Image9 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData9,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData10[] = {
   0xE000,0x0000,0x0000,0x0000,0xF800,0x0000,0x0000,0x0000,
   0xFE1F,0xFFFF,0xFFFF,0xFFF0,0xF800,0x0000,0x0000,0x0000,
   0xE000,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000,
   0xF800,0x0000,0x0000,0x0000,0xFE1F,0xFFFF,0xFFFF,0xFFF0,
   0xF800,0x0000,0x0000,0x0000,0xE000,0x0000,0x0000,0x0000
};

static struct Image Image10 = {
   0,0,   /* XY origin relative to container TopLeft */
   60,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData10,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget thin_line = {
   &med_line,   /* next gadget */
   135,99,   /* origin XY of hit box relative to window TopLeft */
   60,5,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE|TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image9,   /* gadget border or image to be rendered */
   (APTR)&Image10,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   7,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData11[] = {
   0x0080,0x0000,0x03E0,0x0000,0x0FF8,0x0000,0x3FFE,0x0000,
   0xFFFF,0x8000,0x0080,0x0000,0x03E0,0x0000,0x0FF8,0x0000,
   0x3FFE,0x0000,0xFFFF,0x8000
};

static struct Image Image11 = {
   0,0,   /* XY origin relative to container TopLeft */
   17,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData11,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget arrow_up = {
   &thin_line,   /* next gadget */
   126,136,   /* origin XY of hit box relative to window TopLeft */
   17,5,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   8,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData12[] = {
   0xFFFF,0x8000,0x3FFE,0x0000,0x0FF8,0x0000,0x03E0,0x0000,
   0x0080,0x0000,0xFFFF,0x8000,0x3FFE,0x0000,0x0FF8,0x0000,
   0x03E0,0x0000,0x0080,0x0000
};

static struct Image Image12 = {
   0,0,   /* XY origin relative to container TopLeft */
   17,5,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData12,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget arrow_down = {
   &arrow_up,   /* next gadget */
   126,143,   /* origin XY of hit box relative to window TopLeft */
   17,5,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image12,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   9,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData13[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image13 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData13,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData14[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image14 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData14,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "300 DPI",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget hp300 = {
   &arrow_down,   /* next gadget */
   10,130,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image13,   /* gadget border or image to be rendered */
   (APTR)&Image14,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   6,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData15[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image15 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData15,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData16[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image16 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData16,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "150 DPI",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget hp150 = {
   &hp300,   /* next gadget */
   10,120,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image15,   /* gadget border or image to be rendered */
   (APTR)&Image16,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   6,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData17[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image17 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData17,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData18[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image18 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData18,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "100 DPI",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget hp100 = {
   &hp150,   /* next gadget */
   10,110,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image17,   /* gadget border or image to be rendered */
   (APTR)&Image18,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   6,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData19[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image19 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData19,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData20[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image20 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData20,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "75 DPI",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget hp75 = {
   &hp100,   /* next gadget */
   10,100,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image19,   /* gadget border or image to be rendered */
   (APTR)&Image20,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   6,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData21[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image21 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData21,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData22[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image22 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData22,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "SHADOW LEFT",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget sl_mode = {
   &hp75,   /* next gadget */
   100,55,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image21,   /* gadget border or image to be rendered */
   (APTR)&Image22,   /* alternate imagery for selection */
   &IText6,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   5,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData23[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image23 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData23,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData24[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image24 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData24,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText7 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "SHADOW RIGHT",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget sr_mode = {
   &sl_mode,   /* next gadget */
   100,65,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image23,   /* gadget border or image to be rendered */
   (APTR)&Image24,   /* alternate imagery for selection */
   &IText7,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   5,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData25[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image25 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData25,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData26[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image26 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData26,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText8 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "TRANSPARENT",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget trans_mode = {
   &sr_mode,   /* next gadget */
   100,40,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image25,   /* gadget border or image to be rendered */
   (APTR)&Image26,   /* alternate imagery for selection */
   &IText8,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   3,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData27[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image27 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData27,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData28[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image28 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData28,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText9 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "STAMPED",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget stamped_mode = {
   &trans_mode,   /* next gadget */
   100,30,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image27,   /* gadget border or image to be rendered */
   (APTR)&Image28,   /* alternate imagery for selection */
   &IText9,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   2,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData29[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image29 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData29,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData30[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image30 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData30,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText10 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "INV",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget inv_mode = {
   &stamped_mode,   /* next gadget */
   20,50,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image29,   /* gadget border or image to be rendered */
   (APTR)&Image30,   /* alternate imagery for selection */
   &IText10,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData31[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image31 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData31,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData32[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image32 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData32,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText11 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "MIX",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget mix_mode = {
   &inv_mode,   /* next gadget */
   20,40,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image31,   /* gadget border or image to be rendered */
   (APTR)&Image32,   /* alternate imagery for selection */
   &IText11,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData33[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image33 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData33,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData34[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image34 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   2,   /* number of bitplanes in Image */
   ImageData34,   /* pointer to ImageData */
   0x0003,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText12 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "TOP",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget top_mode = {
   &mix_mode,   /* next gadget */
   20,30,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image33,   /* gadget border or image to be rendered */
   (APTR)&Image34,   /* alternate imagery for selection */
   &IText12,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 top_mode

static struct IntuiText IText16 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   128,82,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Line Options",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText15 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,82,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "HP LaserJet",   /* pointer to text */
   &IText16   /* next IntuiText structure */
};

static struct IntuiText IText14 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   126,124,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Percent Black",   /* pointer to text */
   &IText15   /* next IntuiText structure */
};

static struct IntuiText IText13 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   55,12,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Standard Options",   /* pointer to text */
   &IText14   /* next IntuiText structure */
};

#define IntuiTextList1 IText13

static struct NewWindow NewWindowStructure1 = {
   220,39,   /* window XY origin relative to TopLeft of screen */
   237,152,   /* window width and height */
   2,1,   /* detail and block pens */
   GADGETDOWN+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &top_mode,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Object Options",   /* window title */
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
struct rule *rule;
unsigned short percent_index;
static unsigned short percent_array[] = {100,10,20,30,40,50,60,70,80,90,100};

if (obj->flags & OUTLINEOBJ)
   box_obj.Flags |= SELECTED;
else
   box_obj.Flags &= (~SELECTED);

if (obj->type==BLOCK) {
   percent_index = obj->density;
   percent_index /= 10;
   }
else {
   percent_index = 0;
   }

sr_mode.Flags &= (~SELECTED);
sl_mode.Flags &= (~SELECTED);

if (obj->flags & SHADOWRIGHT) {
   sr_mode.Flags |= SELECTED;
   }

if (obj->flags & SHADOWLEFT) {
   sl_mode.Flags |= SELECTED;
   }

top_mode.Flags &= (~SELECTED);
inv_mode.Flags &= (~SELECTED);
mix_mode.Flags |= SELECTED;

if (obj->flags & TOP) {
   mix_mode.Flags &= (~SELECTED);
   top_mode.Flags |= SELECTED;
   }

if (obj->flags & INV) {
   mix_mode.Flags &= (~SELECTED);
   inv_mode.Flags |= SELECTED;
   }

hp75.Flags &= (~SELECTED);
hp100.Flags &= (~SELECTED);
hp150.Flags &= (~SELECTED);
hp300.Flags &= (~SELECTED);

switch (obj->dpi) {
   case 75: hp75.Flags |= SELECTED;
            break;
   case 100: hp100.Flags |= SELECTED;
            break;
   case 150: hp150.Flags |= SELECTED;
            break;
   case 300: hp300.Flags |= SELECTED;
            break;
   }

thin_line.Flags &= (~SELECTED);
med_line.Flags &= (~SELECTED);
thick_line.Flags &= (~SELECTED);

if (obj->type==RULE) {
   rule = (struct rule *)obj->data;

   if (rule->pattern != 0xFFFF) {
      dash_line.Flags |= SELECTED;
      }
   else {
      if (rule->width <= 8) thin_line.Flags |= SELECTED;
      if (rule->width == 16) med_line.Flags |= SELECTED;
      if (rule->width >= 24) thick_line.Flags |= SELECTED;
      }
   }

if (obj->flags & STAMPED) {
   stamped_mode.Flags |= SELECTED;
   }
else {
   stamped_mode.Flags &= (~SELECTED);
   }

if (obj->flags & TRANSPARENT) {
   trans_mode.Flags |= SELECTED;
   }
else {
   trans_mode.Flags &= (~SELECTED);
   }

NewWindowStructure1.Screen=screen;
w=(struct Window *)OpenWindow(&NewWindowStructure1);
if (w==NULL) return(NULL);
PrintIText(w->RPort,&IntuiTextList1,0,0);
show_percent_val(w,percent_array[percent_index]);

#if 0
if (obj->type != RULE) {
   OffGadget(&dash_line,w,0);
   OffGadget(&thin_line,w,0);
   OffGadget(&med_line,w,0);
   OffGadget(&thick_line,w,0);
   }
else {
   OnGadget(&dash_line,w,0);
   OnGadget(&thin_line,w,0);
   OnGadget(&med_line,w,0);
   OnGadget(&thick_line,w,0);
   }
#endif

while (TRUE) {

   Wait(1<<w->UserPort->mp_SigBit);
   while ((message=GetMsg(w->UserPort))!=NULL) {
      class=message->Class;
      address=(struct Gadget *)message->IAddress;
      ReplyMsg(message);


      if (class==GADGETDOWN) {
         if (address==&arrow_down && percent_index > 1) {
            percent_index--;
            show_percent_val(w,percent_array[percent_index]);
            }
         else if (address==&arrow_up && percent_index <10 && percent_index) {
            percent_index++;
            show_percent_val(w,percent_array[percent_index]);
            }
         else {
            toggle_mutexc(w, address);
            }
         }

      if (class==CLOSEWINDOW) {
         obj->flags=0;
         if (sr_mode.Flags & SELECTED) obj->flags |= SHADOWRIGHT;
         if (sl_mode.Flags & SELECTED) obj->flags |= SHADOWLEFT;
         if (top_mode.Flags & SELECTED) obj->flags |= TOP;
         if (inv_mode.Flags & SELECTED) obj->flags |= INV;

         if (hp75.Flags & SELECTED) obj->dpi=75;
         if (hp100.Flags & SELECTED) obj->dpi=100;
         if (hp150.Flags & SELECTED) obj->dpi=150;
         if (hp300.Flags & SELECTED) obj->dpi=300;

         if (stamped_mode.Flags & SELECTED) obj->flags |= STAMPED;
         if (trans_mode.Flags & SELECTED) obj->flags |= TRANSPARENT;

         if (obj->type==RULE) {
            if (thin_line.Flags & SELECTED) rule->width = 8;
            if (med_line.Flags & SELECTED) rule->width = 16;
            if (thick_line.Flags & SELECTED) rule->width = 24;
            rule->pattern = 0xFFFF;
            if (dash_line.Flags & SELECTED) {
               rule->width = 8;
               rule->pattern = 0x3333;
               }
            }

         if (obj->type==BLOCK) {
            obj->density = percent_array[percent_index];
            }

         if (box_obj.Flags & SELECTED) {
            if (thin_line.Flags & SELECTED)
               obj->flags |= (OUTLINEOBJ|LINEWIDTH1);
            else if (med_line.Flags & SELECTED)
               obj->flags |= (OUTLINEOBJ|LINEWIDTH2);
            else
               obj->flags |= (OUTLINEOBJ|LINEWIDTH3);
            }
         else {
            obj->flags &= (~(OUTLINEOBJ|LINEWIDTH1|LINEWIDTH2|LINEWIDTH3));
            }

printf("obj->flags %x\n",obj->flags);
/** Reset pointers & skip system CLOSE and DRAG gadgets **/
         NewWindowStructure1.FirstGadget=w->FirstGadget->NextGadget->NextGadget;
         CloseWindow(w);
         return(TRUE);
         }

      }
   }
}


show_percent_val(w, per)

struct Window *w;
int per;

{
char z[6];
unsigned short len;

sprintf(z,"%d%%",per);
len = strlen(z);
z[len]=' ';
/* z[len+1]=' '; */
Move(w->RPort,160, 144);
Text(w->RPort,z,4);
}

