#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"
#include "/include/psound.h"

#include "smpte.pwc"

extern struct Disk_Samp samp[MAX_SAMPLES];
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */
extern struct list cue_list;
extern struct studioBase *studioBase;

struct time_code smpte_code;
struct Window *smpte_win;

void open_smpte(this_module)
struct Module *this_module;
{
void closelibraries();
void refresh_smpte_win();
struct Screen *OpenScreen();
struct Window *OpenWindow();

NewWindowStructure1.Screen=studioBase->screen;
this_module->window=OpenWindow(&NewWindowStructure1);
smpte_win = this_module->window;
if (smpte_win==0) {
   telluser("can't open smpte window","");
   setdown();
   exit(10);
   }

SetMenuStrip(smpte_win,Titles12);   /* turn on menus */

smpte_code.hours=0;
smpte_code.minutes=0;
smpte_code.seconds=0;
smpte_code.frames=0;

refresh_smpte_win();
}


void do_smpte(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
void refresh_smpte_win();

if (class==REFRESHWINDOW) {
   BeginRefresh(smpte_win);
   refresh_smpte_win();
   EndRefresh(smpte_win, TRUE);
   return;
   }

if (class==NEWSIZE) {
   refresh_smpte_win();
   return;
   }
}

void refresh_smpte_win()
{
short w,h;
short cellw,cellh;
short x;

w=smpte_win->Width;
h=smpte_win->Height;

SetAPen(smpte_win->RPort, BROWN);
RectFill(smpte_win->RPort, 2,11, w-3, h-10);
RectFill(smpte_win->RPort, 2,h-9, w-17, h-2);

draw_indent(smpte_win->RPort, 10, 14, w-15, h-10);
draw_digits();
}

draw_digits()
{
short w,h;
short cellw,cellh;
short x;

w=smpte_win->Width;
h=smpte_win->Height;

cellh = h-30;
cellw = (w-27)/12;
x = 15;

/* big_digit(smpte_win->RPort,val, xoff, yoff,height,width); */

big_digit(smpte_win->RPort,tens(smpte_code.hours), x, 17,cellh,cellw);
x = x + cellw+cellw/5;
big_digit(smpte_win->RPort,ones(smpte_code.hours), x, 17,cellh,cellw);
x = x + cellw;
big_digit(smpte_win->RPort,10, x, 17,cellh,cellw);
x = x + cellw;

big_digit(smpte_win->RPort,tens(smpte_code.minutes), x, 17,cellh,cellw);
x = x + cellw+cellw/5;
big_digit(smpte_win->RPort,ones(smpte_code.minutes), x, 17,cellh,cellw);
x = x + cellw;
big_digit(smpte_win->RPort,10, x, 17,cellh,cellw);
x = x + cellw;

big_digit(smpte_win->RPort,tens(smpte_code.seconds), x, 17,cellh,cellw);
x = x + cellw+cellw/5;
big_digit(smpte_win->RPort,ones(smpte_code.seconds), x, 17,cellh,cellw);
x = x + cellw;
big_digit(smpte_win->RPort,10, x, 17,cellh,cellw);
x = x + cellw;

big_digit(smpte_win->RPort,tens(smpte_code.frames), x, 17,cellh,cellw);
x = x + cellw+cellw/5;
big_digit(smpte_win->RPort,ones(smpte_code.frames), x, 17,cellh,cellw);
}

tens(x)
int x;
{
return(x/10);
}

ones(x)
int x;
{
return (x-tens(x)*10);
}
interm_refresh_smpte_win()
{
struct cue_event *e;
struct cue_event *find_cue_event();

if (smpte_win) {
   e=find_cue_event(cue_list.current);
   smpte_code.hours=e->hours;
   smpte_code.minutes=e->minutes;
   smpte_code.seconds=e->seconds;
   smpte_code.frames=e->frames;
   draw_digits();
   }
}

