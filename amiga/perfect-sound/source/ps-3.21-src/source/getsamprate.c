#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"

/*****************************/
/** request an integer value */
/*****************************/

getsamprate(Window,ival,imin,imax,title,width)

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
int class,code;
struct Gadget *address;
char undo[80],ascii[80];
int i,hor;
short hbdy;
int old_flag;
extern int vid_clock;

static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,IHI-1,
                   0,IHI-1,
                   0,0,
                   0,11,
                   99,11};

if (ival > imax)
   imax = ival;

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
hbdy=0xFFFF/(imax-imin+1);
if (hbdy*(imax-imin+1)==0xFFFF) hbdy--;
prop.Flags=AUTOKNOB|FREEHORIZ;
prop.HorizPot=(SHORT)(((ULONG)(ival-imin)*(ULONG)0xFFFF)/(ULONG)(imax-imin+1))+hbdy;
prop.VertPot=0;
prop.HorizBody=hbdy;
prop.VertBody=0xFFFF;


/* string gadget info */

s.Buffer=ascii;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=(SHORT)80;
s.LongInt=ival;


old_flag = Window->Flags&REPORTMOUSE;
ReportMouse(FALSE,Window);

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
      ReportMouse(old_flag,Window);
      return((int)s.LongInt);
      }
   }

if (class==MOUSEMOVE) {
   ival=(ULONG)(prop.HorizPot*(ULONG)(imax-imin+1))/(ULONG)0xFFFF+imin;
   if (ival > imax) ival=imax;

   ival=(vid_clock/10)/ival;  /** round requested record speed **/
   ival=(vid_clock/10)/ival;  /** to an available record speed **/

   stci_d(ascii,ival,80);
   s.LongInt=ival;
   RefreshGList(&g[2],Window, &loadreq, 1);
   }

if (class==GADGETUP && address==&g[2]) {    /* new integer */
   ival=s.LongInt;
   if (ival>imax) ival=imax;
   if (ival<imin) ival=imin;
   hor=(SHORT)(((ULONG)(ival-imin)*(ULONG)0xFFFF)/(ULONG)(imax-imin+1))+hbdy;
   ModifyProp(&g[1],Window,&loadreq,AUTOKNOB|FREEHORIZ,hor,0,hbdy,0xFFFF);
   }

goto loop;   
}

