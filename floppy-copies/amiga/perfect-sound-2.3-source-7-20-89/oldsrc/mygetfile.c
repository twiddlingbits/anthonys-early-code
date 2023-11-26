#include "exec/types.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"
#include "psound.h"


/**********************************/
/**  request a file name         **/
/**********************************/

getfile(Window,fname,drawer,ok,title)

struct Window *Window;
char *fname,*drawer;
char *ok,*title;


{
extern WORD diskchange;
static char blank[]="                            ";
char line[80];
int i;
int class,code,x,y;
SHORT ver;
static int top,numfiles,vpot;
struct Gadget *address;
static char undo[80];
static char fn[90][36];
static int ft[80];
static struct Border b[4];
static struct IntuiText t[5];
static struct IntuiText nt[SHOW];
static struct Gadget g[5];
static struct Gadget name[SHOW];
static struct StringInfo s[2];
static struct PropInfo prop;
static struct Requester loadreq;
static struct IntuiMessage *message;
static struct Image image;

static SHORT bd0[]={0,0,
                    274,0,
                    274,149,
                    0,149,
                    0,0,
                    0,1,
                    273,1,
                    273,148,
                    1,148,
                    1,0,
                    0,0,
                    0,12,
                    274,12};
static SHORT bd1[]={0,100,275,100};
static SHORT bd2[]={30,130,86,130,86,142,30,142,30,130};
static SHORT bd3[]={180,130,
            236,130,
            236,142,
            180,142,
            180,130};

int lock,success;
struct FileInfoBlock *info;

if (drawer[0]==0) strcpy(drawer,"df0:");

if ((info=(struct FileInfoBlock *)
          AllocMem(sizeof(struct FileInfoBlock),0))==0) {
   return(-1);
   }

for (i=0;i<4;i++) {
   b[i].DrawMode=JAM1;
   b[i].LeftEdge=0;
   b[i].TopEdge=0;
   b[i].FrontPen=3;
   b[i].BackPen=0;
   b[i].NextBorder=&b[i+1];
}
b[3].NextBorder=NULL;
b[0].Count=13;
b[0].XY=&bd0[0];
b[1].Count=2;
b[1].XY=&bd1[0];
b[2].Count=5;
b[2].XY=&bd2[0];
b[3].Count=5;
b[3].XY=&bd3[0];

/** initilize text structures **/

for (i=0;i<5;i++) {
   t[i].DrawMode=JAM1;
   t[i].FrontPen=1;
   t[i].BackPen=0;
   t[i].ITextFont=NULL;
   t[i].NextText=&t[i+1];
}

t[4].NextText=NULL;

t[0].LeftEdge=8;
t[0].TopEdge=105;
t[0].IText="Drawer:";

t[1].LeftEdge=23;
t[1].TopEdge=119;
t[1].IText="File:";

t[2].LeftEdge=42;
t[2].TopEdge=133;
t[2].IText=ok;

t[3].LeftEdge=184;
t[3].TopEdge=133;
t[3].IText="CANCEL";

t[4].LeftEdge=70;
t[4].TopEdge=3;
t[4].IText=title;

/* add gadgets */
for (i=0;i<SHOW;i++) {
   name[i].GadgetType=REQGADGET|BOOLGADGET;
   name[i].Flags=GADGHCOMP;
   name[i].NextGadget=&name[i+1];
   name[i].Activation=RELVERIFY;
   name[i].GadgetRender=NULL;
   name[i].SelectRender=NULL;
   name[i].GadgetText=&nt[i];
   name[i].LeftEdge=10;
   name[i].TopEdge=i*10+15;
   name[i].Width=230;
   name[i].Height=11;

   nt[i].DrawMode=JAM2;
   nt[i].FrontPen=1;
   nt[i].BackPen=2;
   nt[i].ITextFont=NULL;
   nt[i].NextText=NULL;
   nt[i].TopEdge=1;
   nt[i].LeftEdge=0;
   nt[i].IText="";
   }
name[SHOW-1].NextGadget=&g[0];

for (i=0;i<5;i++) {
   g[i].GadgetType=REQGADGET;
   g[i].Flags=GADGHNONE;
   g[i].NextGadget=&g[i+1];
   g[i].Activation=RELVERIFY;
   g[i].GadgetRender=NULL;
   g[i].SelectRender=NULL;
   g[i].GadgetText=NULL;
}

g[4].NextGadget=NULL;

/* load */
g[0].LeftEdge=30;
g[0].TopEdge=130;
g[0].Width=56;
g[0].Height=12;
g[0].GadgetType|=BOOLGADGET;
g[0].Activation|=ENDGADGET;
/* cancel */
g[1].LeftEdge=180;
g[1].TopEdge=130;
g[1].Width=56;
g[1].Height=12;
g[1].GadgetType|=BOOLGADGET;
g[1].Activation|=ENDGADGET;
/* Drawer */
g[2].LeftEdge=70;
g[2].TopEdge=105;
g[2].Width=180;
g[2].Height=15;
g[2].GadgetType|=STRGADGET;
g[2].SpecialInfo=(APTR)&s[0];
g[2].Flags=GADGHCOMP;
/* file */
g[3].LeftEdge=70;
g[3].TopEdge=119;
g[3].Width=180;
g[3].Height=15;
g[3].GadgetType|=STRGADGET;
g[3].SpecialInfo=(APTR)&s[1];
g[3].Flags=GADGHCOMP;
/* autoknob */
g[4].LeftEdge=250;
g[4].TopEdge=13;
g[4].Width=24;
g[4].Height=87;
g[4].GadgetType|=PROPGADGET;
g[4].SpecialInfo=(APTR)&prop;
g[4].GadgetRender=&image;

/* prop structure */
prop.Flags=AUTOKNOB|FREEVERT;
prop.HorizPot=0;
prop.VertPot=0;
prop.HorizBody=0xFFFF;
prop.VertBody=0xFFFF;


/* string gadget info */

s[0].Buffer=drawer;
s[1].Buffer=fname;

for (i=0;i<2;i++) {
   s[i].UndoBuffer=undo;
   s[i].BufferPos=0;
   s[i].DispPos=0;
   s[i].MaxChars=80;
}


InitRequester(&loadreq);

loadreq.LeftEdge=15;
loadreq.TopEdge=11;
loadreq.Width=275;
loadreq.Height=150;
loadreq.ReqBorder=&b[0];
loadreq.ReqText=&t[0];
loadreq.ReqGadget=&name[0];
loadreq.BackFill=2;   /* black */

Request(&loadreq,Window);
if (diskchange==FALSE) goto quick;

/**  load current dir **/

newdir:
lock=Lock(drawer,ACCESS_READ);
if (lock==0) {
   numfiles=0;
   top=0;
   goto fresh;
   }

if ((success=Examine(lock,info))==0) {
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(-1);
   }

ExNext(lock,info);  /* skip volume name */

i=0;
do {
   strncpy(fn[i],info->fib_FileName,30);
   ft[i]=info->fib_DirEntryType;   /* + is dir, - is file */
   if (ft[i]>0) strcpy(fn[i]+strlen(fn[i])," (dir)");
   i++;
   } while (i<90 && ExNext(lock,info)!=0);
UnLock(lock);
numfiles=i;
diskchange=FALSE;
top=0;  /* 1st file shown in list */
vpot=0;  /* prop position */

quick:

ver=(WORD)((float)0xFFFF*(float)SHOW/(float)numfiles);
ModifyProp(&g[4],Window,&loadreq,AUTOKNOB|FREEVERT,0,vpot,0xFFFF,ver);
fresh:

for (i=0;i<SHOW;i++)
   nt[i].IText=blank;

RefreshGadgets(&name[0],Window,&loadreq);

for (i=top;i<top+SHOW;i++) {
   if (i<numfiles) nt[i-top].IText=fn[i];
   }

RefreshGadgets(&name[0],Window,&loadreq);

/* wait for a msg */
loop:
Wait(1<<Window->UserPort->mp_SigBit);

while (message=GetMsg(Window->UserPort)) {
class=message->Class;
code=message->Code;
address=message->IAddress;
x=message->MouseX;
y=message->MouseY;
ReplyMsg(message);

if (class!=GADGETUP) {
   goto skip;
   }

if (address==&g[0]) {    /* Load file */
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(0);
   }

if (address==&g[1]) {   /* cancel */
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(-1);
   }

if (address==&g[2]) {    /* drawer */
   goto newdir;
   }

if (address==&g[4]) {  /* prop gadget */
   top=prop.VertPot*numfiles/0xFFFF;
   vpot=prop.VertPot;
   goto fresh;
   }

for (i=0;i<SHOW;i++)  /* picking an entry? */
   if (&name[i]==address) {
      i=i+top;
      if (i>=numfiles) goto skip;
      if (ft[i]>0) {      /* directory */
         strncpy(line,fn[i],strlen(fn[i])-6);
         add(drawer,line);
         goto newdir;
         }
      else {
         strcpy(fname,fn[i]);
         RefreshGadgets(&name[0],Window,&loadreq);
         }
      }

skip: ;
}
goto loop;
}

