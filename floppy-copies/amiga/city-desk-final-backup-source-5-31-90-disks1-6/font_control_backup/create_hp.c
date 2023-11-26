/*** USE -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"

create_hp()

{
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

static struct IntuiText IText1 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Compress",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget compress = {
   NULL,   /* next gadget */
   10,95,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
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

static struct IntuiText IText2 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Italic",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget italic = {
   &compress,   /* next gadget */
   10,85,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   (APTR)&Image4,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
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

static struct IntuiText IText3 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   20,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Bold",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget bold = {
   &italic,   /* next gadget */
   10,75,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   (APTR)&Image6,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors1[] = {
   0,0,
   77,0,
   77,19,
   0,19,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText5 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   5,10,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   ".HP FILE",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   5,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "GENERATE",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct Gadget gen_table = {
   &bold,   /* next gadget */
   248,81,   /* origin XY of hit box relative to window TopLeft */
   74,18,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE cd_sizeSIBuff[80] =
   "10";
static struct StringInfo cd_sizeSInfo = {
   cd_sizeSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors2[] = {
   0,0,
   38,0,
   38,9,
   0,9,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget cd_size = {
   &gen_table,   /* next gadget */
   200,65,   /* origin XY of hit box relative to window TopLeft */
   35,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&cd_sizeSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE cd_nameSIBuff[80] =
   "";
static struct StringInfo cd_nameSInfo = {
   cd_nameSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors3[] = {
   0,0,
   136,0,
   136,9,
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

static struct Gadget cd_name = {
   &cd_size,   /* next gadget */
   200,55,   /* origin XY of hit box relative to window TopLeft */
   133,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&cd_nameSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE fontsizeSIBuff[80] =
   "8";
static struct StringInfo fontsizeSInfo = {
   fontsizeSIBuff,   /* buffer where text will be edited */
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
   38,0,
   38,9,
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

static struct Gadget fontsize = {
   &cd_name,   /* next gadget */
   200,45,   /* origin XY of hit box relative to window TopLeft */
   35,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&fontsizeSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE fontnameSIBuff[80] =
   "topaz.font";
static struct StringInfo fontnameSInfo = {
   fontnameSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors5[] = {
   0,0,
   136,0,
   136,9,
   0,9,
   0,0
};
static struct Border Border5 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget fontname = {
   &fontsize,   /* next gadget */
   200,35,   /* origin XY of hit box relative to window TopLeft */
   133,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&fontnameSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

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

static SHORT BorderVectors6[] = {
   0,0,
   136,0,
   136,9,
   0,9,
   0,0
};
static struct Border Border6 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors6,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget w_path = {
   &fontname,   /* next gadget */
   200,25,   /* origin XY of hit box relative to window TopLeft */
   133,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&w_pathSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static UBYTE r_pathSIBuff[160] =
   "";
static struct StringInfo r_pathSInfo = {
   r_pathSIBuff,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   160,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   0,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static SHORT BorderVectors7[] = {
   0,0,
   136,0,
   136,9,
   0,9,
   0,0
};
static struct Border Border7 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   1,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors7,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget r_path = {
   &w_path,   /* next gadget */
   200,15,   /* origin XY of hit box relative to window TopLeft */
   133,8,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type flags */
   (APTR)&Border7,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&r_pathSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 r_path

static struct IntuiText IText11 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,65,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "City Desk Font Size:",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText10 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,55,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "City Desk Font Name:",   /* pointer to text */
   &IText11   /* next IntuiText structure */
};

static struct IntuiText IText9 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,45,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Screen Font Size:",   /* pointer to text */
   &IText10   /* next IntuiText structure */
};

static struct IntuiText IText8 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,35,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Screen Font Name:",   /* pointer to text */
   &IText9   /* next IntuiText structure */
};

static struct IntuiText IText7 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,25,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   ".hp Output Path:",   /* pointer to text */
   &IText8   /* next IntuiText structure */
};

static struct IntuiText IText6 = {
   1,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   10,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Soft Font Path:",   /* pointer to text */
   &IText7   /* next IntuiText structure */
};

#define IntuiTextList1 IText6

static struct NewWindow NewWindowStructure1 = {
   139,75,   /* window XY origin relative to TopLeft of screen */
   342,107,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   &r_path,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Create .hp File",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};

struct Window *window;
struct Gadget *g;
struct IntuiMessage *message,*GetMsg();
SHORT newsize;
ULONG class;
USHORT code;
int fsize,cdsize,attr;

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

      if (class == GADGETUP && g==&gen_table) {
         sscanf(fontsizeSIBuff, "%d",&fsize);
         sscanf(cd_sizeSIBuff, "%d",&cdsize);
         attr=0;
         if (bold.Flags&SELECTED) attr |= BOLD;
         if (italic.Flags&SELECTED) attr |= ITALIC;
         if (compress.Flags&SELECTED) attr |= COMPRESS;
         busy_ptr(window);
         hp_soft_to_cd(r_pathSIBuff, w_pathSIBuff,
                       fontnameSIBuff, fsize,
                       cd_nameSIBuff, cdsize,
                       0, attr);  /** fontid not used **/
         normal_ptr(window);
         }

      if (class == CLOSEWINDOW) {
         CloseWindow(window);
         return(0);
         }
      }
   }
}

