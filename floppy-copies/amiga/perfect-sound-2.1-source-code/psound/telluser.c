#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "psound.h"


/*******************/
/** telluser.c  ****/
/*******************/

telluser(line1,line2)
char *line1,*line2;
{
struct Window *w,*OpenWindow();
struct IntuiMessage *message;
struct Gadget *gad;

static struct IntuiText IText1 = {
   RED,BLACK,JAM2,   /* front and back text pens and drawmode */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   " OK ",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText cancel = {
   RED,BLACK,JAM2,   /* front and back text pens and drawmode */
   0,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   " CANCEL ",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget1 = {
   NULL,   /* next gadget */
   272,36,   /* origin XY of hit box relative to window TopLeft */
   32,8,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   NULL,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static struct Gadget gadcan = {
   &Gadget1,   /* next gadget */
   20,36,   /* origin XY of hit box relative to window TopLeft */
   64,8,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   NULL,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &cancel,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

/* Gadget list */

static struct IntuiText IText3 = {
   WHITE,0,JAM1,   /* front and back text pens and drawmode */
   31,24,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   NULL,   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   WHITE,0,JAM1,   /* front and back text pens and drawmode */
   29,12,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   NULL,   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

#define ITextList IText2

static struct NewWindow nw = {
   129,51,   /* window XY origin relative to TopLeft of screen */
   330,53,   /* window width and height */
   0,0,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   BORDERLESS+ACTIVATE,   /* other window flags */
   &gadcan,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};

/* end of PowerWindows source generation */

IText2.IText=line1;
IText3.IText=line2;

w=OpenWindow(&nw);
if (w==NULL) {
   printf("Not Enough Memory To Open Window!\n");
   return(FALSE);
   }

SetDrMd(w->RPort,JAM1);
SetAPen(w->RPort,BLUE);
SetBPen(w->RPort,BLUE);
RectFill(w->RPort,0,0,
         w->Width,
         w->Height);

RefreshGadgets(w->FirstGadget,w,NULL);   /* fill erase "OK" box */
PrintIText(w->RPort,&ITextList,0,0);      /* draw telluser text */

while ((message=GetMsg(w->UserPort))==NULL)
   Wait(1<<w->UserPort->mp_SigBit);
gad=(struct Gadget *)message->IAddress;
ReplyMsg(message);

CloseWindow(w);
if (gad==&gadcan) return(FALSE);
return(TRUE);
}

