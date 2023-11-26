#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"
#include "/include/psound.h"

#include "edit_cue_event.pwc"

static struct knob my_knob;
extern struct Disk_Samp samp[MAX_SAMPLES];
extern struct Window *edit_cue_event_win;
extern struct studioBase *studioBase;

void open_edit_cue_event()
{
void closelibraries();
void draw_indent();
void refresh_list();
void refresh_edit_cue_event_win();
struct Screen *OpenScreen();
extern struct Menu Titles12[];    /* menus done with MenuEd, Shareware */
struct Window *OpenWindow();

NewWindowStructure1.Screen=studioBase->screen;
edit_cue_event_win=OpenWindow(&NewWindowStructure1);
if (edit_cue_event_win==0) {
   telluser("can't open window","");
   setdown();
   exit(0);
   }

SetMenuStrip(edit_cue_event_win,Titles12);   /* turn on menus */

SetAPen(edit_cue_event_win->RPort, BROWN);
RectFill(edit_cue_event_win->RPort, 2,11, NewWindowStructure1.Width-3,NewWindowStructure1.Height-2);
RefreshGadgets(&description,edit_cue_event_win,0);
PrintIText(edit_cue_event_win->RPort,&IntuiTextList1,0,0);
refresh_edit_cue_event_win();
}


void do_edit_cue_event(class,code,x,y,g)
struct Gadget *g;
ULONG class;
USHORT code;
int y,x;
{
extern struct Window *edit_cue_event_win;
extern struct list cue_list;
struct cue_event *find_cue_event();
struct cue_event *e;
int i1,i2,i3,i4;

printf("event: %x\n",class);

if (class==GADGETUP && g==&in_pt) {
   e=find_cue_event(cue_list.current);
   sscanf(in_ptSIBuff,"%d:%d:%d:%d",&i1,&i2,&i3,&i4);
   e->hours=i1;
   e->minutes=i2;
   e->seconds=i3;
   e->frames=i4;
   update_event_text(e);
   refresh_list(&cue_list);
   interm_refresh_smpte_win();
   }


if (class==ACTIVEWINDOW) {
   ActivateGadget(&description, edit_cue_event_win, 0);
   }

if (class==GADGETUP && g==&description) {
   e=find_cue_event(cue_list.current);
   strncpy(e->description, descriptionSIBuff, 40);
   update_event_text(e);
   refresh_list(&cue_list);
   }
}

void refresh_edit_cue_event_win()
{
struct cue_event *e;
e=find_cue_event(cue_list.current);
strncpy(descriptionSIBuff,e->description, 40);
sprintf(in_ptSIBuff,"%02d:%02d:%02d:%02d",e->hours,e->minutes,e->seconds,e->frames);
RefreshGList(&description,edit_cue_event_win, 0, 1);
RefreshGList(&in_pt,edit_cue_event_win, 0, 1);
}

struct cue_event *find_cue_event(k)

int k;

{
struct cue_event *e;
extern struct cue_event *first_cue_event;

e=first_cue_event;
while (k > 0) {
   e = e->next;
   k--;
   }

return(e);
}

