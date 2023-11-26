
#include <exec/types.h>
#include <graphics/gfx.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <graphics/text.h>
#include "desk.h"


/*     open a requester, get decision on word replace    */


replacereq(screen,mywin)
struct Screen *screen;
struct Window *mywin;
{

static SHORT BorderVectors1[] = {
   0,0,
   63,0,
   63,12,
   0,12,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   13,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Quit",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget3 = {
   NULL,   /* next gadget */
   157,15,   /* origin XY of hit box relative to window TopLeft */
   60,11,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   63,0,
   63,12,
   0,12,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Don't",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget2 = {
   &Gadget3,   /* next Gadget */
   84,15,   /* origin XY of hit box relative to window TopLeft */
   60,11,   /* hit box width and height */
   NULL,   /* Gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* Gadget type flags */
   (APTR)&Border2,   /* Gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* Gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   63,0,
   63,12,
   0,12,
   0,0
};
static struct Border Border3 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   2,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Replace",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget1 = {
   &Gadget2,   /* next Gadget */
   11,15,   /* origin XY of hit box relative to window TopLeft */
   60,11,   /* hit box width and height */
   NULL,   /* Gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* Gadget type flags */
   (APTR)&Border3,   /* Gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* Gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct NewWindow NewWindow = {
   1,10,   /* window XY origin relative to TopLeft of screen */
   229,32,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   SIMPLE_REFRESH+ACTIVATE+RMBTRAP+WINDOWDRAG,   /* other window flags */
   &Gadget1,   /* first Gadget in Gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

int end;
struct Window *win;
struct IntuiMessage *mess;
LONG mclass;
APTR maddr;

NewWindow.Screen = screen;
NewWindow.BlockPen = 1<<screen->BitMap.Depth -1;
if (mywin->TopEdge > 30)
   NewWindow.TopEdge = mywin->TopEdge-22;
else
   NewWindow.TopEdge = mywin->TopEdge+27;

if ((win = (struct Window *)OpenWindow(&NewWindow))== NULL) {
     return(-1);
     }

ReportMouse(TRUE,win);

RefreshGadgets(win->FirstGadget,win,0L);

again:
mess = NULL;
Wait(1<<win->UserPort->mp_SigBit);
while(mess = ( struct IntuiMessage *) GetMsg(win->UserPort)) {
   mclass = mess->Class;
   maddr = mess->IAddress;
   if (mess != NULL) ReplyMsg(mess);

   if (&Gadget3 == (struct Gadget *) maddr) {
      end = BADIFF;
      goto end1;
      }
   if (&Gadget2 == (struct Gadget *) maddr) {
      end = CANCEL;
      goto end1;
      }
   if (&Gadget1 == (struct Gadget *) maddr) {
      end = AOK;
      goto end1;
      }
   }
goto again;

end1:

CloseWindow(win);
return(end);

}

