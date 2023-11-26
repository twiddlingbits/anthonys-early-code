#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

/*************************************/
/*** Let user change ruler options ***/
/*************************************/

lineopts(screen,rule)

struct Screen *screen;
struct rule *rule;
{

static USHORT BorderVectors1[] = {0,0,28,0,28,12,0,12,0,0};
static struct Border Border1 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   4,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget ok = {
   NULL,   /* next gadget */
   98,48,   /* origin XY of hit box relative to window TopLeft */
   25,11,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   12,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors2[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border2 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_0 = {
   &ok,   /* next gadget */
   171,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   0,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors3[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border3 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_1 = {
   &bit_0,   /* next gadget */
   159,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors4[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border4 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_2 = {
   &bit_1,   /* next gadget */
   147,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   2,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors5[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border5 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_3 = {
   &bit_2,   /* next gadget */
   135,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   3,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
   };

static USHORT BorderVectors6[] = {0,0,11,0,11,5,0,5,0,0};
static struct Border Border6 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_5 = {
   &bit_3,   /* next gadget */
   111,33,   /* origin XY of hit box relative to window TopLeft */
   8,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   5,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors7[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border7 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors7,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_4 = {
   &bit_5,   /* next gadget */
   123,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   4,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors8[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border8 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors8,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_6 = {
   &bit_4,   /* next gadget */
   99,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border8,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   6,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors9[] = {0,0,10,0,10,5,0,5,0,0};
static struct Border Border9 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors9,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget bit_7 = {
   &bit_6,   /* next gadget */
   87,33,   /* origin XY of hit box relative to window TopLeft */
   7,4,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border9,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   7,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors10[] = {0,0,48,0,48,10,0,10,0,0};
static struct Border Border10 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors10,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   2,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "THICK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget width_thick = {
   &bit_7,   /* next gadget */
   170,15,   /* origin XY of hit box relative to window TopLeft */
   45,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border10,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   99,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors11[] = {0,0,33,0,33,10,0,10,0,0};
static struct Border Border11 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors11,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   3,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "MED",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget width_med = {
   &width_thick,   /* next gadget */
   131,15,   /* origin XY of hit box relative to window TopLeft */
   30,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   99,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors12[] = {0,0,41,0,41,10,0,10,0,0};
static struct Border Border12 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors12,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   3,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "THIN",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget width_thin = {
   &width_med,   /* next gadget */
   83,15,   /* origin XY of hit box relative to window TopLeft */
   38,9,   /* hit box width and height */
   GADGHCOMP+SELECTED,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border12,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   99,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

/* Gadget list */

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   8,31,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Pattern:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   22,16,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Width:",   /* pointer to text */
   &IText6   /* next IntuiText structure */
};

#define ITextList IText5

static struct NewWindow NewWindowStructure = {
   163,39,   /* window XY origin relative to TopLeft of screen */
   229,64,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP|GADGETDOWN,   /* IDCMP flags */
   WINDOWDRAG|NOCAREREFRESH|ACTIVATE,   /* other window flags */
   &width_thin,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Line Options",   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   229,64,   /* minimum width and height */
   229,64,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};


struct Window *w;
USHORT pat;
ULONG class;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();



/*** set pattern gadgets by rule->pattern **/
 
if (rule->pattern & 1) bit_0.Flags |= SELECTED;
else bit_0.Flags &= (~SELECTED);
if (rule->pattern & 2) bit_1.Flags |= SELECTED;
else bit_1.Flags &= (~SELECTED);
if (rule->pattern & 4) bit_2.Flags |= SELECTED;
else bit_2.Flags &= (~SELECTED);
if (rule->pattern & 8) bit_3.Flags |= SELECTED;
else bit_3.Flags &= (~SELECTED);
if (rule->pattern & 16) bit_4.Flags |= SELECTED;
else bit_4.Flags &= (~SELECTED);
if (rule->pattern & 32) bit_5.Flags |= SELECTED;
else bit_5.Flags &= (~SELECTED);
if (rule->pattern & 64) bit_6.Flags |= SELECTED;
else bit_6.Flags &= (~SELECTED);
if (rule->pattern & 128) bit_7.Flags |= SELECTED;
else bit_7.Flags &= (~SELECTED);

width_thin.Flags &= (~SELECTED);
width_med.Flags &= (~SELECTED);
width_thick.Flags &= (~SELECTED);

if (rule->width <= 8) width_thin.Flags |= SELECTED;
if (rule->width==15) width_med.Flags |= SELECTED;
if (rule->width==31) width_thick.Flags |= SELECTED;

NewWindowStructure.Screen=screen;
w=(struct Window *)OpenWindow(&NewWindowStructure);
if (w==NULL) return(NULL);
PrintIText(w->RPort,&ITextList,0,0);

loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   if (class==GADGETDOWN && address->GadgetID==99)  {
      mutexc(w,address);    /** my own mutual exculd **/
      goto loop;
      }

   if (class==GADGETUP && address==&ok) {
      pat=0;
      if (bit_0.Flags & SELECTED) pat=pat|1;
      if (bit_1.Flags & SELECTED) pat=pat|2;
      if (bit_2.Flags & SELECTED) pat=pat|4;
      if (bit_3.Flags & SELECTED) pat=pat|8;
      if (bit_4.Flags & SELECTED) pat=pat|16;
      if (bit_5.Flags & SELECTED) pat=pat|32;
      if (bit_6.Flags & SELECTED) pat=pat|64;
      if (bit_7.Flags & SELECTED) pat=pat|128;
      rule->pattern=(pat<<8)|pat;

      if (width_thin.Flags & SELECTED) rule->width=8;
      if (width_med.Flags & SELECTED) rule->width=15;
      if (width_thick.Flags & SELECTED) rule->width=31;

      NewWindowStructure.FirstGadget=w->FirstGadget->NextGadget;

      CloseWindow(w);
      return(TRUE);
      }
   }
goto loop;
}
