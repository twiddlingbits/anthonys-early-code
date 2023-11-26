#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "open_list.pwc"

/*** global's used only by open_list module ****/

static struct knob my_knob;
struct Window *open_list_win;
struct list open_list;

extern struct Disk_Samp samp[MAX_SAMPLES];

/***********************************************************/

void open_open_list(this_module)
struct Module *this_module;
{
void closelibraries();
void draw_indent();
void refresh_list();
struct Screen *OpenScreen();
extern struct Screen *screen;
extern struct Window *open_list_win;
extern struct list open_list;
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */
struct Window *OpenWind();

this_module->window=OpenWind(&NewWindowStructure1);
open_list_win = this_module->window;

SetMenuStrip(open_list_win,Titles12);   /* turn on menus */

draw_indent(open_list_win->RPort, 10, 14, 240, 80);
draw_indent(open_list_win->RPort, 245, 14, 260, 80);
init_list(open_list_win,&open_list, &my_knob,15, OPEN_OFFSETY, 7, open_list.list_size,0,open_list.current,27);
init_knob(open_list_win,&my_knob, &open_list, 247,15,258,79,open_list.show_size,open_list.list_size, KNOB_3D, LIGHT_BROWN, BROWN, 0);
refresh_list(&open_list);
}

/***********************************************************/

void do_open_list(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
void copy_samp_to_list();
struct Module *find_module();
extern struct studioBase *studioBase;
extern struct list open_list;
extern struct Window *open_list_win;
struct Module *m;
int hl_code;

hl_code=handle_list(&open_list, class, code, x, y);

if (hl_code==NEW_SELECT) {
   m = find_module("SoundEdit");
   if (m)
      if (m->window)
         refresh_edit_win();
   }

if (class==GADGETUP && g==&edit) {
   m = find_module("SoundEdit");
   if (m && m->window==0) {
      (*m->open_window)(m);
      }
   }

if (class==GADGETUP && g==&cue) {
   m = find_module("CueList");
   if (m && m->window==0) {
      (*m->open_window)(m);
      }
   else {
      if (samp[open_list.current].flags&SAMPLE_OPEN)
         add_cue_event(open_list.current, 0,0, 0,0);
      }
   }

if (class==GADGETUP && g==&open) {
   if (load_samp(open_list_win,studioBase->defaults.samp_io_path)!=-1) {
      copy_samp_to_list(&open_list);
      refresh_knob(&my_knob);
      refresh_list(&open_list);
      }
   }

if (class==GADGETUP && g==&PLAYgad) 
   if (samp[open_list.current].flags & SAMPLE_OPEN)
      play(open_list_win,0,samp[open_list.current].length,&samp[open_list.current]);

if (class==GADGETUP && g==&close) {
   if (askuser("OK to close sample:",open_list.list_text[open_list.current])) {
      delete_sample(open_list.current);
      copy_samp_to_list(&open_list);
      refresh_knob(&my_knob);
      refresh_list(&open_list);
      }
   }
}

/***********************************************************/

void copy_samp_to_list(list)

struct list *list;

{
int i;

for (i=0; i < MAX_SAMPLES; i++) {
   if (samp[i].flags&SAMPLE_OPEN) {
      list->list_text[i] = samp[i].name;
      list->list_size = i+1;
      }
   else {
      list->list_text[i] = 0;
      }
   }
}

