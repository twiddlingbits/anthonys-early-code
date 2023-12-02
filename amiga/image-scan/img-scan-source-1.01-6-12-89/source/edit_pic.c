#include <exec/types.h>
#include <intuition/intuition.h>
#include "scan.h"
#include <math.h>

#define IMIN -32
#define IMAX 32

extern struct Screen *is_screen;
extern struct Window *is_window;
extern struct config config;

static short bri_val = 0;
static short con_val = 0;

void edit_pic()

{
static SHORT BorderVectors1[] = {
   0,0,
   47,0,
   47,13,
   0,13,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   9,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "LOG",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget log = {
   NULL,   /* next gadget */
   110,93,   /* origin XY of hit box relative to window TopLeft */
   44,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   23,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   47,0,
   47,13,
   0,13,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   1,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "B & W",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget bw_only = {
   &log,   /* next gadget */
   110,76,   /* origin XY of hit box relative to window TopLeft */
   44,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   23,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors3[] = {
   0,0,
   21,0,
   21,12,
   0,12,
   0,0
};
static struct Border Border3 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText3 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   1,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget ok = {
   &bw_only,   /* next gadget */
   124,120,   /* origin XY of hit box relative to window TopLeft */
   18,11,   /* hit box width and height */
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

static SHORT BorderVectors4[] = {
   0,0,
   77,0,
   77,13,
   0,13,
   0,0
};
static struct Border Border4 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText4 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   1,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Histogram",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget histogram = {
   &ok,   /* next gadget */
   97,48,   /* origin XY of hit box relative to window TopLeft */
   74,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors5[] = {
   0,0,
   53,0,
   53,13,
   0,13,
   0,0
};
static struct Border Border5 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors5,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText5 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   1,2,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Update",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget update = {
   &histogram,   /* next gadget */
   108,29,   /* origin XY of hit box relative to window TopLeft */
   50,12,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo conSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,0x3ff,   /* horizontal and vertical body values */
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   17,86,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget con = {
   &update,   /* next gadget */
   55,27,   /* origin XY of hit box relative to window TopLeft */
   25,90,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&conSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo briSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,0x3ff,   /* horizontal and vertical body values */
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   17,86,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget bri = {
   &con,   /* next gadget */
   17,27,   /* origin XY of hit box relative to window TopLeft */
   25,90,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&briSInfo,   /* SpecialInfo structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 bri

static struct IntuiText IText7 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   55,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Con",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText6 = {
   15,0,JAM1,   /* front and back text pens, drawmode and fill byte */
   18,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Bri",   /* pointer to text */
   &IText7   /* next IntuiText structure */
};

#define IntuiTextList1 IText6

static struct NewWindow NewWindowStructure1 = {
   70,37,   /* window XY origin relative to TopLeft of screen */
   184,140,   /* window width and height */
   0,15,   /* detail and block pens */
   MOUSEMOVE+CLOSEWINDOW+GADGETUP,   /* IDCMP flags */
   WINDOWDRAG+ACTIVATE+RMBTRAP+NOCAREREFRESH,   /* other window flags REPORTMOUSE */
   &bri,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Edit Scanned Picture",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};


struct Window *window;
struct IntuiMessage *message,*GetIntuiMessage(), *GetMsg();
struct Gadget *g;
struct PropInfo *prop;
ULONG class;
USHORT code;
USHORT val_to_pot();
long ival;
void disp_bright_cont();
void showval();

briSInfo.VertPot = val_to_pot(bri_val);
conSInfo.VertPot = val_to_pot(con_val);

NewWindowStructure1.Screen = is_screen;
NewWindowStructure1.LeftEdge = 0;
NewWindowStructure1.TopEdge = 0;
window = (struct Window *)OpenWindow(&NewWindowStructure1);
if (window == 0){
   return(OUTOFMEM);
   }

PrintIText(window->RPort,&IntuiTextList1,0,0);

showval(window,0,bri_val);       /** Update numbers under prop gadgets **/
showval(window,1,con_val);

while (TRUE) {
   while ((message=GetMsg(window->UserPort))==0) ;
   class=message->Class;
   code=message->Code;
   g=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   if (class == GADGETUP && g==&ok) {
      CloseWindow(window);
      return(AOK);
      }

   if (class == GADGETUP && g==&update) {
      newpointer(is_screen, BUSY);
      disp_bright_cont(bri_val, con_val);
      NewWindowStructure1.LeftEdge = window->LeftEdge;
      NewWindowStructure1.TopEdge = window->TopEdge;
      CloseWindow(window);
      refresh_screen();
      window = (struct Window *)OpenWindow(&NewWindowStructure1);
      if (window == 0){
         return(OUTOFMEM);
         }
      PrintIText(window->RPort,&IntuiTextList1,0,0);
      showval(window,0,bri_val);       /** Update numbers under prop gadgets **/
      showval(window,1,con_val);
      newpointer(is_screen, NORMAL);
      }

   if (class == GADGETUP && g==&histogram) {
      newpointer(is_screen, BUSY);
      do_hist(is_window->RPort,0,20,100,320,config.rawdata,TRUE);
      newpointer(is_screen, NORMAL);
      }

   if (class == GADGETUP && g==&bw_only) {
      newpointer(is_screen, BUSY);
      if (bw_only.Flags&SELECTED)
         config.flags |= IMG_BW;
      else
         config.flags &= (~IMG_BW);
      config.flags &= (~IMG_LOG);
      disp_bright_cont(bri_val, con_val);
      NewWindowStructure1.LeftEdge = window->LeftEdge;
      NewWindowStructure1.TopEdge = window->TopEdge;
      CloseWindow(window);
      log.Flags &= (~SELECTED);
      refresh_screen();
      window = (struct Window *)OpenWindow(&NewWindowStructure1);
      if (window == 0){
         return(OUTOFMEM);
         }
      PrintIText(window->RPort,&IntuiTextList1,0,0);
      showval(window,0,bri_val);       /** Update numbers under prop gadgets **/
      showval(window,1,con_val);
      newpointer(is_screen, NORMAL);
      }

   if (class == GADGETUP && g==&log) {
      newpointer(is_screen, BUSY);
      if (log.Flags&SELECTED)
         config.flags |= IMG_LOG;
      else
         config.flags &= (~IMG_LOG);
      config.flags &= (~IMG_BW);
      disp_bright_cont(bri_val, con_val);
      NewWindowStructure1.LeftEdge = window->LeftEdge;
      NewWindowStructure1.TopEdge = window->TopEdge;
      CloseWindow(window);
      bw_only.Flags &= (~SELECTED);
      refresh_screen();
      window = (struct Window *)OpenWindow(&NewWindowStructure1);
      if (window == 0){
         return(OUTOFMEM);
         }
      PrintIText(window->RPort,&IntuiTextList1,0,0);
      showval(window,0,bri_val);       /** Update numbers under prop gadgets **/
      showval(window,1,con_val);
      newpointer(is_screen, NORMAL);
      }

   if (g==&bri || g==&con) {
      prop = (struct PropInfo *)g->SpecialInfo; 
      ival=(prop->VertPot*(IMAX - IMIN) + (1<<15)) >>16;
      ival += IMIN;
      ival = -ival;     /** why is this needed!!?  **/

      if (g==&bri) {
         bri_val = ival;
         showval(window,0,ival);
         }
      else if (g==&con) {
         con_val = ival;
         showval(window,1,ival);
         }
      }
   }
}

void showval(w,pos,val)

struct Window *w;
short pos,val;

{

static char t[20];

static struct IntuiText intuival = {
     15,0,JAM2,     /* front and back text pens and drawmode */
     0,125,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     t,     /* pointer to text */
     NULL     /* next IntuiText structure */
   };
intuival.LeftEdge = 10 + 30*pos;
sprintf(t,"%3d",val);
PrintIText(w->RPort,&intuival,0,0);
}
USHORT val_to_pot(x)

int x;

{
long ans;

ans = (0xFFFF*(-x-IMIN))/(IMAX - IMIN);

return((USHORT)ans);
}

/************************************************************************/
/* disp_bright_cont(bright, contrast);                                  */
/*                                                                      */
/* Calculate the per-color LUTs using brightness and contrast values.   */
/* Contrast is a fixed-point number with 8 bits integer and 8 bits      */
/* fraction, so $0100 is no stretch, <$0100 reduces contrast, and       */
/* >$0100 increases it.  Brightness is an integer (positive or negative)*/
/* that is added to the value after applying the contrast.              */
/************************************************************************/

VOID disp_bright_cont(bright, contrast)
short bright;
short contrast;
{
long i;
long con;
extern struct config config;
double x,r,c;

if (config.flags&IMG_BW) {

   con = ((-bright)+32)*4;

   for (i=0; i < con; i++)
      config.translate[i]=0;
   for (i=con; i < 256; i++)
      config.translate[i]=15;
   return;
   }

c = 2.708;

con = cc_to_fixed(contrast/2);

for (i=0; i<256; i++) {
   config.translate[i] = L((((i-127L) * con) + (127L<<8) + (bright<<11)) >> 8);
   if (config.flags&IMG_LOG) {
     /* printf("i %d t[i] %d\n",i,config.translate[i]); */
      x = 256.0 - (double)config.translate[i];
      r = log(x)*c;
      /*   printf("i %d t %d log %f\n",i,config.translate[i],r); */
      config.translate[i] = 15 - (unsigned char)r;
      }
   else {
      config.translate[i] = config.translate[i]/16;
      }
   }
}

/******************************************************************/
/*** Convert the color control values to 16bit fixed for bobby's **/
/*** routines.                                                   **/
/******************************************************************/

cc_to_fixed(val)

short val;

{
/* static short ctbl[32] = {
   0x0000,0x0011,0x0022,0x0033,0x0044,0x0055,0x0066,0x0077,
   0x0088,0x0099,0x00aa,0x00bb,0x00cc,0x00dd,0x00ee,0x0100,
   0x0140,0x0180,0x01c0,0x0200,0x0240,0x0280,0x02c0,0x0300,
   0x0380,0x0400,0x0500,0x0600,0x0800,0x1000,0x3000
   };    */

static short ctbl[65] = {
   0x000,0x008,0x010,0x018,0x020,0x028,0x030,0x038,
   0x040,0x048,0x050,0x058,0x060,0x068,0x070,0x078,
   0x080,0x088,0x090,0x098,0x0a0,0x0a8,0x0b0,0x0b8,
   0x0c0,0x0c8,0x0d0,0x0d8,0x0e0,0x0e8,0x0f0,0x0f8,0x0100,
   0x0110,0x0120,0x0130,0x0140,0x0150,0x0160,0x0170,0x0180,
   0x0190,0x01a0,0x01b0,0x01c0,0x01d0,0x01e0,0x01f0,0x0200,
   0x0210,0x0220,0x0230,0x0240,0x0250,0x0260,0x0270,0x0280,
   0x02c0,0x0300,0x0400,0x0500,0x0600,0x0700,0x0800,0x0900};

return(ctbl[val+32]);
}
