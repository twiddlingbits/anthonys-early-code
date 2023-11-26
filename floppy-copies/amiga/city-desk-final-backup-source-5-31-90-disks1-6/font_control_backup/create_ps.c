/*** USE -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"

struct font_afm_info {
   char *afm_name;      /** adobe file name or null for none exits **/
   char *afm_name_b;    /** adobe file name for bold**/
   char *afm_name_i;    /** adobe file name for italic**/
   char *afm_name_bi;   /** adobe file name for bold-italic **/
   char *people_name;   /** name to use in city desk font requesters **/
   char *font_name;     /** name of amiga bitmap font to use on display **/
   char sizes[80];      /** ascii sizes to generate this font in **/
   };

extern struct font_afm_info f[13];

create_ps()

{
static UBYTE w_pathSIBuff[160] =
   "laserfonts:";
static struct StringInfo w_pathSInfo = {
   w_pathSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   160,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors1[] = {
   0,0,
   187,0,
   187,9,
   0,9,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget w_path = {
   NULL,   /* next gadget */
   115,15,   /* origin XY of hit box relative to window TopLeft */
   184,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&w_pathSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   72,0,
   72,21,
   0,21,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   3,11,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   ".PS FILE",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   2,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "GENERATE",   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

static struct Gadget gen_fonts = {
   &w_path,   /* next gadget */
   232,54,   /* origin XY of hit box relative to window TopLeft */
   69,20,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   97,0,
   97,9,
   0,9,
   0,0
};
static struct Border Border3 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText3 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   3,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Enter Sizes",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget enter_sizes = {
   &gen_fonts,   /* next gadget */
   204,26,   /* origin XY of hit box relative to window TopLeft */
   94,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE point_sizesSIBuff[80] =
   "10,12";
static struct StringInfo point_sizesSInfo = {
   point_sizesSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors4[] = {
   0,0,
   80,0,
   80,9,
   0,9,
   0,0
};
static struct Border Border4 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget point_sizes = {
   &enter_sizes,   /* next gadget */
   116,26,   /* origin XY of hit box relative to window TopLeft */
   77,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&point_sizesSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData1[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData2[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText4 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Gen Plain Sizes?",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget plain = {
   &point_sizes,   /* next gadget */
   10,50,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText5 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Gen Bold-Italic Sizes?",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget bold_italic = {
   &plain,   /* next gadget */
   10,80,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText structure */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText6 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Gen Italic Sizes?",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget italic = {
   &bold_italic,   /* next gadget */
   10,70,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   &IText6,   /* first IntuiText structure */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
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
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText7 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Gen Bold Sizes?",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget bold = {
   &italic,   /* next gadget */
   10,60,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image7,   /* gadget border or image to be rendered */
   (APTR)&Image8,   /* alternate imagery for selection */
   &IText7,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static USHORT ImageData9[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image9 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData9,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData10[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image10 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   1,   /* number of bitplanes in Image */
   ImageData10,   /* pointer to ImageData */
   0x0001,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct IntuiText IText8 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Gen Same Size For Each Font?",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget same_size = {
   &bold,   /* next gadget */
   10,37,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image9,   /* gadget border or image to be rendered */
   (APTR)&Image10,   /* alternate imagery for selection */
   &IText8,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 same_size

static struct IntuiText IText10 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,25,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Font Size:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText9 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Output Path:",   /* pointer to text */
   &IText10   /* next IntuiText structure */
};

#define IntuiTextList1 IText9

static struct NewWindow NewWindowStructure1 = {
   172,75,   /* window XY origin relative to TopLeft of screen */
   311,93,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWDEPTH+WINDOWCLOSE+NOCAREREFRESH,   /* other window flags */
   &same_size,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Create .ps File",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};

struct Window *window;
struct Gadget *g;
struct IntuiMessage *message,*GetMsg();
ULONG class;
USHORT code;
char *names[13];
char *values[13];
int i;

font_data();   /** file 'f' structure with its data **/

window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(0);

PrintIText(window->RPort,&IntuiTextList1,0,0);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g = message->IAddress;
      ReplyMsg(message);

/* printf("class=%x gd %x gu %x\n",class,GADGETDOWN,GADGETUP); */

      if (class == GADGETUP && g==&enter_sizes) {
         for (i=0; i<10; i++) {
            names[i] = f[i].people_name;
            values[i] = f[i].sizes;
            }
         names[i] = "";
         itemlist(names, values, "Enter Font Sizes To Generate");
         }

      if (class == GADGETUP && g==&gen_fonts) {
         busy_ptr(window);
         if (same_size.Flags&SELECTED) {
            gen_ps_font(w_pathSIBuff, point_sizesSIBuff,
                  plain.Flags&SELECTED,
                  bold.Flags&SELECTED,
                  italic.Flags&SELECTED,
                  bold_italic.Flags&SELECTED);
            }
         else {
            gen_ps_font(w_pathSIBuff, "",
                  plain.Flags&SELECTED,
                  bold.Flags&SELECTED,
                  italic.Flags&SELECTED,
                  bold_italic.Flags&SELECTED);
            }
         normal_ptr(window);
         }

      if (class == CLOSEWINDOW) {
         CloseWindow(window);
         return(0);
         }
      }
   }
}

