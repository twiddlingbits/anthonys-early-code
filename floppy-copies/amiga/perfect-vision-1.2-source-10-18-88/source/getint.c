#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"

#define IHI 50

/*****************************/
/** request an integer value */
/*****************************/

getint(screen,ival,imin,imax,title,width)

struct Screen *screen;
int ival,width,imin,imax;
char *title;

{
struct Window *w;
struct Border b;
struct IntuiText t[2];
struct Gadget g[3];
struct StringInfo s;
struct PropInfo prop;
struct Requester loadreq;
struct IntuiMessage *message;
struct Image image;
int class,code,x,y;
struct Gadget *address;
char undo[80],ascii[80];
int i,hor;

static struct NewWindow rw ={
   0,0,   /* window XY origin relative to TopLeft of screen */
   0,0,   /* window width and height */
   0,1,   /* detail and block pens */
   MOUSEMOVE|GADGETUP|GADGETDOWN,   /* IDCMP flags */
   BORDERLESS,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   320,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

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
g[1].Activation=FOLLOWMOUSE|RELVERIFY;
g[1].Flags=GADGHNONE;

/* prop structure */
prop.Flags=AUTOKNOB|FREEHORIZ;
prop.HorizPot=min(0xFFFF,((LONG)(ival-imin)<<16) / (imax-imin));
prop.VertPot=0;
prop.HorizBody=0xFFFF/(imax - imin+1);
prop.VertBody=0xFFFF;


/* string gadget info */

s.Buffer=ascii;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=80;
s.LongInt=ival;


rw.Width=width;
rw.MinWidth=width;
rw.MaxWidth=width;
rw.Height=IHI;
rw.MinHeight=IHI;
rw.MaxHeight=IHI;
rw.TopEdge=20;
rw.LeftEdge=(320-width)/2;
rw.Screen=screen;

w=(struct Window *)OpenWindow(&rw);
if (w==NULL) return(-1);

InitRequester(&loadreq);

loadreq.LeftEdge=0;
loadreq.TopEdge=0;
loadreq.Width=width;
loadreq.Height=IHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t[0];
loadreq.ReqGadget=&g[0];
loadreq.BackFill=2;   /* black */

Request(&loadreq,w);

loop:
Wait(1<<w->UserPort->mp_SigBit);

while (message=GetMsg(w->UserPort)) {
   class=message->Class;
   code=message->Code;
   address=(struct Gadget *)message->IAddress;
   x=message->MouseX;
   y=message->MouseY;
   ReplyMsg(message);
}

if (class==GADGETUP && address==&g[0]) {  /* ok! */
   CloseWindow(w);
   return((int)s.LongInt);
   }

if (class==GADGETUP && address==&g[1]) {
   ival=(prop.HorizPot*(imax - imin) + (1<<15)) >>16;
   ival += imin;
   stci_d(ascii,ival,80);
   s.LongInt=ival;
   RefreshGadgets(&g[2],w,&loadreq);
   }

if (class==MOUSEMOVE) {
   ival=(prop.HorizPot*(imax - imin) + (1<<15)) >>16;
   ival += imin;
   stci_d(ascii,ival,80);
   s.LongInt=ival;
   RefreshGadgets(&g[2],w,&loadreq);
   }

if (class==GADGETUP && address==&g[2]) {    /* new integer */
   ival=s.LongInt;
   if (ival>imax) ival=imax;
   if (ival<imin) ival=imin;
   hor=min(0xFFFF,((LONG)(ival-imin)<<16) / (imax-imin));
   ModifyProp(&g[1],w,&loadreq,AUTOKNOB|FREEHORIZ,hor,0,
       prop.HorizBody,0xFFFF);
   }

goto loop;   
   
}
