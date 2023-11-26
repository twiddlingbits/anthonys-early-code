#include "exec/types.h"
#include "intuition/intuition.h"

/******************************/
/*** about city desk window ***/
/******************************/

aboutcd(screen)
struct Screen *screen;
{

struct Window *w,*OpenWindow();
struct IntuiMessage *message,*GetMsg();

static USHORT BorderVectors1[] = {0,0,53,0,53,18,0,18,0,0};
static struct Border Border1 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   18,5,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget ok = {
   NULL,   /* next gadget */
   252,74,   /* origin XY of hit box relative to window TopLeft */
   50,17,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   2,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

/* Gadget list */

static struct IntuiText IText8 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   62,81,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Evelyn Quan",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText7 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   62,71,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "John Bruner",   /* pointer to text */
   &IText8   /* next IntuiText structure */
};

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   62,61,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Thom Robertson",   /* pointer to text */
   &IText7   /* next IntuiText structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   62,51,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Bobby Deen",   /* pointer to text */
   &IText6   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   62,41,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "Anthony J. Wood",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   31,26,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "members of SunRize Industries:",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   31,17,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "City Desk was developed by these",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

#define ITextList IText2

static struct NewWindow nw = {
   153,34,   /* window XY origin relative to TopLeft of screen */
   319,98,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   WINDOWDRAG+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &ok,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "About City Desk 2.0",   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

/* end of PowerWindows source generation */

nw.Screen=screen;

w=OpenWindow(&nw);
if (w==NULL) return(0);

PrintIText(w->RPort,&ITextList,0,0);

Wait(1<<w->UserPort->mp_SigBit);

while (message=GetMsg(w->UserPort))
   ReplyMsg(message);

CloseWindow (w);
}
