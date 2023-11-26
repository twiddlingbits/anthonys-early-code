#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

extern struct Window *coord_window;

void open_coord(screen,up)

struct Screen *screen;
struct MsgPort *up;

{

static struct IntuiText IText3 = {
   BLACK,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   15,35,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Object H W:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   BLACK,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   15,25,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Object X Y:",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   BLACK,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   23,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Mouse X Y:",   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewWindowStructure1 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   207,50,   /* window width and height */
   0,1,   /* detail and block pens */
   0,   /* IDCMP flags */
   WINDOWDRAG+NOCAREREFRESH+WINDOWCLOSE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Coordinates:",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN
};

setmenus(screen, FALSE);
NewWindowStructure1.Screen = screen;

coord_window = OpenWindow(&NewWindowStructure1);
if (coord_window==0) {
   return;
   }

PrintIText(coord_window->RPort,&IntuiTextList1,0,0);
coord_window->UserPort=up;
ModifyIDCMP(coord_window,CLOSEWINDOW);
setmenus(screen, TRUE);
}

void close_coord()
{
closetoolbox(coord_window);
coord_window = 0;
}

