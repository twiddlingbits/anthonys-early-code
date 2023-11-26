#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

static long lp;     /** last page + 1 **/

open_page(screen,pn,closeall)

struct Screen *screen;
int pn;
short *closeall;

{


static USHORT ImageData1[] = {
   0xFFFF,0xC003,0xC363,0xC003,0xC3D3,0xC003,0xC373,0xC003,
   0xC363,0xC003,0xFFFF,0x0000,0x01C0,0x0020,0x00C0,0x0100,
   0x01E0
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   16,17,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   2,0,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget even_page = {
   NULL,   /* next gadget */
   72,63,   /* origin XY of hit box relative to window TopLeft */
   16,17,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData2[] = {
   0xFFFF,0xC003,0xCBC3,0xC003,0xCB43,0xC003,0xD5C3,0xC003,
   0xDAC3,0xC003,0xFFFF,0x0000,0x0380,0x0580,0x0180,0x0180,
   0x07E0
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   16,17,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   2,0,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget odd_page = {
   &even_page,   /* next gadget */
   49,63,   /* origin XY of hit box relative to window TopLeft */
   16,16,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData3[] = {
   0xFFFE,0x0000,0xC006,0x0000,0xCD46,0x0000,0xC03F,0xFF80,
   0xCBB0,0x0180,0xC033,0x6980,0xCFFF,0xE180,0xCC00,0x6980,
   0xCCDE,0x6180,0xCC00,0x6180,0xFCD6,0x6980,0x0C00,0x6180,
   0x0CBE,0x6180,0x0C00,0x7F80,0x0CB6,0x6000,0x0C00,0x6000,
   0x0FFF,0xE000
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   25,17,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   2,0,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget open_page = {
   &odd_page,   /* next gadget */
   56,40,   /* origin XY of hit box relative to window TopLeft */
   26,17,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData4[] = {
   0x0003,0xFFE0,0x0000,0x003C,0x003E,0x2000,0x01C0,0x0003,
   0x6000,0x0600,0x0001,0xE000,0x0000,0x000F,0xE000,0x0000,
   0x0000,0x0000,0xFFFE,0x003F,0xFF80,0xC006,0x0030,0x0180,
   0xCDE6,0x0033,0x7980,0xC006,0x0030,0x0180,0xCD66,0x0033,
   0x5980,0xC006,0x0030,0x0180,0xCBE6,0x0032,0xF980,0xC006,
   0x0030,0x0180,0xCB66,0x0032,0xD980,0xC006,0x0030,0x0180,
   0xFFFE,0x003F,0xFF80
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   41,17,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData4,   /* pointer to ImageData */
   2,0,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget change_page = {
   &open_page,   /* next gadget */
   48,16,   /* origin XY of hit box relative to window TopLeft */
   43,17,   /* hit box width and height */
   GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE Gadget2SIBuff[4] =
   "1";
static struct StringInfo Gadget2SInfo = {
   Gadget2SIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   3,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors1[] = {
   0,0,
   37,0,
   37,9,
   0,9,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   GRAY2,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget Gadget2 = {
   &change_page,   /* next gadget */
   52,90,   /* origin XY of hit box relative to window TopLeft */
   34,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+GADGIMMEDIATE+STRINGCENTER,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget2SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo Gadget1SInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,0,   /* horizontal and vertical pot values */
   -1,662,   /* horizontal and vertical body values */
};

static struct Image Image5 = {
   0,3,   /* XY origin relative to container TopLeft */
   18,4,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   2,0,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget Gadget1 = {
   &Gadget2,   /* next gadget */
   12,14,   /* origin XY of hit box relative to window TopLeft */
   26,99,   /* hit box width and height */
   0,   /* gadget flags */
   FOLLOWMOUSE|GADGIMMEDIATE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&Gadget1SInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 Gadget1

static struct NewWindow NewWindowStructure1 = {
   324,43,   /* window XY origin relative to TopLeft of screen */
   106,116,   /* window width and height */
   BLACK,GRAY2,   /* detail and block pens */
   MOUSEMOVE+CLOSEWINDOW+GADGETDOWN+GADGETUP,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+SMART_REFRESH+NOCAREREFRESH+ACTIVATE,   /* other window flags */
   &Gadget1,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Open Page",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   -1,-1,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *window;
struct IntuiMessage *message,*GetMsg();
struct Gadget *g;
ULONG class,val;
USHORT code,mouse_move;

lp = last_page();
Gadget1SInfo.VertBody = 0xFFFF/(lp+1);

*closeall = FALSE;
Gadget1SInfo.VertPot = val_to_pot(pn);
sprintf(Gadget2SIBuff,"%d",pn);

NewWindowStructure1.Screen = screen;
pop_to_cursor(screen, &NewWindowStructure1);
window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   mouse_move = FALSE;
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g=message->IAddress;
      ReplyMsg(message);


      if (class == MOUSEMOVE) {
          mouse_move = TRUE;
         }

      else if (class == GADGETDOWN && g==&Gadget1) {
         mouse_move = TRUE;
         }

      else if (class == CLOSEWINDOW) {
         CloseWindow(window);
         return(0);
         }

      else if (class == GADGETDOWN || class==GADGETUP) {

         if (g==&even_page) {
            CloseWindow(window);
            return(DEFAULTEVEN);
            }

         if (g==&odd_page) {
            CloseWindow(window);
            return(DEFAULTODD);
            }

         if (g==&open_page)  {
            stcd_i(Gadget2SIBuff,&val);
            CloseWindow(window);
            return(val);
            }

         if (g==&change_page)  {
            stcd_i(Gadget2SIBuff,&val);
            CloseWindow(window);
            *closeall = TRUE;
            return(val);
            }

         stcd_i(Gadget2SIBuff,&val);
         Gadget1SInfo.VertPot = val_to_pot(val);
         RefreshGList(&Gadget1, window, 0, 1);
         }
      }

   if (mouse_move) {
      val=(Gadget1SInfo.VertPot*lp + (1<<15)) >>16;
      val++;
      sprintf(Gadget2SIBuff,"%d",val);
      RefreshGList(&Gadget2, window, 0, 1);
      }
   }
}

val_to_pot(x)

int x;

{
long ans;

if (x >= lp)
   ans = 0xFFFF;
else
   ans = (0xFFFFL*(x-1))/lp;

return(ans);
}

