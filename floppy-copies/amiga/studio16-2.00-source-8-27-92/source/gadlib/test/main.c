#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "//include/studio16.h"
#include "//include/gadlib.h"

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
void kn_func();

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
    0,&colors,0,0,0,5,1,0,-1,-1};

static struct NewSlider sample_slider2= {
    &sample_slider1,70,20,250,40,SLIDER_COOL|FREE_HORIZ|SLIDER_ARROWS|MANUAL_XOR,
    num_func,0,&colors,0,0,0,10,1,5,40,-1};

static struct NewButton sample_button1 = {
   &sample_slider2,260,50,333,64,BUTN_COOL|BUTN_LOOP,0,0,&colors,
	0,0,0,"Looper"};

static struct NewButton sample_button2 = {
   &sample_button1,260,70,320,85,BUTN_COOL|MANUAL_XOR|BUTN_HIT|BUTN_TOGGLE,butn_func,
	0,&colors,0,0,0,"XToggle"};

static struct NewButton sample_box = {
   &sample_button2,260,87,340,99,BOX_COOL,0,0,&colors,0,0,0,0};

static struct NewButton sample_button4 = {
	&sample_box,300,100,320,108,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE|BUTN_HIT,
	0,0,&colors,1,0,0,"Butn4"};

static struct NewButton sample_button5 = {
   &sample_button4,300,120,320,128,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,&colors,1,0,0,"Butn5"};

static struct NewButton sample_button6 = {
   &sample_button5,300,140,320,148,BUTN_COOL|BUTN_TOGGLE|BUTN_X|MANUAL_XOR|KEEP_ONE_ACTIVE,
	0,0,&colors,1,0,0,"Butn6"};

static char *entries[] = {"100","200","300","400","500","600",""};
static unsigned long eud[] = {100,200,300,400,500,600};

static struct NewLoopList sample_looplist = {
   &sample_button6,300,15,380,27,LOOPLIST_COOL,0,
	0,&colors,0,0,0, entries, eud,4};

static struct NewDropList sample_droplist = {
   &sample_looplist,300,30,380,80,DROPLIST_COOL,list_func,
	0,&colors,0,0,0, entries, eud,4};

static struct NewList sample_list = {
   &sample_droplist,70,50,250,150,LIST_COOL|HIDE_GADGET|LIST_SINGLESELECT|LIST_USEKEYS|LIST_DRAGABLE,list_func,
	0,&colors,0,0,0, entries, eud,1};

static struct NewKeyNotice new_key = {NULL,kn_func,0};

struct IntuiMessage *message,*get_all_msgs(),*GetMsg();
struct list *l1;
unsigned long class;
int i;
char buf[60];
struct MsgPort *testport;
struct GadMsg *msg;
struct key_notice *kn;

setup();       /* open window, screen, libraries */
testport = (struct MsgPort *)CreatePort(0,0);
sample_droplist.notifyme=testport;
sample_list.notifyme=testport;
sample_box.notifyme=testport;

/** create all gadget linked together with NewGadgetHeader->next **/

if (create_gadget_chain(window,&sample_list)==0) {
   printf("can't create gadgets!\n");
   setdown();
   exit(0);
   }

new_key.w=window;
kn=(struct key_notice *)create_key_notice(&new_key);

l1=(struct list *)sample_list.gad_ptr;	/** filled in by create_gadget_chain **/

/*debug_print_gadgets(); */ /* scan the gadget list for fun */
if (l1) {
/*#if 0*/
	for (i=0; i < 75; i++) {
	   sprintf(buf,"THIS IS ENTRY # %d",i);
	   add_list_entry(l1, buf, i);  
	   }
/*#endif*/
	show_gadget(l1);
	refresh_list(l1);
	}
/*printf("l1->active_entry %x name %s current %x\n",l1->active_entry,l1->active_entry ?  l1->active_entry->text : "NULL",l1->current);*/
printf("ASK %x\n",askuser("GadLib Test V 1.0","By Anthony J. Wood",0));

/*SetAPen(window->RPort, BROWN);
RectFill(window->RPort,2,11,window->Width-2,window->Height-2);
Delay(60);
refresh_gadget_chain(window);*/


/** Scroll to last entry **/
/*scroll_to_show(l1, l1->list_size-1);*/
highlight_list_entry(l1, get_list_entry_ptr(l1,l1->list_size-1),0);
telluser("Gonna put back!",0,0);
unhighlight_list_entry(l1, get_list_entry_ptr(l1,l1->list_size-1));
while (TRUE) {
#if 0
printf("start\n");
	for (i=0; i < 200000; i++) {
		LockGadLibRender(); 
		Move(window->RPort, 50, 50);
		Draw(window->RPort, 60, 60);
		UnLockGadLibRender();
		}
printf("end\n");
#endif

   Wait((1<<window->UserPort->mp_SigBit) | (1<<testport->mp_SigBit)); 
	
	while (msg=(struct GadMsg *)GetMsg(testport)) {
		printf("got msg! g %x type %d\n",msg->g,msg->type);
		switch (msg->type) {
			case GADMSG_DRAG_AND_DROP:
				if (msg->text)
					kprintf("text is '%s'\n",msg->text);
				if (msg->le)
					kprintf("dropped on '%s'\n",msg->le->text);
				if (msg->g==sample_box.gad_ptr) {
					set_box_text(msg->g,msg->text);
					refresh_gadget(msg->g);
					}
				break;
			case GADMSG_ACTIVE:
				break;
			}
		ReplyMsg(msg);
		}

   if ((message=(struct IntuiMessage *)GetMsg(window->UserPort))!=0) {
      class=message->Class;
      ReplyMsg(message);

      if (class == CLOSEWINDOW) {
			delete_key_notice(kn);
         while ((message=GetMsg(window->UserPort))!=0)
            ReplyMsg(message);
			
			sample_droplist.notifyme=NULL;
         delete_gadget_chain(window);  /** delete all gadgets in this window */
			while (msg=(struct GadMsg *)GetMsg(testport)) {
				printf("Close: got msg!g %x type %d\n",msg->g,msg->type);
				ReplyMsg(msg);
				}
			DeletePort(testport); 
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
static char buf[40];

if (l->flags&GAD_DROPLIST) {
	l = ((struct droplist *)l)->l;
	}

if (l->flags&LIST_DOUBLESELECT) {
   sprintf(buf,"user_data is %d",l->active_entry->user_data);
   telluser("doubleselect list_func:",buf,NOTICE_RETURN_NOW);
   }

#if 0
if (l->flags&LIST_NEWSELECT && l->active_entry->user_data > 100) {
	if (l->current!=-1) {
	   sprintf(buf,"user_data is %d",l->active_entry->user_data);
	   telluser("select list_func:",buf,NOTICE_RETURN_NOW);
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
static count=0;
char temp[50];

sprintf(temp,"Hit %d",++count);
set_button_text(b, temp);
refresh_gadget(b);

#if 0
if (b->flags&BUTN_HIT)
   telluser("button hit!","",NOTICE_RETURN_NOW);
else
   telluser("button not hit!","",NOTICE_RETURN_NOW);
#endif
}

/**************************************************************/

void kn_func(kn,code,qualifier)
unsigned short code;
unsigned short qualifier;
struct new_key_notice *kn;
{
char bug[80];

sprintf(bug,"key code %x qualifier %x\n",code,qualifier);
kprintf(bug);
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

struct GfxBase *GfxBase;
struct ArpBase *ArpBase;
struct IntuitionBase *IntuitionBase;
struct Library *LayersBase;
struct Library *GadLibBase;

void openlibraries()

{
void closelibraries();
IntuitionBase = 0;
GfxBase = 0;
LayersBase=0;
GadLibBase=0;

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

GadLibBase = (struct Library *)OpenLibrary("gadlib.library",0);
if (GadLibBase==NULL) {
	printf("can't open gadget library!\n");
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

if (GadLibBase)
	CloseLibrary(GadLibBase);
}

