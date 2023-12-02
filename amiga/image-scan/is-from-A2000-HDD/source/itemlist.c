#include "exec/types.h"
#include "stdio.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"

#define BLACK 0
#define WHITE 15

/******************************************************/
/*** itemlist.c   -- update a list of string gadgets **/
/******************************************************/

itemlist(screen,names,values,title)

struct Screen *screen;
char *names[],*values[];
char *title;

{
char undo[80];
int len,numitems,i;
ULONG class;
APTR address;
struct IntuiMessage *GetMsg(),*message;
struct StringInfo *s;
struct Gadget *g[23];
struct IntuiText *t[23];
struct Window *w;


static USHORT BorderVectors1[] = {0,0,32,0,32,10,0,10,0,0};
static struct Border Border1 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   WHITE,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors1,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static USHORT BorderVectors2[] = {0,0,48,0,48,10,0,10,0,0};
static struct Border Border2 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   WHITE,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   WHITE,0,JAM1,   /* front and back text pens and drawmode */
   8,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct IntuiText IText2 = {
   WHITE,0,JAM1,   /* front and back text pens and drawmode */
   8,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "HELP",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget Gadget2 = {
   NULL,   /* next gadget */
   0,0,   /* origin XY of hit box relative to window TopLeft */
   45,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static struct Gadget Gadget1 = {
   &Gadget2,   /* next gadget */
   0,0,   /* origin XY of hit box relative to window TopLeft */
   29,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border1,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};


static struct NewWindow rw = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   0,0,   /* window width and height */
   BLACK,WHITE,   /* detail and block pens */
   GADGETUP,   /* IDCMP flags */
   ACTIVATE,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};


undo[0]=0;
len=0;
numitems=0;
while ((*names[numitems])!=0 && numitems<23) {
   i=strlen(names[numitems]);
   if (len < i) len=i;   /* find longest length */
   g[numitems]=(struct Gadget *)AllocMem(sizeof(struct Gadget),MEMF_CLEAR);
   if (g[numitems]==0) {
      for(i=0;i<numitems;i++)
         FreeMem(g[i],sizeof(struct Gadget));
      return(FALSE);
      }
   g[numitems]->SpecialInfo=(APTR)AllocMem(sizeof(struct StringInfo),MEMF_CLEAR);
   if (g[numitems]->SpecialInfo==0) {
      return(FALSE);   /* put memory cleanup in later !!!!!!! */
      }

   t[numitems]=(struct IntuiText *)AllocMem(sizeof(struct IntuiText),MEMF_CLEAR);
   if (t[numitems]==0) {
      return(FALSE);   /* put memory cleanup in later !!!!!!! */
      }
   
   numitems++;   /* count items */
   }


for (i=0;i<numitems;i++) {
   g[i]->LeftEdge=len*8+8;
   g[i]->TopEdge=14+i*10;
   g[i]->Width=80;
   g[i]->Height=10;
   g[i]->Flags=GADGHCOMP;
   g[i]->GadgetType=STRGADGET;
   g[i]->Activation=NULL;
   s=(struct StringInfo *)g[i]->SpecialInfo;
   s->Buffer=values[i];
   s->UndoBuffer=&undo[0];
   s->MaxChars=60;

   t[i]->FrontPen=WHITE;
   t[i]->BackPen=BLACK;
   t[i]->DrawMode=JAM1;
   t[i]->LeftEdge=5;
   t[i]->TopEdge=14+i*10;
   t[i]->IText=names[i];
   }

for (i=0;i<numitems-1;i++) {
   g[i]->NextGadget=g[i+1];
   t[i]->NextText=t[i+1];
   }

g[numitems-1]->NextGadget=&Gadget1;

t[numitems]=(struct IntuiText *)AllocMem(sizeof(struct IntuiText),MEMF_CLEAR);

t[numitems]->FrontPen=WHITE;
t[numitems]->BackPen=BLACK;
t[numitems]->DrawMode=JAM1;
/* t[numitems]->LeftEdge=(len*8+96-strlen(title)*8)/2; */ /* center title */
t[numitems]->TopEdge=2;
t[numitems]->IText=title;
t[numitems-1]->NextText=t[numitems];


Gadget1.TopEdge=16+10*numitems;
Gadget1.LeftEdge=len*8+58;  /* Bottom Right Corner */

Gadget2.TopEdge=16+10*numitems;
Gadget2.LeftEdge=15;  /* Bottom Right Corner */

/*** define unknown intil now newwindow parameters **/

rw.Width=len*8+96;
if (rw.Width < (strlen(title)*8 + 8))  /** make sure title will fit **/
   rw.Width = strlen(title)*8 + 8;
t[numitems]->LeftEdge=(rw.Width-strlen(title)*8)/2; /* center title */
rw.MaxWidth = rw.Width;
rw.MinWidth = rw.Width;
rw.Height=28+numitems*10;
rw.MaxHeight = rw.Height;
rw.MinHeight = rw.Height;
rw.TopEdge=40;
rw.LeftEdge=(screen->Width-rw.Width)/2;
rw.Screen=screen;
rw.FirstGadget=g[0];

w=(struct Window *)OpenWindow(&rw);
if (w==NULL) return(FALSE);  /** !!!!! Put some kind of error msg here ? ***/
                              /** also need to cleanup memory **/
/* SetDrMd(w->RPort,JAM1);
SetAPen(w->RPort,BLACK);
RectFill(w->RPort,1,2,
         w->Width-2,
         w->Height-1); */

RefreshGadgets(w->FirstGadget,w,NULL);    /* fill erase "OK" box */
PrintIText(w->RPort,t[0],0,0);
SetRGB4(&screen->ViewPort, 1, 13, 13, 13);    /** make gray black temp. **/

loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=message->IAddress;
   ReplyMsg(message);

   if (class==GADGETUP && address==(APTR)&Gadget1) {
     CloseWindow(w);
     for(i=0;i<numitems;i++) {
         FreeMem(g[i]->SpecialInfo,sizeof(struct StringInfo));
         FreeMem(g[i],sizeof(struct Gadget));
         FreeMem(t[i],sizeof(struct IntuiText));
         }
      FreeMem(t[numitems],sizeof(struct IntuiText));
      SetRGB4(&screen->ViewPort, 1, 1, 1, 1);        /** Gray **/
      return(TRUE);
      }
   if (class==GADGETUP && address==(APTR)&Gadget2) {
      scan_area_help();
      SetRGB4(&screen->ViewPort, 1, 13, 13, 13);    /** needs restoring if telluser was called */
      }
   }
goto loop;
}

