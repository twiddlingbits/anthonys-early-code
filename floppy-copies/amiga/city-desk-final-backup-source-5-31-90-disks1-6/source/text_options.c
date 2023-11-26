/** USE -ad when compiling this module **/

#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"
#include "text.h"

static int frac[]={0,125,25,375,5,625,75,875};
extern struct text_state defaultstate;
extern char default_cmd_list[256];
extern UWORD xinch, yinch;

newtextformat(screen,state)

struct Screen *screen;
struct text_state *state;

{
static UBYTE UNDOBUFFER[160];

static SHORT BorderVectors1[] = {
   0,0,
   103,0,
   103,9,
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

static struct IntuiText IText1 = {
   3,0,JAM2,
   1,0,
   NULL,
   "Change Font:",
   NULL
};

static struct Gadget new_font = {
   NULL,
   20,66,
   100,8,
   NULL,
   RELVERIFY,
   BOOLGADGET,
   (APTR)&Border1,
   NULL,
   &IText1,
   NULL,
   NULL,
   98,
   NULL
};

static UBYTE cmd_stringSIBuff[160];
static struct StringInfo cmd_stringSInfo = {
   cmd_stringSIBuff,
   UNDOBUFFER,
   0,
   160,
   0,
   0,0,0,0,0,
   0,
   0,
   NULL
};

static SHORT BorderVectors2[] = {
   0,0,
   311,0,
   311,9,
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

static struct IntuiText IText2 = {
   3,0,JAM2,
   85,-11,
   NULL,
   "Command Sequence:",
   NULL
};

static struct Gadget cmd_string = {
   &new_font,
   11,91,
   308,8,
   NULL,
   RELVERIFY,
   STRGADGET,
   (APTR)&Border2,
   NULL,
   &IText2,
   NULL,
   (APTR)&cmd_stringSInfo,
   99,
   NULL
};

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

static struct IntuiText IText3 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Underlined",
   NULL
};

static struct Gadget underlined = {
   &cmd_string,
   180,50,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image1,
   (APTR)&Image2,
   &IText3,
   NULL,
   NULL,
   8,
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

static struct IntuiText IText4 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Italic",
   NULL
};

static struct Gadget italic = {
   &underlined,
   180,40,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image3,
   (APTR)&Image4,
   &IText4,
   NULL,
   NULL,
   7,
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

static struct IntuiText IText5 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Flush Right",
   NULL
};

static struct Gadget right = {
   &italic,
   20,30,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image5,
   (APTR)&Image6,
   &IText5,
   NULL,
   NULL,
   2,
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

static struct IntuiText IText6 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Compressed",
   NULL
};

static struct Gadget compressed = {
   &right,
   180,20,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image7,
   (APTR)&Image8,
   &IText6,
   NULL,
   NULL,
   3,
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

static struct IntuiText IText7 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Filled",
   NULL
};

static struct Gadget fill = {
   &compressed,
   20,50,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image9,
   (APTR)&Image10,
   &IText7,
   NULL,
   NULL,
   2,
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

static struct IntuiText IText8 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Centered",
   NULL
};

static struct Gadget centered = {
   &fill,
   20,40,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image11,
   (APTR)&Image12,
   &IText8,
   NULL,
   NULL,
   2,
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

static struct IntuiText IText9 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Bold",
   NULL
};

static struct Gadget bold = {
   &centered,
   180,30,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image13,
   (APTR)&Image14,
   &IText9,
   NULL,
   NULL,
   4,
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

static struct IntuiText IText10 = {
   3,0,JAM2,
   20,0,
   NULL,
   "Flush Left",
   NULL
};

static struct Gadget left = {
   &bold,
   20,20,
   13,7,
   GADGHIMAGE+GADGIMAGE,
   GADGIMMEDIATE+TOGGLESELECT,
   BOOLGADGET,
   (APTR)&Image15,
   (APTR)&Image16,
   &IText10,
   NULL,
   NULL,
   2,
   NULL
};

#define GadgetList1 left

static struct NewWindow NewWindowStructure1 = {
   146,42,
   328,106,
   0,2,
   GADGETUP+GADGETDOWN+CLOSEWINDOW,
   WINDOWDRAG+WINDOWCLOSE+ACTIVATE,
   &left,
   NULL,
   "Text Object Options",
   NULL,
   NULL,
   5,5,
   640,200,
   CUSTOMSCREEN
};

struct Window *w;
ULONG class;
struct Gadget *address;
struct IntuiMessage *message,*GetMsg();
COORD fsize;
short can;
/* struct text_state oldstate; */     /** remember in case of cancel **/
char fontname[60], fontsize[20];


/* oldstate=*state; */

/** Currently the cmd_string is not supported on any object except **/
/** the default object. This hides string when not needed **/

if (state==&defaultstate) {
   underlined.NextGadget = &cmd_string;
   }
else {
   underlined.NextGadget = &new_font;
   }

/*** set attribute gadgets to correct state ***/

if (state->style & ITALIC) italic.Flags |= SELECTED;
else italic.Flags &= (~SELECTED);

if (state->style & BOLD) bold.Flags |= SELECTED;
else bold.Flags &= (~SELECTED);

if (state->style & UNDERLINE) underlined.Flags |= SELECTED;
else underlined.Flags &= (~SELECTED);

if (state->style & COMPRESS) compressed.Flags |= SELECTED;
else compressed.Flags &= (~SELECTED);


/*** set justification gadgets to correct state ***/


if (!(state->flags & TEXTS_RAGLEFT) && state->flags & TEXTS_RAGRIGHT)
   left.Flags |= SELECTED;
else left.Flags &= (~SELECTED);

if (!(state->flags & TEXTS_RAGRIGHT) && state->flags & TEXTS_RAGLEFT)
   right.Flags |= SELECTED;
else right.Flags &= (~SELECTED);

if ((state->flags & TEXTS_RAGLEFT) && (state->flags & TEXTS_RAGRIGHT))
    centered.Flags |= SELECTED;
else centered.Flags &= (~SELECTED);

if (((state->flags & TEXTS_RAGLEFT)==0) && ((state->flags & TEXTS_RAGRIGHT)==0))
    fill.Flags |= SELECTED;
else fill.Flags &= (~SELECTED);

if (state->lfont)    /** get fontname and size **/
   strcpy(fontname, state->lfont->fontname);
else
   strcpy(fontname, state->font->tf_Message.mn_Node.ln_Name);

sprintf(fontsize,"%d.%dpt",state->font_size>>3,frac[state->font_size&7]);

if (state==&defaultstate) {
   strcpy(cmd_stringSIBuff, default_cmd_list);
   initdefaultstate2();      /** clear state out **/
   }
else {
   cmd_stringSIBuff[0]=0;
   }

/**** now open the window and display window text ***/

NewWindowStructure1.Screen=screen;

w=(struct Window *)OpenWindow(&NewWindowStructure1);
if (w==NULL) return(OUTOFMEM);

show_font_data(w, fontname, fontsize);

loop:

Wait(1<<w->UserPort->mp_SigBit);
while ((message=GetMsg(w->UserPort))!=NULL) {
   class=message->Class;
   address=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   if (class == GADGETDOWN) {
      toggle_mutexc(w, address);
      }

   if (class==GADGETUP && address==&new_font) {
      fsize = convert_string(fontsize,NUM_COORD,NUM_DEF_POINT);
      can=selectfont(screen,fontname,&fsize);
      if (can != AOK)
         goto loop;
      sprintf(fontsize,"%d.%dpt",fsize>>3,frac[fsize&7]);
      show_font_data(w, fontname, fontsize);
      goto loop;
      }

   if (class == CLOSEWINDOW) {

      NewWindowStructure1.FirstGadget=w->FirstGadget->NextGadget->NextGadget;
      CloseWindow(w);

      state->flags &=(~(TEXTS_RAGRIGHT | TEXTS_RAGLEFT));
      if (left.Flags & SELECTED)  state->flags |= TEXTS_RAGRIGHT;
      if (right.Flags & SELECTED) state->flags |= TEXTS_RAGLEFT;
      if (centered.Flags & SELECTED)
         state->flags |=(TEXTS_RAGLEFT | TEXTS_RAGRIGHT);

      state->style=0;
      if (bold.Flags & SELECTED)  state->style=BOLD;
      if (italic.Flags & SELECTED)  state->style |= ITALIC;
      if (underlined.Flags & SELECTED)  state->style |= UNDERLINE;
      if (compressed.Flags & SELECTED)  state->style |= COMPRESS;

      state->font_size = convert_string(fontsize,NUM_COORD,NUM_DEF_POINT);

      my_open_font(fontname, state, NULL);
      if (state==&defaultstate) {
         set_default_state(state, cmd_stringSIBuff);
         strcpy(default_cmd_list, cmd_stringSIBuff);
         }
      return(AOK);
      }

   }
goto loop;
}

show_font_data(w, fontname, fontsize)

char *fontname, *fontsize;
struct Window *w;

{
SetAPen(w->RPort, WHITE);
RectFill(w->RPort,132,66,325,76);
SetAPen(w->RPort, BLACK);
Move(w->RPort, 132, 72);
Text(w->RPort, fontname, strlen(fontname));
Move(w->RPort,270,72);
Text(w->RPort,fontsize,strlen(fontsize));
}

convert_string(s,units,def_units)

char *s;
int units;
int def_units;
{
char *cp;
FIXED i;

if (s[0]==' ')
   cp=s+1;
else
   cp=s;
/* i = get_num(&cp , NUM_DEF_POINT); */
i = get_fixed_pt(&cp, units, def_units);
i = i>>16;
return(i);
}

/*** takes a FIXED number (32 bit with 16 bit fraction) and
     returns the fration part as an integer number of thousandths */


fixedfrac(x)
FIXED x;
{
int i;
i=(x&0xFFFF)*1000;
i=i/0x10000;
return(i);
}

#if 0
stringtofixed(s)
char *s;
{
char *cp;
cp=s;
return(get_fixed(&cp));
}
#endif

/* int i;   the yucky way
float r,f;
FIXED fix;

sscanf("%f",&f);
i=(int)f;
fix=i<<16;
r=(f-(float)i) * (float)0x10000;
fix=fix|((int)r);
return(fix);
}  */

/**************************************************************************/
/*** mutual exclude gadgets. Given window and gadget, reset all other   ***/
/*** gadgets with same GadgetID as gadget passed.  Select gadget passed  **/
/**************************************************************************/


mutexc(w,setg)

struct Window *w;
struct Gadget *setg;

{
USHORT id;
struct Gadget *g;

if (setg->Flags&SELECTED)   /* don't re-select current gadget */
   return(0);

id=setg->GadgetID;

RemoveGadget(w,setg);
setg->Flags= setg->Flags | SELECTED;
AddGadget(w,setg,-1);
RefreshGList(setg,w,0,1);

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

