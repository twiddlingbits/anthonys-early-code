#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

/*****************************/
/** request an integer value */
/*****************************/

getint(Window,ival,imin,imax,title,width)

struct Window *Window;
int ival,width,imin,imax;
char *title;

{
struct Border b;
struct IntuiText t[2];
struct Gadget g[3];
struct StringInfo s;
struct PropInfo prop;
struct Requester loadreq;
struct IntuiMessage *message;
struct IntuiMessage *GetMsg();
struct Image image;
int class,code,x,y;
struct Gadget *address;
char undo[80],ascii[80];
int i,hor;
static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,IHI-1,
                   0,IHI-1,
                   0,0,
                   0,11,
                   99,11};

bd[2]=width-1;
bd[4]=bd[2];
bd[12]=bd[2];

/* initilize string (int) gadget */
stci_d(ascii,ival,80);


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

for (i=0;i<2;i++) {
   t[i].DrawMode=JAM1;
   t[i].FrontPen=1;
   t[i].BackPen=0;
   t[i].ITextFont=NULL;
   t[i].NextText=&t[i+1];
}

t[1].NextText=NULL;

t[0].LeftEdge=(width-8*strlen(title))/2;
t[0].TopEdge=2;
t[0].IText=title;

t[1].LeftEdge=width-60;
t[1].TopEdge=IHI-15;
t[1].IText=" OK! ";
t[1].DrawMode=JAM2;

/* add gadgets */

for (i=0;i<3;i++) {
   g[i].GadgetType=REQGADGET;
   g[i].Flags=GADGHCOMP;
   g[i].NextGadget=&g[i+1];
   g[i].Activation=RELVERIFY;
   g[i].GadgetRender=NULL;
   g[i].SelectRender=NULL;
   g[i].GadgetText=NULL;
}

g[2].NextGadget=NULL;

/* ok! */
g[0].LeftEdge=width-60;
g[0].TopEdge=IHI-15;
g[0].Width=40;
g[0].Height=8;
g[0].GadgetType|=BOOLGADGET;
g[0].Activation|=ENDGADGET;

/* int string (enter value) */
g[2].LeftEdge=30;
g[2].TopEdge=IHI-15;
g[2].Width=42;
g[2].Height=11;
g[2].GadgetType|=STRGADGET;
g[2].Activation|=LONGINT|STRINGCENTER;
g[2].SpecialInfo=(APTR)&s;

/* autoknob */
g[1].LeftEdge=10;
g[1].TopEdge=20;
g[1].Width=width-20;
g[1].Height=9;
g[1].GadgetType|=PROPGADGET;
g[1].SpecialInfo=(APTR)&prop;
g[1].GadgetRender=&image;
g[1].Activation=FOLLOWMOUSE|GADGIMMEDIATE;
g[1].Flags=GADGHNONE;

/* prop structure */
prop.Flags=AUTOKNOB|FREEHORIZ;
prop.HorizPot=(SHORT)((LONG)(ival-imin)*(LONG)0xFFFF/(LONG)(imax-imin));
prop.VertPot=0;
prop.HorizBody=1;
prop.VertBody=0xFFFF;


/* string gadget info */

s.Buffer=ascii;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=(SHORT)80;
s.LongInt=ival;


InitRequester(&loadreq);

loadreq.LeftEdge=100;
loadreq.TopEdge=50;
loadreq.Width=width;
loadreq.Height=IHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t[0];
loadreq.ReqGadget=&g[0];
loadreq.BackFill=2;   /* brown */

Request(&loadreq,Window);

loop:
Wait(1<<Window->UserPort->mp_SigBit);

while (message=GetMsg(Window->UserPort)) {
   class=message->Class;
   code=message->Code;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   if (class==GADGETUP && address==&g[0]) {  /* ok! */
      while (message=GetMsg(Window->UserPort))
         ReplyMsg(message);
      return((int)s.LongInt);
      }
   }

if (class==MOUSEMOVE) {
   ival=prop.HorizPot*(imax-imin)/0xFFFF+imin;
   stci_d(ascii,ival,80);
   s.LongInt=ival;
   RefreshGList(&g[2],Window, &loadreq, 1);
   }

if (class==GADGETUP && address==&g[2]) {    /* new integer */
   ival=s.LongInt;
   if (ival>imax) ival=imax;
   if (ival<imin) ival=imin;
   hor=(int)((LONG)(ival-imin)*(LONG)0xFFFF/(LONG)(imax-imin));
   ModifyProp(&g[1],Window,&loadreq,AUTOKNOB|FREEHORIZ,hor,0,1,0xFFFF);
   }

goto loop;   
   
}
getstr(Window,string,title,width)

struct Window *Window;
int width;
char *title,*string;

{
struct Border b;
struct IntuiText t;
struct Gadget g;
struct StringInfo s;
struct Requester loadreq;
struct IntuiMessage *message;
char undo[80];
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

g.GadgetType=REQGADGET|STRGADGET;
g.Flags=GADGHCOMP;
g.Activation=ENDGADGET|RELVERIFY;
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

InitRequester(&loadreq);

loadreq.LeftEdge=(640-width)/2;
loadreq.TopEdge=50;
loadreq.Width=width;
loadreq.Height=SHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t;
loadreq.ReqGadget=&g;
loadreq.BackFill=2;   /* black */

Request(&loadreq,Window);

Delay(20);
ActivateGadget(&g,Window,&loadreq);

again:
Wait(1<<Window->UserPort->mp_SigBit);
while(message=GetMsg(Window->UserPort)) {
   if (message->Class==GADGETUP) {
      ReplyMsg(message);
      return(0);
      }
   ReplyMsg(message);
   }
goto again;

}
