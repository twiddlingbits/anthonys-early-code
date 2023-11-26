/*** USE -ad WHEN COMPILING THIS MODULE **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"

extern COORD x_gridsize;
extern COORD y_gridsize;
extern SHORT snaptogrid;
extern SHORT displaygrid;

grid_options(screen)

struct Screen *screen;

{

static UBYTE y_valSIBuff[10];
static struct StringInfo y_valSInfo = {
   y_valSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   10,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors1[] = {
   0,0,
   45,0,
   45,9,
   0,9,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   GRAY1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget y_val = {
   NULL,   /* next gadget */
   130,46,   /* origin XY of hit box relative to window TopLeft */
   42,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   GADGIMMEDIATE+STRINGCENTER,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&y_valSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE x_valSIBuff[10];
static struct StringInfo x_valSInfo = {
   x_valSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   10,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors2[] = {
   0,0,
   45,0,
   45,9,
   0,9,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   GRAY1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget x_val = {
   &y_val,   /* next gadget */
   61,46,   /* origin XY of hit box relative to window TopLeft */
   42,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   GADGIMMEDIATE+STRINGCENTER,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&x_valSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData1[] = {
   0xFFF8,0x3FE0,0x0F80,0x0200
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,4,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget down = {
   &x_val,   /* next gadget */
   20,50,   /* origin XY of hit box relative to window TopLeft */
   13,4,   /* hit box width and height */
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
   0x0200,0x0F80,0x3FE0,0xFFF8
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,4,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget up = {
   &down,   /* next gadget */
   20,45,   /* origin XY of hit box relative to window TopLeft */
   13,4,   /* hit box width and height */
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
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData3,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData4[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData4,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget independant = {
   &up,   /* next gadget */
   20,35,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData5[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData5,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData6[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData6,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget show = {
   &independant,   /* next gadget */
   20,25,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData7[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image7 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData7,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData8[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image8 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData8,   /* pointer to ImageData */
   0x0002,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget snap = {
   &show,   /* next gadget */
   20,15,   /* origin XY of hit box relative to window TopLeft */
   14,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   (APTR)&Image8,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 snap

static struct IntuiText IText6 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   185,46,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "points",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText5 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   113,46,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Y",   /* pointer to text */
   &IText6   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   44,46,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "X",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   44,35,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Independant X/Y Grid",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   44,25,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Show Grid",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   GRAY1,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   44,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Snap To Grid",   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewWindowStructure1 = {
   170,43,   /* window XY origin relative to TopLeft of screen */
   242,65,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETDOWN+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &snap,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Grid Options",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   242,65,   /* minimum width and height */
   242,65,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

#define LISTSIZE 8

static char *val_list[LISTSIZE] = {"1","4","8","16","32","48","72","100"};
char *buf;
struct Window *window;
struct IntuiMessage *message,*GetMsg();
struct Gadget *g;
ULONG class;
USHORT code;
SHORT val_kount;
COORD i;

snap.Flags &= (~SELECTED);
show.Flags &= (~SELECTED);

if (snaptogrid)
   snap.Flags |= SELECTED;

if (displaygrid)
   show.Flags |= SELECTED;

sprintf(x_valSIBuff,"%d",(x_gridsize+4)>>3);
sprintf(y_valSIBuff,"%d",(y_gridsize+4)>>3);

buf = x_valSIBuff;         /*** default independant selection is x ***/
val_kount = get_val_kount(val_list,buf);

NewWindowStructure1.Screen = screen;
pop_to_cursor(screen, &NewWindowStructure1);

window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(0);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g=message->IAddress;
      ReplyMsg(message);

      if (class == GADGETDOWN && g==&x_val) {
         buf = x_valSIBuff;
         val_kount = get_val_kount(val_list,buf);
         }
      else if (class == GADGETDOWN && g==&y_val) {
         buf = y_valSIBuff;
         val_kount = get_val_kount(val_list,buf);
         }

      else if (class == GADGETDOWN && g==&up) {
         val_kount++;
         if (val_kount == LISTSIZE)
            val_kount--;

         if (independant.Flags&SELECTED) {
            strcpy(buf, val_list[val_kount]);
            }
         else {
            strcpy(x_valSIBuff, val_list[val_kount]);
            strcpy(y_valSIBuff, val_list[val_kount]);
            }
         RefreshGList(&x_val, window, 0, 1);
         RefreshGList(&y_val, window, 0, 1);
         }

      else if (class == GADGETDOWN && g==&down) {
         val_kount--;
         if (val_kount < 0)
            val_kount=0;

         if (independant.Flags&SELECTED) {
            strcpy(buf, val_list[val_kount]);
            }
         else {
            strcpy(x_valSIBuff, val_list[val_kount]);
            strcpy(y_valSIBuff, val_list[val_kount]);
            }
         RefreshGList(&x_val, window, 0, 1);
         RefreshGList(&y_val, window, 0, 1);
         }

      else if (class == CLOSEWINDOW) {
         CloseWindow(window);
         i=convert_string(x_valSIBuff);
         if (i>1 && i < 4896)
            x_gridsize = i;
         i=convert_string(y_valSIBuff);
         if (i>1 && i < 4896)
            y_gridsize = i;

         if (snap.Flags & SELECTED)
            snaptogrid = TRUE;
         else
            snaptogrid = FALSE;

         if (show.Flags & SELECTED)
            displaygrid = TRUE;
         else
            displaygrid = FALSE;

         return(AOK);
         }
      }
   }
}

/*** Search list of gridsizes for a close match to current size passed **/

get_val_kount(list,val)

char *list[LISTSIZE];
char *val;

{
short i;
int ival,ilist;

stcd_i(val,&ival);

for (i=0; i<LISTSIZE; i++) {
   stcd_i(list[i],&ilist);
   if (ilist >= ival)
      return(i);
   }
return(LISTSIZE-1);
}

