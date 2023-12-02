#include <exec/types.h>
#include <intuition/intuition.h>
#include "scan.h"

get_screen_size()

{
static SHORT BorderVectors1[] = {
   0,0,
   82,0,
   82,12,
   0,12,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   3,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "640 X 400",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget R640X400 = {
   NULL,   /* next gadget */
   2,46,   /* origin XY of hit box relative to window TopLeft */
   79,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   82,0,
   82,12,
   0,12,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   3,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "640 X 200",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget R640X200 = {
   &R640X400,   /* next gadget */
   2,16,   /* origin XY of hit box relative to window TopLeft */
   79,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   82,0,
   82,12,
   0,12,
   0,0
};
static struct Border Border3 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText3 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   3,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "320 X 400",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget R320X400 = {
   &R640X200,   /* next gadget */
   2,31,   /* origin XY of hit box relative to window TopLeft */
   79,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors4[] = {
   0,0,
   82,0,
   82,12,
   0,12,
   0,0
};
static struct Border Border4 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText4 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   3,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "320 X 200",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget R320X200 = {
   &R320X400,   /* next gadget */
   2,1,   /* origin XY of hit box relative to window TopLeft */
   79,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 R320X200

static struct NewWindow NewWindowStructure1 = {
   253,75,   /* window XY origin relative to TopLeft of screen */
   83,58,   /* window width and height */
   5,15,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   BORDERLESS+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &R320X200,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   83,58,   /* minimum width and height */
   83,58,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *window;
struct IntuiMessage *message,*GetIntuiMessage(), *GetMsg();
struct Gadget *g;
ULONG class;
extern struct Screen *is_screen;
extern struct config config;

NewWindowStructure1.LeftEdge = config.sw/2-45;

R320X200.Flags &=(~SELECTED);
R320X400.Flags &=(~SELECTED);
R640X200.Flags &=(~SELECTED);
R640X400.Flags &=(~SELECTED);

if (config.sw==320 && config.sh==200)
   R320X200.Flags |= SELECTED;
if (config.sw==320 && config.sh==400)
   R320X400.Flags |= SELECTED;
if (config.sw==640 && config.sh==200)
   R640X200.Flags |= SELECTED;
if (config.sw==640 && config.sh==400)
   R640X400.Flags |= SELECTED;

if (config.flags&IMG_NOLACE) {
   R320X200.NextGadget = &R640X200;
   R640X200.NextGadget = NULL;
   NewWindowStructure1.Height = 28;
   }

NewWindowStructure1.Screen = is_screen;
window = (struct Window *)OpenWindow(&NewWindowStructure1);
if (window == 0){
   return(OUTOFMEM);
   }

while (TRUE) {
   while ((message=GetMsg(window->UserPort))!=0) {
      class=message->Class;
      g=(struct Gadget *)message->IAddress;
      ReplyMsg(message);

      if (class == GADGETUP) {
         CloseWindow(window);
         if (g==&R320X200) {
            config.sw=320;
            config.sh=200;
            }
         if (g==&R320X400) {
            config.sw=320;
            config.sh=400;
            }
         if (g==&R640X200) {
            config.sw=640;
            config.sh=200;
            }
         if (g==&R640X400) {
            config.sw=640;
            config.sh=400;
            }
         return(AOK);
         }
      }
   }
}
