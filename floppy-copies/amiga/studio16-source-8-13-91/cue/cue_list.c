#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"
#include "/include/psound.h"

#include "cue_list.pwc"

struct list cue_list;

struct cue_event *first_cue_event;
struct cue_event *last_cue_event;

struct Window *cue_list_win=0;
struct Window *edit_cue_event_win=0;

static struct knob my_knob;
extern struct list cue_list;
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */
extern struct studioBase *studioBase;

void open_cue_list(this_module)
struct Module *this_module;
{
void closelibraries();
void draw_indent();
void refresh_list();
void refresh_cue_list_win();
struct Screen *OpenScreen();
extern struct list cue_list;
struct Window *OpenWindow();

NewWindowStructure1.Screen=studioBase->screen;
this_module->window=OpenWindow(&NewWindowStructure1);
cue_list_win = this_module->window;
if (cue_list_win==0) {
   telluser("can't open window","");
   setdown();
   exit(0);
   }

SetMenuStrip(cue_list_win,Titles12);   /* turn on menus */

refresh_cue_list_win();
}


void do_cue_list(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
extern struct Window *smpte_win;
extern struct list cue_list;
int hl_code;

hl_code=handle_list(&cue_list, class, code, x, y);
if (hl_code==DOUBLE_SELECT && edit_cue_event_win==0) {
      open_edit_cue_event();
   }
else if (hl_code==NEW_SELECT && edit_cue_event_win) {
   refresh_edit_cue_event_win();
   }
if (hl_code==NEW_SELECT && smpte_win) {    /**!!! temp **/
   interm_refresh_smpte_win();
   }

if (class==REFRESHWINDOW) {
   BeginRefresh(cue_list_win);
   refresh_cue_list_win();
   EndRefresh(cue_list_win, TRUE);
   return;
   }

if (class==NEWSIZE) {
   refresh_cue_list_win();
   return;
   }

/* if (class==GADGETUP && g==&edit)
   ed_main(); */
}

int add_cue_event(samp_num, f, s, m, h)

struct Disk_Samp *samp;
short f, s, m, h; /** frames -> hours **/
{
struct cue_event *e;
struct cue_event *p;
int frame_count_val;
int i;

e = (struct cue_event *)AllocMem(sizeof(struct cue_event), MEMF_CLEAR);
if (e==0)
   return(OUT_OF_MEMORY); 

e->time_code.frames = f;
e->time_code.seconds = s;
e->time_code.minutes = m;
e->time_code.hours = h;
e->samp = samp;
update_event_text(e);

if (first_cue_event==0) {
   first_cue_event = e;
   last_cue_event = e;
   }

/** search for the event to put the new event BEFORE **/
else {
   frame_count_val=frame_count(f,s,m,h);
   for (p=first_cue_event; p; p=p->next)
      if (frame_count(p->frames,p->seconds,p->minutes,p->hours)>frame_count_val)
         break;

   if (p) {
      e->prev = p->prev;
      e->next = p;
      p->prev = e;
      e->prev->next = e;
      return(0);
      }


   last_cue_event->next = e;
   e->prev = last_cue_event;
   last_cue_event = e;
   }

cue_list.list_size++;
p = first_cue_event;
for (i=0; i < cue_list.list_size; i++) {
   cue_list.list_text[i] = p->event_text;
   p = p->next;
   }
refresh_list(&cue_list);
refresh_knob(&my_knob);
return(0);
}

frame_count(f,s,m,h)
short f,s,m,h;
{
return(f+s*30+m*60*30+h*60*60*30);
}

void refresh_cue_list_win()
{
short w,h,top;

w=cue_list_win->Width;
h=cue_list_win->Height;

SetAPen(cue_list_win->RPort, BROWN);
RectFill(cue_list_win->RPort, 2,11, w-3, h-10);
RectFill(cue_list_win->RPort, 2,h-9, w-17, h-2);

/* RefreshGadgets(&play,cue_list_win,0); */

draw_indent(cue_list_win->RPort, 10, 14, w-35, h-10);
draw_indent(cue_list_win->RPort, w-30, 14, w-15, h-10);

top = cue_list.top_element;
if (top+(h-27)/9 > cue_list.list_size) {
   top = cue_list.list_size-(h-27)/9;
   if (top < 0) top = 0;
   }

init_list(cue_list_win,&cue_list, &my_knob,15, OPEN_OFFSETY, (h-27)/9, cue_list.list_size,top,cue_list.current, (w-50)/8);
init_knob(cue_list_win,&my_knob, &cue_list, w-28,15,w-17,h-11,cue_list.show_size,cue_list.list_size, KNOB_3D, LIGHT_BROWN, BROWN, top);

refresh_list(&cue_list);
}

void update_event_text(e)

struct cue_event *e;
{
sprintf(e->event_text,"%02d:%02d:%02d:%02d  %s %s",
e->hours,e->minutes,e->seconds,e->frames,e->samp->name,e->description);
}

