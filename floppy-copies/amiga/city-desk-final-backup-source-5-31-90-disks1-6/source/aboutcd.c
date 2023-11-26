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

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   94,68,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Special Thanks to Evelyn Quan",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   150,50,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Thom Robertson",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   173,41,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Bob Deen",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   148,32,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Anthony J. Wood",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "City Desk was developed at SunRize Industries by:",   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow nw = {
   95,51,   /* window XY origin relative to TopLeft of screen */
   431,83,   /* window width and height */
   0,3,   /* detail and block pens */
   CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   " About City Desk 2.0 ",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

/* end of PowerWindows source generation */

nw.Screen=screen;

w=OpenWindow(&nw);
if (w==NULL) return(0);

PrintIText(w->RPort,&IntuiTextList1,0,0);

Wait(1<<w->UserPort->mp_SigBit);

while (message=GetMsg(w->UserPort))
   ReplyMsg(message);

CloseWindow (w);
}
