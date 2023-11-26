#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "laser.h"

cdprefs(screen)

struct Screen *screen;

{

static USHORT ImageData1[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image1 = {
   0,0,
   13,7,
   2,
   ImageData1,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData2[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image2 = {
   0,0,
   13,7,
   2,
   ImageData2,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText1 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Greek Text",
   NULL
};

static struct Gadget g_greek = {
   NULL,
   25,45,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image1,
   (APTR)&Image2,
   &IText1,
   NULL,
   NULL,
   4,
   NULL
};

static USHORT ImageData3[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image3 = {
   0,0,
   13,7,
   2,
   ImageData3,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData4[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image4 = {
   0,0,
   13,7,
   2,
   ImageData4,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText2 = {
   3,0,JAM2,
   20,0,
   NULL,
   "PostScript",
   NULL
};

static struct Gadget g_postscript = {
   &g_greek,
   250,45,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image3,
   (APTR)&Image4,
   &IText2,
   NULL,
   NULL,
   13,
   NULL
};

static USHORT ImageData5[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image5 = {
   0,0,
   13,7,
   2,
   ImageData5,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData6[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image6 = {
   0,0,
   13,7,
   2,
   ImageData6,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText3 = {
   3,0,JAM2,
   20,0,
   NULL,
   "HP LaserJet+",
   NULL
};

static struct Gadget g_hplaserjet = {
   &g_postscript,
   250,35,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image5,
   (APTR)&Image6,
   &IText3,
   NULL,
   NULL,
   13,
   NULL
};

static USHORT ImageData7[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image7 = {
   0,0,
   13,7,
   2,
   ImageData7,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData8[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image8 = {
   0,0,
   13,7,
   2,
   ImageData8,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText4 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Amiga Preferences",
   NULL
};

static struct Gadget g_preferences = {
   &g_hplaserjet,
   250,25,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image7,
   (APTR)&Image8,
   &IText4,
   NULL,
   NULL,
   13,
   NULL
};

static USHORT ImageData9[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image9 = {
   0,0,
   13,7,
   2,
   ImageData9,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData10[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image10 = {
   0,0,
   13,7,
   2,
   ImageData10,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText5 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Strip Spaces",
   NULL
};

static struct Gadget g_stripspaces = {
   &g_preferences,
   25,121,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image9,
   (APTR)&Image10,
   &IText5,
   NULL,
   NULL,
   99,
   NULL
};

static USHORT ImageData11[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image11 = {
   0,0,
   13,7,
   2,
   ImageData11,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData12[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image12 = {
   0,0,
   13,7,
   2,
   ImageData12,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText6 = {
   3,0,JAM2,
   20,0,
   NULL,
   "ASCII (2 CR/Para)",
   NULL
};

static struct Gadget g_ascii2 = {
   &g_stripspaces,
   25,101,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image11,
   (APTR)&Image12,
   &IText6,
   NULL,
   NULL,
   12,
   NULL
};

static USHORT ImageData13[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image13 = {
   0,0,
   13,7,
   2,
   ImageData13,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData14[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image14 = {
   0,0,
   13,7,
   2,
   ImageData14,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText7 = {
   3,0,JAM2,
   20,0,
   NULL,
   "WordPerfect",
   NULL
};

static struct Gadget g_wordperfect = {
   &g_ascii2,
   25,71,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image13,
   (APTR)&Image14,
   &IText7,
   NULL,
   NULL,
   12,
   NULL
};

static USHORT ImageData15[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image15 = {
   0,0,
   13,7,
   2,
   ImageData15,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData16[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image16 = {
   0,0,
   13,7,
   2,
   ImageData16,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText8 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Notepad",
   NULL
};

static struct Gadget g_notepad = {
   &g_wordperfect,
   25,91,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image15,
   (APTR)&Image16,
   &IText8,
   NULL,
   NULL,
   12,
   NULL
};

static USHORT ImageData17[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image17 = {
   0,0,
   13,7,
   2,
   ImageData17,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData18[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image18 = {
   0,0,
   13,7,
   2,
   ImageData18,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText9 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Scribble!",
   NULL
};

static struct Gadget g_scribble = {
   &g_notepad,
   25,81,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image17,
   (APTR)&Image18,
   &IText9,
   NULL,
   NULL,
   12,
   NULL
};

static USHORT ImageData19[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image19 = {
   0,0,
   13,7,
   2,
   ImageData19,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData20[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image20 = {
   0,0,
   13,7,
   2,
   ImageData20,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText10 = {
   3,0,JAM2,
   20,0,
   NULL,
   "ASCII (1 CR/Para)",
   NULL
};

static struct Gadget g_ascii1 = {
   &g_scribble,
   25,111,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image19,
   (APTR)&Image20,
   &IText10,
   NULL,
   NULL,
   12,
   NULL
};

static USHORT ImageData21[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image21 = {
   0,0,
   13,7,
   2,
   ImageData21,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData22[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image22 = {
   0,0,
   13,7,
   2,
   ImageData22,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText11 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Hide Column Guides",
   NULL
};

static struct Gadget g_hide = {
   &g_ascii1,
   25,35,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image21,
   (APTR)&Image22,
   &IText11,
   NULL,
   NULL,
   3,
   NULL
};

static USHORT ImageData23[] = {
   0x1FC0,0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8008,0x8008,0x8008,0x6030,0x1FC0
};

static struct Image Image23 = {
   0,0,
   13,7,
   2,
   ImageData23,
   0x0003,0x0000,
   NULL
};

static USHORT ImageData24[] = {
   0x1FC0,0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0,0x1FC0,
   0x6030,0x8F88,0x9FC8,0x8F88,0x6030,0x1FC0
};

static struct Image Image24 = {
   0,0,
   13,7,
   2,
   ImageData24,
   0x0003,0x0000,
   NULL
};

static struct IntuiText IText12 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Interlaced",
   NULL
};

static struct Gadget g_interlace = {
   &g_hide,
   25,25,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image23,
   (APTR)&Image24,
   &IText12,
   NULL,
   NULL,
   2,
   NULL
};

#define GadgetList1 g_interlace

static struct IntuiText IText15 = {
   3,0,JAM2,
   260,14,
   NULL,
   "Printer Type",
   NULL
};

static struct IntuiText IText14 = {
   3,0,JAM2,
   14,60,
   NULL,
   "Imported Text Format",
   &IText15
};

static struct IntuiText IText13 = {
   3,0,JAM2,
   17,14,
   NULL,
   "Display Options",
   &IText14
};

#define IntuiTextList1 IText13

static struct NewWindow NewWindowStructure1 = {
   104,28,
   443,144,
   0,2,
   GADGETDOWN+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE,
   &g_interlace,
   NULL,
   "City Desk Preferences",
   NULL,
   NULL,
   5,5,
   640,200,
   CUSTOMSCREEN
};


/* end of PowerWindows source generation */
extern struct printerprefs printerprefs;
extern SHORT screensize;
extern SHORT std_height;      /** Size of non interlaced screen **/
extern SHORT displayguides;
extern SHORT greeking;
extern SHORT strip_spaces;
extern SHORT double_newlines;
extern SHORT scribble_style;
extern SHORT dot_commands;
extern SHORT wp_style;
extern SHORT ansi_escapes;
extern SHORT real_ascii;

struct Window *window;
struct Gadget *g;
struct IntuiMessage *message,*GetMsg();
SHORT newsize;
ULONG class;
USHORT code;

g_interlace.Flags &= (~SELECTED);
g_hide.Flags &= (~SELECTED);
g_greek.Flags &= (~SELECTED);
g_wordperfect.Flags &= (~SELECTED);
g_scribble.Flags &= (~SELECTED);
g_notepad.Flags &= (~SELECTED);
g_ascii1.Flags &= (~SELECTED);
g_ascii2.Flags &= (~SELECTED);
g_stripspaces.Flags &= (~SELECTED);
g_preferences.Flags &= (~SELECTED);
g_hplaserjet.Flags &= (~SELECTED);
g_postscript.Flags &= (~SELECTED);

if (screensize>std_height)
   g_interlace.Flags |= SELECTED;

if (!displayguides)
   g_hide.Flags |= SELECTED;

if (greeking)
   g_greek.Flags |= SELECTED;

if (wp_style)
   g_wordperfect.Flags |= SELECTED;

if (scribble_style)
   g_scribble.Flags |= SELECTED;

if (ansi_escapes)
   g_notepad.Flags |= SELECTED;

if (real_ascii && double_newlines)
   g_ascii2.Flags |= SELECTED;

if (real_ascii && !double_newlines)
   g_ascii1.Flags |= SELECTED;

if (strip_spaces)
   g_stripspaces.Flags |= SELECTED;

if (printerprefs.printertype==PREFERENCES)
   g_preferences.Flags |= SELECTED;

if (printerprefs.printertype==HPLASER)
   g_hplaserjet.Flags |= SELECTED;

if (printerprefs.printertype==POSTSCRIPT)
   g_postscript.Flags |= SELECTED;

NewWindowStructure1.Screen = screen;

window = OpenWindow(&NewWindowStructure1);
if (window==0)
   return(screensize);

PrintIText(window->RPort,&IntuiTextList1,0,0);

outline_window(window, BLACK);

while (TRUE) {

   Wait( 1<< (window->UserPort->mp_SigBit));
   while( (message=GetMsg(window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      g = message->IAddress;
      ReplyMsg(message);

      if (class == GADGETDOWN) {
         toggle_mutexc(window, g);
         }

      if (class == CLOSEWINDOW) {
/** Reset pointers & skip system CLOSE and DRAG gadgets **/
         NewWindowStructure1.FirstGadget=window->FirstGadget->NextGadget->NextGadget;
         CloseWindow(window);

         if (g_interlace.Flags & SELECTED)
            newsize = std_height*2;
         else
            newsize = std_height;

         if (g_hide.Flags & SELECTED)
            displayguides = FALSE;
         else
            displayguides = TRUE;

         if (g_greek.Flags & SELECTED)
            greeking = TRUE;
         else
            greeking = FALSE;

         if (g_wordperfect.Flags & SELECTED)
            setwp();

         if (g_scribble.Flags & SELECTED)
            setscribble();

         if (g_notepad.Flags & SELECTED)
            setansi();

         if (g_ascii1.Flags & SELECTED)
            setascii1();

         if (g_ascii2.Flags & SELECTED)
            setascii2();

         if (g_stripspaces.Flags & SELECTED)
            strip_spaces = TRUE;
         else
            strip_spaces = FALSE;

         if (g_preferences.Flags & SELECTED)
            printerprefs.printertype = PREFERENCES;

         if (g_hplaserjet.Flags & SELECTED)
            printerprefs.printertype = HPLASER;

         if (g_postscript.Flags & SELECTED)
            printerprefs.printertype = POSTSCRIPT;

         return(newsize);
         }
      }
   }
}

/**************************************************************************/
/*** mutual exclude gadgets. Given window and gadget, reset all other   ***/
/*** gadgets with same GadgetID as gadget passed.  Select gadget passed  **/
/**************************************************************************/


toggle_mutexc(w,setg)

struct Window *w;
struct Gadget *setg;

{
USHORT id;
struct Gadget *g;

id=setg->GadgetID;

for (g=w->FirstGadget; g!=NULL; g=g->NextGadget) {

   if ( (g->GadgetType&BOOLGADGET) && (id==g->GadgetID) &&
       (g->Flags&SELECTED) && g!=setg) {

      RefreshGList(g,w,0,1);
      RemoveGadget(w,g);
      g->Flags= (g->Flags&(~SELECTED));
      AddGadget(w,g,-1);
      RefreshGList(g,w,0,1);
      }
   }
}

