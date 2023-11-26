#include "exec/types.h"
#include "intuition/intuition.h"
#include "vid.h"

extern short lace;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;
extern struct IntuitionBase *IntuitionBase;
extern struct GfxBase *GfxBase;


/************************/
/*** Create a "menu" ****/
/************************/


short menu_x[15];
short menu_w[15];

create_menu(item)

char *item[];

{
int i,x,len;
static struct Gadget g[15];
struct IntuiMessage *message,*GetMsg();
struct RastPort *mrp;

static struct Gadget gad = {
   NULL,   /* next gadget */
   0,MENUTOP-8,   /* origin XY of hit box relative to window TopLeft */
   25,10,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)0,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

while (menu_window->FirstGadget!=0)
   RemoveGadget(menu_window, menu_window->FirstGadget);

/* menu_window->FirstGadget = &g[0]; */

for (i=0; i<15; i++) {
   g[i] = gad;
   g[i].GadgetID = i+1;
   }

mrp = menu_window->RPort;       /** Menu rastport **/
SetDrMd(mrp, JAM1);
SetAPen(mrp, 0);
SetBPen(menu_window->RPort, 0);
RectFill(mrp,0, 0, 319, MENUHEIGHT-1);

i=0;
x=4;

while(item[i]) {
   g[i].LeftEdge = x-2;
   len = strlen(item[i]);
   g[i].Width = len*8+5;
   g[i].NextGadget = 0;
   AddGadget(menu_window, &g[i], -1);

   SetAPen(mrp,WHITE);
   RectFill(mrp,x-2, 1, x+len*8+2, MENUHEIGHT-2);

   SetAPen(mrp, 1);
   Move(menu_window->RPort, x, MENUTOP);
   Text(menu_window->RPort, item[i], len);

   x = x + len*8+8;
/*   SetAPen(mrp,WHITE);
   Move(menu_window->RPort,x-4,1);
   Draw(menu_window->RPort,x-4,MENUHEIGHT-2); */
   i++;
   }

/* Move(mrp,0,0);
Draw(mrp,0,MENUHEIGHT-1);
Draw(mrp,x-4,MENUHEIGHT-1);
Draw(mrp,x-4,0);
Draw(mrp,0,0); */

/* g[i-1].NextGadget=0; */
RefreshGadgets(menu_window->FirstGadget, menu_window, 0);

/** Flush any old msgs **/

while ((message=GetMsg(menu_window->UserPort))!=0) {
   ReplyMsg(message);
   }        /** Flush Buffer **/
}


/************************************************/
/** Wait for a user to select a menu item     ***/
/************************************************/

wait_menu()
{
short i;

while((i=check_menu())==0)
   Wait( 1 << (menu_window->UserPort->mp_SigBit));
return(i);
}

/************************************************/
/** Check's to see if a menu item was selected **/
/************************************************/

check_menu()
{
struct IntuiMessage *message,*GetMsg();
struct Gadget *g;
ULONG class;
USHORT code;
short x;
int i;
static short tofront=FALSE;


if ((message=GetMsg(menu_window->UserPort))!=0) {
   class=message->Class;
   code=message->Code;
   x=message->MouseX;
   g=message->IAddress;
   ReplyMsg(message);

   while ((message=GetMsg(menu_window->UserPort))!=0) {
      ReplyMsg(message);
      }        /** Flush Buffer **/

   if (class == RAWKEY) {
      if (code == 89) {    /** f10 = toggle menus **/
         if (tofront) {
            ScreenToFront(menu_screen);
            newpointer(menu_screen,NORMAL);
            }
         else {
            ScreenToBack(menu_screen);
            newpointer(menu_screen,ZERO);
            }
         tofront = (!tofront);
         }
      }

      if (class == GADGETUP) {
         return(g->GadgetID);
         }
   }
return(0);
}


