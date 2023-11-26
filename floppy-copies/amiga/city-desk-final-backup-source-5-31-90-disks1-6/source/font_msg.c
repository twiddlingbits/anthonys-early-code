#include "exec/types.h"
#include "intuition/intuition.h"

static struct Window *fw;
extern struct Screen *screen;

open_font_msg()

{

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,5,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Downloading Soft Font List to LaserJet...",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewWindowStructure1 = {
   168,77,   /* window XY origin relative to TopLeft of screen */
   353,17,   /* window width and height */
   3,3,   /* detail and block pens */
   NULL,   /* IDCMP flags */
   NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

NewWindowStructure1.Screen = screen;

fw = OpenWindow(&NewWindowStructure1);

if (fw)
   PrintIText(fw->RPort,&IntuiTextList1,0,0);
}

close_font_msg()
{
CloseWindow(fw);
}

