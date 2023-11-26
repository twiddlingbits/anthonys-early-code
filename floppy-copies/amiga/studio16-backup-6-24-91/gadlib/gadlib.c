/** lc -v -b0 -r0 gadlib.c can be used but it will crash when calling
    graphic.libary unless lcnb.lib is used **/

#include <exec/types.h>
#include <exec/exec.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <intuition/intuition.h>
#include <intuition/intuitionbase.h>
#include <graphics/gfx.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"

/**** global variables *******/


struct MinList gad_list;

struct Task *tc_gadlib;       /** gadlib task's tc **/

short lock_gadlib=0;          /** inc when shouldnt close gadlib */
short doing_init=TRUE;			/** use in init_gadlib **/
short error_during_init=FALSE;

char NameString[]="GadLibHandler";
struct GadEvent event_buffer[512];
short read_ptr=0;    /** Next read by gadlib comes from this index **/
short write_ptr=0;   /** Next event write by handler goes to this index **/
short signal_ok=1;   /** Tell input.device handler to signal us **/
long gadlib_sigbit;  /** signal this task by this bit when new events **/
long gadlib_sigbit3; /** used to signal tc_gadlib to remove itself **/
struct GadgetHeader *last_active_gadget;

struct IOStdReq *inputReqBlk;
struct MsgPort *inputPort;
struct Interrupt *inputHandler;
struct Library  *LayersBase;
struct MsgPort *gadlib_reply_port;
struct SignalSemaphore baselock;
struct SignalSemaphore renderlock;
extern struct IntuitionBase *IntuitionBase;

/***** functions *********/

int user_notice_code();
void handle_inputevent();
struct button *create_button();
struct slider *create_slider();
struct list *create_list();
struct GadgetHeader *create_gadget();
struct Task *MyCreateProc();
void active_gadget();
void inhandler();
void delete_list();
void handle_button();
void handle_slider();
void handle_list();
void handle_gadgets();
void delete_gadget();
void delete_slider();
void delete_button();
void draw_knob();
void drag_knob();
void start_drag_knob();
void draw_indent();
void refresh_slider();
void refresh_knob();  
void calc_knob_size();
void calc_knob_xy();
void calc_top_element();
void scroll_knob();
void scroll_list();
void draw_button();
void refresh_button();
void refresh_list_text();
void refresh_list();
void xor_button();
void move_knob();
void dec_knob();
void inc_knob();
void rem_gadlib_inhandler();
void delete_list_entry();
void SaveRast();
void RestoreRast();
void dl_list_func();
void dl_butn_func();
void refresh_droplist();
struct Window *QuickWindow();
void hide_gadget();
void show_gadget();
void delete_droplist();
void handle_new_select();
struct droplist *create_droplist();
struct list_entry *get_list_entry_ptr();
void refresh_gadget();
void calc_slider_internals();
void main_gadlib();
void killself();
void send_msg();
struct looplist *create_looplist();
void ll_butn_func();
void refresh_looplist();
void delete_looplist();

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
	returns TRUE if ok or FALSE if error
##############################################################*/

BOOL init_gadlib()
{
/*tc_gadlib=MyCreateProc("gadlibtask",1,main_gadlib,6000);*/

tc_gadlib=(struct Task *)CreateTask("gadlibtask",1,main_gadlib,6000);
if (tc_gadlib) {
	while (doing_init && !error_during_init) { /** Let gadlibtask initilize **/
		WaitTOF();
		}
	if (error_during_init) {
		RemTask(tc_gadlib);
		return(FALSE);
		}
	else {
		return(TRUE);
		}
	}
else { 
	return(FALSE);
	}
}

/**************************************************************/

struct Task *MyCreateProc(name,pri,code,stack)
char *name;
int pri;
unsigned long code;
int stack;
{
static unsigned long my_seg[4];
struct MsgPort *gaddfltport;
struct MsgPort *CreatePort();
struct MsgPort *CreateProc();
unsigned long word1;
unsigned long word2;

word1=(code>>16)&0xFFFF;
word2=(code&0xFFFF)<<16;
my_seg[0]=16;         /** segment length in bytes is 4 long words **/
my_seg[1]=0;          /** BPTR to next seg **/
my_seg[2]=0x4EF90000; /* JMP 0000 **/
my_seg[3]=0x00000000; /** part of jmp 00000**/

my_seg[2] |= word1;   /** make jmp code **/
my_seg[3] |= word2;

gaddfltport=CreateProc(name,pri,((unsigned long)(&my_seg[1]))>>2,stack);
if (gaddfltport==0) {
   return(0);
   }
return((struct Task *)((int)gaddfltport - sizeof(struct Task)));
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
Signal(tc_gadlib, 1<<gadlib_sigbit3);	/** kill self **/
}

void killself()
{
while (lock_gadlib>0) 
	WaitTOF();    /** Icky busy loop waiting for tc_gadlib **/
rem_gadlib_inhandler();
CloseLibrary(LayersBase);
DeletePort(gadlib_reply_port);
RemTask(0L);
}


/**************************************************************/

void main_gadlib()  
{
NewList(&gad_list);

LayersBase = (struct Library *)OpenLibrary("layers.library",33L);
if (LayersBase==NULL) {
	error_during_init=TRUE;
	Wait(0);
	}

if (!add_gadlib_inhandler()) {
	error_during_init=TRUE;
   Wait(0);
	}

if ((gadlib_sigbit3=AllocSignal(-1))==-1) { /** RemTask bit **/
	error_during_init=TRUE;
	Wait(0);
	}

if ((gadlib_reply_port=CreatePort(0,0))==0) {
	error_during_init=TRUE;
	Wait(0);
	}

InitSemaphore(&baselock);
InitSemaphore(&renderlock);

doing_init = FALSE;		/** finished initalizing **/

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
struct GadMsg *msg;

mask=Wait( (1<<gadlib_sigbit) | (1<<gadlib_sigbit3) | (1<<gadlib_reply_port->mp_SigBit));

if (mask&(1<<gadlib_reply_port->mp_SigBit)) {
	while (msg = (struct GadMsg *)GetMsg(gadlib_reply_port)) 
		FreeMem(msg,sizeof(struct GadMsg));
	}

if (mask&(1<<gadlib_sigbit3)) {
	killself();
	Wait(0);
	}

do {
   while (read_ptr!=write_ptr) {
      code=event_buffer[read_ptr].type; 
      window = event_buffer[read_ptr].window; 
      x = event_buffer[read_ptr].x;
      y = event_buffer[read_ptr].y;
      if (code==GE_DELTAMOVE && IntuitionBase->ActiveScreen->Height < 400) {
         count += y;
         y = count/2;
         count = count - y*2;
         }
      else {
         count = 0;
         }
      read_ptr=(read_ptr+1)&511;
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
	ng pointer to first NewGadgetHeader structure in this chain
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
else if (ng->flags&GAD_SLIDER)
   ptr = (struct GadgetHeader *)create_slider(w,ng);
else if (ng->flags&GAD_DROPLIST)
   ptr = (struct GadgetHeader *)create_droplist(w,ng);
else if (ng->flags&GAD_LOOPLIST)
   ptr = (struct GadgetHeader *)create_looplist(w,ng);

if (ptr) {
	ObtainSemaphore(&baselock);
   AddTail(&gad_list, ptr);
	ReleaseSemaphore(&baselock);
	}
ng->gad_ptr=ptr;
return(ptr);
}

/*############################################################
NAME
	refresh_gadget -
FUNCTION
	redraw's a gadget's imagery
INPUTS
	g  pointer to a GadgetHeader struct
RESULTS
	NONE
##############################################################*/

void refresh_gadget(g)
struct GadgetHeader *g;
{
if (g->flags&GAD_LIST)	refresh_list(g);
else if (g->flags&GAD_BUTTON) refresh_button(g);
else if (g->flags&GAD_SLIDER) refresh_slider(g);
else if (g->flags&GAD_DROPLIST) refresh_droplist(g);
else if (g->flags&GAD_LOOPLIST) refresh_looplist(g);
}

/*############################################################
NAME
	refresh_gadget_chain -
FUNCTION
	refresh's all gadgets in the specified window
INPUTS
	w  pointer to window 
RESULTS
	NONE
##############################################################*/

void refresh_gadget_chain(w)
struct Window *w;
{
struct MinNode *node;
struct GadgetHeader *g;

ObtainSemaphore(&baselock);
for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   g=(struct GadgetHeader *)node;
   if (w==g->w && !(g->flags&GAD_SIBLING))
      refresh_gadget(g);
   }
ReleaseSemaphore(&baselock);
}

/*############################################################
NAME
	delete_gadget_chain -
FUNCTION
	delete's all gadgets in the specified window
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

ObtainSemaphore(&baselock);
node=gad_list.mlh_Head;
while(node->mln_Succ) {
   g=(struct GadgetHeader *)node;
	node = node->mln_Succ;
   if (w==g->w && !(g->flags&GAD_SIBLING)) {
      delete_gadget(g);
		}
	}
ReleaseSemaphore(&baselock);
}

/*############################################################
NAME
	delete_gadget -
FUNCTION
	deletes the specified gadget. Note, attached gadgets are
	also deleted.  For example, deleting a list gadget will
	delete a slider and two buttons.
INPUTS
	g pointer to GadgetHeader struct to delete (not NewGadget!)
RESULTS
	NONE
##############################################################*/

void delete_gadget(g)
struct GadgetHeader *g;
{
if (g->flags&GAD_LIST)
   delete_list(g);
else if (g->flags&GAD_BUTTON)
   delete_button(g);
else if (g->flags&GAD_SLIDER)
   delete_slider(g);
else if (g->flags&GAD_DROPLIST)
   delete_droplist(g);
else if (g->flags&GAD_LOOPLIST)
   delete_looplist(g);
}

/**************************************************************/

void handle_gadgets(window, code, x, y)
struct Window *window;
{
struct MinNode *node;
unsigned long flags;
ObtainSemaphore(&baselock);
for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (window==((struct GadgetHeader *)node)->w) {
		flags=((struct GadgetHeader *)node)->flags;
		if (!(flags&HIDE_GADGET)) {
			if (flags&GAD_LIST) 
      	  handle_list(node, code, x, y);
			else if (flags&GAD_BUTTON)
				handle_button(node, code, x, y);
			else if (flags&GAD_SLIDER)
				handle_slider(node, code, x, y); 
			}
      }
   }
ReleaseSemaphore(&baselock);
}

/**************************************************************/

void active_gadget(g)
struct GadgetHeader *g;
{
if (g->w && g->func)
	SaveRast(g->w->RPort);
if (g->func)
   (*g->func)(g);
if (g->w && g->func)
	RestoreRast();
if (g->notifyme)
	send_msg(g->notifyme, g, GADMSG_ACTIVE, g->flags);
last_active_gadget = g;
}

/**************************************************************/
/***** Slider Handeling Routines ********************************/
/**************************************************************/

struct slider *create_slider(w,nk)
struct Window *w;
struct NewSlider *nk;
{
struct slider *k;
struct NewButton arrow_button;


setmem(&arrow_button,sizeof(struct NewButton),0);

k = (struct slider *)AllocMem(sizeof(struct slider), MEMF_CLEAR);
if (k==0)
   return(0);


k->box_x1=nk->box_x1;
k->box_y1=nk->box_y1;
k->box_x2=nk->box_x2;
k->box_y2=nk->box_y2;
k->flags = nk->flags;
k->kw = w;
k->id=nk->id;
k->c=nk->c;		/* colors */
k->notifyme=nk->notifyme;

if (k->flags&SLIDER_ARROWS) {
   if (k->flags&FREE_HORIZ) {
      k->box_x2 -= (STD_H_ARROW_SIZE*2+6);
      arrow_button.box_x1=k->box_x2+3;
      arrow_button.box_x2=arrow_button.box_x1+STD_H_ARROW_SIZE;
      arrow_button.box_y1=k->box_y1;
      arrow_button.box_y2=k->box_y2;
      arrow_button.flags = BUTN_COOL|BUTN_LEFT_ARROW|HIDE_GADGET|GAD_SIBLING|(k->flags&MANUAL_XOR); /** refresh_slider removes HIDE_GADGET */
      arrow_button.func = dec_knob;
      arrow_button.user_data = (APTR)k;
		arrow_button.c=k->c;
      k->b1=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b1==0) {
         FreeMem(k, sizeof(struct slider));
         return(0);
         }
      arrow_button.box_x1=arrow_button.box_x2+3;
      arrow_button.box_x2=arrow_button.box_x1+STD_H_ARROW_SIZE;
      arrow_button.flags = BUTN_COOL|BUTN_RIGHT_ARROW|HIDE_GADGET|GAD_SIBLING|(k->flags&MANUAL_XOR);
      arrow_button.func = inc_knob;
      k->b2=(struct button *)create_gadget(k->kw,&arrow_button);
      if (k->b2==0) {
         delete_button(k->b1);
         FreeMem(k, sizeof(struct slider));
         return(0);
         }
      }
   if (k->flags&FREE_VERT) {
      k->box_y2 -= (STD_V_ARROW_SIZE*2+3);
      arrow_button.box_x1=k->box_x1;
      arrow_button.box_x2=k->box_x2;
      arrow_button.box_y1=k->box_y2+2;
      arrow_button.box_y2=arrow_button.box_y1+STD_V_ARROW_SIZE;
      arrow_button.flags = BUTN_COOL|BUTN_UP_ARROW|HIDE_GADGET|GAD_SIBLING|(k->flags&MANUAL_XOR);
      arrow_button.func = dec_knob;
      arrow_button.user_data = (APTR)k;
		arrow_button.c=k->c;
      k->b1=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b1==0) {
         FreeMem(k, sizeof(struct slider));
         return(0);
         }
      
      arrow_button.box_y1=arrow_button.box_y2+1;
      arrow_button.box_y2=arrow_button.box_y1+STD_V_ARROW_SIZE;
      arrow_button.flags = BUTN_COOL|BUTN_DOWN_ARROW|HIDE_GADGET|GAD_SIBLING|(k->flags&MANUAL_XOR);
      arrow_button.func = inc_knob;
      k->b2=(struct button *)create_gadget(k->kw, &arrow_button);
      if (k->b2==0) {
         delete_button(k->b1);
         FreeMem(k, sizeof(struct slider));
         return(0);
         }
      }
   }

k->show_size = nk->show_size;
k->list_size = nk->list_size;
k->top_element = nk->top_element;
k->func = nk->func;
k->user_data = nk->user_data;

calc_slider_internals(k,nk);
refresh_slider(k);
return(k);
}
/*############################################################
NAME	
	calc_slider_internals -

FUNCTION
	Recalculates a knob's x,y,width, and height based on
	box_x and box_y's and AUTOKNOB.  Box_x and box_y may also
	be affected if BORDER_3D is selected.
	
	Call calc_slider_internals() after you change box_x & box_y.
	For example, if the user resizes your window you may wish
	to change the size of a slider.  You can do this by altering
	slider->box_x1 through slider->box_y2.  Then call 
	calc_slider_internals(); followed by a call to refresh_gadget_chain()
	or refresh_gadget.
INPUTS
	s  pointer to slider to recalc
	ns pointer to a NewSlider struct.  ns.knob_width and ns.knob_height
		are used.
RESULTS
	NONE
##############################################################*/

void calc_slider_internals(s,ns)
struct slider *s;
struct NewSlider *ns;
{
if (s->flags&BORDER_3D) {
   s->box_x1 += 2;
   s->box_x2 -= 2;
   s->box_y1 += 1;
   s->box_y2 -= 1;
   }

calc_knob_size(s);

if (!(s->flags&AUTO_KNOB)) {
   if (ns->knob_width!=-1) 
      s->knob_width = ns->knob_width;
	if (ns->knob_height!=-1)
		s->knob_height = ns->knob_height;
	}
calc_knob_xy(s);
}


/****************************************************************/

void delete_slider(k)
struct slider *k;
{
ObtainSemaphore(&baselock);
Remove(k);     /** this is the same as Remove(&k->node) **/
ReleaseSemaphore(&baselock);
if (k->flags&SLIDER_ARROWS) {
	delete_button(k->b1);
	delete_button(k->b2);
	}
FreeMem(k, sizeof(struct slider));
}

/****************************************************************/

/*############################################################
NAME
	refresh_slider -
FUNCTION
	Redraws a slider's imagery.  This includes it's 3D border, knob
	and hitbox.  
INPUTS
	k pointer to the slider to redraw
RESULTS
	NONE
##############################################################*/

void refresh_slider(k)  

struct slider *k;
{
if (!(k->flags&HIDE_GADGET)) {
	if (k->flags&BORDER_3D) {
	   draw_indent(k->kw->RPort, k->box_x1-2, k->box_y1-1, k->box_x2+2, k->box_y2+1,k->c->slider_box.topleft_3d,k->c->slider_box.bottomright_3d);
	   } 
	refresh_knob(k);
	if (k->flags&SLIDER_ARROWS) {
		k->b1->flags &= (~HIDE_GADGET);
		k->b2->flags &= (~HIDE_GADGET);
		refresh_button(k->b1);
		refresh_button(k->b2);	
		}
	}
}

/*############################################################
NAME
	refresh_knob -
FUNCTION
	Redraws a knob's imagery & the hitbox where it can drag.
	Call refresh_knob if you change the size of the knob or the
	position of the knob and you want to display the new knob.
	Since refresh_knob updates the hitbox (or slider area) of
	the slider, the old knob will be erased.
INPUTS
	k pointer to the slider to redraw
RESULTS
	NONE
##############################################################*/

void refresh_knob(k)  

struct slider *k;
{
if (!(k->flags&HIDE_GADGET)) {
	SaveRast(k->kw->RPort);
	SetAPen(k->kw->RPort, k->c->slider_box.hitbox_unselected);
	RectFill(k->kw->RPort,k->box_x1, k->box_y1, k->box_x2, k->box_y2);
	draw_knob(k,k->c->slider_knob.hitbox_unselected);
	RestoreRast();
	}
}

/****************************************************************/

void calc_knob_size(k)
struct slider *k;
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
struct slider *k;
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
struct slider *k;
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

void handle_slider(k, code, x, y)
struct slider *k;
USHORT code;
int x,y;
{
extern struct slider *draging_slider;

if (draging_slider==k) {
   drag_knob(k, code, x, y);
   return;
   }

if (code==GE_LBUTTONDOWN) {
   if (hit_knob(k, x, y)) {
      start_drag_knob(k, x, y);
      }
   else {
      if (hit_knob_up(k, x, y) || hit_knob_left(k, x, y)) {
         k->top_element=max(0,k->top_element-k->show_size);
         move_knob(k); 
         }
      else if (hit_knob_down(k, x, y) || hit_knob_right(k, x, y)) {
         k->top_element=min(k->list_size-1,k->top_element+k->show_size);
         move_knob(k);
         }
      }
   }
}
/*******************************************************************/
/** called by an up arrow button attached to a knob **/

void dec_knob(b)
struct button *b;
{
struct slider *k;
k=(struct slider *)b->user_data;
k->top_element--;
move_knob(k);
}

/*******************************************************************/
/** called by a down arrow button attached to a knob **/

void inc_knob(b)
struct button *b;
{
struct slider *k;
k=(struct slider *)b->user_data;
k->top_element++;
move_knob(k);
}

/*############################################################
NAME
	move_knob -
FUNCTION
	after changing a slider's top_element call this to move
	the knob imgary
INPUTS
	k  pointer to a slider structure
RESULTS
	NONE
##############################################################*/

void move_knob(k)
struct slider *k;
{
int old_knob_x;
int old_knob_y;

if (k->top_element >= (k->list_size-k->show_size))
   k->top_element = k->list_size-k->show_size;
if (k->top_element < 0) k->top_element = 0;
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
static struct slider *draging_slider = NULL;

/*******************************************************************/

void start_drag_knob(k, x, y)
struct slider *k;
int x,y;
{
draging_slider = k;
global_old_knob_x = k->knob_x;
global_old_knob_y = k->knob_y;
global_old_top = k->top_element;
if (k->c->slider_knob.hitbox_selected != k->c->slider_knob.hitbox_unselected)
   draw_knob(k,k->c->slider_knob.hitbox_selected);   /* knob can change color when being draged */
}

/*******************************************************************/

void drag_knob(k, code, newx, newy)
USHORT code;
int newy, newx;
struct slider *k;
{
/*** NEED TO EAT EVENTS???? ***/

if (code==GE_LBUTTONUP) {
   if (k->c->slider_knob.hitbox_selected != k->c->slider_knob.hitbox_unselected)
      draw_knob(k,k->c->slider_knob.hitbox_unselected);   /* knob can change color when being draged */
   draging_slider = NULL;
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
   calc_top_element(k);
   if (global_old_top!=k->top_element)
      active_gadget(k);
   global_old_top = k->top_element;
   }
}

/************************************************************/

void scroll_knob(k, old_knob_x, old_knob_y)
struct slider *k;
int old_knob_x;
int old_knob_y;
{
SaveRast(k->kw->RPort);
SetBPen(k->kw->RPort,k->c->slider_box.hitbox_unselected);

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
RestoreRast();
}

/*############################################################
NAME	-
	draw_knob -
FUNCTION
	Redraws a knob's imagery.  The sliders hitbox is not updated.
	Use this if you want to change the color of the knob, or you
	have trashed the part of the knob's imagery.
INPUTS
	k - pointer to the slider that this knob is in
	color - the new color for the knob
RESULTS
	NONE
##############################################################*/

void draw_knob(k,color)
struct slider *k;
int color;
{
struct RastPort *rp;
short x1,x2,y1,y2;

rp = k->kw->RPort;
SaveRast(rp);
SetAPen(rp, color);
if (k->flags&KNOB_3D)
   RectFill(rp,k->knob_x+2, k->knob_y+1, k->knob_x+k->knob_width-3, k->knob_y+k->knob_height-2);
else
   RectFill(rp,k->knob_x, k->knob_y, k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1);

if (k->flags&KNOB_3D) {
   draw_indent(rp, k->knob_x, k->knob_y,
       k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1,
       k->c->slider_knob.topleft_3d,k->c->slider_knob.bottomright_3d);
	}

if (k->flags&KNOB_TODDISH) {
	x1=k->knob_x+k->knob_width/2-KNOB_TODDISH_SIZE;
	x2=x1+KNOB_TODDISH_SIZE*2;
	y1=k->knob_y+k->knob_height/2-KNOB_TODDISH_SIZE/2;
	y2=y1+KNOB_TODDISH_SIZE;
   draw_indent(rp, x1,y1,x2,y2,k->c->slider_knob.bottomright_3d,k->c->slider_knob.topleft_3d);
   }

active_gadget(k);
RestoreRast();
}

/****************************************************************/

int hit_knob(k, x, y)

struct slider *k;
int x,y;
{
return(x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->knob_y && y <=(k->knob_y+k->knob_height));
}

/****************************************************************/

hit_knob_up(k, x, y)

struct slider *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->box_y1 && y < k->knob_y);
}

/****************************************************************/

hit_knob_down(k, x, y)

struct slider *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y > (k->knob_y+k->knob_height) && y <= k->box_y2);
}
/****************************************************************/

hit_knob_left(k, x, y)

struct slider *k;
int x,y;
{
return(k->flags&FREE_HORIZ && y>=k->knob_y && y<= (k->knob_y+k->knob_height-1) && x >= k->box_x1 && x < k->knob_x);
}

/****************************************************************/

hit_knob_right(k, x, y)

struct slider *k;
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
int select;
int select_len;
struct list_entry *le;

if (!(x>=l->box_x && x<= (l->box_x2-STD_SLIDER_WIDTH-STD_SLIDER_GAP) && y>=l->box_y && y<=l->box_y2))
	return;

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
				handle_new_select(l, le, select);
				}
         }
      }
   }
}

/****************************************************************/

void handle_new_select(l, le,  select)

struct list *l;
struct list_entry *le;
int select;
{
struct MinNode *node;

if (le->flags&LE_SELECTED) {
	if (l->flags&LIST_SINGLESELECT) {
		l->flags|=LIST_DOUBLESELECT;
		active_gadget(l);
		l->flags&=(~LIST_DOUBLESELECT);
		}
	else {
		le->flags &= (~LE_SELECTED);
		refresh_list_text(l);
		l->flags|=LIST_NEWSELECT;
		active_gadget(l);
		l->flags&=(~LIST_NEWSELECT);
		}
	}
else {		/* selecting an entry not already selected */
	if (l->flags&LIST_SINGLESELECT) {
		/** If only one entry allowed selected, clear all other entries */
		for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) 
			((struct list_entry *)node)->flags &= (~LE_SELECTED);
		}
	le->flags |= LE_SELECTED;
	l->active_entry = le;
	l->current = select;
	refresh_list_text(l);
	l->flags|=LIST_NEWSELECT;
	active_gadget(l);
	l->flags&=(~LIST_NEWSELECT);
	}
}
               
/****************************************************************/

struct list *create_list(w, nl)
struct Window *w;
struct NewList *nl;
{
struct NewSlider standard_list_slider;
struct list *l;
int i;

l = (struct list *)AllocMem(sizeof(struct list), MEMF_CLEAR);
if (l==0) return(0);

setmem(&standard_list_slider, sizeof(struct NewSlider), 0);

NewList(&l->entries);     /** chain of items being listed **/
l->box_x = nl->box_x1;
l->box_y = nl->box_y1;
l->box_x2 = nl->box_x2;
l->box_y2 = nl->box_y2;
l->show_size = (nl->box_y2 - nl->box_y1 - 6)/8; /** change to font height! */
l->lw = w;
l->id=nl->id;
l->c=nl->c;
l->notifyme=nl->notifyme;
l->max_show_chars = (nl->box_x2 - nl->box_x1 - STD_SLIDER_WIDTH - STD_SLIDER_GAP - 3)/8;
l->flags = nl->flags;
l->func = nl->func;
l->user_data = nl->user_data;
l->current=-1; /* -1 means no current item */

standard_list_slider.box_x1 = nl->box_x2 - STD_SLIDER_WIDTH + 1;
standard_list_slider.box_x2 = nl->box_x2;
standard_list_slider.box_y1 = nl->box_y1;
standard_list_slider.box_y2 = nl->box_y2;
standard_list_slider.c = nl->c;
standard_list_slider.list_size = l->list_size;
standard_list_slider.show_size = l->show_size;
standard_list_slider.top_element = l->top_element;
standard_list_slider.func = scroll_list;
standard_list_slider.user_data = (APTR)l;
standard_list_slider.flags=GAD_SLIDER|AUTO_KNOB|FREE_VERT|GAD_SIBLING|(nl->flags&(KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH|KNOB_NOGAP|HIDE_GADGET));
l->slider = (struct slider *)create_gadget(l->lw, &standard_list_slider);
if (l->slider==0) {
   FreeMem(l, sizeof (struct list));
   return(0);
   }

refresh_list(l);

if (nl->e_text) {		/** Add pre-initilized list entris **/
	i=0;
	while (nl->e_text[i][0]!=0) {
	   add_list_entry(l, nl->e_text[i], nl->e_user_data ? nl->e_user_data[i] : 0);  
		i++;
		}
	if (nl->initial_value < i && nl->initial_value!=-1) {
		handle_new_select(l,get_list_entry_ptr(l, nl->initial_value),nl->initial_value);
		}
	}
return(l);
}
/****************************************************************/

void delete_list(l)
struct list *l;
{
while(l->list_size) 
   delete_list_entry(l,0);
ObtainSemaphore(&baselock);
Remove(l);     /** this is the same as Remove(&l->node) **/
ReleaseSemaphore(&baselock);
delete_slider(l->slider);
FreeMem(l, sizeof(struct list));
}


/****************************************************************/
/** called by slider handler whenever top_element changes        **/
/****************************************************************/

void scroll_list(k)
struct slider *k;
{
struct list *l;

l = (struct list *)k->user_data;
l->top_element = k->top_element;
refresh_list_text(l);
}


/*############################################################
NAME
	refresh_list -
FUNCTION
	redraws a struct list's complete imagery.  This includes the
	associated slider.
INPUTS
	l pointer to the list structure to update
RESULTS
	NONE
##############################################################*/

void refresh_list(l)
struct list *l;
{
if (!(l->flags&HIDE_GADGET)) {
	SaveRast(l->lw->RPort);
	if (l->flags&GAD_SIBLING)	/** must be droplist!!!! AGHHH NEED BETER WAY **/
		SetAPen(l->lw->RPort, l->c->droplist.hitbox_unselected);
	else
		SetAPen(l->lw->RPort, l->c->list.hitbox_unselected);
	RectFill(l->lw->RPort, l->box_x, l->box_y,
	            l->box_x2-STD_SLIDER_WIDTH-STD_SLIDER_GAP, l->box_y2);

	RestoreRast();
	if (l->flags&GAD_SIBLING)	/** must be droplist!!!! AGHHH NEED BETER WAY **/
		draw_indent(l->lw->RPort, l->box_x, l->box_y,
		            l->box_x2-STD_SLIDER_WIDTH-STD_SLIDER_GAP, l->box_y2,l->c->droplist.topleft_3d,l->c->droplist.bottomright_3d);
	else
		draw_indent(l->lw->RPort, l->box_x, l->box_y,
		            l->box_x2-STD_SLIDER_WIDTH-STD_SLIDER_GAP, l->box_y2,l->c->list.topleft_3d,l->c->list.bottomright_3d);
	refresh_list_text(l);
	l->slider->flags &= (~HIDE_GADGET);
	refresh_slider(l->slider);
	}
}


/*############################################################
NAME
	refresh_list_text -
FUNCTION
	redraws a struct list's entries' text
INPUTS
	l pointer to the list structure to update
RESULTS
	NONE
##############################################################*/

void refresh_list_text(l)
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

if (l->flags&HIDE_GADGET) 	
	return;

rp = l->lw->RPort;
SaveRast(rp);

if (l->flags&GAD_SIBLING) {
	SetAPen(rp, l->c->droplist.draw_unselected);
	SetBPen(rp, l->c->droplist.hitbox_unselected);
	}
else {
	SetAPen(rp, l->c->list.draw_unselected);
	SetBPen(rp, l->c->list.hitbox_unselected);
	}
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
   if (le->flags&LE_SELECTED) {
		if (l->flags&GAD_SIBLING) {
			SetAPen(rp, l->c->droplist.draw_selected);
			SetBPen(rp, l->c->droplist.hitbox_selected);
			}
		else {
			SetAPen(rp, l->c->list.draw_selected);
			SetBPen(rp, l->c->list.hitbox_selected);
			}
      }
   Move(rp, l->box_x+3, l->box_y+(i-l->top_element)*9+8);
   len=my_strlen(le->text);
   if (len > l->max_show_chars)
      len = l->max_show_chars;
   Text(rp, le->text, len);
   if (le->flags&LE_SELECTED) {
		if (l->flags&GAD_SIBLING) {
			SetAPen(rp, l->c->droplist.draw_unselected);
			SetBPen(rp, l->c->droplist.hitbox_unselected);
			}
		else {
			SetAPen(rp, l->c->list.draw_unselected);
			SetBPen(rp, l->c->list.hitbox_unselected);
			}
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
RestoreRast();
}

/****************************************************************/
/** re-size & draw a list's slider ***/
/****************************************************************/

void refresh_list_slider(l)
struct list *l;
{
struct slider*k;

k=l->slider;
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
char *mem;
int len;
struct list_entry *tle,*le;
struct MinNode *node;

/* printf("add_list:l %x c %s p %x\n",l,c,p); */

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

/** Alphabetize list **/

node=l->entries.mlh_Head;
while(node->mln_Succ) {
   tle=(struct list_entry *)node;
	if (stricmp(c,tle->text) < 0)
		break;
	node = node->mln_Succ;
	}

ObtainSemaphore(&baselock);
Insert(&l->entries, le, node->mln_Pred);
ReleaseSemaphore(&baselock);
l->active_entry=le;
l->current = l->list_size-1;
l->flags|=LIST_NEWSELECT;
active_gadget(l);
l->flags&=(~LIST_NEWSELECT);
refresh_list_text(l);
refresh_list_slider(l);
/* printf("done add l %x cur %d\n",l,l->current); */
return(NO_ERROR);
}

/*############################################################
NAME	
	delete_list_entry -
FUNCTION
	removes an entry from sturct *list.
INPUTS
	list, entry index (0 - list_size-1)
RESULTS
	NONE
##############################################################*/

void delete_list_entry(l, e)

struct list *l;	/* list */
int e;   			/* entry index */
{
int len;
struct list_entry *le;
struct list_entry *newcurle;
short needrefresh;

if (e < 0 || e >= l->list_size)		/* avoid them crashes */
	return;

/* printf("dle: l %x e %d cur %d\n",l,e,l->current); */

if (le=get_list_entry_ptr(l, e)) {
   if (e==l->current) {
      newcurle = NULL;
      needrefresh = TRUE;
      }
   else {
      newcurle = get_list_entry_ptr(l,l->current);
      needrefresh = FALSE;
      }
	
	ObtainSemaphore(&baselock);
   Remove(le);    /* &le->node */
	ReleaseSemaphore(&baselock);
   l->list_size--;
   if (l->list_size < l->show_size) {
      l->top_element=0;
      }
   else {
      if (l->top_element >= (l->list_size-l->show_size))
         l->top_element = l->list_size-l->show_size;
      }
   len = my_strlen(le->text)+1;
   if (len) {
      FreeMem(le->text, len);
      }
   FreeMem(le, sizeof(struct list_entry));
   l->current = get_list_entry_num(l, newcurle);
   l->active_entry=get_list_entry_ptr(l, l->current);
   if (needrefresh) {
      l->flags|=LIST_NEWSELECT;
      active_gadget(l);
      l->flags&=(~LIST_NEWSELECT);
      }
   refresh_list_text(l);
   refresh_list_slider(l);
   return;
   }
}
/*############################################################
NAME	
	delete_list_entry_by_userdata-

FUNCTION
	Searches a list and delete's any entries that have user_data
	set to the specified value.
INPUTS
	list, user_data
RESULTS
	int (count of entries that matched user_data)
##############################################################*/

int delete_list_entry_by_userdata(l, ud)
struct list *l;
unsigned long ud;			/* match this to l->user_data */
{
struct MinNode *node;
int index,count;

index=count=0;
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if ( ((struct list_entry *)node)->user_data==ud) {
		delete_list_entry(l, index);
		count++;
		}
	index++;
   }
return(count);
}

/*############################################################
NAME 
      get_list_entry_ptr -

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

if (e < 0)
   return(NULL);

for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (e--<=0) 
      return((struct list_entry *)node);
   }
return(NULL);
}

/***********************************************************/
int get_list_entry_num(l, le)
struct list *l;
struct list_entry *le;
{
struct MinNode *node;
int i;

i=0;
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (le==(struct list_entry *)node)
      return(i);
   i++;
   }
return(-1);
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
SaveRast(rp);
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

RestoreRast();
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
b->id=nb->id;
b->c=nb->c;
b->notifyme=nb->notifyme;
b->flags = nb->flags;
b->func = nb->func;
b->user_data = nb->user_data;

refresh_button(b);
return(b);
}

/****************************************************************/

void delete_button(b)
struct button *b;
{
ObtainSemaphore(&baselock);
Remove(b);     /** this is the same as Remove(&b->node) **/
ReleaseSemaphore(&baselock);
if (b->text) {
   FreeMem(b->text, my_strlen(b->text)+1);
	b->text=NULL;
	}
FreeMem(b, sizeof(struct button));
}

/*############################################################
NAME	-
	refresh_button
FUNCTION
	refreshes a button's imagery.  Takes into account MANUAL_XOR flags
	and BUTN_HIT.
INPUTS
	b - button to refresh
RESULTS
	NONE
##############################################################*/

void refresh_button(b)
struct button *b;
{
if (!(b->flags&HIDE_GADGET)) {
	if (b->flags&MANUAL_XOR) {
	   draw_button(b,b->flags&BUTN_HIT);
	   }
	else {
	   draw_button(b, FALSE);
	   if (b->flags&BUTN_HIT) {
	      ClipBlit(b->bw->RPort,b->box_x1,b->box_y1,
	               b->bw->RPort,b->box_x1,b->box_y1,
	               b->box_x2-b->box_x1+1,b->box_y2-b->box_y1+1,0x50);
			}
		}
	}
}

/*############################################################
NAME	-
	draw_button -
FUNCTION
	Renders a button and its associated text.
INPUTS
	b - button to render
	selected - use selected or unselected colors? (TRUE or FALSE)
RESULTS
	NONE
##############################################################*/

void draw_button(b,selected)  

struct button *b;
int selected;
{
static USHORT chip ImageData1[] = {
	~0xF3C0,~0xF3C0,~0x9240,~0xC0C0,~0xE1C0,~0xF3C0,~0x0000};

static USHORT chip LoopData[] = {
	0x7F00,0xC180,0xC180,0xC7E0,0xC3C0,0xC180,0xC000,0xC180,0x7F00};
struct BitMap mybm;
	
int xpos, ypos, len;
short apen, bpen, top_col, bot_col;
short mirror;

if (b->flags&HIDE_GADGET)
	return;

SaveRast(b->bw->RPort);
if (selected) {
   apen=b->c->button.draw_selected;	
   bpen=b->c->button.hitbox_selected;
   top_col =  b->c->button.bottomright_3d;
   bot_col =  b->c->button.topleft_3d;
   }
else {
   apen=b->c->button.draw_unselected;
   bpen=b->c->button.hitbox_unselected;
   top_col = b->c->button.topleft_3d;
   bot_col = b->c->button.bottomright_3d;
   }

SetAPen(b->bw->RPort, bpen);
RectFill(b->bw->RPort,b->box_x1, b->box_y1, b->box_x2, b->box_y2);
if (b->flags&BORDER_3D) {
   draw_indent(b->bw->RPort, b->box_x1, b->box_y1, b->box_x2, b->box_y2, top_col, bot_col);
   }

if (b->text) {
   len = my_strlen(b->text);
   ypos = b->box_y1+(b->box_y2 - b->box_y1+6)/2;
	if (b->flags&BUTN_X)	{	/** X boxes's text goes to the right of box */
		SetAPen(b->bw->RPort, b->c->button.draw_unselected);
		SetDrMd(b->bw->RPort, JAM1);
	   xpos = b->box_x2+4;
		}
	else if (b->flags&BUTN_DROP_ARROW) {
		SetAPen(b->bw->RPort, apen);
		SetBPen(b->bw->RPort, b->c->droplist.hitbox_unselected);
		SetDrMd(b->bw->RPort, JAM2);
		xpos = b->box_x1 - 4 - 8*len - STD_SLIDER_GAP;
		}
	else if (b->flags&BUTN_LOOP) {
	   xpos = 19 + b->box_x1;
		SetAPen(b->bw->RPort, apen);
		SetDrMd(b->bw->RPort, JAM1);
		}
	else {
	   xpos = b->box_x1+(b->box_x2 - b->box_x1 - len*8)/2;
		SetAPen(b->bw->RPort, apen);
		SetDrMd(b->bw->RPort, JAM1);
		}
   Move(b->bw->RPort, xpos, ypos);
   Text(b->bw->RPort, b->text, len);
   }

SetAPen(b->bw->RPort, apen);

if (b->flags&BUTN_UP_ARROW) { 
	mirror = (b->box_x2-b->box_x1)/2 - 4;
   Move(b->bw->RPort, b->box_x1+4, b->box_y2-2);
   Draw(b->bw->RPort, b->box_x1+4+mirror, b->box_y1+2);
   Move(b->bw->RPort, b->box_x1+4+mirror*2, b->box_y2-2);
   Draw(b->bw->RPort, b->box_x1+4+mirror, b->box_y1+2);
   }
else if (b->flags&BUTN_DOWN_ARROW) { 
	mirror = (b->box_x2-b->box_x1)/2 - 4;
   Move(b->bw->RPort, b->box_x1+4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x1+4+mirror, b->box_y2-2);

   Move(b->bw->RPort, b->box_x1+4+mirror*2, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x1+4+mirror, b->box_y2-2);
   }
else if (b->flags&BUTN_LEFT_ARROW) { 
   mirror=(b->box_y2-b->box_y1)/2 - 2;
   Move(b->bw->RPort, b->box_x2-4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x1+4, b->box_y1+2+mirror);
   Move(b->bw->RPort, b->box_x2-4, b->box_y1+2+mirror*2);
   Draw(b->bw->RPort, b->box_x1+4, b->box_y1+2+mirror);
   }
else if (b->flags&BUTN_RIGHT_ARROW) { 
   mirror=(b->box_y2-b->box_y1)/2 - 2;
   Move(b->bw->RPort, b->box_x1+4, b->box_y1+2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y1+2+mirror);
   Move(b->bw->RPort, b->box_x1+4, b->box_y1+2+mirror*2);
   Draw(b->bw->RPort, b->box_x2-4, b->box_y1+2+mirror);
   }
else if (b->flags&BUTN_X && b->flags&BUTN_HIT) {
	Move(b->bw->RPort, b->box_x1+4, b->box_y1+2);
	Draw(b->bw->RPort, b->box_x2-4, b->box_y2-2);
	Move(b->bw->RPort, b->box_x2-4, b->box_y1+2);
	Draw(b->bw->RPort, b->box_x1+4, b->box_y2-2);
	}
else if (b->flags&BUTN_DROP_ARROW) {
	setmem(&mybm,sizeof(struct BitMap),0);
	mybm.BytesPerRow = 2;
	mybm.Rows = 7;
	mybm.Planes[0] = (PLANEPTR)ImageData1;
	mybm.Planes[1] = (PLANEPTR)ImageData1;
	mybm.Planes[2] = (PLANEPTR)ImageData1;
	mybm.Planes[3] = (PLANEPTR)ImageData1;
	mybm.Planes[4] = (PLANEPTR)ImageData1;
	mybm.Depth = b->bw->RPort->BitMap->Depth;

	/** first, cut shape out of all planes **/
	BltBitMap(&mybm,0,0,b->bw->RPort->BitMap,b->bw->LeftEdge+b->box_x1+4,
	b->bw->TopEdge+b->box_y1+3,10,7,0x20,0xFF,NULL);

	/** now, OR shape into planes specified by drawing color **/
	BltBitMap(&mybm,0,0,b->bw->RPort->BitMap,b->bw->LeftEdge+b->box_x1+4,
	b->bw->TopEdge+b->box_y1+3,10,7,0xe0,apen,NULL);
   }
else if (b->flags&BUTN_LOOP) {
	setmem(&mybm,sizeof(struct BitMap),0);
	mybm.BytesPerRow = 2;
	mybm.Rows = 9;
	mybm.Planes[0] = (PLANEPTR)LoopData;
	mybm.Planes[1] = (PLANEPTR)LoopData;
	mybm.Planes[2] = (PLANEPTR)LoopData;
	mybm.Planes[3] = (PLANEPTR)LoopData;
	mybm.Planes[4] = (PLANEPTR)LoopData;
	mybm.Depth = b->bw->RPort->BitMap->Depth;

   ypos = b->bw->TopEdge + b->box_y1+(b->box_y2 - b->box_y1+1-9)/2;

	/** first, cut shape out of all planes **/
	BltBitMap(&mybm,0,0,b->bw->RPort->BitMap,b->bw->LeftEdge+b->box_x1+4,
	ypos,11,9,0x20,0xFF,NULL);

	/** now, OR shape into planes specified by drawing color **/
	BltBitMap(&mybm,0,0,b->bw->RPort->BitMap,b->bw->LeftEdge+b->box_x1+4,
	ypos,11,9,0xe0,apen,NULL);
   }
RestoreRast();
}

/****************************************************************/

void handle_button(b, code, x, y)
struct button *b;
USHORT code;
int x,y;
{
static struct button *releasebutton=NULL;
struct button *temp;
struct MinNode *node;

if (code==GE_LBUTTONUP && releasebutton) {
   xor_button(releasebutton);
   temp=releasebutton;
   releasebutton=NULL;
   active_gadget(temp);		//**!!!!! SHOULD I SET/RESET BUTN_HIT AROUND THIS?? **/
   return;
   }


if (code==GE_LBUTTONDOWN) {
   if (hit_button(b, x, y)) {
      if (!(b->flags&BUTN_TOGGLE)) {
	      xor_button(b);
			}
		else {			/** BUTN_TOGGLE **/
			if (!(b->flags&KEEP_ONE_ACTIVE && b->flags&BUTN_HIT))
		      xor_button(b);
			active_gadget(b);
			if (b->flags&BUTN_HIT && b->id!=0) { /** MUTUAL EXCLUDE! **/
				ObtainSemaphore(&baselock);
				for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
					temp=(struct button *)node;
				   if (b->bw==temp->bw && b->id==temp->id && b!=temp) {
						if (temp->flags&BUTN_HIT) {
							xor_button(temp);
				         active_gadget(temp);
							}
						}
					}
				ReleaseSemaphore(&baselock);
				}
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
   draw_button(b,b->flags&BUTN_HIT);   
else{
	SaveRast(b->bw->RPort); 
   ClipBlit(b->bw->RPort,b->box_x1,b->box_y1,
            b->bw->RPort,b->box_x1,b->box_y1,
            b->box_x2-b->box_x1+1,b->box_y2-b->box_y1+1,0x50);
	RestoreRast();
	}
}


/****************************************************************/
/****  droplist  ************************************************/
/****************************************************************/

struct droplist *create_droplist(w,nd)
struct Window *w;
struct NewDropList *nd;
{
struct NewButton nb;
struct NewList nl;
struct droplist *d;

d=(struct droplist *)AllocMem(sizeof(struct droplist),MEMF_CLEAR);
if (d==0) return(0);

setmem(&nb,sizeof(struct NewButton),0);
setmem(&nl,sizeof(struct NewList),0);
nb.flags = BUTN_COOL|BUTN_DROP_ARROW|MANUAL_XOR|GAD_SIBLING;
nl.flags = LIST_COOL|HIDE_GADGET|GAD_SIBLING|LIST_SINGLESELECT;

d->box_x1 = nd->box_x1;
d->box_x2 = nd->box_x2;
d->box_y1 = nd->box_y1;
d->box_y2 = nd->box_y2;
d->dw = w;
d->id = nd->id;
d->c = nd->c;
d->notifyme=nd->notifyme;
d->flags = nd->flags;
d->func = nd->func;
d->user_data = nd->user_data;

nb.box_x1 = d->box_x2 - STD_DROP_ARROW_WIDTH + 1;
nb.box_x2 = d->box_x2;
nb.box_y1 = d->box_y1;
nb.box_y2 = d->box_y1+ STD_DROP_ARROW_HEIGHT - 1;
nb.text = NULL;
nb.c = d->c;
nb.func = dl_butn_func;
nb.user_data = (APTR)d;

if ((d->b=(struct button *)create_gadget(d->dw, &nb))==0) {
	FreeMem(d, sizeof(struct droplist));
	return(0);
	}

nl.box_x1 = 2;
nl.box_x2 = d->box_x2 - d->box_x1 + 2;
nl.box_y1 = 0;
nl.box_y2 = d->box_y2 - d->box_y1 - STD_DROP_ARROW_HEIGHT;
nl.e_text = nd->e_text;
nl.e_user_data = nd->e_user_data;
nl.c = d->c;
nl.initial_value = nd->initial_value;
nl.func = dl_list_func;
nl.user_data = (APTR)d;

if ((d->l=(struct list *)create_gadget(d->dw, &nl))==0) {
	delete_gadget(d->b);
	FreeMem(d, sizeof(struct droplist));
	return(0);
	}

/** since droplists are created hidden, they have no window **/

d->l->lw = NULL;
d->l->slider->kw = NULL;
d->l->slider->b1->bw = NULL;
d->l->slider->b2->bw = NULL;

refresh_droplist(d);
return(d);
}

/*############################################################
NAME	
	refresh_droplist-

FUNCTION
	redraw's a droplist's imagery
INPUTS
	d pointer to the droplist to refresh
RESULTS
	NONE
##############################################################*/

void refresh_droplist(d)
struct droplist *d;
{
SaveRast(d->dw->RPort); 
SetAPen(d->dw->RPort, d->c->droplist.hitbox_unselected);
RectFill(d->dw->RPort, d->box_x1, d->box_y1,
    d->box_x2 - STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP,
	 d->box_y1 + STD_DROP_ARROW_HEIGHT - 1);

draw_indent(d->dw->RPort, d->box_x1, d->box_y1, d->box_x2 - STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP,
	 d->box_y1 + STD_DROP_ARROW_HEIGHT - 1, d->c->droplist.topleft_3d,
	 d->c->droplist.bottomright_3d);

refresh_list(d->l);
refresh_button(d->b);
RestoreRast();
}

/***********************************************************/

void dl_butn_func(b)
struct button *b;
{
struct droplist *d;
struct Window *w;

d=(struct droplist *)b->user_data;

if (d->l->flags&HIDE_GADGET) {
	w=QuickWindow(d->c, d->box_x1+d->dw->LeftEdge-2, d->box_y1+d->dw->TopEdge + STD_DROP_ARROW_HEIGHT + 2, 
		d->box_x2+d->dw->LeftEdge+2, d->box_y2+d->dw->TopEdge + 3,0,BORDERLESS, 0, NULL);
	if (w) {
		d->l->lw = w;
		d->l->slider->kw = w;
		d->l->slider->b1->bw = w;
		d->l->slider->b2->bw = w;
		show_gadget(d->l);
		refresh_list(d->l);
		}
	}
else {
	hide_gadget(d->l);
	CloseWindow(d->l->lw);
	d->l->lw = NULL;
	d->l->slider->kw = NULL;
	d->l->slider->b1->bw = NULL;
	d->l->slider->b2->bw = NULL;
	}
}

/***********************************************************/

void dl_list_func(l)
struct list *l;
{
struct droplist *d;

d = (struct droplist *)l->user_data;
if (l->current==-1)
	d->b->text = NULL;
else
	d->b->text = l->active_entry->text; 
refresh_button(d->b);

if (!(l->flags&HIDE_GADGET)) {
	active_gadget(d);
	hide_gadget(d->l);
	CloseWindow(d->l->lw);
	d->l->lw=NULL;
	d->l->slider->kw = NULL;
	d->l->slider->b1->bw = NULL;
	d->l->slider->b2->bw = NULL;
	}
}
/****************************************************************/
/****  looplist  ************************************************/
/****************************************************************/

struct looplist *create_looplist(w,nll)
struct Window *w;
struct NewLoopList *nll;
{
struct NewButton nb;
struct NewList nl;
struct looplist *ll;

ll=(struct looplist *)AllocMem(sizeof(struct looplist),MEMF_CLEAR);
if (ll==0) return(0);

setmem(&nb,sizeof(struct NewButton),0);
setmem(&nl,sizeof(struct NewList),0);
nb.flags = BUTN_COOL|BUTN_LOOP|MANUAL_XOR|GAD_SIBLING;
nl.flags = LIST_COOL|HIDE_GADGET|GAD_SIBLING|LIST_SINGLESELECT;

ll->box_x1 = nll->box_x1;
ll->box_x2 = nll->box_x2;
ll->box_y1 = nll->box_y1;
ll->box_y2 = nll->box_y2;
ll->llw = w;
ll->id = nll->id;
ll->c = nll->c;
ll->notifyme=nll->notifyme;
ll->flags = nll->flags;
ll->func = nll->func;
ll->user_data = nll->user_data;

nb.box_x1 = ll->box_x1;
nb.box_x2 = ll->box_x2;
nb.box_y1 = ll->box_y1;
nb.box_y2 = ll->box_y2;
nb.text = NULL;
nb.c = ll->c;
nb.func = ll_butn_func;
nb.user_data = (APTR)ll;

if ((ll->b=(struct button *)create_gadget(ll->llw, &nb))==0) {
	FreeMem(ll, sizeof(struct looplist));
	return(0);
	}

nl.e_text = nll->e_text;
nl.e_user_data = nll->e_user_data;
nl.c = ll->c;
nl.initial_value = nll->initial_value;
nl.user_data = (APTR)ll;

if ((ll->l=(struct list *)create_gadget(ll->llw, &nl))==0) {
	delete_gadget(ll->b);
	FreeMem(ll, sizeof(struct looplist));
	return(0);
	}

/** since looplists are never rendered. Just filled with data **/

ll->l->lw = NULL;
ll->l->slider->kw = NULL;
ll->l->slider->b1->bw = NULL;
ll->l->slider->b2->bw = NULL;

refresh_looplist(ll);
return(ll);
}

/*############################################################
NAME	
	refresh_looplist-

FUNCTION
	redraw's a looplist's imagery
INPUTS
	ll pointer to the looplist to refresh
RESULTS
	NONE
##############################################################*/

void refresh_looplist(ll)
struct looplist *ll;
{
if (ll->l->current==-1)
	ll->b->text = NULL;
else
	ll->b->text = ll->l->active_entry->text; 
refresh_button(ll->b);
}

/***********************************************************/

void ll_butn_func(b)
struct button *b;
{
struct looplist *ll;
struct list_entry *le;
int next;

ll=(struct looplist *)b->user_data;

next = ll->l->current+1;
if (next >= ll->l->list_size)
	next = 0;

le=get_list_entry_ptr(ll->l, next);
if (le) {
	handle_new_select(ll->l, le, next);
	}

if (ll->l->current==-1)
	ll->b->text = NULL;
else
	ll->b->text = ll->l->active_entry->text; 
refresh_button(ll->b);
active_gadget(ll);
}


/*############################################################
NAME	
	QuickWindow-

FUNCTION
	Opens a new window. Flags ACTIVATE,NOCAREREFRESH
	are always set. A "3D" type border is drawn unless BORDERLESS
	is set.
INPUTS
RESULTS
	NONE
##############################################################*/

struct Window *QuickWindow(c,x1,y1,x2,y2,idcmp,flags, screen, title)

struct GadgetColors *c;
int x1,x2;
int y1,y2;
ULONG idcmp;					/* NewWindow.IDCMPFags */
ULONG flags;					/* NewWindow.Flags */
struct Screen *screen;		/* pointer to screen or NULL for active screen */
char *title;					/* NULL or pointer to title string **/
{
struct Window *w;
struct NewWindow NewWindow;

setmem(&NewWindow,sizeof(struct NewWindow),0);

NewWindow.DetailPen = BLACK;
NewWindow.BlockPen = WHITE;
NewWindow.Type = CUSTOMSCREEN;
NewWindow.IDCMPFlags = idcmp;
NewWindow.Flags = flags|ACTIVATE|NOCAREREFRESH;
if (title)
	NewWindow.Flags |= WINDOWDRAG;
	
NewWindow.LeftEdge = x1;
NewWindow.TopEdge  = y1;
NewWindow.Width    = x2 - x1 + 1;
NewWindow.Height   = y2 - y1 + 1;
NewWindow.MinWidth    = 20;
NewWindow.MinHeight   = 10;
NewWindow.MaxWidth    = 1024;
NewWindow.MaxHeight   = 1024;
NewWindow.Title = title;
if (screen)
	NewWindow.Screen=screen;
else
	NewWindow.Screen = IntuitionBase->ActiveScreen;
w = (struct Window *)OpenWindow(&NewWindow);
if (w) {
	SetAPen(w->RPort, c->button.background);
	RectFill(w->RPort, 0,title ? 10:0, w->Width-1, w->Height-1);
	if (!(flags&BORDERLESS)) 
		draw_indent(w->RPort, 0, title ? 10:0, w->Width-1,w->Height-2, c->button.topleft_3d, c->button.bottomright_3d);
	}
return (w);
}
/*############################################################
NAME	
	hide_gadget -

FUNCTION
	Sets the HIDE_GADGET flag in a gadget, and all its associated
	gadgets.  Thus, a list gadget would also affect a slider and
	two arrow buttons.
	
	When HIDE_GADGET is set, a gadget's imagery is never rendered
	and the gadget can not be selected by the user.
INPUTS
	g - pointer to a gadget
RESULTS
	NONE
##############################################################*/

void hide_gadget(g)
struct GadgetHeader *g;
{
g->flags |= HIDE_GADGET;

if (g->flags&GAD_SLIDER && g->flags&SLIDER_ARROWS) {
	hide_gadget(((struct slider *)g)->b1);
	hide_gadget(((struct slider *)g)->b2);
	}
else if (g->flags&GAD_LIST) {
	hide_gadget(((struct list *)g)->slider);
	}
else if (g->flags&GAD_DROPLIST) {
	hide_gadget(((struct droplist *)g)->l);
	hide_gadget(((struct droplist *)g)->b);
	}
else if (g->flags&GAD_LOOPLIST) {
	hide_gadget(((struct looplist *)g)->l);
	hide_gadget(((struct looplist *)g)->b);
	}
}

/*############################################################
NAME	
	show_gadget -

FUNCTION
	resets the HIDE_GADGET flag in a gadget, and all its associated
	gadgets.  Thus, a list gadget would also affect a slider and
	two arrow buttons.
INPUTS
	g - pointer to a gadget
RESULTS
	NONE
##############################################################*/

void show_gadget(g)
struct GadgetHeader *g;
{
g->flags &= (~HIDE_GADGET);

if (g->flags&GAD_SLIDER && g->flags&SLIDER_ARROWS) {
	show_gadget(((struct slider *)g)->b1);
	show_gadget(((struct slider *)g)->b2);
	}
else if (g->flags&GAD_LIST) {
	show_gadget(((struct list *)g)->slider);
	}
else if (g->flags&GAD_DROPLIST) {
	show_gadget(((struct droplist *)g)->l);
	show_gadget(((struct droplist *)g)->b);
	}
else if (g->flags&GAD_LOOPLIST) {
	show_gadget(((struct looplist *)g)->l);
	show_gadget(((struct looplist *)g)->b);
	}
}

/**************************************************************/

void delete_droplist(d)
struct droplist *d;
{
hide_gadget(d);
if (d->l->lw) {
	CloseWindow(d->l->lw);
	d->l->lw = NULL;
	d->l->slider->kw = NULL;
	d->l->slider->b1->bw = NULL;
	d->l->slider->b2->bw = NULL;
	}
ObtainSemaphore(&baselock);
Remove(d);     /** this is the same as Remove(&d->node) **/
ReleaseSemaphore(&baselock);
delete_list(d->l);
d->b->text = NULL;
delete_button(d->b);
FreeMem(d, sizeof(struct droplist));
}

/*************************************************************/
void delete_looplist(ll)
struct looplist *ll;
{
hide_gadget(ll);
ObtainSemaphore(&baselock);
Remove(ll);     /** this is the same as Remove(&d->node) **/
ReleaseSemaphore(&baselock);
delete_list(ll->l);
ll->b->text = NULL;
delete_button(ll->b);
FreeMem(ll, sizeof(struct looplist));
}

/*************************************************************/

/** variables used by telluser, askuser **/

static char *notice_line1[MAX_NOTICES];
static char *notice_line2[MAX_NOTICES];
static unsigned long notice_flags[MAX_NOTICES];
static int next_notice=0;

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

askuser(l1, l2,flags)
char *l1,*l2;
unsigned long flags;
{
return(user_notice(l1, l2, NOTICE_CANCEL|flags));
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

void telluser(l1, l2,flags)
char *l1,*l2;
unsigned long flags;
{
user_notice(l1, l2, flags);
}

/*************************************************************/

user_notice(l1, l2, flags)
char *l1,*l2;
unsigned long flags;
{
notice_line1[next_notice]=l1;
notice_line2[next_notice]=l2;
notice_flags[next_notice]=flags|NOTICE_CANCEL;

if (flags&NOTICE_RETURN_NOW) {
	CreateTask("GadLib_UserNotice",0,user_notice_code,2000);
	return(TRUE);
	}
return(user_notice_code());
}
	
/*************************************************************/

int user_notice_code()
{
#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,RED,BLACK,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,BLACK,RED}

static struct GadgetColors tucolors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	BUTNCOLORS};

static struct NewButton okbutnguide = 
	{NULL,20,30,50,45,BUTN_COOL,0,0,&tucolors,0,0,0,"OK"};
char *l1,*l2;
unsigned long flags;
struct NewWindow NewWindow;
struct NewButton okbutn;
int width;
struct Window *w;
struct GadgetHeader *g;

l1=notice_line1[next_notice];
l2=notice_line2[next_notice];
flags=notice_flags[next_notice];
next_notice = (next_notice+1)&(MAX_NOTICES-1);

okbutn = okbutnguide;

width = max(my_strlen(l1),my_strlen(l2));
width = width*8 + 20;

setmem(&NewWindow,sizeof(struct NewWindow),0);
NewWindow.Width = width;
NewWindow.TopEdge = 75;
NewWindow.LeftEdge = (640-width)/2;
NewWindow.Screen = IntuitionBase->ActiveScreen;
NewWindow.DetailPen = BLACK;
NewWindow.BlockPen = WHITE;
NewWindow.Flags = ACTIVATE|NOCAREREFRESH|BORDERLESS;
NewWindow.Height   = 50;
NewWindow.MinWidth    = NewWindow.Width;
NewWindow.MinHeight   = NewWindow.Height;
NewWindow.MaxWidth    = NewWindow.Width;
NewWindow.MaxHeight   = NewWindow.Height;
NewWindow.Type = CUSTOMSCREEN;

lock_gadlib++;

if (w=(struct Window *)OpenWindow(&NewWindow)) {
   draw_indent(w->RPort, 0, 0, w->Width-1,w->Height-1, tucolors.button.topleft_3d, tucolors.button.bottomright_3d);
	SetAPen(w->RPort,BLACK);
   Move(w->RPort, 10, 10);
   Text(w->RPort, l1, my_strlen(l1)); 
   Move(w->RPort, 10, 20);
   Text(w->RPort, l2, my_strlen(l2));

   okbutn.box_x1=(w->Width-30)/2;
   okbutn.box_x2=okbutn.box_x1+30;
	okbutn.notifyme=CreatePort(0,0);
	if (okbutn.notifyme) {
	   if (g=create_gadget(w, &okbutn)) {
   	   WaitPort(okbutn.notifyme);
			ReplyMsg(GetMsg(okbutn.notifyme));
      	delete_gadget(g);
	      }
		DeletePort(okbutn.notifyme);
		}
   CloseWindow(w);
   }
lock_gadlib--;
if (flags&NOTICE_RETURN_NOW) {
	RemTask(0);
	Wait(0);
	}
return(TRUE);
}

/****************************************************************/
/****************************************************************/
/** INPUT.DEVICE ROUTINES                                    ****/
/****************************************************************/
/****************************************************************/

/********************************************************************/

add_gadlib_inhandler()
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

void rem_gadlib_inhandler()
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

/********************************************************************/

void send_msg(port, g, type, flags)
struct GadgetHeader *g;
struct MsgPort *port;
unsigned long flags,type;
{
struct GadMsg *msg;

msg=(struct GadMsg *)AllocMem(sizeof(struct GadMsg),MEMF_CLEAR);
if (msg) {
   msg->message.mn_Node.ln_Type = NT_MESSAGE;
   msg->message.mn_ReplyPort = gadlib_reply_port;
	msg->g = g;
	msg->flags = flags;
   msg->type = type; 
   PutMsg(port, msg);
   }
}
/*############################################################
NAME
	LockGadLib -
FUNCTION
	Obtain's a Semaphore lock on the gadlib's internal strucutres.
	No gadlib access to internal structures or gadlib rendering
	operations will occur while a lock is in place.
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void LockGadLib()
{
ObtainSemaphore(&baselock);
}
/*############################################################
NAME	
	UnLockGadLib() -
FUNCTION
	Free's the lock obtained by a call to LockGadLib().  Every
	call to LockGadLib() must be followed by a call to
	UnLockGadLib();
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void UnLockGadLib()
{
ReleaseSemaphore(&baselock);
}

/********************************************************************/

void SaveRast(rp)
struct RastPort *rp;
{
ObtainSemaphore(&renderlock);
}

/********************************************************************/

void RestoreRast()
{
ReleaseSemaphore(&renderlock);
}

/********************************************************************/

void LockGadLibRender()
{
ObtainSemaphore(&baselock);
}

void UnLockGadLibRender()
{
ReleaseSemaphore(&baselock);
}

/********************************************************************/

