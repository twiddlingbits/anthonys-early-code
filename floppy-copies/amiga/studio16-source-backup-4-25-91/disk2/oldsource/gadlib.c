/** lc -v gadlib.c **/

#include <exec/types.h>
#include <exec/exec.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"

/**** global variables *******/


struct MinList gad_list;

struct Task *tc_gadlib;       /** gadlib task's tc **/
struct Task *tc_main;         /** program that launches gadlib tc **/

short signal_waitforgad=FALSE; /** signal main task about gadget? */

short lock_gadlib=0;          /** inc when shouldnt close gadlib */

char NameString[]="GadLibHandler";
struct GadEvent event_buffer[512];
short read_ptr=0;    /** Next read by gadlib comes from this index **/
short write_ptr=0;   /** Next event write by handler goes to this index **/
short signal_ok=1;   /** Tell input.device handler to signal us **/
long gadlib_sigbit;  /** signal this task by this bit when new events **/
long gadlib_sigbit2; /** used by WaitForGadget **/
struct GadgetHeader *last_active_gadget;

struct IOStdReq *inputReqBlk;
struct MsgPort *inputPort;
struct Interrupt *inputHandler;

extern struct IntuitionBase *IntuitionBase;

/***** functions *********/

void handle_inputevent();
struct GadgetHeader *WaitForGadget();
struct button *create_button();
struct knob *create_knob();
struct list *create_list();
struct GadgetHeader *create_gadget();
void active_gadget();
void inhandler();
void delete_list();
void handle_button();
void handle_knob();
void handle_list();
void handle_gadgets();
void delete_gadget();
void delete_knob();
void delete_button();
void draw_knob();
void drag_knob();
void start_drag_knob();
void draw_indent();
void refresh_knob();
void calc_knob_size();
void calc_knob_xy();
void calc_top_element();
void scroll_knob();
void scroll_list();
void refresh_button();
void xor_button();
void move_knob();
void dec_knob();
void inc_knob();
void rem_gadlib_handler();
void delete_list_entry();
struct list_entry *get_list_entry_ptr();
__saveds void main_gadlib();

/**************************************************************/
/***** gadget list  Routines   ********************************/
/**************************************************************/

/*############################################################
NAME
	init_gadlib -
FUNCTION
	Sets up gadget library's internal variables and launches task
INPUTS
	NONE
RESULTS
	returns a pointer to the launched Task struct or
	NULL if an error occured.
##############################################################*/

struct Task *init_gadlib()
{
NewList(&gad_list);
if (gadlib_sigbit2=AllocSignal(-1)==-1) 
   return(0);     /** no sigbit available! */

tc_main = (struct Task *)FindTask(0);
tc_gadlib=(struct Task *)CreateTask("gadlibtask",1,main_gadlib,4000);
return(tc_gadlib);
}

/*############################################################
NAME
	rem_gadlib -
FUNCTION
	removes gadget library task and memory
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void rem_gadlib()
{
while (lock_gadlib>0) ;    /** Icky busy loop waiting for tc_gadlib **/
rem_gadlib_handler();
DeleteTask(tc_gadlib);
FreeSignal(gadlib_sigbit2);
}


/**************************************************************/

__saveds void main_gadlib()    /** load a4 with LinkerDB on entry **/
{
if (!add_gadlib_handler())
   Wait(0);

while (TRUE) {
   handle_inputevent();
   }
}

/**************************************************************/

void handle_inputevent()      /** must be re-entrant! **/
{
extern long gadlib_sigbit;
extern short signal_ok;
extern struct GadEvent event_buffer[512];
extern short read_ptr;    /** Next read by app prog comes from this index **/
extern short write_ptr;   /** Next event write by handler goes to this index **/
static short count=0;
short again;
unsigned long mask;
int x,y;
short code;
struct window *window;

mask=Wait(1<<gadlib_sigbit);
do {
   while (read_ptr!=write_ptr) {
      code=event_buffer[read_ptr].type; 
      window = event_buffer[read_ptr].window; 
      x = event_buffer[read_ptr].x;
      y = event_buffer[read_ptr].y;
      if (code==GE_DELTAMOVE) {
         count += y;
         y = count/2;
         count = count - y*2;
         }
      else {
         count = 0;
         }
      read_ptr=(read_ptr+1)&511;
/*      window=IntuitionBase->ActiveWindow; */
      handle_gadgets(window, code, x, y);
      }
   Disable();
   if (write_ptr==read_ptr) {
      signal_ok = TRUE;    /** enable handler.device signals **/
      again=FALSE;
      }
   else {
      again=TRUE;
      }
   Enable();
   } while (again);
}

/*############################################################
NAME
	create_gadget_chain -
FUNCTION
	adds a list of gadgets to the gadget list
INPUTS
	w  pointer to window pointer this gadget should be in
	ng pointer to first NewGadget structure in this chain
RESULTS
	returns a pointer (GadgetHeader *)to the last gadget added or
	NULL if an error occured.
##############################################################*/

struct GadgetHeader *create_gadget_chain(w,ng)
struct Window *w;
struct NewGadgetHeader *ng;
{
struct GadgetHeader *ptr;

while (ng) {
   if ((ptr=create_gadget(w,ng))==0)
      return(0);
   ng = ng->next;
   }
return(ptr);   
}

/*############################################################
NAME
	create_gadget -
FUNCTION
	adds a one gadgets to the gadget list
INPUTS
	w  pointer to window pointer this gadget should be in
	ng pointer to a NewGadget structure describing the gadget
RESULTS
	returns a pointer to GadgetHeader struct added or
	NULL if an error occured.
##############################################################*/

struct GadgetHeader *create_gadget(w,ng)
struct Window *w;
struct NewGadgetHeader *ng;
{
struct GadgetHeader *ptr;

ptr = 0;

if (ng->flags&GAD_LIST)
   ptr = (struct GadgetHeader *)create_list(w,ng);
else if (ng->flags&GAD_BUTTON)
   ptr = (struct GadgetHeader *)create_button(w,ng);
else if (ng->flags&GAD_KNOB)
   ptr = (struct GadgetHeader *)create_knob(w,ng);

if (ptr)
   AddTail(&gad_list, ptr);
return(ptr);
}

/*############################################################
NAME
	delete_gadget_chain -
FUNCTION
	deletes all gadgets in the specified window
INPUTS
	w  pointer to window 
RESULTS
	NONE
##############################################################*/

void delete_gadget_chain(w)
struct Window *w;
{
struct MinNode *node;
struct GadgetHeader *g;

node=gad_list.mlh_Head;

while (node->mln_Succ) {
   g=(struct GadgetHeader *)node;
   node = node->mln_Succ;
   if (w==g->w)
      delete_gadget(g);
   }
}

/*############################################################
NAME
	delete_gadget -
FUNCTION
	deletes the specified gadget
INPUTS
	g pointer to GadgetHeader struct to delete (not NewGadget!)
RESULTS
	NONE
##############################################################*/

void delete_gadget(g)
struct GadgetHeader *g;
{
Remove(g);     /** this is the same as Remove(&g->node) **/
if (g->flags&GAD_LIST)
   delete_list(g);
else if (g->flags&GAD_BUTTON)
   delete_button(g);
else if (g->flags&GAD_KNOB)
   delete_knob(g);
}

/**************************************************************/

void handle_gadgets(window, code, x, y)
struct Window *window;
{
struct MinNode *node;
unsigned long flags;

for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (window==((struct GadgetHeader *)node)->w) {
      flags=((struct GadgetHeader *)node)->flags;
      if (flags&GAD_LIST)
        handle_list(node, code, x, y);
        else if (flags&GAD_BUTTON)
           handle_button(node, code, x, y);
        else if (flags&GAD_KNOB)
            handle_knob(node, code, x, y);
      }
   }
}

/**************************************************************/

void active_gadget(g)
struct GadgetHeader *g;
{
Forbid();
if (g->func)
   (*g->func)(g);
Permit();
if (signal_waitforgad)
   Signal(tc_main, 1<<gadlib_sigbit2);
last_active_gadget = g;
}

/*############################################################
NAME
	WaitForGadget -
FUNCTION
	Waits for any gadget to activate
INPUTS
	NONE
RESULTS
	returns a pointer (GadgetHeader *) to the activated gadget
##############################################################*/

struct GadgetHeader *WaitForGadget()
{
if ((struct Task *)FindTask(0)==tc_gadlib) {    /** called for gadlib task? **/
   last_active_gadget=0;
   signal_ok = TRUE;    /** enable handler.device signals **/
   while (last_active_gadget==0)
      handle_inputevent();
   }
else {
   signal_waitforgad = TRUE;
   Wait(1<<gadlib_sigbit2);
   signal_waitforgad = FALSE;
   }
return(last_active_gadget);
}

/**************************************************************/
/***** Knob Handeling Routines ********************************/
/**************************************************************/

struct knob *create_knob(w,nk)
struct Window *w;
struct NewKnob *nk;
{
struct knob *k;
static struct NewButton arrow_button = {
   NULL,0,0,0,0,GAD_BUTTON|BORDER_3D|BUTN_LEFT_ARROW,0,0,WHITE,LIGHT_BROWN,BROWN,0};

k = (struct knob *)AllocMem(sizeof(struct knob), MEMF_CLEAR);
if (k==0)
   return(0);


k->box_x1=nk->box_x1;
k->box_y1=nk->box_y1;
k->box_x2=nk->box_x2;
k->box_y2=nk->box_y2;
k->flags = nk->flags;
k->kw = w;

if (k->flags&KNOB_ARROWS) {
   if (k->flags&FREE_HORIZ) {
      k->box_x2 -= (STD_H_ARROW_SIZE*2+6);
      arrow_button.box_x1=k->box_x2+3;
      arrow_button.box_x2=arrow_button.box_x1+STD_H_ARROW_SIZE;
      arrow_button.box_y1=k->box_y1;
      arrow_button.box_y2=k->box_y2;
      arrow_button.flags = GAD_BUTTON|BORDER_3D|BUTN_LEFT_ARROW|(k->flags&MANUAL_XOR);
      arrow_button.func = dec_knob;
      arrow_button.user_data = (APTR)k;
      k->b1=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b1==0) {
         FreeMem(k, sizeof(struct knob));
         return(0);
         }
      arrow_button.box_x1=arrow_button.box_x2+3;
      arrow_button.box_x2=arrow_button.box_x1+STD_H_ARROW_SIZE;
      arrow_button.flags = GAD_BUTTON|BORDER_3D|BUTN_RIGHT_ARROW|(k->flags&MANUAL_XOR);
      arrow_button.func = inc_knob;
      k->b2=(struct button *)create_gadget(k->kw,&arrow_button);
      if (k->b2==0) {
         delete_button(k->b1);
         FreeMem(k, sizeof(struct knob));
         return(0);
         }
      }
   if (k->flags&FREE_VERT) {
      k->box_y2 -= (STD_V_ARROW_SIZE*2+4);
      arrow_button.box_x1=k->box_x1;
      arrow_button.box_x2=k->box_x2;
      arrow_button.box_y1=k->box_y2+2;
      arrow_button.box_y2=arrow_button.box_y1+STD_V_ARROW_SIZE;
      arrow_button.flags = GAD_BUTTON|BORDER_3D|BUTN_UP_ARROW;
      arrow_button.func = dec_knob;
      arrow_button.user_data = (APTR)k;
      k->b1=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b1==0) {
         FreeMem(k, sizeof(struct knob));
         return(0);
         }
      
      arrow_button.box_y1=arrow_button.box_y2+2;
      arrow_button.box_y2=arrow_button.box_y1+STD_V_ARROW_SIZE;
      arrow_button.flags = GAD_BUTTON|BORDER_3D|BUTN_DOWN_ARROW;
      arrow_button.func = inc_knob;
      k->b2=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b2==0) {
         delete_button(k->b1);
         FreeMem(k, sizeof(struct knob));
         return(0);
         }
      }
   }

if (k->flags&BORDER_3D) {
   k->box_x1 += 2;
   k->box_x2 -= 2;
   k->box_y1 += 1;
   k->box_y2 -= 1;
   }

k->show_size = nk->show_size;
k->list_size = nk->list_size;
k->top_element = nk->top_element;
k->APen = nk->APen;
k->BPen = nk->BPen;
k->CPen = LIGHT_BROWN;
k->func = nk->func;
k->user_data = nk->user_data;
calc_knob_size(k);

if (!(k->flags&AUTO_KNOB)) {
   if (nk->knob_width!=-1) 
      k->knob_width = nk->knob_width;	
	if (nk->knob_height!=-1)
		k->knob_height = nk->knob_height;
	}
calc_knob_xy(k);
refresh_knob(k,k->APen);
return(k);
}

/****************************************************************/

void delete_knob(k)
struct knob *k;
{
FreeMem(k, sizeof(struct knob));
}

/****************************************************************/

void refresh_knob(k)  

struct knob *k;
{
Forbid();
SetAPen(k->kw->RPort, k->BPen);
RectFill(k->kw->RPort,k->box_x1, k->box_y1, k->box_x2, k->box_y2);
if (k->flags&BORDER_3D) {
   draw_indent(k->kw->RPort, k->box_x1-2, k->box_y1-1, k->box_x2+2, k->box_y2+1, DARK_BROWN, WHITE);
   } 
draw_knob(k,k->APen);
Permit();
}

/****************************************************************/

void calc_knob_size(k)
struct knob *k;
{
if (k->flags&FREE_VERT) { 
   if (k->show_size >= k->list_size) {
      k->knob_height = k->box_y2-k->box_y1+1;
      k->top_element = 0;
      }
   else {
      k->knob_height = ((k->box_y2-k->box_y1+1)*k->show_size)/k->list_size;
      }
   k->knob_width = k->box_x2 - k->box_x1-1;
   if (k->flags&KNOB_NOGAP)
      k->knob_width += 2;
   }
if (k->flags&FREE_HORIZ) {
   if (k->show_size > k->list_size) {
      k->knob_width = k->box_x2-k->box_x1+1;
      k->top_element = 0;
      }
   else {
      k->knob_width = ((k->box_x2-k->box_x1+1)*k->show_size)/k->list_size;
      }
   k->knob_height = k->box_y2 - k->box_y1-1;
   if (k->flags&KNOB_NOGAP)
      k->knob_height += 2;
   }
}

/****************************************************************/
/** calcuate knob->x and knob->y from top_element, show_size,  **/
/** and list_size                                              **/
/****************************************************************/

void calc_knob_xy(k)
struct knob *k;
{
if (k->flags & FREE_VERT) {
   if (k->show_size >= k->list_size)
      k->knob_y=k->box_y1;
   else
      k->knob_y=k->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_size-k->show_size)+1;
   if (k->knob_y+k->knob_height > k->box_y2)
      k->knob_y = k->box_y2 - k->knob_height+1;
   k->knob_x = k->box_x1+1;
   if (k->flags&KNOB_NOGAP)
      k->knob_x -= 1;
   }

if (k->flags & FREE_HORIZ) {
   if (k->show_size > k->list_size)
      k->knob_x=k->box_x1;
   else
      k->knob_x=k->box_x1+((k->box_x2 - k->box_x1-k->knob_width+1)*k->top_element)/(k->list_size-k->show_size)+1;
   if (k->knob_x+k->knob_width > k->box_x2)
      k->knob_x = k->box_x2 - k->knob_width+1;
   k->knob_y = k->box_y1+1;
   if (k->flags&KNOB_NOGAP)
      k->knob_y -= 1;
   }
}

/****************************************************************/
/** calc top_element from show knob_x & knob_y                ***/
/****************************************************************/

void calc_top_element(k)
struct knob *k;
{
if (k->show_size >= k->list_size) {
   k->top_element = 0;
   }
else {
   if (k->flags&FREE_VERT)
      k->top_element = ((k->list_size-k->show_size)*(k->knob_y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);
   if (k->flags&FREE_HORIZ)
      k->top_element = ((k->list_size-k->show_size)*(k->knob_x-k->box_x1))/(k->box_x2-k->box_x1-k->knob_width+1);
   }
}

/****************************************************************/

void handle_knob(k, code, x, y)
struct knob *k;
USHORT code;
int x,y;
{
void refresh_list();
void move_knob();
extern struct knob *draging_knob;
int old_knob_y, old_knob_x;

if (draging_knob==k) {
   drag_knob(k, code, x, y);
   return;
   }

if (code==GE_LBUTTONDOWN) {
   if (hit_knob(k, x, y)) {
      start_drag_knob(k, x, y);
      }
   else {
      if (hit_knob_up(k, x, y) || hit_knob_left(k, x, y)) {
         old_knob_y = k->knob_y;
         old_knob_x = k->knob_x;
         if (k->flags&FREE_VERT) 
            k->knob_y = max(k->box_y1,k->knob_y - k->knob_height);
         else 
            k->knob_x = max(k->box_x1,k->knob_x - k->knob_width);
         calc_top_element(k);
         scroll_knob(k, old_knob_x, old_knob_y);
         active_gadget(k);

        /* k->top_element--;
         move_knob(k); */
         }
      else if (hit_knob_down(k, x, y) || hit_knob_right(k, x, y)) {
         old_knob_y = k->knob_y;
         old_knob_x = k->knob_x;
         if (k->flags&FREE_VERT) 
            k->knob_y = min(k->box_y2-k->knob_height,k->knob_y + k->knob_height);
         else 
            k->knob_x = min(k->box_x2-k->knob_height,k->knob_x + k->knob_width);
         calc_top_element(k);
         scroll_knob(k, old_knob_x, old_knob_y);
         active_gadget(k);

/*         k->top_element++;
         move_knob(k);*/
         }
      }
   }
}
/*******************************************************************/
/** called by an up arrow button attached to a knob **/

void dec_knob(b)
struct button *b;
{
struct knob *k;
k=(struct knob *)b->user_data;
k->top_element--;
move_knob(k);
}

/*******************************************************************/
/** called by an down arrow button attached to a knob **/

void inc_knob(b)
struct button *b;
{
struct knob *k;
k=(struct knob *)b->user_data;
k->top_element++;
move_knob(k);
}

/*############################################################
NAME
	move_knob -
FUNCTION
	after changing a knob's top_element call this to move
	the knob imgary
INPUTS
	k  pointer to a knob structure
RESULTS
	NONE
##############################################################*/

void move_knob(k)
struct knob *k;
{
int old_knob_x;
int old_knob_y;

if (k->top_element < 0) k->top_element = 0;
if (k->top_element >= (k->list_size-k->show_size))
   k->top_element = k->list_size-k->show_size;
old_knob_y = k->knob_y;
old_knob_x = k->knob_x;
calc_knob_xy(k);
scroll_knob(k, old_knob_x, old_knob_y);
active_gadget(k);
}

/*******************************************************************/
/** the following local global variables are used by knob draging **/
/*******************************************************************/

static int global_old_knob_y;
static int global_old_knob_x;
static int global_old_top;
static short global_old_flag;
/* static int global_old_mouse_x;
static int global_old_mouse_y; */
static struct knob *draging_knob = NULL;

/*******************************************************************/

void start_drag_knob(k, x, y)
struct knob *k;
int x,y;
{
draging_knob = k;
global_old_flag = k->kw->Flags&REPORTMOUSE;
global_old_knob_x = k->knob_x;
global_old_knob_y = k->knob_y;
global_old_top = k->top_element;
/* global_old_mouse_x = x;
global_old_mouse_y = y; */
ReportMouse(TRUE,k->kw);
if (k->CPen != k->APen)
   draw_knob(k,k->CPen);   /* CPen is knob color when being draged */
}

/*******************************************************************/

void drag_knob(k, code, newx, newy)
USHORT code;
int newy, newx;
struct knob *k;
{
void refresh_list();

/*** NEED TO EAT EVENTS???? ***/

if (code==GE_LBUTTONUP) {
   ReportMouse(global_old_flag,k->kw);
   if (k->CPen != k->APen)
      draw_knob(k,k->APen);   /* CPen is knob color when being draged */
   draging_knob = NULL;
   return;
   }

if (code==GE_DELTAMOVE) {
   if (k->flags & FREE_VERT) {
      k->knob_y += newy; /* delta for this code */
      if (k->knob_y < k->box_y1)
         k->knob_y = k->box_y1;
      if (k->knob_y+k->knob_height > k->box_y2)
         k->knob_y = k->box_y2 - k->knob_height+1;
      }
    if (k->flags & FREE_HORIZ) {
      k->knob_x += newx;      /* delta x */
      if (k->knob_x < k->box_x1)
         k->knob_x = k->box_x1;
      if (k->knob_x+k->knob_width > k->box_x2)
         k->knob_x = k->box_x2 - k->knob_width+1;
      }
   
   scroll_knob(k, global_old_knob_x, global_old_knob_y);
   
   global_old_knob_y = k->knob_y;
   global_old_knob_x = k->knob_x;
/*   global_old_mouse_y = newy;
   global_old_mouse_x = newx; */
   calc_top_element(k);
   if (global_old_top!=k->top_element)
      active_gadget(k);
   global_old_top = k->top_element;
   }
}


/************************************************************/

void scroll_knob(k, old_knob_x, old_knob_y)
struct knob *k;
int old_knob_x;
int old_knob_y;
{
Forbid();
SetBPen(k->kw->RPort,k->BPen);

if (k->flags&FREE_VERT) { 
   if (k->knob_y < old_knob_y)    /** dragging up? **/
      ScrollRaster(k->kw->RPort, 0, old_knob_y-k->knob_y, 
         k->box_x1, k->knob_y, k->box_x2, old_knob_y+k->knob_height-1);   
   else 
      ScrollRaster(k->kw->RPort, 0, old_knob_y-k->knob_y, 
         k->box_x1, old_knob_y, k->box_x2, k->knob_y+k->knob_height-1);
   }

if (k->flags&FREE_HORIZ) { 
   if (k->knob_x < old_knob_x)    /** dragging left? **/
      ScrollRaster(k->kw->RPort, old_knob_x-k->knob_x,0, 
          k->knob_x, k->box_y1, old_knob_x+k->knob_width-1, k->box_y2);   
   else 
      ScrollRaster(k->kw->RPort, old_knob_x-k->knob_x, 0,
          old_knob_x, k->box_y1, k->knob_x+k->knob_width-1, k->box_y2);
   } 
/* WaitBlit(); */
Permit();
}

/****************************************************************/

void draw_knob(k,color)
struct knob *k;
int color;
{
struct RastPort *rp;

Forbid();
rp = k->kw->RPort;
SetAPen(rp, color);
if (k->flags&KNOB_3D)
   RectFill(rp,k->knob_x+2, k->knob_y+1, k->knob_x+k->knob_width-3, k->knob_y+k->knob_height-2);
else
   RectFill(rp,k->knob_x, k->knob_y, k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1);

if (k->flags&KNOB_3D) {
   draw_indent(rp, k->knob_x, k->knob_y,
       k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1,
       WHITE, DARK_BROWN);
#if 0
   SetAPen(rp, WHITE);
   Move(rp, k->knob_x+k->knob_width-2, k->knob_y);
   Draw(rp, k->knob_x, k->knob_y);
   Draw(rp, k->knob_x, k->knob_y+k->knob_height-1);
   Move(rp, k->knob_x+1, k->knob_y);
   Draw(rp, k->knob_x+1, k->knob_y+k->knob_height-1);

   SetAPen(rp, DARK_BROWN);
   Move(rp, k->knob_x+k->knob_width-1, k->knob_y);
   Draw(rp, k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1);
   Move(rp, k->knob_x+k->knob_width-2, k->knob_y+1);
   Draw(rp, k->knob_x+k->knob_width-2, k->knob_y+k->knob_height-1);
   Draw(rp, k->knob_x+1, k->knob_y+k->knob_height-1);
#endif
   }

active_gadget(k);
Permit();
}

/****************************************************************/

int hit_knob(k, x, y)

struct knob *k;
int x,y;
{
return(x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->knob_y && y <=(k->knob_y+k->knob_height));
}

/****************************************************************/

hit_knob_up(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->box_y1 && y < k->knob_y);
}

/****************************************************************/

hit_knob_down(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y > (k->knob_y+k->knob_height) && y <= k->box_y2);
}
/****************************************************************/

hit_knob_left(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_HORIZ && y>=k->knob_y && y<= (k->knob_y+k->knob_height-1) && x >= k->box_x1 && x < k->knob_x);
}

/****************************************************************/

hit_knob_right(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_HORIZ && y>=k->knob_y && y<= (k->knob_y+k->knob_height-1) && x > (k->knob_x+k->knob_width) && x <= k->box_x2);
}

/**************************************************************/
/***** List Handeling Routines ********************************/
/**************************************************************/

void handle_list(l, code, x, y)

struct list *l;
USHORT code;
int x,y;
{
void refresh_list();
int select;
int select_len;
struct list_entry *le;

if (code==GE_LBUTTONDOWN) {
   select = (y-l->box_y-2)/9;
   if (select >= 0 && select < l->show_size) {
      select = select + l->top_element;
      le = (struct list_entry *)get_list_entry_ptr(l, select);
      if (le) {
         select_len =  my_strlen(le->text);
         if (select_len > l->max_show_chars)
            select_len = l->max_show_chars - 1;
         if (x > l->box_x && x < l->box_x+10+8*select_len) {
            if (l->current != select ) {
               l->active_entry = le;
               l->current = select;
               refresh_list(l);
               l->flags|=LIST_NEWSELECT;
               active_gadget(l);
               l->flags&=(~LIST_NEWSELECT);
               }
            else {   /** double slect **/
               l->flags|=LIST_DOUBLESELECT;
               active_gadget(l);
               l->flags&=(~LIST_DOUBLESELECT);
               }
            }
         }
      }
   }
}

/****************************************************************/

#define STD_KNOB_WIDTH 24
#define STD_KNOB_GAP 10

struct list *create_list(w, nl)
struct Window *w;
struct NewList *nl;
{
struct NewKnob standard_list_knob;
struct list *l;

l = (struct list *)AllocMem(sizeof(struct list), MEMF_CLEAR);
if (l==0) return(0);

setmem(&standard_list_knob, sizeof(struct NewKnob), 0);

NewList(&l->entries);     /** chain of items being listed **/
l->box_x = nl->box_x1;
l->box_y = nl->box_y1;
l->show_size = (nl->box_y2 - nl->box_y1 - 6)/8; /** change to font height! */
l->lw = w;
l->APen = nl->ListAPen;
l->BPen = nl->ListBPen;
l->max_show_chars = (nl->box_x2 - nl->box_x1 - STD_KNOB_WIDTH - STD_KNOB_GAP - 4)/8;
l->flags = nl->flags;
l->func = nl->func;
l->user_data = nl->user_data;
l->current=-1; /* -1 means no current item */

standard_list_knob.box_x1 = nl->box_x2 - STD_KNOB_WIDTH;
standard_list_knob.box_x2 = nl->box_x2;
standard_list_knob.box_y1 = nl->box_y1;
standard_list_knob.box_y2 = nl->box_y2;
standard_list_knob.APen = nl->KnobAPen;
standard_list_knob.BPen = nl->KnobBPen;
standard_list_knob.CPen = nl->KnobCPen;
standard_list_knob.list_size = l->list_size;
standard_list_knob.show_size = l->show_size;
standard_list_knob.top_element = l->top_element;
standard_list_knob.func = scroll_list;
standard_list_knob.user_data = (APTR)l;
standard_list_knob.flags=GAD_KNOB|AUTO_KNOB|FREE_VERT|KNOB_3D|BORDER_3D|(nl->flags&KNOB_ARROWS);
l->knob = (struct knob *)create_gadget(l->lw, &standard_list_knob);
if (l->knob==0) {
   FreeMem(l, sizeof (struct list));
   return(0);
   }

Forbid();
SetAPen(l->lw->RPort, l->BPen);
RectFill(l->lw->RPort, nl->box_x1, nl->box_y1,
            nl->box_x2-STD_KNOB_WIDTH-STD_KNOB_GAP, nl->box_y2);

Permit();
draw_indent(l->lw->RPort, nl->box_x1, nl->box_y1,
            nl->box_x2-STD_KNOB_WIDTH-STD_KNOB_GAP, nl->box_y2, DARK_BROWN, WHITE);

return(l);
}
/****************************************************************/

void delete_list(l)
struct list *l;
{
int i;

for (i=0; i < l->list_size; i++)
   delete_list_entry(l,0);
FreeMem(l, sizeof(struct list));
}


/****************************************************************/
/** called by knob handler whenever top_element changes        **/
/****************************************************************/

void scroll_list(k)
struct knob *k;
{
struct list *l;

l = (struct list *)k->user_data;
l->top_element = k->top_element;
refresh_list(l);
}

/*############################################################
NAME
	refresh_list -
FUNCTION
	redraws a struct list's imagery
INPUTS
	l pointer to the list structure to update
RESULTS
	NONE
##############################################################*/

void refresh_list(l)
struct list *l;
{
static char blank[]="                                                                             ";
int i;
static int max_size=0;
int len;
struct RastPort *rp;
struct MinNode *node;
struct MinNode *top_node;
struct list_entry *le;
int blank_lines;

Forbid();
rp = l->lw->RPort;

SetAPen(rp, l->APen);
SetBPen(rp, l->BPen);
SetDrMd(rp, JAM2);

i=0;
blank_lines = l->show_size;
top_node=l->entries.mlh_Head;
for (node=top_node; node->mln_Succ; node = node->mln_Succ) {
   le=(struct list_entry *)node;
   if (i++==l->top_element)
      top_node=node;
   if ((len=my_strlen(le->text))>max_size)
      max_size = len;
   }

i=l->top_element;
for (node=top_node; node->mln_Succ; node = node->mln_Succ) {
   le=(struct list_entry *)node;
   if (i==l->current) {
      SetAPen(rp, l->BPen);
      SetBPen(rp, l->APen);
      }
   Move(rp, l->box_x+3, l->box_y+(i-l->top_element)*9+8);
   len=my_strlen(le->text);
   if (len > l->max_show_chars)
      len = l->max_show_chars;
   Text(rp, le->text, len);
   if (i==l->current) {
      SetAPen(rp, l->APen);
      SetBPen(rp, l->BPen);
      }
   len = min(max_size, l->max_show_chars) - len;
   Text(rp, blank, len);
   i++;
   if (--blank_lines==0)
      break;
   }

/** now clear all lines at bottom of list **/

while (blank_lines--) {
   Move(rp, l->box_x+3, l->box_y+(i-l->top_element)*9+8);
   Text(rp, blank, min(max_size, l->max_show_chars));
   i++;
   }

Permit();
}

/****************************************************************/
/** re-size & draw a list's slider ***/
/****************************************************************/

void refresh_list_knob(l)
struct list *l;
{
struct knob *k;

k=l->knob;
k->top_element = l->top_element;
k->show_size=l->show_size;
k->list_size=l->list_size;
calc_knob_size(k);
calc_knob_xy(k);
refresh_knob(k);
}

/*############################################################
NAME
	add_list_entry -
FUNCTION
	add's an item to a list structure
INPUTS
	l add item to this list
	c text to add to list.  The text is copied, and you don't
	  need to keep it after this call.
   p user_data for this list item    
      
RESULTS
	returns standard error code (NO_ERROR or OUT_OF_MEMORY)
##############################################################*/

int add_list_entry(l, c, p)

struct list *l;
char *c;
unsigned long p;
{
void refresh_list();
char *mem;
int len;
struct list_entry *le;

le =(struct list_entry *)AllocMem(sizeof(struct list_entry),MEMF_CLEAR);
if (le==0)
   return(OUT_OF_MEMORY);

len = my_strlen(c)+1;
mem = (char *)AllocMem(len, 0);
if (mem==0) {
   FreeMem(le, sizeof(struct list_entry));
   return(OUT_OF_MEMORY);
   }
strncpy(mem, c, len);
le->text = mem;
le->user_data = p;
l->list_size++;
AddTail(&l->entries, le);
l->active_entry=le;
if (l->current==-1) {   /* no currently selected item?? */
   l->current = l->list_size-1;
   l->flags|=LIST_NEWSELECT;
   active_gadget(l);
   l->flags&=(~LIST_NEWSELECT);
   }
refresh_list(l);
refresh_list_knob(l);
return(NO_ERROR);
}

/****************************************************************/

void delete_list_entry(l, e)

struct list *l;
int e;   /* entry num */
{
void refresh_list();
int len;
struct list_entry *le;

if (le=get_list_entry_ptr(l, e)) {
   Remove(le);    /* &le->node */
   l->list_size--;
   if (l->list_size==0) {
      l->top_element=0;
      }
   else {
      if (l->top_element >= l->list_size)
         l->top_element = l->list_size-1;
      }
   len = my_strlen(le->text)+1;
   if (len) {
      FreeMem(le->text, len);
      }
   FreeMem(le, sizeof(struct list_entry));
   if (e==l->current) {
      l->current = -1;
      l->active_entry=0;
      l->flags|=LIST_NEWSELECT;
      active_gadget(l);
      l->flags&=(~LIST_NEWSELECT);
      }
   refresh_list(l);
   return;
   }
}


/*############################################################
NAME  get_list_entry_ptr -

FUNCTION
      given a list and entry number (int), this returns a pointer
      to the list_entry structure
INPUTS
      pointer to the list the entry applies to
      entry_num
RESULTS
   pointer to list_entry struct
##############################################################*/

struct list_entry *get_list_entry_ptr(l, e)
struct list *l;
int e;
{
struct MinNode *node;

for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (e--<=0) 
      return((struct list_entry *)node);
   }
return(NULL);
}

/***********************************************************/

int my_strlen(c)
char *c;
{
if (c)
   return(strlen(c));
else
return(0);
}

/*############################################################
NAME
	draw_indent -
FUNCTION
	draw's a "3D" looking box
INPUTS
	Pass upper left and lower right corners.  All paramaters
   are inclusive
RESULTS
	NONE
##############################################################*/

void draw_indent(rp, x1, y1, x2, y2, top_col, bot_col)

struct RastPort *rp;
int x1,x2;
int y1,y2;
int top_col, bot_col;
{
Forbid();
SetAPen(rp, top_col);
Move(rp, x2-1, y1);
Draw(rp, x1, y1);
Draw(rp, x1, y2);
Move(rp, x1+1, y1);
Draw(rp, x1+1, y2);

SetAPen(rp, bot_col);
Move(rp, x2, y1);
Draw(rp, x2, y2);
Move(rp, x2-1, y1+1);
Draw(rp, x2-1, y2);
Draw(rp, x1+1, y2);

#if 0
SetAPen(rp, top_col);   /* DARK_BROWN */
Move(rp, x2, y1);
Draw(rp, x1, y1);
Draw(rp, x1, y2);
SetAPen(rp, bot_col);  /* WHITE */
Draw(rp, x2, y2);
Draw(rp, x2, y1);

Move(rp, x2-1,y1);
Draw(rp, x2-1,y2);
SetAPen(rp, top_col);
Move(rp, x1+1,y1);
Draw(rp, x1+1,y2-1);
#endif
Permit();
}

/***********************************************************/

struct button *create_button(w,nb)
struct Window *w;
struct NewButton *nb;
{
struct button *b;
int len;

b=(struct button *)AllocMem(sizeof(struct button),MEMF_CLEAR);
if (b==0) return(0);

if (nb->text) {
   len = my_strlen(nb->text)+1;
   b->text = (char *)AllocMem(len, 0);
   if (b->text==0) {
      FreeMem(b, sizeof(struct button));
      return(0);
      }
   strncpy(b->text, nb->text, len);
   }

b->box_x1 = nb->box_x1;
b->box_x2 = nb->box_x2;
b->box_y1 = nb->box_y1;
b->box_y2 = nb->box_y2;
b->bw = w;
b->APen = nb->APen;
b->BPen = nb->BPen;
b->CPen = nb->CPen;
b->flags = nb->flags;
b->func = nb->func;
b->user_data = nb->user_data;

if (b->flags&MANUAL_XOR) {
   refresh_button(b,b->flags&BUTN_HIT);  
   }
else {
   refresh_button(b, FALSE);
   if (b->flags&BUTN_HIT) {
      ClipBlit(b->bw->RPort,b->box_x1,b->box_y1,
               b->bw->RPort,b->box_x1,b->box_y1,
               b->box_x2-b->box_x1+1,b->box_y2-b->box_y1+1,0x50);
      }
   }
return(b);
}

/****************************************************************/

void delete_button(b)
struct button *b;
{
if (b->text)
   FreeMem(b->text, my_strlen(b->text)+1);
FreeMem(b, sizeof(struct button));
}

/****************************************************************/

void refresh_button(b,invert)  

struct button *b;
int invert;
{
int xpos, ypos, len;
short apen, bpen, top_col, bot_col;

Forbid();
if (invert) {
   apen=b->BPen;
   bpen=b->CPen;
   top_col = DARK_BROWN;
   bot_col = WHITE;
   }
else {
   apen=b->APen;
   bpen=b->BPen;
   top_col = WHITE;
   bot_col = DARK_BROWN;
   }

SetAPen(b->bw->RPort, bpen);
RectFill(b->bw->RPort,b->box_x1, b->box_y1, b->box_x2, b->box_y2);
if (b->flags&BORDER_3D) {
   draw_indent(b->bw->RPort, b->box_x1, b->box_y1, b->box_x2, b->box_y2, top_col, bot_col);
   }
SetAPen(b->bw->RPort, apen);
SetBPen(b->bw->RPort, bpen);
SetDrMd(b->bw->RPort, JAM2);
if (b->text) {
   len = my_strlen(b->text);
   xpos = b->box_x1+(b->box_x2 - b->box_x1 - len*8)/2;
   ypos = b->box_y1+(b->box_y2 - b->box_y1+8)/2;
   Move(b->bw->RPort, xpos, ypos);
   Text(b->bw->RPort, b->text, len);
   }
if (b->flags&BUTN_UP_ARROW) { 
   Move(b->bw->RPort, b->box_x1+4, b->box_y2-2);
   Draw(b->bw->RPort, b->box_x1+(b->box_x2-b->box_x1)/2, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y2-2);
   }
if (b->flags&BUTN_DOWN_ARROW) { 
   Move(b->bw->RPort, b->box_x1+4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x1+(b->box_x2-b->box_x1)/2, b->box_y2-2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y1+2);
   }
if (b->flags&BUTN_LEFT_ARROW) { 
   Move(b->bw->RPort, b->box_x2-4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x1+4, b->box_y1+(b->box_y2-b->box_y1)/2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y2-2);
   }
if (b->flags&BUTN_RIGHT_ARROW) { 
   Move(b->bw->RPort, b->box_x1+4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y1+(b->box_y2-b->box_y1)/2);
   Draw(b->bw->RPort, b->box_x1+4, b->box_y2-2);
   }
Permit();
}

/****************************************************************/

void handle_button(b, code, x, y)
struct button *b;
USHORT code;
int x,y;
{
static struct button *releasebutton=NULL;
struct button *temp;

if (code==GE_LBUTTONUP && releasebutton) {
   xor_button(releasebutton);
   temp=releasebutton;
   releasebutton=NULL;
   active_gadget(temp);
   return;
   }

if (code==GE_LBUTTONDOWN) {
   if (hit_button(b, x, y)) {
      xor_button(b);
      if (b->flags&BUTN_TOGGLE) {
         active_gadget(b);
         return;
         } 
      releasebutton=b;
      }
   }
}

/****************************************************************/

hit_button(b, x, y)
struct button *b;
int x,y;
{
return(x>=b->box_x1 && x<=b->box_x2 && y>=b->box_y1 && y<=b->box_y2);
}

/*############################################################
NAME
	xor_button - 
FUNCTION
	Togles that state of BUTN_HIT as well as a button's imagery
INPUTS
	pointer to button gadget
RESULTS
	NONE
##############################################################*/

void xor_button(b)
struct button *b;
{
if (b->flags&BUTN_HIT)
   b->flags &=(~BUTN_HIT);
else
   b->flags |=BUTN_HIT;

if (b->flags&MANUAL_XOR)
   refresh_button(b,b->flags&BUTN_HIT);  
else
   ClipBlit(b->bw->RPort,b->box_x1,b->box_y1,
            b->bw->RPort,b->box_x1,b->box_y1,
            b->box_x2-b->box_x1+1,b->box_y2-b->box_y1+1,0x50);
}

/*############################################################
NAME
	askuser -
FUNCTION
	displays a message and waits for OK or CANCEL to be clicked
INPUTS
	Pass two pieces of text. eg. 
   askuser("Are you Sure","You want to quit?");
   NULL is ok for text not being used.
RESULTS
	TRUE if user clicked OK
   FALSE if user clicked CANCEL
##############################################################*/

askuser(l1, l2)
char *l1,*l2;
{
return(user_notice(l1, l2, TRUE));
}

/*############################################################
NAME
	telluser -
FUNCTION
	displays a message and waits for OK to be clicked
INPUTS
	Pass two pieces of text. eg. telluser("warning:","diskerror");
   NULL is ok for text not being used.
RESULTS
	NONE
##############################################################*/

void telluser(l1, l2)
char *l1,*l2;
{
user_notice(l1, l2, FALSE);
}

/*************************************************************/

user_notice(l1, l2, cancel)
char *l1,*l2;
int cancel;
{
static struct NewWindow NewWindow = {
   0,75,0,50,BLACK, WHITE,0,ACTIVATE|NOCAREREFRESH|BORDERLESS,
   0,0,
   0, /** TITLE **/
   0,0,40,20,640,200,CUSTOMSCREEN};

static struct NewButton okbutn = {
   NULL,20,30,50,45,GAD_BUTTON|BORDER_3D,0,0,WHITE,LIGHT_BROWN, BROWN,
   "OK"};

int width;
struct Window *w;
struct NewGadget *g;

width = max(my_strlen(l1),my_strlen(l2));
width = width*8 + 20;
NewWindow.Width = width;
NewWindow.LeftEdge = (640-width)/2;
NewWindow.Screen = IntuitionBase->ActiveScreen;

lock_gadlib++;

if (w=(struct Window *)OpenWindow(&NewWindow)) {
   draw_indent(w->RPort, 0, 0, w->Width-1,w->Height-1, WHITE, DARK_BROWN);
   Move(w->RPort, 10, 10);
   Text(w->RPort, l1, my_strlen(l1)); 
   Move(w->RPort, 10, 20);
   Text(w->RPort, l2, my_strlen(l2));

   okbutn.box_x1=(w->Width-30)/2;
   okbutn.box_x2=okbutn.box_x1+30;
   if (g=create_gadget(w, &okbutn)) {
      while (WaitForGadget()!= g) ;
      delete_gadget(g);
      }
   CloseWindow(w);
   }
lock_gadlib--;
return(TRUE);
}

/****************************************************************/
/****************************************************************/
/** INPUT.DEVICE ROUTINES                                    ****/
/****************************************************************/
/****************************************************************/


/********************************************************************/

add_gadlib_handler()
{
if ((gadlib_sigbit=AllocSignal(-1))!=-1) {
   if (inputPort=(struct MsgPort *)CreatePort(0,0)) {
      if (inputHandler=(struct Interrupt *)AllocMem(sizeof(struct Interrupt),MEMF_CLEAR|MEMF_PUBLIC)) {
         if (inputReqBlk=(struct IOStdReq *)CreateExtIO(inputPort,sizeof(struct IOStdReq))) {
            if (!OpenDevice("input.device",NULL,inputReqBlk,NULL)) {
               inputHandler->is_Code=inhandler;
               inputHandler->is_Data = NULL;
               inputHandler->is_Node.ln_Pri = 51;  /** IDCMP+1 **/
               inputHandler->is_Node.ln_Name=NameString;
               inputReqBlk->io_Data=(APTR)inputHandler;
               inputReqBlk->io_Command=IND_ADDHANDLER;
               DoIO(inputReqBlk);
               return(1);
               }
            DeleteExtIO(inputReqBlk);
            }
         FreeMem(inputHandler,sizeof(struct Interrupt));
         }
      DeletePort(inputPort);
      }
   FreeSignal(gadlib_sigbit);
   }
return(0);     /** an allocation error occured **/
}

/********************************************************************/

void rem_gadlib_handler()
{
inputReqBlk->io_Data=(APTR)inputHandler;
inputReqBlk->io_Command=IND_REMHANDLER;
DoIO(inputReqBlk);
CloseDevice(inputReqBlk);
DeleteExtIO(inputReqBlk);
FreeMem(inputHandler,sizeof(struct Interrupt));
DeletePort(inputPort);
FreeSignal(gadlib_sigbit);
}

