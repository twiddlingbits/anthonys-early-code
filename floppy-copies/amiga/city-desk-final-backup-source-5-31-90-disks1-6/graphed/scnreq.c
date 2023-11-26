
#include <exec/types.h>
#include <graphics/gfx.h>
#include <stdio.h>
#include <ctype.h>
#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <graphics/text.h>
#include <exec/memory.h>


/*     open a requester, get changes to screen size and depth  */


scnreq(screen,mode,num)
int *mode,*num;
struct Screen *screen;
{

static USHORT ImageData1[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image1 = {
   0,0,   /* XY origin relative to container TopLeft */
   13,7,   /* Image width and height in pixels */
   4,   /* *number of bitplanes in Image */
   ImageData1,   /* pointer to ImageData */
   0x000f,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image structure */
};

static USHORT ImageData2[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
   ,0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image2 = {
   0,0,   /* XY origin relative to container TopLeft */
   14,7,   /* Image width and height in pixels */
   4,   /* *number of bitplanes in Image */
   ImageData2,   /* pointer to ImageData */
   0x000f,0x0000,   /* PlanePick and PlaneOnOff */
   NULL   /* next Image static structure */
};

static struct IntuiText IText1 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "16 shades",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget shades4 = {
   NULL,   /* next gadget */
   125,47,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   222,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText2 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "8 shades",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget shades3 = {
   &shades4,   /* next gadget */
   125,36,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   222,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText3 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "4 shades",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget shades2 = {
   &shades3,   /* next gadget */
   125,25,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText3,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   222,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText4 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "2 shades",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget shades1 = {
   &shades2,   /* next gadget */
   125,14,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText4,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   222,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors1[] = {
   0,0,
   58,0,
   58,14,
   0,14,
   0,0
};
static struct Border Border1 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and draw*mode */
   5,   /* *number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText5 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   4,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "CANCEL",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget cancel = {
   &shades1,   /* next gadget */
   149,62,   /* origin XY of hit box relative to window TopLeft */
   55,13,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText5,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText6 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "640 x 400",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget size4 = {
   &cancel,   /* next gadget */
   14,47,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText6,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   111,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText7 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "320 x 400",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget size3 = {
   &size4,   /* next gadget */
   14,36,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText7,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   111,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText8 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "640 x 200",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget size2 = {
   &size3,   /* next gadget */
   14,25,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText8,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   111,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static SHORT BorderVectors2[] = {
   0,0,
   30,0,
   30,14,
   0,14,
   0,0
};
static struct Border Border2 = {
   -2,-1,   /* XY origin relative to container TopLeft */
   15,0,JAM1,   /* front pen, back pen and draw*mode */
   5,   /* *number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText9 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   4,3,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget ok = {
   &size2,   /* next gadget */
   27,62,   /* origin XY of hit box relative to window TopLeft */
   27,13,   /* hit box width and height */
   NULL,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText9,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   NULL,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};

static struct IntuiText IText10 = {
   15,0,JAM2,   /* front and back text pens, draw*mode and fill byte */
   21,0,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for default */
   "320 x 200",   /* pointer to text */
   NULL   /* next IntuiText static structure */
};

static struct Gadget size1 = {
   &ok,   /* next gadget */
   14,14,   /* origin XY of hit box relative to window TopLeft */
   13,7,   /* hit box width and height */
   GADGHIMAGE+GADGIMAGE,   /* gadget flags */
   GADGIMMEDIATE+TOGGLESELECT,   /* activation flags */
   BOOLGADGET,   /* gadget type flags */
   (APTR)&Image1,   /* gadget border or image to be rendered */
   (APTR)&Image2,   /* alternate imagery for selection */
   &IText10,   /* first IntuiText static structure */
   NULL,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo static structure */
   111,   /* user-definable data */
   NULL   /* pointer to user-definable data */
};


static struct NewWindow NewWindow = {
   13,15,   /* window XY origin relative to TopLeft of screen */
   235,81,   /* window width and height */
   0,2,   /* detail and block pens */
   GADGETUP | GADGETDOWN,   /* IDCMP flags */
   WINDOWDRAG+ACTIVATE+RMBTRAP,   /* other window flags */
   &size1,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Graphic Editor",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

int i,end;
struct Window *win;
struct IntuiMessage *mess;
struct Gadget *psize[4],*pshades[4];
LONG mclass;
APTR maddr;

NewWindow.Screen = screen;
NewWindow.BlockPen = 1<<screen->BitMap.Depth -1;

psize[0] = &size1;
psize[1] = &size2;
psize[2] = &size3;
psize[3] = &size4;
pshades[0] = &shades1;
pshades[1] = &shades2;
pshades[2] = &shades3;
pshades[3] = &shades4;

if ((win = (struct Window *)OpenWindow(&NewWindow))== NULL) {
     return(-1);
     }

ReportMouse(TRUE,win);

RefreshGadgets(win->FirstGadget,win,0L);


RemoveGadget(win,psize[*mode]);
psize[*mode]->Flags= psize[*mode]->Flags | SELECTED;
AddGadget(win,psize[*mode],-1);
RefreshGList(psize[*mode],win,0,1);

RemoveGadget(win,pshades[(*num)-1]);
pshades[(*num)-1]->Flags= pshades[(*num)-1]->Flags | SELECTED;
AddGadget(win,pshades[(*num)-1],-1);
RefreshGList(pshades[(*num) - 1],win,0,1);

toggle_mutexc(win,psize[*mode]);
toggle_mutexc(win,pshades[(*num) - 1]);


again:
mess = NULL;
Wait(1<<win->UserPort->mp_SigBit);
while(mess = ( struct IntuiMessage *) GetMsg(win->UserPort)) {
   mclass = mess->Class;
   maddr = mess->IAddress;
   if (mess != NULL) ReplyMsg(mess);

   if (mclass==GADGETUP || mclass == GADGETDOWN) {

      if (&size1 == (struct Gadget *) maddr) {
         *mode = 0;
         toggle_mutexc(win,&size1);
         }
      if (&size2 == (struct Gadget *) maddr) {
         *mode = 1;
         toggle_mutexc(win,&size2);
          }
      if (&size3 == (struct Gadget *) maddr) {
         *mode = 2;
         toggle_mutexc(win,&size3);
         }
      if (&size4 == (struct Gadget *) maddr) {
         *mode = 3;
         toggle_mutexc(win,&size4);
         }

      if (&shades1 == (struct Gadget *) maddr) {
         *num = 1;
         toggle_mutexc(win,&shades1);
         }
      if (&shades2 == (struct Gadget *) maddr) {
         *num = 2;
         toggle_mutexc(win,&shades2);
         }
      if (&shades3 == (struct Gadget *) maddr) {
         *num = 3;
         toggle_mutexc(win,&shades3);
         }
      if (&shades4 == (struct Gadget *) maddr) {
         *num = 4;
         toggle_mutexc(win,&shades4);
         }

      if (&ok == (struct Gadget *) maddr) {
         end = 0;
         goto ender;
         }
      if (&cancel == (struct Gadget *) maddr) {
         end = -1;
         goto ender;
         }
      RemoveGadget(win,psize[*mode]);
      psize[*mode]->Flags= psize[*mode]->Flags | SELECTED;
      AddGadget(win,psize[*mode],-1);
      RefreshGList(psize[*mode],win,0,1);

      RemoveGadget(win,pshades[(*num)-1]);
      pshades[(*num)-1]->Flags= pshades[(*num)-1]->Flags | SELECTED;
      AddGadget(win,pshades[(*num)-1],-1);
      RefreshGList(pshades[(*num) - 1],win,0,1);

      }
   }
goto again;
ender:

NewWindow.FirstGadget=win->FirstGadget->NextGadget;
CloseWindow(win);
return(end);

}

