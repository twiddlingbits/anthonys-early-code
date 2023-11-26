#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

/** I found a bug that causes WB1.3 to do wierd things if your gadget
    width allows strings bigger than MaxChars **/

newgetstr(string,title,width)
int width;
UBYTE *title,*string;

{
extern struct studioBase *studioBase;
struct Window *Window;
struct Border b;
struct IntuiText t;
struct Gadget g;
struct StringInfo s;
struct Requester loadreq;
struct IntuiMessage *message,*GetMsg();
UBYTE undo[40];

static struct NewWindow NewWindow = {
   113,52,   /* window XY origin relative to TopLeft of screen */
   370,103,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   ACTIVATE+NOCAREREFRESH+BORDERLESS,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};
static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,SHI-1,
                   0,SHI-1,
                   0,0,
                   0,11,
                   99,11};

/* printf("str '%s'\n",string);
printf("undo '%s'\n",undo); */

bd[2]=width-1;
bd[4]=bd[2];
bd[12]=bd[2];


b.DrawMode=JAM1;
b.LeftEdge=0;
b.TopEdge=0;
b.FrontPen=3;
b.BackPen=0;
b.NextBorder=NULL;
b.NextBorder=NULL;
b.Count=7;
b.XY=&bd[0];

/** initilize text structures **/

t.DrawMode=JAM1;
t.FrontPen=1;
t.BackPen=0;
t.ITextFont=NULL;
t.NextText=NULL;
t.LeftEdge=(width-8*strlen(title))/2;
t.TopEdge=2;
t.IText=title;


/* add gadgets */

g.GadgetType=REQGADGET|STRGADGET;
g.Flags=GADGHCOMP;
g.Activation=ENDGADGET|RELVERIFY;
g.GadgetRender=NULL;
g.SelectRender=NULL;
g.GadgetText=NULL;
g.NextGadget=NULL;
g.TopEdge=SHI-15;
g.Width=width-40;
if (g.Width > 312)   /** 39 X 8 = 304 (40 chars max & 8 pix per char) **/
   g.Width = 312;
g.LeftEdge=(width-g.Width)/2;
g.Height=11;
g.SpecialInfo=(APTR)&s;

s.Buffer=string;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=40;

InitRequester(&loadreq);

loadreq.LeftEdge=0;
loadreq.TopEdge=0;
loadreq.Width=width;
loadreq.Height=SHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t;
loadreq.ReqGadget=&g;
loadreq.BackFill=2;   /* black */

NewWindow.LeftEdge=(640-width)/2;
NewWindow.TopEdge=50;
NewWindow.Width=width;
NewWindow.Height=SHI;
NewWindow.Screen=studioBase->screen;

Window = (struct Window *)OpenWindow(&NewWindow);
if (Window) {
   Request(&loadreq,Window);

   Delay(20);
   ActivateGadget(&g,Window,&loadreq);

   again:
   Wait(1<<Window->UserPort->mp_SigBit);
   while(message=GetMsg(Window->UserPort)) {
      if (message->Class==GADGETUP) {
         ReplyMsg(message);
         string[39]=0;
         CloseWindow(Window);
         return(0);
         }
      ReplyMsg(message);
      }
   goto again;
   }
}

