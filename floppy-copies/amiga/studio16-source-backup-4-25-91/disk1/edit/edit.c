#include <exec/types.h>
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "devices/audio.h"
#include "libraries/dosextens.h"
#include "/include/gadlib.h"
#include "/include/soundedit.h"
#include "/include/studio16.h"


/** variables global to this module **/

static short dragging;
static struct Window *edit_win;
static unsigned long UsedInstanceBits=0;
static struct GadgetColors colors;		/** TEMP put in studioBase **/

struct EditInstance EditInstance[32];
struct EditInstance *ei;

int cur;		/*** DELETE ME!!!!! **/

/** This modules function delclerations **/

int start_dragging();
int set_new_em();
int in_graph();
void pad_str();
void mix_samples();
void draw_marked();
void delete_sample();
struct Disk_Samp *get_sample_ptr();
struct Module *find_module();
void close_edit();
struct Window *QuickWindow ();
void FreeEditInstance();
int AllocEditInstance();

/** function's used in this module but defined externally **/


/** variables used in the module but defined externally **/

extern struct studioBase *studioBase;
extern struct list *open_list;
extern struct Menu Titles[];    /* menus done with MenuEd, Shareware */
extern struct IntuitionBase *IntuitionBase;

/************************************************************/

void open_edit(m)
struct Module *m;
{
struct MinNode *node;
int i;

colors.button.background=BROWN;		/** DELTE THIS! **/

for (node=open_list->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	if (((struct list_entry *)node)->flags&LE_SELECTED) {
		i = AllocEditInstance();
printf("alloc i %d\n",i);
		if (i==-1)
			return;

		if ((m->window=(struct Window *)QuickWindow(&colors,20,20+i*20,400,90+i*20,
					MOUSEMOVE|MENUPICK|MOUSEBUTTONS|RAWKEY|CLOSEWINDOW|NEWSIZE,
					WINDOWSIZING|REPORTMOUSE|ACTIVATE|NOCAREREFRESH|SMART_REFRESH|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE,NULL))==NULL) {
		   telluser("Can't open edit window","");
		   return;
		   }

		EditInstance[i].edit_win = m->window;
		SetMenuStrip(EditInstance[i].edit_win,Titles);   /* turn on menus */

		EditInstance[i].edit_win->UserData = (BYTE *)&EditInstance[i];
		EditInstance[i].inserting  = FALSE;        /** true if finding the insert point **/
		EditInstance[i].mark_drawn = FALSE;
		EditInstance[i].marked_samp = NULL;
		EditInstance[i].insert_samp = NULL;
		EditInstance[i].cur = NULL;
		EditInstance[i].graph_width = EditInstance[i].edit_win->Width - GRAPH_X - GRAPH_X;
		EditInstance[i].graph_height = EditInstance[i].edit_win->Height - GRAPH_Y - GRAPH_Y;
		EditInstance[i].old_x1=0;
		EditInstance[i].old_x2=0;
		EditInstance[i].start_mark=0;
		EditInstance[i].end_mark=0;
		EditInstance[i].insert_mark=0;
		EditInstance[i].marked_samp=NULL;
		EditInstance[i].insert_samp=NULL;
		EditInstance[i].cur=(struct Disk_Samp *)((struct list_entry *)node)->user_data;
		ei = &EditInstance[i];
		refresh_edit_win(EditInstance[i].edit_win);
		}
	}
}

/************************************************************/

void do_edit(class,code,mousex,mousey,gad)
struct Gadget *gad;
ULONG class;
USHORT code;
int mousey,mousex;
{
int temp;
USHORT menunum,itemnum,subnum;
struct IntuiMessage *message;
struct Window *aw;

aw = IntuitionBase->ActiveWindow;
ei = (struct EditInstance *)aw->UserData;
if (ei==NULL || ei->edit_win!=aw)
	return;

switch (class) {

case NEWSIZE:
   ei->graph_width = ei->edit_win->Width - GRAPH_X - GRAPH_X;
   ei->graph_height = ei->edit_win->Height - GRAPH_Y - GRAPH_Y;
   refresh_edit_win(ei->edit_win);
   break;

case MOUSEMOVE:
   if (dragging) {
      if (ei->inserting) {
         ei->insert_mark = set_new_em(mousex);
         ei->insert_samp = ei->cur;
         }
      else {
         ei->end_mark = set_new_em(mousex);
         }
      draw_marked(ei->edit_win);
      }
   do {
      message=(struct IntuiMessage *)GetMsg(ei->edit_win->UserPort);
      if (message==0)  {
/*         if (dragging) update_se_mark(ei->edit_win, line); */
         goto ignore;
         }
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      gad=(struct Gadget *)message->IAddress;
      ReplyMsg(message);
      } while (class==MOUSEMOVE);

case MOUSEBUTTONS:
   if (code!=SELECTDOWN) {
      if (dragging) {
         dragging=FALSE;
         if (ei->start_mark > ei->end_mark) {     /** swap **/
            temp = ei->start_mark;
            ei->start_mark = ei->end_mark-1;   /** -1 to make even **/
            ei->end_mark = temp-1;         /** -1 to make odd **/
            }
         /*update_se_mark(edit_win, line);*/
         }
      goto ignore;
      }

   if (in_graph(mousex, mousey) && ei->cur!=NULL) {
      dragging = TRUE;
      if (ei->inserting) {
         ei->insert_mark = start_dragging(mousex);
         ei->insert_samp = ei->cur;
         }
      else {
         if (ei->marked_samp==ei->cur && closeto (mousex, ei->end_mark)) {
            ei->end_mark = set_new_em(mousex);
            draw_marked(ei->edit_win);
            }
         else if (ei->marked_samp==ei->cur && closeto (mousex, ei->start_mark)) {
            ei->start_mark = ei->end_mark-1;
            ei->end_mark = set_new_em(mousex);
            draw_marked(ei->edit_win);
            }
         else {
            ei->start_mark = start_dragging(mousex);
            ei->end_mark = ei->start_mark+1;
            ei->marked_samp = ei->cur;
            }
         }
      /*update_se_mark(edit_win, line);*/
      goto ignore;
      }

case RAWKEY:
   if (code==64) {      /** space bar cancels **/
      if (ei->inserting) {
         ei->inserting=FALSE;
         show_msg(edit_win,101);
         goto graph_update;
         }
      goto ignore;
      }
   if (code==25) {
      play(ei->edit_win,ei->start_mark,ei->end_mark-ei->start_mark+1,ei->cur);
      goto ignore;
      }
   goto ignore;
	}

if (class!=MENUPICK)
   goto ignore;

/** ALL GADGET commands must be above here **/
/** Only menu options follow **/

menunum=MENUNUM(code);
itemnum=ITEMNUM(code);
subnum=SUBNUM(code);

if (menunum==0 && itemnum==2) {     /* paste */
   }

if (menunum==1 && itemnum==3) {     /** erase **/
      }

if (menunum==1 && itemnum==4) {  /** insert **/
	}

if (menunum==1 && itemnum==7) {  /** set playback rate **/
   if (ei->cur==NULL) goto ignore;
   ei->cur->rate=getint(ei->edit_win,ei->cur->rate,3000,60000,get_msg_ptr(235),450);
   }

if (menunum==2 && itemnum==0) {        /* flip marked range */
   }

if (menunum==2 && itemnum==5) {			/** Mix (5) **/
   }

/**** everything comes here when done ****/

graph_update:

refresh_edit_win(ei->edit_win);

update:
ignore:       /** here when ready for new input event **/
return;
}

/************************************************************/

void close_edit(m)
struct Module *m;
{
short i;
unsigned long ChanBit;
short did_not_close;

did_not_close = TRUE;

for (i=0; i < 32 ; i++)	{
	ChanBit=(1<<i);
	if ((UsedInstanceBits&ChanBit)!=0) {		/** Alloced chanel !! **/
		ClearMenuStrip(EditInstance[i].edit_win);
		FreeEditInstance(i);
		CloseWindow(EditInstance[i].edit_win);
		EditInstance[i].edit_win=NULL;
		m->window=0;
		did_not_close = FALSE;
		}
	}
if (did_not_close)
	m->flags |= MOD_DID_NOT_CLOSE;
}

/************************************************************/

/** Handle EditInstance allocation/ deallocations **/

int AllocEditInstance()
{
short i;
unsigned long ChanBit;

for (i=0; i < 32 ; i++)	{
	ChanBit=(1<<i);
	if ((UsedInstanceBits&ChanBit)==0) {		/** Free chanel !! **/
		UsedInstanceBits |= ChanBit;
		return((int)i);
		}
	}
return(-1);
}

void FreeEditInstance(ChanNum)
int ChanNum;
{
UsedInstanceBits &= (~(1<<ChanNum));
}

/************************************************************/

/** given the pointer position, update the global variable "marker" **/

int start_dragging(mousex)

int mousex;
{
int marker;    /** sample where dragging starts **/

SetDrMd(ei->edit_win->RPort, COMPLEMENT);

if (ei->mark_drawn)
   RectFill(ei->edit_win->RPort, ei->old_x1, GRAPH_Y, ei->old_x2, GRAPH_Y+ei->graph_height-1);

ei->mark_drawn=TRUE;    /** erase old marked range **/
ei->old_x1 = ei->old_x2 = mousex;

marker = ((mousex - GRAPH_X)*ei->cur->length) + (ei->graph_width/2); /* .5 round */
marker = even(marker / ei->graph_width);
Move (ei->edit_win->RPort, mousex, GRAPH_Y);
Draw (ei->edit_win->RPort, mousex, GRAPH_Y+ei->graph_height-1);
return(marker);
}

/*****************************************************/

void draw_marked(Window)

struct Window *Window;

{
int x1,x2;
int temp;

SetDrMd(ei->edit_win->RPort, COMPLEMENT);
if (ei->inserting) {
   x1 = GRAPH_X + (ei->insert_mark*ei->graph_width)/ei->cur->length;
   x2 = GRAPH_X + (ei->insert_mark*ei->graph_width)/ei->cur->length;
   }
else {
   x1 = GRAPH_X + (ei->start_mark*ei->graph_width)/ei->cur->length;
   x2 = GRAPH_X + (ei->end_mark*ei->graph_width)/ei->cur->length;
   }

if (x2 < x1) {    /** swap **/
   temp = x1;
   x1 = x2;
   x2 = temp;
   }

if (ei->old_x1==x1 && ei->old_x2==x2 && ei->mark_drawn)
   return;

if (ei->mark_drawn) {
   if (x1 < ei->old_x1)
      RectFill(ei->edit_win->RPort, x1, GRAPH_Y, ei->old_x1-1, GRAPH_Y+ei->graph_height-1);
   if (x1 > ei->old_x1)
      RectFill(ei->edit_win->RPort, ei->old_x1, GRAPH_Y, x1-1, GRAPH_Y+ei->graph_height-1);
   if (x2 > ei->old_x2)
      RectFill(ei->edit_win->RPort, ei->old_x2+1, GRAPH_Y, x2, GRAPH_Y+ei->graph_height-1);
   if (x2 < ei->old_x2)
      RectFill(ei->edit_win->RPort, x2+1, GRAPH_Y, ei->old_x2, GRAPH_Y+ei->graph_height-1);
   }
else {
   ei->mark_drawn=TRUE;
   RectFill(ei->edit_win->RPort, x1, GRAPH_Y, x2, GRAPH_Y+ei->graph_height-1);
   }
ei->old_x1 = x1;
ei->old_x2 = x2;
}

/*****************************************************/

int in_graph(x, y)

int x, y;

{
return (x>GRAPH_X && x<(GRAPH_X+ei->graph_width) && y>GRAPH_Y && y<(GRAPH_Y+ei->graph_height));
}

/*****************************************************/

int set_new_em(mousex)
int mousex;
{
int marker;

marker = ((mousex - GRAPH_X)*ei->cur->length)/ei->graph_width;
marker = odd(marker);
if (marker >= ei->cur->length)
   marker = ei->cur->length-1;
if (marker < 0)
   marker = 1;
return (marker);
}

/*****************************************************/

closeto(mousex, mark)
int mousex, mark;
{
int i;

i = mousex - GRAPH_X - mark*ei->graph_width/ei->cur->length;
if (i < 0)
   i = -i;
if (i < 10)
   return (TRUE);
else
   return(FALSE);
}

/*****************************************************/

void pad_str(line)

char *line;

{
strncat(line,"            ",30-strlen(line));
}
