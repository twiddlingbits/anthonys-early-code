#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "/include/psound.h"

#include "setup.pwc"

struct Screen *screen;
struct Window *window;

void setup();
void setdown();
void openlibraries();
void closelibraries();

/**
struct NewKnob {
   void (*func)();
   struct Window *window;
   short box_x1
   short box_y1;
   short box_x2;
   short box_y2;
   short knob_width;
   short knob_height;
   short show_size;
   short list_size;
   short top;
   short apen;
   short bpen;
   unsigned long flags;
   };
**/

void main()
{
void test_func();
static struct NewKnob sample_knob1=
   {test_func,0,20,20,60,150,-1,10,1,5,0,LIGHT_BROWN,BROWN,AUTO_KNOB|FREE_VERT|KNOB_3D|BORDER_3D};
static struct NewKnob sample_knob2=
   {test_func,0,70,20,250,40,0,0,1,10,0,LIGHT_BROWN,BROWN,KNOB_3D|BORDER_3D|FREE_HORIZ|AUTO_KNOB};
struct IntuiMessage *message,*get_all_msgs(),*GetMsg();
struct Gadget *g;
int x,y;
ULONG class;
USHORT code;
struct knob *k1, *k2;

setup();
sample_knob1.window = window;
sample_knob2.window = window;
k1=(struct knob *)create_knob(&sample_knob1);
if (k1==0) {
   setdown();
   exit(0);
   }
k2=(struct knob *)create_knob(&sample_knob2);
if (k2==0) {
   setdown();
   exit(0);
   }  

while (TRUE) {
while ((message=(struct IntuiMessage *)GetMsg(window->UserPort))==0)
   Wait(1<<window->UserPort->mp_SigBit);
   class=message->Class;
   code=message->Code;
   y = message->MouseY;
   x = message->MouseX;
   g=(struct Gadget *)message->IAddress;
   ReplyMsg(message);

   handle_knob(k1, class, code, x, y);
   handle_knob(k2, class, code, x, y); 

/*   if (class!=MOUSEMOVE)
      printf("top %d\n",k->top_element); */

   if (class == CLOSEWINDOW) {
      while ((message=GetMsg(window->UserPort))!=0)
         ReplyMsg(message);
      setdown();
      exit();
      }
   }
}

void test_func(k)
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

