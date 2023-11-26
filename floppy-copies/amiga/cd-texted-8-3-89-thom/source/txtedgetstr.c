
#include "exec/types.h"
#include "intuition/intuition.h"


textedgetstr(screen,string,title,width)

struct Screen *screen;
int width;
char *title,*string;

{
struct Window *Window;
struct IntuiMessage *mess;
LONG mclass;
APTR maddr;
int aflag;

static SHORT BorderVectors1[] = {
   0,0,
   72,0,
   72,9,
   0,9,
   0,0
};
static struct Border Border1 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors1,
   NULL
};

static struct Gadget Gadget3 = {
   NULL,
   137,39,
   69,8,
   NULL,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border1,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL
};

static SHORT BorderVectors2[] = {
   0,0,
   72,0,
   72,9,
   0,9,
   0,0
};
static struct Border Border2 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors2,
   NULL
};

static struct IntuiText IText3 = {
   3,0,JAM2,
   20,-30,
   NULL,
   "Enter Search String",
   NULL
};


static struct IntuiText IText2 = {
   3,0,JAM2,
   139,0,
   NULL,
   "Quit",
   &IText3
};

static struct IntuiText IText1 = {
   3,0,JAM2,
   14,0,
   NULL,
   "Clear",
   &IText2
};

static struct Gadget Gadget2 = {
   &Gadget3,
   16,39,
   69,8,
   NULL,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border2,
   NULL,
   &IText1,
   NULL,
   NULL,
   NULL,
   NULL
};

static struct StringInfo Gadget1SInfo = {
   NULL,
   NULL,
   0,
   1,
   0,
   0,0,0,0,0,
   0,
   0,
   NULL
};

static SHORT BorderVectors3[] = {
   0,0,
   197,0,
   197,10,
   0,10,
   0,0
};
static struct Border Border3 = {
   -2,-1,
   3,0,JAM1,
   5,
   BorderVectors3,
   NULL
};

static struct Gadget Gadget1 = {
   &Gadget2,
   14,23,
   194,9,
   NULL,
   RELVERIFY+STRINGCENTER,
   STRGADGET,
   (APTR)&Border3,
   NULL,
   NULL,
   NULL,
   (APTR)&Gadget1SInfo,
   NULL,
   NULL
};

#define GadgetList1 Gadget1

#define IntuiTextList1 IText3

static struct NewWindow NewWindow = {
   171,50,
   224,54,
   0,1,
   GADGETUP,
   ACTIVATE,
   &Gadget1,
   NULL,
   NULL,
   NULL,
   NULL,
   5,5,
   640,200,
   WBENCHSCREEN
};

NewWindow.Type = CUSTOMSCREEN;
NewWindow.Screen = screen;

/** initilize text static structures **/

/* add gadgets */

Gadget1SInfo.Buffer=string;
Gadget1SInfo.UndoBuffer=NULL;
Gadget1SInfo.MaxChars=80;

IText3.IText = title;

if ((Window = (struct Window *)OpenWindow(&NewWindow))== NULL)
     return(1);

do {
   aflag = ActivateGadget(&Gadget1,Window,0L);
   } while (aflag != TRUE);

again:
mess = NULL;
Wait(1<<Window->UserPort->mp_SigBit);
while(mess = GetMsg(Window->UserPort)) {
   mclass = mess->Class;
   maddr = mess->IAddress;
   if (mess != NULL) ReplyMsg(mess);

   if (mclass==GADGETUP) {

      if (&Gadget1 == (struct Gadget *) maddr) {
         CloseWindow(Window);
         return(0);
         }
      if (&Gadget3 == (struct Gadget *) maddr) {
         CloseWindow(Window);
         return(0);
         }
      if (&Gadget2 == (struct Gadget *) maddr) {
         setmem(string,80,0);
         RefreshGadgets(&Gadget1,Window,0L);
         do {
            aflag = ActivateGadget(&Gadget1,Window,0L);
            } while (aflag != TRUE);
         }
      }
   }
goto again;

}


