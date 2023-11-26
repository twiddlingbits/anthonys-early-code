#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"

#include "main.pwc"

/**********************************/
/*** Test program for gadlib.c ****/
/**********************************/

struct Screen *screen;
struct Window *window;

void setup();
void setdown();
void openlibraries();
void closelibraries();
void num_func();
void butn_func();
void list_func();
void debug_print_gadgets();

#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,LIGHT_BROWN,WHITE,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BLUE,BLACK,BLACK}
#define DROPCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,WHITE,BLACK}

static struct GadgetColors colors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	DROPCOLORS};

void main()
{
static struct NewSlider sample_slider1= {
    NULL,20,20,60,150,SLIDER_COOL|AUTO_KNOB|FREE_VERT,num_func,
    0,&colors,0,0,5,1,0,-1,-1};

static struct NewSlider sample_slider2= {
    &sample_slider1,70,20,250,40,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,&colors,0,0,10,1,5,40,-1};

static struct NewButton sample_button1 = {
   &sample_slider2,260,50,320,65,BUTN_COOL|BUTN_TOGGLE,0,0,&colors,
	0,0,"Hit Me"};

static struct NewButton sample_button2 = {
   &sample_button1,260,70,320,85,BUTN_COOL|MANUAL_XOR|BUTN_HIT|BUTN_TOGGLE,butn_func,
	0,&colors,0,0,"Toggle"};

static struct NewButton sample_button4 = {
	&sample_button2,300,100,320,108,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE|BUTN_HIT,
	0,0,&colors,1,0,"Butn4"};

static struct NewButton sample_button5 = {
   &sample_button4,300,120,320,128,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,&colors,1,0,"Butn5"};

static struct NewButton sample_button6 = {
   &sample_button5,300,140,320,148,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,&colors,1,0,"Butn6"};

static char *entries[] = {"100","200","300","400","500","600",""};
static unsigned long eud[] = {100,200,300,400,500,600};

static struct NewList sample_droplist = {
   &sample_button6,300,30,380,80,DROPLIST_COOL,list_func,
	0,&colors,0,0, entries, eud};

static struct NewList sample_list = {
   &sample_droplist,70,50,250,150,LIST_COOL|HIDE_GADGET,list_func,
	0,&colors,0,0, entries, eud};

struct IntuiMessage *message,*get_all_msgs(),*GetMsg();
struct list *l1;
unsigned long class;
int i;
char buf[60];

setup();       /* open window, screen, libraries */
init_gadlib(); /*!!!MUST CALL THIS BEFORE USING GADGETS!!!*/

/** create all gadget linked together with NewGadgetHeader->next **/

if (create_gadget_chain(window,&sample_list)==0) {
   printf("can't create gadgets!\n");
   rem_gadlib();
   setdown();
   exit(0);
   }

l1=(struct list *)sample_list.gad_ptr;	/** filled in by create_gadget_chain **/

/*debug_print_gadgets(); */ /* scan the gadget list for fun */

if (l1) {
	for (i=0; i < 25; i++) {
	   sprintf(buf,"ENTRY # %d",i);
	   add_list_entry(l1, buf, i);  
	   }
	show_gadget(l1);
	refresh_list(l1);
	}

telluser("GadLib Test V 1.0","By Anthony J. Wood");

SetAPen(window->RPort, BROWN);
RectFill(window->RPort,2,11,window->Width-2,window->Height-2);
Delay(60);
refresh_gadget_chain(window);

while (TRUE) {
   Wait(1<<window->UserPort->mp_SigBit); 
   if ((message=(struct IntuiMessage *)GetMsg(window->UserPort))!=0) {
      class=message->Class;
      ReplyMsg(message);

      if (class == CLOSEWINDOW) {
         while ((message=GetMsg(window->UserPort))!=0)
            ReplyMsg(message);
         delete_gadget_chain(window);  /** delete all gadgets in this window */
	for (i=0; i < 500000; i++) ;
         rem_gadlib();
         setdown();
         exit();
         }
      }
   }
}

/**************************************************************/
#if 0

void debug_print_gadgets()
{
extern struct MinList gad_list; /* in gadlib.c */
struct MinNode *node;

for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ)
   printf("GADGET flags %x\n",((struct GadgetHeader *)node)->flags);
}

#endif
/**************************************************************/

void list_func(l)    /** USE -v when compiling!!! **/
struct list *l;
{
char buf[40];

if (l->flags&GAD_DROPLIST) {
	l = ((struct droplist *)l)->l;
	}

if (l->flags&LIST_DOUBLESELECT) {
   sprintf(buf,"user_data is %d",l->active_entry->user_data);
   telluser("doubleselect list_func:",buf);
   }

#if 0
if (l->flags&LIST_NEWSELECT && l->active_entry->user_data > 100) {
	if (l->current!=-1) {
	   sprintf(buf,"user_data is %d",l->active_entry->user_data);
	   telluser("select list_func:",buf);
		}
   }
#endif
}

/**************************************************************/

void num_func(k)     /** use -v when compiling !!! **/
struct slider *k;
{
char buf[20];

sprintf(buf,"%2d",k->top_element);
SetAPen(k->kw->RPort, WHITE);
SetBPen(k->kw->RPort, LIGHT_BROWN);
SetDrMd(k->kw->RPort, JAM2);
Move(k->kw->RPort, k->knob_x+3, k->knob_y+7);
if (k->knob_width <=20)
   Text(k->kw->RPort, buf+1, 1);
else
   Text(k->kw->RPort, buf, 2);

}

/**************************************************************/

void butn_func(b)    /** USE -v when compiling **/
struct button *b;
{
if (b->flags&BUTN_HIT)
   telluser("button hit!","");
else
   telluser("button not hit!","");
}

/**************************************************************/

void setup()
{
void setup_defaults();
void setdown();
char *AllocMem();
struct Screen *OpenScreen();
struct Window *OpenWindow();

openlibraries();

screen=OpenScreen(&NewScreenStructure);
if (screen==0) {
   printf("can't open screen");
   setdown();
   exit(10);
   }

LoadRGB4(&screen->ViewPort,Palette,PaletteColorCount);

NewWindow.Screen = screen;

window = (struct Window *)QuickWindow (&colors, 200, 15, 600, 190,
	CLOSEWINDOW,WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,screen, "AW's Test Window");

if (window==0) {
   printf("can't open back window\n");
   setdown();
   exit(10);
   }
/* printf("w %d h %d\n",screen->Width, screen->Height); */
}

void setdown()
{
if (window)
   CloseWindow(window);
if (screen)
   CloseScreen(screen);
closelibraries();
}

/** do not use -b0 when compiling this module **/

struct GfxBase near *GfxBase;
struct ArpBase near *ArpBase;
struct IntuitionBase near *IntuitionBase;
struct Library near *LayersBase;
void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;

/* open intuition library */

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==NULL) {
   printf("Use Workbench 1.2 or greater.");
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!");
   closelibraries();
   exit(10);
   }

LayersBase = (struct Library *)OpenLibrary("layers.library",33L);
if (LayersBase==NULL) {
	printf("can't open layers library!\n");
	closelibraries();
	exit(10);
	}
}

void closelibraries()
{
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);

if (LayersBase)
	CloseLibrary(LayersBase);
}

