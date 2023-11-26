#include <exec/types.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "text.h"

struct NewWindow NewWindowStructure = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,10,   /* window width and height */
   0,1,   /* detail and block pens */
   MENUPICK,   /* IDCMP flags */
   WINDOWDRAG+WINDOWDEPTH+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "CD Print 1.1  © 1987 MicroSearch.  Written By SunRize Industries",
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   493,10,   /* minimum width and height */
   493,10,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};

/* end of PowerWindows source generation */


char stTopaz[] = "topaz.font";

struct TextAttr taPlain =
{
   stTopaz, 8, FS_NORMAL, FPF_ROMFONT
};

struct TextAttr taBIU =
{
   stTopaz, 8, FSF_BOLD | FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taBU =
{
   stTopaz, 8, FSF_BOLD | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taBI =
{
   stTopaz, 8, FSF_BOLD | FSF_ITALIC, FPF_ROMFONT
};

struct TextAttr taB =
{
   stTopaz, 8, FSF_BOLD, FPF_ROMFONT
};

struct TextAttr taIU =
{
   stTopaz, 8, FSF_ITALIC | FSF_UNDERLINED, FPF_ROMFONT
};

struct TextAttr taI =
{
   stTopaz, 8, FSF_ITALIC, FPF_ROMFONT
};

struct TextAttr taU =
{
   stTopaz, 8, FSF_UNDERLINED, FPF_ROMFONT
};

struct IntuiText ItemText[] =
{
   { 0, 1, JAM1, 0, 1, &taPlain, "Open Project      ", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Open Preferences", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Print Project", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Print Page", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Print Options", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "", NULL },
   { 0, 1, JAM1, 0, 1, &taPlain, "Quit", NULL },
};

struct MenuItem Items[] =
{
   {
      &Items[1], 0, 0, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[0], NULL, 'S', NULL, NULL 
   },
   {
      &Items[2], 0, 10, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[1], NULL, 'U', NULL, NULL 
   },
   {
      &Items[3], 0, 20, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[2], NULL, 'N', NULL, NULL 
   },
   {
      &Items[4], 0, 30, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[3], NULL, 'R', NULL, NULL 
   },
   {
      &Items[5], 0, 40, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[4], NULL, 'I', NULL, NULL 
   },
   {
      &Items[6], 0, 50, 187, 10, ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[5], NULL, 'Z', NULL, NULL 
   },
   {
      NULL, 0, 60, 187, 10, ITEMENABLED | ITEMTEXT | COMMSEQ | HIGHCOMP,
      0, (APTR)&ItemText[6], NULL, 'E', NULL, NULL 
   },
};

struct Menu Titles[] =
{
   { NULL, 3, 0, 78, 10, MENUENABLED, "Project ", &Items[0] },
};

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *DiskfontBase;

COORD columnwidth;
COORD gridsize;    /* 1/2 inch */
COORD outlinespc;  /* space page outline is offseted from margins **/
SHORT snaptogrid;
SHORT displaygrid;
SHORT displayguides;
SHORT texterrors;        /* TRUE means display text errors */
SHORT screensize;        /* non interlaced */
SHORT strip_spaces;      /* strip multiple space on load */
SHORT double_newlines;   /* 2 CR marks para end, else use 1 CR **/
SHORT dot_commands;      /* interpret scribble dot commands **/
SHORT scribble_style;    /* interpret scribble embeded commands */
SHORT snaptotop;         /* start flowing at top */
SHORT autoflow;          /* autoflow to next column */
SHORT ansi_escapes;      /* Notepage style text load */
SHORT real_ascii;        /* filter nonpritable ascii */
SHORT flow_warnings;     /* Warn when not finished flowing */
SHORT usedefault;        /* Reflow uses default, not same */
SHORT time_for_same;     /* Set when we want flow to use same */
SHORT do_trans = TRUE;   /* turns on and off translation */
UBYTE *translate;        /* points to translation table */
SHORT wp_style;          /* Input Text is from word perfect. */
char default_font_name[40];   /* Just what it says. */

struct text_state defaultstate;
struct text_state samestate;     /* set by reflowchain, sometimes */
struct pageinfo   defaultpage;
struct rule       defaultrule;
struct Window     *clipboard;
struct laserfont  *firstlaserfont;
struct printerprefs printerprefs;
struct pageinfo   page[MAXPAGES];
struct Window *canwin;   /** window used to display cancel msg on print **/


ULONG xpix;
ULONG ypix;
UWORD xinch,yinch;
FIXED cpertextpixx,cpertextpixy;
char printername[60];
char *pname;


void main()
{
struct IntuiMessage *message,*GetMsg();
struct Window *w;
struct Screen *s;
ULONG class;
USHORT code;
SHORT menunum,itemnum;
int i,j;
int pg=1;
char buffer[80];

firstlaserfont=0; /* no laser fonts loaded */
canwin=0;         /* no cancel window open yet */
IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library",0);

if (IntuitionBase == 0) {
   printf("Can't open intuition.\n");
   exit(10);
   }

GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase == 0) {
   printf("Can't open graphics lib.\n");
   CloseLibrary(IntuitionBase);
   exit(10);
   }

DiskfontBase = (struct Library *)OpenLibrary("diskfont.library", 0);
if (DiskfontBase==NULL) {
   printf("Can't open diskfonts lib.\n");
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   exit(10);
   }


w = (struct Window *)OpenWindow(&NewWindowStructure);
if (w == 0) {
   printf("Can't Open a window!!\n");
   Delay(100);
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   CloseLibrary(DiskfontBase);
   exit (10);
   }

clipboard = w;    /** for those telluser's that expect this set **/
s = w->WScreen;   /** Get pointer to this screen **/
init_fonts();          /** initialize the available fonts chain **/

/** set up the default printer options **/

initdefaultstate();
printerprefs.printertype=PREFERENCES;
loadfonts(s);         /** find out what preferences printer is **/
cpertextpixx=(576<<16)/xinch;
cpertextpixy=(576<<16)/yinch;

/** initilize all pages **/

for (i=0;i<MAXPAGES;i++) {
   NewList(&page[i].objects.lh_Head);
   page[i].view=NULL;
   page[i].pn=i;              /* set page number */
   page[i].flags = 0;
   }


SetMenuStrip(w,Titles);

while (TRUE) {

   while((message=(struct IntuiMessage *)GetMsg(w->UserPort))==0) {
      Wait(1<<w->UserPort->mp_SigBit);
      }

   class=message->Class;
   code=message->Code;
   ReplyMsg(message);

   if (class != MENUPICK)
      continue;            /** get new event **/

   menunum=MENUNUM(code);
   itemnum=ITEMNUM(code);

   if (menunum!=0)
      continue;         /** This should never happen **/

   switch(itemnum) {

      case 0:  /** open project **/
         i=loadall();
         newpointer(s,NORMAL);
         if (i!=AOK) {
            showstat(s,i);
            }

         break;

      case 1:  /** open preferences **/
         showstat(s,loadpreffile());
         printerprefs.printertype=PREFERENCES;
         loadfonts(s);   /** open the default font **/
         break;

      case 2:  /** print project **/

         if (printerprefs.printertype != PREFERENCES) {
            telluser2(s,"This Utility Only Prints",
              "to Preferences Printers",FALSE);
            break;
            }

         if (!printerprefs.flags&USE_FILE) {
            if (!telluser2(s,"Print Document..."
               ,"Click OK when your printer is ready.",TRUE))
               break;
               }
         if ((page[1].flags&PAGEINUSE)==0) {
            telluser2(s,"You Must Load a City Desk",
               "Project Before You Print!",FALSE);
            break;
            }

         i=laserpage(&page[1]);
         showstat(s,i);
         if (i!=AOK)
            break;
       
          for (j=2; j < MAXPAGES; j++) {
            if (page[j].flags&PAGEINUSE) {
               i=laserpage(&page[j]);
               if (i!=AOK) {
                  showstat(s,i);
                  break;
                  }
               }
            } 
         break;

      case 3:  /** print page **/

         if (printerprefs.printertype != PREFERENCES) {
            telluser2(s,"This Utility Only Prints",
              "to Preferences Printers",FALSE);
            break;
            }

         pg=getint(s,pg,1,MAXPAGES-1,"Select Page to Print:",275);
         if (pg < 1 || pg > 99) {
            telluser2(s,"Illegal page number!","",FALSE);
            break;
            }
         if ((page[pg].flags&PAGEINUSE)==0) {
            telluser2(s,"This Page is Empty!","",FALSE);
            break;
            }

         if (!printerprefs.flags&USE_FILE) {
            sprintf(buffer,"Print page %d.",pg);
            if (!telluser2(s,buffer,
                "Click OK when your printer is ready.",TRUE))
               break;
            }

         i=laserpage(&page[pg],&printerprefs,FALSE);
         showstat(s,i);
         break;

      case 4:  /** print options **/
         break;

      case 6:  /** quit **/
         if (telluser2(s,"Are You Sure","You Want To Quit?",TRUE)) {
         for (i=MAXPAGES-1; i >= 0; i--) {   /* delete all objects and windows */
            delete_page(i);
            }
            ClearMenuStrip(w);
            CloseWindow(w);
            close_fonts();
            CloseLibrary(IntuitionBase);
            CloseLibrary(GfxBase);
            CloseLibrary(DiskfontBase);
            exit(0);
            }
         break;

      } /* end switch */
   }  /* end while */
}  /* end main */

