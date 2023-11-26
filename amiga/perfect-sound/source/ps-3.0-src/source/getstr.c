#include "exec/types.h"
#include "intuition/intuition.h"

#define SHI 35

/**********************************/
/** input a string from a Window **/
/**********************************/

getstr(screen,string,title,width)

struct Screen *screen;
int width;
char *title,*string;

{
struct Border b;
struct IntuiText t;
struct Gadget g;
struct StringInfo s;
struct IntuiMessage *message, *GetMsg();
char undo[80];
struct Window *w;
static struct NewWindow rw ={
   0,0,   /* window XY origin relative to TopLeft of screen */
   0,SHI,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   BORDERLESS|ACTIVATE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,SHI,   /* minimum width and height */
   640,SHI,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,SHI-1,
                   0,SHI-1,
                   0,0,
                   0,11,
                   99,11};

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

g.GadgetType=STRGADGET;
g.Flags=GADGHCOMP;
g.Activation=RELVERIFY;
g.GadgetRender=NULL;
g.SelectRender=NULL;
g.GadgetText=NULL;
g.NextGadget=NULL;
g.LeftEdge=20;
g.TopEdge=SHI-15;
g.Width=width-40;
g.Height=11;
g.SpecialInfo=(APTR)&s;

s.Buffer=string;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=80;

rw.Width=width;
rw.MaxWidth = width;
rw.MinWidth = width;
rw.TopEdge=50;
rw.LeftEdge=(640-width)/2;
rw.Screen=screen;
rw.FirstGadget = &g;

w=(struct Window *)OpenWindow(&rw);
if (w==NULL) return(-1);

SetAPen(w->RPort, 2);
RectFill(w->RPort, 0, 0, width, SHI);
DrawBorder(w->RPort, &b, 0,0);
PrintIText(w->RPort, &t, 0,0);
ActivateGadget(&g, w, 0);

again:
Wait(1<<w->UserPort->mp_SigBit);
while(message=GetMsg(w->UserPort)) {
   if (message->Class==GADGETUP) {
      ReplyMsg(message);
      CloseWindow(w);
      return(0);
      }
   ReplyMsg(message);
   }
goto again;

}
