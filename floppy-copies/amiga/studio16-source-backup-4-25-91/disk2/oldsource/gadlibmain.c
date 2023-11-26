#include "exec/types.h"
#include "exec/memory.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"

#include "setup.pwc"

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


void main()
{
static struct NewKnob sample_knob1= {
    NULL,20,20,60,150,GAD_KNOB|AUTO_KNOB|FREE_VERT|KNOB_3D|BORDER_3D,num_func,
    0,LIGHT_BROWN,BROWN,LIGHT_BROWN,5,1,0,-1,-1};

static struct NewKnob sample_knob2= {
    &sample_knob1,70,20,250,40,GAD_KNOB|KNOB_3D|BORDER_3D|FREE_HORIZ|KNOB_ARROWS,
    num_func,0,LIGHT_BROWN,BROWN,LIGHT_BROWN,10,1,5,40,-1};

static struct NewButton sample_button1 = {
   &sample_knob2,260,50,320,65,GAD_BUTTON|BORDER_3D,0,0,WHITE,LIGHT_BROWN,
   "Hit Me"};

static struct NewButton sample_button2 = {
   &sample_button1,260,70,320,85,GAD_BUTTON|BUTN_TOGGLE|BORDER_3D,butn_func,0,
   WHITE,LIGHT_BROWN,"Toggle"};

static struct NewButton sample_button3 = {
   &sample_button2,260,90,280,100,GAD_BUTTON|BORDER_3D|BUTN_LEFT_ARROW,
   butn_func,0,WHITE,LIGHT_BROWN,0};

static struct NewList sample_list = {
   NULL,70,50,250,150,GAD_LIST|KNOB_ARROWS,list_func,0,
   WHITE,BROWN,LIGHT_BROWN,BROWN,LIGHT_BROWN};

struct IntuiMessage *message,*get_all_msgs(),*GetMsg();
struct Gadget *g;
struct list *l1;
int x,y;
ULONG class,mask;
USHORT code;
int i;
char buf[60];
extern long gadlib_sigbit;
extern short signal_ok;
extern struct GadEvent event_buffer[512];
extern short read_ptr;    /** Next read by app prog comes from this index **/
extern short write_ptr;   /** Next event write by handler goes to this index **/
short end_ptr;
short count;
short again;

setup();       /* open window, screen, libraries */
init_gadlib(); /*!!!MUST CALL THIS BEFORE USING GADGETS!!!*/

/** create all gadget linked together with NewGadgetHeader->next **/

if (create_gadget_chain(window,&sample_button3)==0) {
   printf("can't create gadgets!\n");
   setdown();
   exit(0);
   }

/** I create list seperately because I need a pointer to the list **/
/** structure in order to call add_list().  I could have avoided this **/
/** by making the NewList struct the last in the chain.  **/
/** create_gadget_chain() returns a pointer to the last struct **/
/** created if it can create the entire chain **/

l1 = (struct list *)create_gadget(window, &sample_list);
if (l1==0) {
   printf("can't create list gadget!\n");
   setdown();
   exit(0);
   }

debug_print_gadgets();  /* scan the gadget list for fun */

for (i=0; i < 25; i++) {
   sprintf(buf,"ENTRY # %d",i);
   add_list(l1, buf);  
   }

count = 0;
while (TRUE) {

mask=Wait((1<<gadlib_sigbit)|(1<<window->UserPort->mp_SigBit));
if (mask&(1<<gadlib_sigbit)) {
   do {
      end_ptr = write_ptr; 
      for (i=read_ptr; i !=end_ptr; i=(i+1)&511) {
/*   printf("event: %d window %x\n",event_buffer[i].type,event_buffer[i].window);  */
         code=event_buffer[i].type; 
         window = event_buffer[i].window; 
         x = event_buffer[i].x;
         y = event_buffer[i].y;
         if (code==GE_DELTAMOVE) {
            count += y;
            y = count/2;
            count = count - y*2;
            }
         else {
            count = 0;
            }
         handle_gadgets(window, code, x, y);
         }

      read_ptr = end_ptr;
      Disable();
      if (write_ptr==end_ptr) {
         signal_ok = TRUE;    /** enable handler.device signals **/
         again=FALSE;
         }
      else {
         again=TRUE;
         }
      Enable();
      } while (again);
   }

if (mask&(1<<window->UserPort->mp_SigBit)) {
   if ((message=(struct IntuiMessage *)GetMsg(window->UserPort))!=0) {
      class=message->Class;
      code=message->Code;
      y = message->MouseY;
      x = message->MouseX;
      g=(struct Gadget *)message->IAddress;
      ReplyMsg(message);

      if (class == CLOSEWINDOW) {
         while ((message=GetMsg(window->UserPort))!=0)
            ReplyMsg(message);
         for (i=0; i < 25; i++)
            delete_list_entry(l1, i);  
         delete_gadget_chain(window);  /** delete all gadgets in this window */
         rem_gadlib();
         setdown();
         exit();
         }
      }
   }
}
}

/**************************************************************/

void debug_print_gadgets()
{
extern struct MinList gad_list; /* in gadlib.c */
struct MinNode *node;

for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ)
   printf("GADGET flags %x\n",((struct GadgetHeader *)node)->flags);
}

/**************************************************************/

void list_func(l)
struct list *l;
{
if (l->flags&LIST_NEWSELECT) printf("newselect! %d\n",l->current);
else if (l->flags&LIST_DOUBLESELECT) printf("doubleselect! %d\n",l->current);
else printf("wierd list func call\n");
}

/**************************************************************/

void num_func(k)
struct knob *k;
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

void butn_func(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
   printf("button hit!\n");
else
   printf("button not hit!\n");
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

window = OpenWindow(&NewWindow);
if (window==0) {
   printf("can't open back window\n");
   setdown();
   exit(10);
   }
printf("w %d h %d\n",screen->Width, screen->Height);
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
}

void closelibraries()
{
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

if (GfxBase)
   CloseLibrary(GfxBase);
}

