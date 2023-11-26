#include "exec/types.h"
#include "intuition/intuition.h"
#include "vid.h"

#define IMIN -15
#define IMAX 15

extern struct Screen *menu_screen;

color_control(bri_val,con_val,sat_val,red_val,grn_val,blu_val)

short *bri_val,*con_val,*sat_val,*red_val,*grn_val,*blu_val;

{

static struct PropInfo bluSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget blu = {
   NULL,   /* next gadget */
   167,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&bluSInfo,   /* SpecialInfo structure */
   5,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo grnSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget grn = {
   &blu,   /* next gadget */
   137,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&grnSInfo,   /* SpecialInfo structure */
   4,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo redSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image3 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget red = {
   &grn,   /* next gadget */
   107,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&redSInfo,   /* SpecialInfo structure */
   3,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo satSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image4 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget sat = {
   &red,   /* next gadget */
   77,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&satSInfo,   /* SpecialInfo structure */
   2,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo conSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image5 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget con = {
   &sat,   /* next gadget */
   47,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image5,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&conSInfo,   /* SpecialInfo structure */
   1,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct PropInfo briSInfo = {
   AUTOKNOB+FREEVERT,   /* PROPINFO flags */
   -1,-1,   /* horizontal and vertical pot values */
   -1,2064,   /* horizontal and vertical body values */
};

static struct Image Image6 = {
   0,0,   /* XY origin relative to container TopLeft */
   10,64,   /* Image width and height in pixels */
   0,   /* number of bitplanes in Image */
   NULL,   /* pointer to ImageData */
   0x0000,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static struct Gadget bri = {
   &con,   /* next gadget */
   17,27,   /* origin XY of hit box relative to window TopLeft */
   18,68,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY+FOLLOWMOUSE,   /* activation flags */
   PROPGADGET,   /* gadget type flags */
   (APTR)&Image6,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   NULL,   /* gadget mutual-exclude long word */
   (APTR)&briSInfo,   /* SpecialInfo structure */
   0,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

#define GadgetList1 bri

static struct IntuiText IText6 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   165,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Blu",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText5 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   135,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Grn",   /* pointer to text */
   &IText6   /* next IntuiText structure */
};

static struct IntuiText IText4 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   105,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Red",   /* pointer to text */
   &IText5   /* next IntuiText structure */
};

static struct IntuiText IText3 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   75,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Sat",   /* pointer to text */
   &IText4   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   45,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Con",   /* pointer to text */
   &IText3   /* next IntuiText structure */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens, drawmode and fill byte */
   15,15,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "Bri",   /* pointer to text */
   &IText2   /* next IntuiText structure */
};

#define IntuiTextList1 IText1

static struct NewWindow NewWindowStructure1 = {
   54,23,   /* window XY origin relative to TopLeft of screen */
   207,120,   /* window width and height */
   0,1,   /* detail and block pens */
   MOUSEMOVE+CLOSEWINDOW,   /* IDCMP flags */
   WINDOWCLOSE+SIMPLE_REFRESH+REPORTMOUSE+ACTIVATE+RMBTRAP+NOCAREREFRESH,   /* other window flags */
   &bri,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Color Controls",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *window;
struct IntuiMessage *message,*GetIntuiMessage();
struct Gadget *g;
struct PropInfo *prop;
ULONG class;
USHORT code;
long ival;

briSInfo.VertPot = val_to_pot(*bri_val);
conSInfo.VertPot = val_to_pot(*con_val);
satSInfo.VertPot = val_to_pot(*sat_val);
redSInfo.VertPot = val_to_pot(*red_val);
grnSInfo.VertPot = val_to_pot(*grn_val);
bluSInfo.VertPot = val_to_pot(*blu_val);

pop_up();
NewWindowStructure1.Screen = menu_screen;
window = (struct Window *)OpenWindow(&NewWindowStructure1);
if (window == 0){
   pop_down();
   return(OUTOFMEM);
    }

PrintIText(window->RPort,&IntuiTextList1,0,0);

showval(window,0,*bri_val);       /** Update numbers under prop gadgets **/
showval(window,1,*con_val);
showval(window,2,*sat_val);
showval(window,3,*red_val);
showval(window,4,*grn_val);
showval(window,5,*blu_val);

while (TRUE) {
   message = GetIntuiMessage(window);
   class=message->Class;
   code=message->Code;
   g=message->IAddress;
   ReplyMsg(message);

   while ((message=GetMsg(window->UserPort))!=0) {
      class=message->Class;
      ReplyMsg(message);
      if (class == CLOSEWINDOW) {
         CloseWindow(window);
         pop_down();
         return(AOK);
         }
      }        /** Flush Buffer **/

   if (class == CLOSEWINDOW) {
      CloseWindow(window);
      pop_down();
      return(AOK);
      }

   if (class == MOUSEMOVE) {
      for (g=&GadgetList1; g; g = g->NextGadget) {

         prop = (struct PropInfo *)g->SpecialInfo;
         ival=(prop->VertPot*(IMAX - IMIN) + (1<<15)) >>16;
         ival += IMIN;
         ival = -ival;     /** why is this needed!!?  **/
         showval(window,g->GadgetID,ival);

         switch (g->GadgetID) {
            case 0: *bri_val = ival;
            case 1: *con_val = ival;
            case 2: *sat_val = ival;
            case 3: *red_val = ival;
            case 4: *grn_val = ival;
            case 5: *blu_val = ival;
            }
         }
      }
   }
}

showval(w,pos,val)

struct Window *w;
short pos,val;

{

static char t[20];

static struct IntuiText intuival = {
     3,0,JAM2,     /* front and back text pens and drawmode */
     0,100,     /* XY origin relative to container TopLeft */
     NULL,     /* font pointer or NULL for defaults */
     t,     /* pointer to text */
     NULL     /* next IntuiText structure */
};
intuival.LeftEdge = 10 + 30*pos;
sprintf(t,"%3d",val);
PrintIText(w->RPort,&intuival,0,0);
}
val_to_pot(x)

int x;

{
long ans;

ans = (0xFFFF*(-x-IMIN))/(IMAX - IMIN);

return(ans);
}

