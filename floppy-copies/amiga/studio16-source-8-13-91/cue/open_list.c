#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"

#include "open_list.pwc"

/*** global's used only by open_list module ****/

struct Window *open_list_win;
struct list *open_list;

void func_open_list();

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
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */
struct Window *OpenWind();
static struct NewList newopenlist = {
   NULL,10,14,260,80,GAD_LIST|KNOB_ARROWS,func_open_list,0,
   WHITE,BROWN,LIGHT_BROWN,BROWN,LIGHT_BROWN};

this_module->window=OpenWind(&NewWindowStructure1);
open_list_win = this_module->window;

SetMenuStrip(open_list_win,Titles12);   /* turn on menus */

open_list = (struct list *)create_gadget(this_module->window, &newopenlist);
if (open_list==0) {
   printf("open_list!!!ERROR!!!!!");
   }
}

/***********************************************************/

void do_open_list(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
struct Module *find_module();
extern struct studioBase *studioBase;
extern struct Window *open_list_win;
struct Module *m;
struct Disk_Samp *samp;
int i;

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
      if (open_list->current!=-1) {
         telluser ("Cue lists not working","in this version!");
/***********
         samp = (struct Disk_Samp *)open_list->active_entry->user_data;
         add_cue_event(samp, 0,0, 0,0);
**********/
         }
      }
   }

if (class==GADGETUP && g==&open) {
   newpointer(PTR_BUSY);
   i=load_samp(open_list_win,studioBase->defaults.samp_io_path);
   newpointer(PTR_NORMAL);
   show_error(i);
   }

if (class==GADGETUP && g==&PLAYgad) 
   if (open_list->current!=-1) {
      samp = (struct Disk_Samp *)open_list->active_entry->user_data;
      play(open_list_win, 0, samp->length, samp);
      }

if (class==GADGETUP && g==&close) {
   if (open_list->current!=-1) {
      if (askuser("OK to close sample:",open_list->active_entry->text)) {
         delete_sample(open_list->active_entry->user_data);
         }
      }
   }
}

/***********************************************************/

void func_open_list(l)
struct list *l;
{
struct Module *find_module();
extern struct studioBase *studioBase;
extern struct Window *open_list_win;
extern struct Disk_Samp *cur;
struct Module *m;

if (l->flags&LIST_NEWSELECT) {
   m = find_module("SoundEdit");
   if (m) {
      if (l->current==-1)
         cur=NULL;
       else
         cur = (struct Disk_Samp *)l->active_entry->user_data;
      if (m->window) 
         refresh_edit_win();
      }
   }

}

void close_open_list(this_module)
struct Module *this_module;
{
delete_gadget_chain(this_module->window);  /** delete all gadgets in this window */
ClearMenuStrip(this_module->window);
CloseWindow(this_module->window);
}

