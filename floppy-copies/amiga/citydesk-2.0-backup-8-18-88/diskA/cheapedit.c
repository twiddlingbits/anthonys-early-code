#include "exec/types.h"
#include "intuition/intuition.h"
#include "stdio.h"

#define BUFFSHOW 65


char *cheapedit(screen,orgtext)
struct Screen *screen;
char *orgtext;

{
static struct Image propimage;

static struct PropInfo propinfo = {
   AUTOKNOB|FREEHORIZ,
   0,0,
   0xFFFF,0xFFFF,
   0,0,0,0,0,0
};


static struct Gadget prop = {
   NULL,   /* next gadget */
   26,35,   /* origin XY of hit box relative to window TopLeft */
   586,8,   /* hit box width and height */
   GADGHNONE,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   PROPGADGET,   /* gadget type */
   &propimage,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   (APTR)&propinfo,   /* SpecialInfo structure for string gadgets */
   4,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static UBYTE SIBuffer44[80] =
   "[";
static struct StringInfo GadgetSI44 = {
   SIBuffer44,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   NULL,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static USHORT BorderVectors11[] = {0,0,133,0,133,10,0,10,0,0};
static struct Border Border11 = {
   -2,-2,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors11,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget search_string = {
   &prop,   /* next gadget */
   153,52,   /* origin XY of hit box relative to window TopLeft */
   130,8,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type */
   (APTR)&Border11,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   (APTR)&GadgetSI44,   /* SpecialInfo structure for string gadgets */
   44,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors22[] = {0,0,20,0,20,10,0,10,0,0};
static struct Border Border22 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors22,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText11 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   0,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   ">>",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget search_right = {
   &search_string,   /* next gadget */
   123,51,   /* origin XY of hit box relative to window TopLeft */
   17,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border22,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText11,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   33,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static USHORT BorderVectors33[] = {0,0,20,0,20,10,0,10,0,0};
static struct Border Border33 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors33,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText22 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   1,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "<<",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget search_left = {
   &search_right,   /* next gadget */
   96,51,   /* origin XY of hit box relative to window TopLeft */
   17,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   GADGIMMEDIATE,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border33,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText22,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   22,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static unsigned short BorderVectors2[] = {0,0,61,0,61,10,0,10,0,0};
static struct Border Border2 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors2,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText1 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   6,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "CANCEL",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget cancel = {
   &search_left,   /* next gadget */
   557,51,   /* origin XY of hit box relative to window TopLeft */
   58,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border2,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText1,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   3,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static unsigned short BorderVectors3[] = {0,0,30,0,30,10,0,10,0,0};
static struct Border Border3 = {
   -2,-1,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors3,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct IntuiText IText2 = {
   3,0,JAM2,   /* front and back text pens and drawmode */
   5,1,   /* XY origin relative to container TopLeft */
   NULL,   /* font pointer or NULL for defaults */
   "OK",   /* pointer to text */
   NULL   /* next IntuiText structure */
};

static struct Gadget ok = {
   &cancel,   /* next gadget */
   26,51,   /* origin XY of hit box relative to window TopLeft */
   27,9,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   BOOLGADGET,   /* gadget type */
   (APTR)&Border3,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   &IText2,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   NULL,   /* SpecialInfo structure for string gadgets */
   2,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

static struct StringInfo GadgetSI1 = {
   NULL,   /* buffer where text will be edited */
   NULL,   /* optional undo buffer */
   0,   /* character position in buffer */
   80,   /* maximum number of characters to allow */
   0,   /* first displayed character buffer position */
   0,0,0,0,0,   /* Intuition initialized and maintained variables */
   0,   /* Rastport of gadget */
   NULL,   /* initial value for integer gadgets */
   NULL   /* alternate keymap (fill in if you set the flag) */
};

static unsigned short BorderVectors4[] = {0,0,589,0,589,10,0,10,0,0};
static struct Border Border4 = {
   -2,-2,   /* border XY origin relative to container TopLeft */
   3,0,JAM1,   /* front pen, back pen and drawmode */
   5,   /* number of XY vectors */
   BorderVectors4,   /* pointer to XY vectors */
   NULL   /* next border in list */
};

static struct Gadget string = {
   &ok,   /* next gadget */
   26,20,   /* origin XY of hit box relative to window TopLeft */
   586,8,   /* hit box width and height */
   GADGHCOMP,   /* gadget flags */
   RELVERIFY,   /* activation flags */
   STRGADGET,   /* gadget type */
   (APTR)&Border4,   /* gadget border or image to be rendered */
   NULL,   /* alternate imagery for selection */
   NULL,   /* first IntuiText structure */
   0,   /* gadget mutual-exclude long word */
   (APTR)&GadgetSI1,   /* SpecialInfo structure for string gadgets */
   1,   /* user-definable data (ordinal gadget number) */
   NULL   /* pointer to user-definable data */
};

/* Gadget list */

static struct NewWindow NewWin = {
   0,41,   /* window XY origin relative to TopLeft of screen */
   640,71,   /* window width and height */
   0,1,   /* detail and block pens */
   GADGETUP|GADGETDOWN,   /* IDCMP flags */
   ACTIVATE|WINDOWDRAG,   /* other window flags */
   &string,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "AVS Text Editor:",   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   640,71,   /* minimum width and height */
   640,71,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};



struct Window *w;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();
ULONG class;
char *editbuff,*newtext;
long i;
long orglen,bufflen,limit;
ULONG hbody,newpos,hpot;
char *oldtitle;

oldtitle = NewWin.Title;
NewWin.Screen=screen;

if (orgtext!=NULL)
   orglen=strlen(orgtext)+1;
else {
   orglen=0;
   NewWin.Title = "Headline Editor:";
   }

bufflen=orglen+2048;
editbuff=(char *)AllocMem(bufflen,0);
if (editbuff==NULL) {
   return(NULL);
   }

*editbuff=NULL;

if (orglen > 0) {
   movmem(orgtext,editbuff,strlen(orgtext)+1);
   }

#if 0
   dest=editbuff;
   src=orgtext;
   while ((*dest=*src)!=NULL) {
      if (*src==10) {
         *dest++='\\';
         *dest='n';
         }
      if (*src=='\\') {
         dest++;
         *dest='\\';
         }
      dest++;
      src++;
      }
#endif


GadgetSI1.Buffer=editbuff;
GadgetSI1.MaxChars=bufflen;


w=(struct Window *)OpenWindow(&NewWin);
NewWin.Title = oldtitle;
if (w==NULL) {
   FreeMem(editbuff,bufflen);
   return(NULL);
   }

if (strlen(editbuff) < GadgetSI1.NumChars) {
   propinfo.HorizPot=0;
   GadgetSI1.BufferPos=0;
   GadgetSI1.DispPos=0;
   }

GadgetSI1.NumChars=strlen(editbuff);  /** +1 ?? Does this include NULL? **/

if (strlen(editbuff) <= GadgetSI1.DispCount) {
   hbody=0xFFFF;
   propinfo.HorizPot=0;
   GadgetSI1.BufferPos=0;
   GadgetSI1.DispPos=0;
   }
else  {
   hbody=((ULONG)GadgetSI1.DispCount*0xFFFFL)/(GadgetSI1.NumChars);
   propinfo.HorizPot=((ULONG)GadgetSI1.DispPos*0xFFFFL)/
            (GadgetSI1.NumChars - GadgetSI1.DispCount);
      }

ModifyProp(&prop,w,0,AUTOKNOB|FREEHORIZ,propinfo.HorizPot,
   propinfo.VertPot,hbody,0xFFFF);

ActivateGadget(&string,w,0);


loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);


   if (class==GADGETDOWN) {
      if (address==&search_right) {
         limit = GadgetSI1.NumChars - strlen(SIBuffer44) -1;
         for (i=GadgetSI1.BufferPos + 1; i < limit; i ++) {
            if (stcpma(editbuff+i,SIBuffer44)) {
               GadgetSI1.BufferPos=i;
               if (i > GadgetSI1.DispCount+GadgetSI1.DispPos)
                  GadgetSI1.DispPos=
                     min(i,GadgetSI1.NumChars - GadgetSI1.DispCount -1);
               RefreshGadgets(&string,w,0);
               goto refreshprop;
               }
            }
         telluser2(screen,"No match found.","",FALSE);
         }

      if (address==&search_left) {
         for (i=GadgetSI1.BufferPos-1; i >0; i--) {
            if (stcpma(editbuff+i,SIBuffer44)) {
               GadgetSI1.BufferPos=i;
               if (i < GadgetSI1.DispPos)
                  GadgetSI1.DispPos=i;
               RefreshGadgets(&string,w,0);
               goto refreshprop;
               }
            }
         telluser2(screen,"No match found.","",FALSE);
         }
      }


   if (class==GADGETUP && address==&cancel) {
      CloseWindow(w);
      FreeMem(editbuff,bufflen);
      return(NULL);
      }

   if (class==GADGETUP && address==&ok) {
      if (editbuff[0]==0) {  /** didn't enter anything **/
         CloseWindow(w);
         FreeMem(editbuff,bufflen);
         return(NULL);
         }

#if 0

      i=0;   /** count and convert \n to newlines **/
      src=editbuff;
      while (*src!=0) {
         if (*src=='\\' && *(src+1)=='n') {
            i++;
            *src++=10;   /* new line */
            }
         if (*src=='\\' && *(src+1)=='\\') {
            i++;
            *src++=8;  /** mark one slash so we will skip it below **/
            }
         src++;
         }

#endif

      CloseWindow(w);
      if (orglen > 0)
         FreeMem(orgtext,orglen);   /* well, there goes the orignal ...*/
/*      newtext=(char *)AllocMem(strlen(editbuff)+1-i,0);   */
      newtext=(char *)AllocMem(strlen(editbuff)+1,0);
      if (newtext==NULL)         /* oh shit, not now */
         return(editbuff);       /* at least giv'em a chance */

#if 0

      src=editbuff;   /** now copy to final buffer and remove 'n's' **/
      dest=newtext;
      while (*src!=NULL) {
         if (*src==8)   /** skip 8's which were backslash **/
            src++;
         *dest=*src;
         if (*src==10)
            src++;
         src++;
         dest++;
         }
      *dest=NULL;

#endif

      movmem(editbuff,newtext,strlen(editbuff)+1);

      FreeMem(editbuff,bufflen);
      return(newtext);          /* return the edited text */
      }

   if (class==GADGETUP && address==&string) {

refreshprop:

      hbody=((ULONG)min(GadgetSI1.NumChars,GadgetSI1.DispCount)*0xFFFFL)/
            GadgetSI1.NumChars;
      hpot=((ULONG)GadgetSI1.DispPos*0xFFFFL)/
            (GadgetSI1.NumChars - GadgetSI1.DispCount);

      ModifyProp(&prop,w,0,AUTOKNOB|FREEHORIZ,hpot,
         propinfo.VertPot,hbody,0xFFFF);

      ActivateGadget(&string,w,0);
      }

   if (class==GADGETUP && address==&prop) { /* moved prop */
      if (GadgetSI1.NumChars > GadgetSI1.DispCount) {
         newpos=((ULONG)(GadgetSI1.NumChars - GadgetSI1.DispCount)*
                 propinfo.HorizPot + (1<<15) ) >> 16;
         }
      else {
         newpos=0;
         }
      GadgetSI1.DispPos=newpos;
      GadgetSI1.BufferPos=newpos;
      RefreshGadgets(&string,w,0);
      }
   }
goto loop;
}


