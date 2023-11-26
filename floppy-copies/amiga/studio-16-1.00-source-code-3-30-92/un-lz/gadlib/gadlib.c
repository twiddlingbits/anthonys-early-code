/** lc -v -b0 -r0 gadlib.c can be used but it will crash when calling
    graphic.libary unless lcnb.lib is used **/

/** !!!! SOME DAY CORECT USAGE OF INITIAL SETING OF HIDE_GAD flag */

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
struct MinList key_list;

/*
#define LISTCOLORS {BROWN,DARK_BROWN,WHITE,BROWN,RED,BLACK,BLACK}	
#define KNOBCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,LIGHT_BROWN,BLACK,BLACK}
#define BUTNCOLORS {BROWN,WHITE,DARK_BROWN,LIGHT_BROWN,BROWN,BLACK,RED}
*/

#define GCL	4
#define DARK	6
#define LITE	1
#define MEAD	2
#define DCL	3
#define ECL	5

#define LISTCOLORS {MEAD,DARK,LITE,/*GCL*/MEAD,DCL,ECL,ECL}
#define KNOBCOLORS {MEAD,LITE,DARK,GCL,DCL,ECL,ECL}
#define BUTNCOLORS {MEAD,LITE,DARK,GCL,DCL,ECL,ECL}

static struct GadgetColors default_colors = {
	LISTCOLORS,
	KNOBCOLORS,
	BUTNCOLORS,
	BUTNCOLORS,
	LISTCOLORS,
	BUTNCOLORS,
	BUTNCOLORS};


struct Task *tc_gadlib;       /** gadlib task's tc **/

short lock_gadlib=0;          /** inc when shouldnt close gadlib */
short doing_init=TRUE;			/** use in init_gadlib **/
short error_during_init=FALSE;

struct GadEvent event_buffer[512];
short read_ptr=0;    /** Next read by gadlib comes from this index **/
short write_ptr=0;   /** Next event write by handler goes to this index **/
short signal_ok=1;   /** Tell input.device handler to signal us **/
long gadlib_sigbit;  /** signal this task by this bit when new events **/
long gadlib_sigbit3; /** used to signal tc_gadlib to remove itself **/

struct GadgetHeader *selected_gadget;	/** used to direct mousemoves **/
void (*selected_gadget_func)();	/** send mousemoves & button up here **/ 

struct IOStdReq *inputReqBlk;
struct MsgPort *inputPort;
struct Interrupt *inputHandler;
struct Library  *LayersBase;
struct MsgPort *gadlib_reply_port;
struct SignalSemaphore baselock;
struct SignalSemaphore renderlock;
extern struct IntuitionBase *IntuitionBase;

long _oserr, _OSERR;

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
void delete_list_entry_by_index();
int delete_list_entry_by_text();
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
void new_list_size();
void new_slider_size();
void new_button_size();
void scroll_list_text();
void refresh_droplist_text();
void set_button_text();
void set_gadget_window();
void l_key_func();

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

/*tc_gadlib=MyCreateProc("gadlibtask",4,main_gadlib,6000);*/

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
while (lock_gadlib>0) 
	WaitTOF();    /** Wait for any UserNotice() to finish **/

Signal(tc_gadlib, 1<<gadlib_sigbit3);	/** kill self **/

/** Wait for child task to finish cleaup before we return **/

while (FindTask("gadlibtask")) {
	WaitTOF();
	WaitTOF();
	}
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
NewList(&key_list);

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
unsigned long waitmask;
short x,y;
short xsum,ysum;
short code;
struct window *window;
struct GadMsg *msg;
struct MinNode *node;
struct key_notice *kn;

/**!!!!!!MOVE THIS VVVV ****/
waitmask = (1<<gadlib_sigbit) | (1<<gadlib_sigbit3) | (1<<gadlib_reply_port->mp_SigBit);
mask=Wait(waitmask);

if (mask&(1<<gadlib_reply_port->mp_SigBit)) {
	while (msg = (struct GadMsg *)GetMsg(gadlib_reply_port)) 
		FreeMem(msg,sizeof(struct GadMsg));
	}

/*PRIORITIZE THESE SIGNALS VVVVVVVVV  !!!!*/

if (mask&(1<<gadlib_sigbit3)) {
	killself();
	Wait(0);
	}

xsum = 0;
ysum = 0;

do {
   while (read_ptr!=write_ptr) {
      code=event_buffer[read_ptr].type; 
      window = event_buffer[read_ptr].window; 
		if (code==GE_RAWKEY) {
			Forbid();
			for (node=key_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   			kn=(struct key_notice *)node;
				if (kn->w==NULL || kn->w==window)
					(*kn->func)(kn,event_buffer[read_ptr].code,event_buffer[read_ptr].qualifier);
				}
			Permit();
			}
		else {	/** Not GE_RAWKEY **/
			if (window) {
		      x = event_buffer[read_ptr].x;
	   	   y = event_buffer[read_ptr].y;
	      	if (code==GE_DELTAMOVE) { /*!!!!!!    SIMPLIFY THIS expresion VVVV ***/
	   	      if (code==GE_DELTAMOVE && IntuitionBase->ActiveScreen->Height < 400) {
   	         count += y;
      		      y = count/2;
         		   count = count - y*2;
	         	   }
	   	      else {
   	   	      count = 0;
      	   	   }
		         xsum += x;        /** Combine all delta moves into one **/
   		      ysum += y;
      		   }
		      else {	/** Something besides GE_DELTAMOVE **/
   		      if (xsum || ysum) {
						if (selected_gadget_func) {
							ObtainSemaphore(&baselock);
							if (selected_gadget_func)
		      		      (*selected_gadget_func)(selected_gadget, GE_DELTAMOVE, xsum, ysum);
							ReleaseSemaphore(&baselock);
							}
	         	   xsum=ysum=0;
   	         	}
					if (code==GE_LBUTTONUP) {
						ObtainSemaphore(&baselock);
						if (selected_gadget_func)
							(*selected_gadget_func)(selected_gadget, code, x, y);
						ReleaseSemaphore(&baselock);

						}
					else { /** handle_gadget's deals with LBUTTONDOWN **/
			         handle_gadgets(window, code, x, y);
						}
					}
	         }
			}
		read_ptr=(read_ptr+1)&511;
      }

   if (xsum || ysum) {
		if (window && selected_gadget_func) {
			ObtainSemaphore(&baselock);
			if (selected_gadget_func)
	   		(*selected_gadget_func)(selected_gadget, GE_DELTAMOVE, xsum, ysum);
			ReleaseSemaphore(&baselock);
			}
       xsum=ysum=0;
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
	create_key_notice
FUNCTION
	Tell's the gadget.library to call a specified function
	when keys are pressed
INPUTS
	nk	pointer to a NewKeyNotice structure
RESULTS
	Pointer to a key_notice strucutre or NULL

##############################################################*/

struct key_notice *create_key_notice(nkn)
struct NewKeyNotice *nkn;
{
struct key_notice *kn;

kn=(struct key_notice *)AllocMem(sizeof(struct key_notice), MEMF_CLEAR);
if (kn) {
	kn->w=nkn->w;
	kn->func=nkn->func;
	kn->user_data=nkn->user_data;
	Forbid();
	AddTail(&key_list, kn);
	Permit();
	}
return(kn);
}

/*############################################################
NAME
	delete_key_notice
FUNCTION
	remove's a previously created key notice
INPUTS
	kn	pointer to a key_notice structure returned from create_key_notice
RESULTS
	NONE

##############################################################*/

void delete_key_notice(kn)
struct key_notice *kn;
{
if (kn) {
	Forbid();
	Remove(kn);
	Permit();
	FreeMem(kn, sizeof(struct key_notice));
	}
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

ObtainSemaphore(&baselock);
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

if (ptr) {/*!!!!!!  CREATE A LIST OF GADGETS FOR EACH WINDOW!*/
   AddTail(&gad_list, ptr);
	}
ng->gad_ptr=ptr;
ReleaseSemaphore(&baselock);
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
	new_gadget_size -
FUNCTION
	resizes a gadget.  No imagery is updated- just internal data
	structures.  Use refresh_gadget() to update image.
INPUTS
	g  pointer to a GadgetHeader struct
RESULTS
	NONE
##############################################################*/

void new_gadget_size(g, x1, y1, x2, y2)
struct GadgetHeader *g;
short x1,x2;
short y1,y2;
{
if (g->flags&GAD_LIST)	new_list_size(g, x1, y1, x2, y2);
else if (g->flags&GAD_BUTTON) new_button_size(g, x1, y1, x2, y2);
else if (g->flags&GAD_SLIDER) new_slider_size(g, x1, y1, x2, y2);
/*else if (g->flags&GAD_DROPLIST) */
/*else if (g->flags&GAD_LOOPLIST) */
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
   if (w==g->w && !(g->flags&GAD_SIBLING) && !(g->flags&NOT_IN_CHAIN))
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
   if (w==g->w && !(g->flags&GAD_SIBLING) && !(g->flags&NOT_IN_CHAIN)) {
      delete_gadget(g);
		}
	}
ReleaseSemaphore(&baselock);
}

/*############################################################
NAME
	delete_gadget_chain_by_id -
FUNCTION
	deletes all gadgets in the specified window with a certain id
INPUTS
	w  pointer to window 
	id GadgetHeader.id to match
RESULTS
	NONE
##############################################################*/

void delete_gadget_chain_by_id(w,id)
struct Window *w;
short id;
{
struct MinNode *node;
struct GadgetHeader *g;

ObtainSemaphore(&baselock);
node=gad_list.mlh_Head;
while(node->mln_Succ) {
   g=(struct GadgetHeader *)node;
	node = node->mln_Succ;
   if (w==g->w && !(g->flags&GAD_SIBLING) && g->id==id && !(g->flags&NOT_IN_CHAIN)) {
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
ObtainSemaphore(&baselock);
g->notifyme=NULL;
if (g==selected_gadget) {
	selected_gadget=NULL;
	selected_gadget_func=NULL;
	}
ReleaseSemaphore(&baselock);

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
short do_restore;

if (g->w && g->func) {
	SaveRast(g->w->RPort); 
	do_restore=TRUE;
	}
else {
	do_restore=FALSE;
	}

if (g->func)
   (*g->func)(g);

if (do_restore)
	RestoreRast();

ObtainSemaphore(&baselock);	/** BUG HERE!! Deadlock!.  SEE SaveRast() **/
if (g->notifyme)
	send_msg(g->notifyme, g, GADMSG_ACTIVE, g->flags);
ReleaseSemaphore(&baselock);
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
         delete_gadget(k->b1);
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
         delete_gadget(k->b1);
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
nk->gad_ptr=(struct GadgetHeader *)k;
refresh_slider(k);
return(k);
}
/*############################################################
NAME	
	calc_slider_internals -

FUNCTION
	Recalculates a knob's x,y,width, and height based on
	box_x and box_y's and AUTO_KNOB.  Box_x and box_y may also
	be affected if BORDER_3D is selected.
	
	Call calc_slider_internals() after you change box_x & box_y.
	For example, if the user resizes your window you may wish
	to change the size of a slider.  You can do this by altering
	slider->box_x1 through slider->box_y2.  Then call 
	calc_slider_internals(); followed by a call to refresh_gadget_chain()
	or refresh_gadget.

	Although, "new_gadget_size()" is the much prefered way of
	changing the size of a gadget.  "calc_slider_internals()"
	is obsolete.
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

calc_knob_size(s);		/** Assumes AUTO_KNOB **/

/** NEED TO PUT DEFAULT width & height into struct slider so that **/
/** ns need not be passed !!!!!!!!!!! **/

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
	delete_gadget(k->b1);
	delete_gadget(k->b2);
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
	SetDrMd(k->kw->RPort,JAM1);
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
	new_slider_size -
FUNCTION
	Allows you to move or resize a slider gadget.  No rendering
	is performed.  You must call refresh_gadget(s) after calling
	new_slider_size() to refresh the imagery.

	You can set slider->list_size before making the call to
	new_slider_size() and the new knob size will be calculated
	for you.

	BUGS: Currently only works with AUTO_KNOB sliders
INPUTS
	s		slider to resize
	x1,x2	new size
	y1,y2	new size
RESULTS
	NONE
##############################################################*/

void new_slider_size(s, x1, y1, x2, y2)
struct slider *s;
short x1,x2;
short y1,y2;
{
short bx1,bx2;
short by1,by2;

if (s->flags&FREE_VERT) {
	if (s->b1) {
      y2 -= (STD_V_ARROW_SIZE*2+3);
      bx1=x1;
      bx2=x2;
      by1=y2+2;
      by2=by1+STD_V_ARROW_SIZE;
		new_button_size(s->b1, bx1, by1, bx2, by2);

      by1=by2+1;
      by2=by1+STD_V_ARROW_SIZE;
		new_button_size(s->b2, bx1, by1, bx2, by2);
		}
	}

if (s->flags&FREE_HORIZ) {
	if (s->b1) {
      x2 -= (STD_H_ARROW_SIZE*2+6);
		bx1=x2+3;
		bx2=bx1+STD_H_ARROW_SIZE;
		by1=y1;
		by2=y2;
		new_button_size(s->b1, bx1, by1, bx2, by2);

		bx1=bx2+3;
	   bx2=bx1+STD_H_ARROW_SIZE;
		new_button_size(s->b2, bx1, by1, bx2, by2);
		}
	}

s->box_x1=x1;
s->box_y1=y1;
s->box_x2=x2;
s->box_y2=y2;

if (s->flags&BORDER_3D) {
   s->box_x1 += 2;
   s->box_x2 -= 2;
   s->box_y1 += 1;
   s->box_y2 -= 1;
   }

if (s->flags&AUTO_KNOB)		/** NEED INDEPENDENT X & Y AUTO_KNOB **/
	calc_knob_size(s);			/** Assume's auto knob! **/

#if 0			/*** FIX FIX THIS!!!!! ****/
if (!(s->flags&AUTO_KNOB)) {
   if (ns->knob_width!=-1) 
      s->knob_width = ns->knob_width;
	if (ns->knob_height!=-1)
		s->knob_height = ns->knob_height;
	}
#endif

if (s->top_element >= (s->list_size-s->show_size))
   s->top_element = s->list_size-s->show_size;
if (s->top_element < 0) s->top_element = 0;

calc_knob_xy(s);
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
	SetDrMd(k->kw->RPort,JAM1);
	SetAPen(k->kw->RPort, k->c->slider_box.hitbox_unselected);
	RectFill(k->kw->RPort,k->box_x1, k->box_y1, k->box_x2, k->box_y2);
	draw_knob(k,k->c->slider_knob.hitbox_unselected);
	RestoreRast();
	}
}

/****************************************************************/
/** Calculate knob sizes assuming AUTO_KNOB.  IE, scale in the  **/
/** direction that we are FREE and maximize in the direction   **/
/** we are not free 															**/
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
		if (k->list_size > 1000000)	/** Prevent 32 bit overflows! **/
	      k->knob_height = ((k->box_y2-k->box_y1+1)*(k->show_size>>6))/max(k->list_size>>6,1);
		else
	      k->knob_height = ((k->box_y2-k->box_y1+1)*k->show_size)/k->list_size;
      }
   k->knob_width = k->box_x2 - k->box_x1-1;
	k->knob_width += 2;	/** Used to be KNOB_NOGAP **/
   }
if (k->flags&FREE_HORIZ) {
   if (k->show_size > k->list_size) {
      k->knob_width = k->box_x2-k->box_x1+1;
      k->top_element = 0;
      }
   else {
		if (k->list_size > 1000000)	/** Prevent 32 bit overflows! **/
	      k->knob_width = ((k->box_x2-k->box_x1+1)*(k->show_size>>6))/max(k->list_size>>6,1);
		else
	      k->knob_width = ((k->box_x2-k->box_x1+1)*k->show_size)/k->list_size;
      }
   k->knob_height = k->box_y2 - k->box_y1-1;
   k->knob_height += 2;		/** Used to be KNOB_NOGAP **/
   }

if (k->knob_height < 4)
	k->knob_height = 4;
if (k->knob_width < 8)
	k->knob_width = 8;
}

/****************************************************************/
/** calcuate knob->x and knob->y from top_element, show_size,  **/
/** and list_size                                              **/
/****************************************************************/

void calc_knob_xy(k)
struct slider *k;
{
if (k->flags & FREE_VERT) {
   if (k->show_size >= k->list_size) {
      k->knob_y=k->box_y1;
		}
   else {
		if (k->top_element > 1000000)	/** Prevent 32 bit overflows! **/
	      k->knob_y=k->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*(k->top_element>>6))/max((k->list_size-k->show_size)>>6,1)+1;
		else
	      k->knob_y=k->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_size-k->show_size)+1;
		}
   if (k->knob_y+k->knob_height > k->box_y2)
      k->knob_y = k->box_y2 - k->knob_height+1;
   k->knob_x = k->box_x1+1;
   k->knob_x -= 1;	/** used to be KNOB_NOGAP **/
   }

if (k->flags & FREE_HORIZ) {
   if (k->show_size >= k->list_size) {
      k->knob_x=k->box_x1;
		}
   else {
		if (k->top_element > 1000000)	/** Prevent 32 bit overflows! **/
	      k->knob_x=k->box_x1+((k->box_x2 - k->box_x1-k->knob_width+1)*(k->top_element>>6))/max((k->list_size-k->show_size)>>6,1)+1;
		else
	      k->knob_x=k->box_x1+((k->box_x2 - k->box_x1-k->knob_width+1)*k->top_element)/(k->list_size-k->show_size)+1;
		}
   if (k->knob_x+k->knob_width > k->box_x2)
      k->knob_x = k->box_x2 - k->knob_width+1;
   k->knob_y = k->box_y1+1;
   k->knob_y -= 1;	/** used to be KNOB_NOGAP **/
   }
}

/****************************************************************/
/** calc top_element from show knob_x & knob_y & knob_height  ***/
/****************************************************************/

void calc_top_element(k)
struct slider *k;
{
long diff;

if (k->show_size >= k->list_size) {
   k->top_element = 0;
   }
else {
	diff=k->list_size-k->show_size;

   if (k->flags&FREE_VERT)
		if (diff > 1000000)		/** avoid overflows! */
	      k->top_element = (diff/(k->box_y2-k->box_y1-k->knob_height+1))*(k->knob_y-k->box_y1);
		else
   	   k->top_element = (diff*(k->knob_y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);

   if (k->flags&FREE_HORIZ)
		if (diff > 1000000)		/** avoid overflows! */
	      k->top_element = (diff/(k->box_x2-k->box_x1-k->knob_width+1))*(k->knob_x-k->box_x1);
		else
   	   k->top_element = (diff*(k->knob_x-k->box_x1))/(k->box_x2-k->box_x1-k->knob_width+1);
   }
}

/****************************************************************/

void handle_slider(k, code, x, y)
struct slider *k;
USHORT code;
int x,y;
{
if (code==GE_LBUTTONDOWN) {	/** MUST BE LBUTTONDOWN!!!! Take this out?? **/
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

ObtainSemaphore(&baselock);
if (k->top_element >= (k->list_size-k->show_size))
   k->top_element = k->list_size-k->show_size;
if (k->top_element < 0) k->top_element = 0;
old_knob_y = k->knob_y;
old_knob_x = k->knob_x;
calc_knob_xy(k);
scroll_knob(k, old_knob_x, old_knob_y);
ReleaseSemaphore(&baselock);
active_gadget(k);
}

/*******************************************************************/
/** the following local global variables are used by knob draging **/
/*******************************************************************/

static int global_old_knob_y;
static int global_old_knob_x;
static int global_old_top;

/*******************************************************************/

void start_drag_knob(k, x, y)
struct slider *k;
int x,y;
{
void drag_knob();

global_old_knob_x = k->knob_x;
global_old_knob_y = k->knob_y;
global_old_top = k->top_element;
k->flags |= BUTN_HIT;		/** !!! SHOULD rename GAD_HIT or something!!! **/
if (k->c->slider_knob.hitbox_selected != k->c->slider_knob.hitbox_unselected)
   draw_knob(k,k->c->slider_knob.hitbox_selected);   /* knob can change color when being draged */
selected_gadget=(struct GadgetHeader *)k;
selected_gadget_func=drag_knob;
}

/*******************************************************************/

void drag_knob(k, code, newx, newy)
USHORT code;
int newy, newx;
struct slider *k;
{
if (code==GE_LBUTTONUP) {
	k->flags &= ~BUTN_HIT;		/** !!! SHOULD rename GAD_HIT or something!!! **/
   if (k->c->slider_knob.hitbox_selected != k->c->slider_knob.hitbox_unselected)
      draw_knob(k,k->c->slider_knob.hitbox_unselected);   /* knob can change color when being draged */
	selected_gadget=NULL;
	selected_gadget_func=NULL;
	ObtainSemaphore(&baselock);	
	if (k->notifyme)
		send_msg(k->notifyme, k, GADMSG_RELEASED, k->flags);
	ReleaseSemaphore(&baselock);

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
	if (x2 < (k->knob_x + k->knob_width - 2) && y2 < (k->knob_y + k->knob_height - 1))
	   draw_indent(rp, x1,y1,x2,y2,k->c->slider_knob.bottomright_3d,k->c->slider_knob.topleft_3d);
   }

RestoreRast();
active_gadget(k);
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


/*############################################################
NAME	
	handle_new_select -

FUNCTION
	Selects a new entry in a list. For example, create_list uses:
   	handle_new_select(l,get_list_entry_ptr(l, nl->initial_value),
								nl->initial_value);
	handle_new_select act's just like the user clicked an entry-
	the user_func() is called etc.
INPUTS
	l			The list to work on
	le			this list_entry is to be slected
	select 	this is the index of the above le.  This info
				is redundant, but it speeds internal things up
				a little.
RESULTS
	NONE
##############################################################*/

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

/*** ERROR? 2 tasks accessing the same list?  No Semephores ***/
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
struct NewKeyNotice nkn;
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
l->show_size = (nl->box_y2 - nl->box_y1 - 1)/9; /** change to font height! */
l->lw = w;
l->id=nl->id;
l->c=nl->c;
l->notifyme=nl->notifyme;
l->max_show_chars = (nl->box_x2 - nl->box_x1 - STD_SLIDER_WIDTH - STD_SLIDER_GAP - 4)/8;
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
standard_list_slider.flags=GAD_SLIDER|AUTO_KNOB|FREE_VERT|GAD_SIBLING|(nl->flags&(KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH|HIDE_GADGET|MANUAL_XOR));
l->slider = (struct slider *)create_gadget(l->lw, &standard_list_slider);
if (l->slider==0) {
   FreeMem(l, sizeof (struct list));
   return(0);
   }

refresh_list(l);

if (nl->e_text) {		/** Add pre-initilized list entries **/
	i=0;
	while (nl->e_text[i][0]!=0) {
	   add_list_entry(l, nl->e_text[i], nl->e_user_data ? nl->e_user_data[i] : 0);  
		i++;
		}
	if (nl->initial_value < i && nl->initial_value!=-1) {
		handle_new_select(l,get_list_entry_ptr(l, nl->initial_value),nl->initial_value);
		}
	}

/** setup funciton to handle up and down arrow keys **/

if (l->flags&LIST_USEKEYS) {
	nkn.func=l_key_func;
	nkn.w=l->lw;
	nkn.user_data=(APTR)l;
	l->kn=create_key_notice(&nkn);
	}

return(l);
}

/****************************************************************/
void l_key_func(kn, code, qualifier)
struct key_notice *kn;
short code,qualifier;
{
struct list *l;
void *tempfunc;

l=(struct list *)kn->user_data;
if (code==0x4c) {	/** up arrow **/
	if (l->current > 0) {
   	handle_new_select(l,get_list_entry_ptr(l, l->current-1),l->current-1);
		if (l->current < l->top_element) {
			l->top_element=l->current;
			refresh_list_text(l);
			l->slider->top_element=l->top_element;
			calc_knob_xy(l->slider);
			tempfunc=l->slider->func;
			l->slider->func=NULL;	/** don't want list_slider callback! **/
			refresh_knob(l->slider);
			l->slider->func=tempfunc;
			}
		}
	}
else if (code==0x4d) {	/** down arrow **/
	if (l->current < (l->list_size-1)) {
   	handle_new_select(l,get_list_entry_ptr(l, l->current+1),l->current+1);
		if (l->current >= (l->top_element+l->show_size)) {
			l->top_element=l->current-l->show_size+1;
			refresh_list_text(l);
			l->slider->top_element=l->top_element;
			calc_knob_xy(l->slider);
			tempfunc=l->slider->func;
			l->slider->func=NULL;	/** don't want list_slider callback! **/
			refresh_knob(l->slider);
			l->slider->func=tempfunc;
			}
		}
	}
else if (code==0x44 && !(l->flags&LIST_SINGLESELECT)) {	/** enter **/
	if (l->current!=-1)
		handle_new_select(l,get_list_entry_ptr(l, l->current),l->current);
	}
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
delete_key_notice(l->kn);
delete_gadget(l->slider);
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
/* scroll_list_text(l, oldtop); This flickers too much **/
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
	SetDrMd(l->lw->RPort, JAM1);
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
static short max_size=0;
short len;
short i;
short blank_lines;
struct RastPort *rp;
struct MinNode *node;
struct MinNode *top_node;
struct list_entry *le;

if (l->flags&HIDE_GADGET) 	
	return;

rp = l->lw->RPort;
SaveRast(rp);
ObtainSemaphore(&baselock);/**!!!!!  GET THIS OUT OF HERE?  REDUCE THEM EVERYWHERE****/

/*SCROLL LIST!!!! AND ADD ONLY ONE ENTRY!!!! USE SEPERATE RASTPORT? AND BLIT?*/

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
ReleaseSemaphore(&baselock);
RestoreRast();
}

/****************************************************************/
#if 0
void scroll_list_text(l, oldtop)
struct list *l;
int oldtop;
{
short delta;
short absdelta;
short i,fe;
int len;
struct RastPort *rp;
struct MinNode *node;
struct MinNode *top_node;
struct list_entry *le;

if (l->flags&HIDE_GADGET) 	
	return;

rp = l->lw->RPort;
SaveRast(rp);
SetDrMd(l->lw->RPort, JAM1);
if (l->flags&GAD_SIBLING) {
	SetAPen(rp, l->c->droplist.draw_unselected);
	SetBPen(rp, l->c->droplist.hitbox_unselected);
	}
else {
	SetAPen(rp, l->c->list.draw_unselected);
	SetBPen(rp, l->c->list.hitbox_unselected);
	}

delta=l->top_element-oldtop;
absdelta=abs(delta);
if (absdelta >= l->show_size) {
	refresh_list_text(l);
	RestoreRast();
	return;
	}

if (delta > 0)
	fe=oldtop+l->show_size;
else
	fe=l->top_element;

i=0;
top_node=l->entries.mlh_Head;
for (node=top_node; node->mln_Succ; node = node->mln_Succ) {
   if (i++==fe) {
      top_node=node;
		break;
		}
   }

ScrollRaster(rp,0,delta*9,l->box_x+3, l->box_y+1,
	            l->box_x2-STD_SLIDER_WIDTH-STD_SLIDER_GAP-3, l->box_y2-1);

SetDrMd(rp, JAM2);
while(absdelta--) {
   le=(struct list_entry *)top_node;
	top_node=top_node->mln_Succ;
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
   Move(rp, l->box_x+3, l->box_y+(fe++-l->top_element)*9+8);
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
   }
RestoreRast();
}

#endif

/****************************************************************/
/** re-size & draw a list's slider ***/
/****************************************************************/

void refresh_list_slider(l)
struct list *l;
{
struct slider*k;

ObtainSemaphore(&baselock);
k=l->slider;
k->top_element = l->top_element;
k->show_size=l->show_size;
k->list_size=l->list_size;
calc_knob_size(k);
calc_knob_xy(k);
refresh_knob(k);
ReleaseSemaphore(&baselock);
}

/*############################################################
NAME	
	new_list_size -

FUNCTION
	Allows you to move or resize an existing list gadget. No
	imagery refresh is performed. Use refresh_gadget();
INPUTS
	l					list who's internal data we should alter
	x1, y1, x2, y2 new position and size of the list
RESULTS
	NONE
##############################################################*/

void new_list_size(l, x1, y1, x2, y2)
struct list *l;
short x1,x2;
short y1,y2;
{
short sx1,sx2;
short sy1,sy2;

l->box_x = x1;
l->box_y = y1;
l->box_x2 = x2;
l->box_y2 = y2;
l->show_size = (y2 - y1 - 1)/9; /** change to font height! */
l->max_show_chars = (x2 - x1 - STD_SLIDER_WIDTH - STD_SLIDER_GAP - 4)/8;

sx1 = x2 - STD_SLIDER_WIDTH + 1;
sx2 = x2;
sy1 = y1;
sy2 = y2;

l->slider->show_size = l->show_size;	/* new_slider_size() will calc_knob.... */
new_slider_size(l->slider, sx1, sy1, sx2, sy2);
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
ObtainSemaphore(&baselock);
l->list_size++;

/** Alphabetize list **/

if (l->flags&LIST_SORTED) {
	node=l->entries.mlh_Head;
	while(node->mln_Succ) {
	   tle=(struct list_entry *)node;
		if (stricmp(c,tle->text) < 0)
			break;
		node = node->mln_Succ;
		}
	Insert(&l->entries, le, node->mln_Pred);
	}
else {      /** Not sorted? Stick it on the end **/
	AddTail(&l->entries, le);
	}

refresh_list_text(l);
refresh_list_slider(l);
ReleaseSemaphore(&baselock);
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

ObtainSemaphore(&baselock);

if (le=get_list_entry_ptr(l, e)) {
/** Remove the list entry that is being deleted **/

   Remove(le);    /* same as &le->node */
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

/** Decide on a new current list entry **/

   if (e==l->current) {
		if (l->flags&KEEP_ONE_ACTIVE && l->list_size>0)
			newcurle=get_list_entry_ptr(l,0);
		else
	      newcurle = NULL;
      needrefresh = TRUE;
      }
   else {
      newcurle = l->active_entry;
      needrefresh = FALSE;
      }

/** Activate the new current list entry **/
	
   l->current = get_list_entry_index(l, newcurle);
   l->active_entry=newcurle;
	if (l->active_entry)
		l->active_entry->flags |= LE_SELECTED;
   if (needrefresh) {
      l->flags|=LIST_NEWSELECT;
      active_gadget(l);
      l->flags&=(~LIST_NEWSELECT);
      }
   refresh_list_text(l);
   refresh_list_slider(l);
   }
ReleaseSemaphore(&baselock);
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
	int (count of entries that matched user_data & where deleted)
##############################################################*/

int delete_list_entry_by_userdata(l, ud)
struct list *l;
unsigned long ud;			/* match this to l->user_data */
{
struct MinNode *node;
int index,count;
unsigned long user_data;

index=count=0;
ObtainSemaphore(&baselock);
node=l->entries.mlh_Head;
while(node->mln_Succ) {
	user_data=((struct list_entry *)node)->user_data;
	node = node->mln_Succ;
	if (user_data==ud) {
		delete_list_entry(l, index);
		count++;
		}
	index++;
   }
ReleaseSemaphore(&baselock);
return(count);
}

/*############################################################
NAME	
	delete_list_entry_by_text -

FUNCTION
	Searches a list and delete's any entries that have text
	that matches the passed string.  Currently case insensative
INPUTS
	list, str
RESULTS
	int (count of entries that matched str & where deleted)
##############################################################*/

int delete_list_entry_by_text(l, str)
struct list *l;
char *str;
{
struct MinNode *node;
int index,count;
char *text;

index=count=0;
ObtainSemaphore(&baselock);
node=l->entries.mlh_Head;
while(node->mln_Succ) {
   text=((struct list_entry *)node)->text;
	node = node->mln_Succ;
   if (stricmp(text,str)==0) {
		delete_list_entry_by_index(l, index);
		count++;
		}
	index++;
   }
ReleaseSemaphore(&baselock);
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

ObtainSemaphore(&baselock);
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (e--<=0)  {
		ReleaseSemaphore(&baselock);
      return((struct list_entry *)node);
		}
   }
ReleaseSemaphore(&baselock);
return(NULL);
}

/*############################################################
NAME	
	get_list_entry_index -
FUNCTION
	given a list entry pointer, it returns the entry's index
	in the list.
INPUTS
	list * & list_entry *
RESULTS
	int between 0 and l->list_size-1, inclusive
	NONE
##############################################################*/

int get_list_entry_index(l, le)
struct list *l;
struct list_entry *le;
{
struct MinNode *node;
int i;

i=0;
ObtainSemaphore(&baselock);
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (le==(struct list_entry *)node) {
		ReleaseSemaphore(&baselock);
      return(i);
		}
   i++;
   }
ReleaseSemaphore(&baselock);
return(-1);
}

/*############################################################
NAME	
	delete_list_entry_by_ptr -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

void delete_list_entry_by_ptr(l, le)
struct list *l;
struct list_entry *le;
{
int i;

i=get_list_entry_index(l, le);
delete_list_entry_by_index(l, i);
}

/*############################################################
NAME	
	delete_list_entry_by_index -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

void delete_list_entry_by_index(l, i)	/*		same as delete_list_entry*/
struct list *l;
int i;
{
delete_list_entry(l,i);
}

/*############################################################
NAME	
	find_list_entry_by_index -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

struct list_entry *find_list_entry_by_index(l, i)		/*		same as get_list_entry_ptr()*/
struct list *l;
int i;
{
return(get_list_entry_ptr(l, i));
}

/*############################################################
NAME	
	find_list_entry_by_text -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

struct list_entry *find_list_entry_by_text(l, str)
struct list *l;
char *str;
{
struct MinNode *node;

ObtainSemaphore(&baselock);
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (stricmp(((struct list_entry *)node)->text,str)==0) {
		ReleaseSemaphore(&baselock);
		return((struct list_entry *)node);
		}
   }
ReleaseSemaphore(&baselock);
return(NULL);
}

/*############################################################
NAME	
	find_list_entry_by_userdata -

FUNCTION
INPUTS
RESULTS
	NONE
##############################################################*/

struct list_entry *find_list_entry_by_userdata(l, ud)
struct list *l;
unsigned long ud;			/* match this to l->user_data */
{
struct MinNode *node;

ObtainSemaphore(&baselock);
for (node=l->entries.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (((struct list_entry *)node)->user_data==ud) {
		ReleaseSemaphore(&baselock);
		return((struct list_entry *)node);
		}
   }
ReleaseSemaphore(&baselock);
return(NULL);
}
/*############################################################
NAME	
	select_list_entry_by_userdata -

FUNCTION
	similar to handle_new_select except all you pass it is
	user data
INPUTS
RESULTS
	NONE
##############################################################*/

void select_list_entry_by_userdata(l, ud)
struct list *l;
unsigned long ud;			/* match this to l->user_data */
{
struct list_entry *le;

le=find_list_entry_by_userdata(l,ud);
if (le) {
	handle_new_select(l, le, get_list_entry_index(l,le));
	}
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

b=(struct button *)AllocMem(sizeof(struct button),MEMF_CLEAR);
if (b==0) return(0);

if (nb->text) 
	set_button_text(b,nb->text);

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
	SetDrMd(b->bw->RPort, JAM1);
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
short oldmask;

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
SetDrMd(b->bw->RPort, JAM1);
RectFill(b->bw->RPort,b->box_x1, b->box_y1, b->box_x2, b->box_y2);
if (b->flags&BORDER_3D) {
   draw_indent(b->bw->RPort, b->box_x1, b->box_y1, b->box_x2, b->box_y2, top_col, bot_col);
   }

if (b->text) {
	len = my_strlen(b->text);
	ypos = b->box_y1+(b->box_y2 - b->box_y1+6)/2;
	if (b->flags&BUTN_X)	{	/** X boxes's text goes to the right of box */
		SetAPen(b->bw->RPort, b->c->button.draw_unselected);
		xpos = b->box_x2+4;
		}
	else if (b->flags&BUTN_LOOP) {
		xpos = 19 + b->box_x1;
		SetAPen(b->bw->RPort, apen);
		}
	else {
		if (len*8 >= (b->box_x2-b->box_x1-5))
			len = (b->box_x2-b->box_x1-5)/8;
		if (len < 0) len=0;
		xpos = b->box_x1+(b->box_x2 - b->box_x1 - len*8)/2;
		SetAPen(b->bw->RPort, apen);
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
	oldmask=b->bw->RPort->Mask;
	b->bw->RPort->Mask=0xFF;
	BltBitMapRastPort(&mybm,0,0,b->bw->RPort,b->box_x1+4,b->box_y1+3,10,7,0x20);

	/** now, OR shape into planes specified by drawing color **/
	b->bw->RPort->Mask=apen;
	BltBitMapRastPort(&mybm,0,0,b->bw->RPort,b->box_x1+4,b->box_y1+3,10,7,0xe0);
	WaitBlit();
	b->bw->RPort->Mask=oldmask;
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

   ypos = b->box_y1+(b->box_y2 - b->box_y1+1-9)/2;

	/** first, cut shape out of all planes **/
	oldmask=b->bw->RPort->Mask;
	b->bw->RPort->Mask=0xFF;
	BltBitMapRastPort(&mybm,0,0,b->bw->RPort,b->box_x1+4,ypos,11,9,0x20);

	/** now, OR shape into planes specified by drawing color **/
	b->bw->RPort->Mask=apen;
	BltBitMapRastPort(&mybm,0,0,b->bw->RPort,b->box_x1+4,ypos,11,9,0xe0);
	WaitBlit();
	b->bw->RPort->Mask=oldmask;
   }
RestoreRast();
}

/*############################################################
NAME
	new_button_size -

FUNCTION
	Resize or move a button gadget.  Imagery is not updated, just
	internal data structures.
INPUTS
	b 	button to update
	x1,x2	new position,size
	y1,y2	new position,size
RESULTS
	NONE
##############################################################*/

void new_button_size(b, x1, y1, x2, y2)

struct button *b;
short x1,x2;
short y1,y2;
{
b->box_x1=x1;
b->box_y1=y1;
b->box_x2=x2;
b->box_y2=y2;
}

/****************************************************************/

static short butnxpos,butnypos;

/****************************************************************/

void handle_button(b, code, x, y)
struct button *b;
USHORT code;
int x,y;
{
struct button *temp;
struct MinNode *node;
void button_aftertaste();

if (code==GE_LBUTTONDOWN) {
   if (hit_button(b, x, y)) {
		butnxpos = x;
		butnypos = y;
      if (!(b->flags&BUTN_TOGGLE)) {
	      xor_button(b);
			selected_gadget=(struct GadgetHeader *)b;
			selected_gadget_func=button_aftertaste;
			}
		else {			/** BUTN_TOGGLE **/
			if (!(b->flags&KEEP_ONE_ACTIVE && b->flags&BUTN_HIT))
		      xor_button(b);
			active_gadget(b);
			if ((b->flags&BUTN_HIT) && b->id!=0) { /** MUTUAL EXCLUDE! **/
				for (node=gad_list.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
					temp=(struct button *)node;
				   if (b->bw==temp->bw && b->id==temp->id && b!=temp) {
						if (temp->flags&BUTN_HIT) {
							xor_button(temp);
				         active_gadget(temp);
							}
						}
					}
				}
         } 
      }
   }
}

/****************************************************************/

void button_aftertaste(b, code, x, y)
struct button *b;
USHORT code;
int y, x;
{
/** Handle pointer moving around with left mouse button down **/
/** (The gadget should deselect when the mouse moves away) **/
/** (and reselect when the mouse moves back over gadget) **/

	if (code==GE_DELTAMOVE) {
		butnxpos += x;
		butnypos += y;

		if (b->flags&BUTN_HIT) {
		   if (!hit_button(b, butnxpos, butnypos)) {
	   	   xor_button(b);
				}
			}
		else {		/** !BUTN_HIT **/				
			if (hit_button(b, butnxpos, butnypos)) {
	   	   xor_button(b);
				}
			}
		}

if (code==GE_LBUTTONUP) {
	if (b->flags&BUTN_HIT) {
	   xor_button(b);		/** Removed some releasebutton=NULL here?? **/
	   active_gadget(b);	/**!!!!! SHOULD I SET/RESET BUTN_HIT AROUND THIS?? **/
		}
	selected_gadget=NULL;
	selected_gadget_func=NULL;

   return;
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

/*############################################################
NAME	-
	set_button_text
FUNCTION
	Changes a button's text.  The old text's memory is free'd
	and a copy is made of the new text.   You must call
	refresh_gadget() to display the new text.
INPUTS
	b - button whos text is to change
	text - char * to text
RESULTS
	NONE
##############################################################*/

void set_button_text(b,text)
struct button *b;
char *text;
{
int len;

if (b->text) {
   len = my_strlen(b->text)+1;
	FreeMem(b->text,len);
	}

len = my_strlen(text)+1;
b->text = (char *)AllocMem(len, 0);
if (b->text)
	strncpy(b->text, text, len);
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
nb.flags = BUTN_COOL|BUTN_DROP_ARROW|MANUAL_XOR|GAD_SIBLING|(nd->flags&HIDE_GADGET);
nl.flags = LIST_COOL|HIDE_GADGET|GAD_SIBLING|LIST_SINGLESELECT|(nd->flags&KEEP_ONE_ACTIVE);

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

hide_gadget(d->l);
set_gadget_window(d->l,NULL);
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
if (d->flags&HIDE_GADGET)
	return;

refresh_list(d->l);
refresh_button(d->b);
refresh_droplist_text(d);
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
	if (w==0) 
		w=QuickWindow(d->c, d->box_x1+d->dw->LeftEdge-2, d->box_y1+d->dw->TopEdge - (d->box_y2-d->box_y1-STD_DROP_ARROW_HEIGHT) - 3, 
			d->box_x2+d->dw->LeftEdge+2, d->box_y1+d->dw->TopEdge - 2,0,BORDERLESS, 0, NULL);
	if (w) {
		set_gadget_window(d->l,w);
 		show_gadget(d->l);
		refresh_list(d->l);
		}
	}
else {
	hide_gadget(d->l);
	CloseWindow(d->l->lw);
	set_gadget_window(d->l,NULL);
	}
}

/***********************************************************/

void dl_list_func(l)
struct list *l;
{
struct droplist *d;

d = (struct droplist *)l->user_data;
refresh_droplist_text(d);

if (!(l->flags&HIDE_GADGET)) {
	active_gadget(d);
	hide_gadget(l);
	CloseWindow(l->lw);
	l->lw = NULL; 
	l->slider->kw = NULL;
	l->slider->b1->bw = NULL;
	l->slider->b2->bw = NULL;
	}
}

/****************************************************************/

void refresh_droplist_text(d)
struct droplist *d;
{
char *text;
int xpos,ypos,len;

if (d->flags&HIDE_GADGET)
	return;

if (d->l) {
	if (d->l->current==-1)
		text = NULL;
	else
		text = d->l->active_entry->text; 
	}
else {
	text = NULL;
	}

SaveRast(d->dw->RPort); 
SetAPen(d->dw->RPort, d->c->droplist.hitbox_unselected);
SetDrMd(d->dw->RPort, JAM1);
RectFill(d->dw->RPort, d->box_x1, d->box_y1,
    d->box_x2 - STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP,
	 d->box_y1 + STD_DROP_ARROW_HEIGHT - 1);

draw_indent(d->dw->RPort, d->box_x1, d->box_y1, d->box_x2 - STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP,
	 d->box_y1 + STD_DROP_ARROW_HEIGHT - 1, d->c->droplist.topleft_3d,
	 d->c->droplist.bottomright_3d);

SetAPen(d->dw->RPort, d->c->droplist.draw_unselected);
SetBPen(d->dw->RPort, d->c->droplist.hitbox_unselected);
SetDrMd(d->dw->RPort, JAM2);
xpos = d->box_x1+4;
ypos = d->box_y1+(STD_DROP_ARROW_HEIGHT-1+6)/2;
len = my_strlen(text);
if (len*8 >= (d->box_x2-d->box_x1- STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP-5))
	len = (d->box_x2-d->box_x1- STD_DROP_ARROW_WIDTH - STD_SLIDER_GAP-5)/8;
if (len < 0) len=0;
Move(d->dw->RPort, xpos, ypos);
Text(d->dw->RPort, text, len);

RestoreRast();
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
nb.flags = BUTN_COOL|BUTN_LOOP|MANUAL_XOR|GAD_SIBLING|(nll->flags&HIDE_GADGET);
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
if (ll->flags&HIDE_GADGET)
	return;
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

ObtainSemaphore(&baselock);
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
ReleaseSemaphore(&baselock);
refresh_button(ll->b);
active_gadget(ll);
}


/*############################################################
NAME	
	QuickWindow-

FUNCTION
	Opens a new window. Flags ACTIVATE,NOCAREREFRESH
	are always set. A "3D" type border is drawn unless BORDERLESS
	is set.  If c==NULL, defaultcolors are used.  See
	"set_default_colors"
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

if (c==NULL)
	c = &default_colors;

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
	SetAPen(w->RPort, c->gadget_window.hitbox_unselected);
	RectFill(w->RPort, 0,title ? 10:0, w->Width-1, w->Height-1);
	if (!(flags&BORDERLESS)) 			/**** FIX THIS BELOW ***/
		draw_indent(w->RPort, 0, title ? 10:0, w->Width-1,w->Height-2, c->gadget_window.topleft_3d, c->gadget_window.bottomright_3d);
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

	A call to hide_gadget() does not alter the currently displayed
	gadget's imagery.
INPUTS
	g - pointer to a gadget
RESULTS
	NONE
##############################################################*/

void hide_gadget(g)
struct GadgetHeader *g;
{
g->flags |= HIDE_GADGET;

if ((g->flags&GAD_SLIDER) && (g->flags&SLIDER_ARROWS)) {
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
	Resets the HIDE_GADGET flag in a gadget, and all its associated
	gadgets.  Thus, a list gadget would also affect a slider and
	two arrow buttons.  The gadgets imagery is not altered.  Use
	refresh_gadget() to refresh the gadget's imagery.
INPUTS
	g - pointer to a gadget
RESULTS
	NONE
##############################################################*/

void show_gadget(g)
struct GadgetHeader *g;
{
g->flags &= (~HIDE_GADGET);

if ((g->flags&GAD_SLIDER) && (g->flags&SLIDER_ARROWS)) {
	show_gadget(((struct slider *)g)->b1);
	show_gadget(((struct slider *)g)->b2);
	}
else if (g->flags&GAD_LIST) {
	show_gadget(((struct list *)g)->slider);
	}
else if (g->flags&GAD_DROPLIST) {
	show_gadget(((struct droplist *)g)->b);
	}
else if (g->flags&GAD_LOOPLIST) {
	show_gadget(((struct looplist *)g)->b);
	}
}

/*############################################################
NAME	
	set_gadget_window -

FUNCTION
	Changes the window a gadget is in.  For example, If you have
	a list gadget inside a window, you may want to close the window,
	but keep the list gadget.  You could call "hide_gadget()" to
	deactivate the gadget, then close the window.  When you want
	to reactivate the list gadget, you would open a new window,
	call set_gadget_window(),show_gadget(),refresh_gadget().
INPUTS
	g 	- gadget to change window on
	w	- new Window pointer
RESULTS
	NONE
##############################################################*/

void set_gadget_window(g,w)
struct GadgetHeader *g;
struct Window *w;
{

g->w = w;

if ((g->flags&GAD_SLIDER) && (g->flags&SLIDER_ARROWS)) {
	set_gadget_window(((struct slider *)g)->b1,w);
	set_gadget_window(((struct slider *)g)->b2,w);
	}
else if (g->flags&GAD_LIST) {
	set_gadget_window(((struct list *)g)->slider,w);
	}
else if (g->flags&GAD_DROPLIST) {
	set_gadget_window(((struct droplist *)g)->l,w);
	set_gadget_window(((struct droplist *)g)->b,w);
	}
else if (g->flags&GAD_LOOPLIST) {
	set_gadget_window(((struct looplist *)g)->l,w);
	set_gadget_window(((struct looplist *)g)->b,w);
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
delete_gadget(d->l);
d->b->text = NULL;
delete_gadget(d->b);
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
delete_gadget(ll->l);
ll->b->text = NULL;
delete_gadget(ll->b);
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
	Pass two pieces of text & flags. eg. 
   askuser("Are you Sure","You want to quit?",0);
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
	Displays a message and waits for OK to be clicked
INPUTS
	Pass two pieces of text. eg. telluser("warning:","diskerror",0);
   NULL is ok for text not being used.
	Flags 0 or NOTICE_RETURN_NOW which causes telluser() to launch
	a task that waits for the user to click OK. If "NOTICE_RETURN_NOW"
	is set, the call to telluser() is asycronous and will return
	immediately.
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
int snapshot;
notice_line1[next_notice]=l1;
notice_line2[next_notice]=l2;
notice_flags[next_notice]=flags;

if (flags&NOTICE_RETURN_NOW) {
	snapshot=next_notice;		/** pri 2 should be gadlibtask+1 **/
	CreateTask("GadLib_UserNotice",2,user_notice_code,2000);
	while (snapshot==next_notice) WaitTOF();
	return(TRUE);
	}
return(user_notice_code());
}
	
/*************************************************************/

int user_notice_code()
{
struct TextAttr MyFont = {"topaz.font",TOPAZ_EIGHTY,FS_NORMAL,FPF_ROMFONT };

static struct NewButton cancelbutnguide = 
	{NULL,20,29,50,41,BUTN_COOL,0,0,&default_colors,0,0,0,"CANCEL"};
static struct NewButton okbutnguide = 
	{NULL,20,29,50,41,BUTN_COOL,0,0,&default_colors,0,0,0,"OK"};
static last_te=0;
static last_le=0;
char *l1,*l2;
unsigned long flags;
struct NewWindow NewWindow;
struct NewButton okbutn;
struct NewButton cancelbutn;
int width;
struct Window *w;
struct GadgetHeader *g;
struct GadMsg *msg;
int return_code;
struct TextFont *fnt;
int MX=IntuitionBase->ActiveScreen->MouseX,MY=IntuitionBase->ActiveScreen->MouseY;
struct Screen *screen=IntuitionBase->ActiveScreen;
int LE,TE;

Forbid();
l1=notice_line1[next_notice];
l2=notice_line2[next_notice];
flags=notice_flags[next_notice];
next_notice = (next_notice+1)&(MAX_NOTICES-1);
Permit();

okbutn = okbutnguide;
cancelbutn = cancelbutnguide;

width = max(my_strlen(l1),my_strlen(l2));
width = width*8 + 20;

if (width<150) width=150;

TE=MY-35;
if(TE<0) TE=0;
if((TE+46) > screen->Height) TE=screen->Height-46;

LE=MX-(width/2);
if(LE<0) LE=0;
if((LE+width) > screen->Width) LE=screen->Width-width;

/** The following code stop's "creeping" **/

if (flags&NOTICE_CANCEL) {
	if (ptinbox(MX,MY,last_le,last_te,width,46)) {
		TE=last_te;
		LE=last_le;
		}
	}
last_le=LE;
last_te=TE;

setmem(&NewWindow,sizeof(struct NewWindow),0);
NewWindow.Height   = 46;
NewWindow.Width = width;
NewWindow.TopEdge = TE;
NewWindow.LeftEdge = LE;
NewWindow.Screen = screen;
NewWindow.DetailPen = BLACK;
NewWindow.BlockPen = WHITE;
NewWindow.Flags = ACTIVATE|NOCAREREFRESH|BORDERLESS;
NewWindow.MinWidth    = NewWindow.Width;
NewWindow.MinHeight   = NewWindow.Height;
NewWindow.MaxWidth    = NewWindow.Width;
NewWindow.MaxHeight   = NewWindow.Height;
NewWindow.Type = CUSTOMSCREEN;

lock_gadlib++;

if (w=(struct Window *)OpenWindow(&NewWindow)) {
	fnt=(struct TextFont *)OpenFont(&MyFont);
	SetFont(w->RPort,fnt);

	SetDrMd(w->RPort,JAM1);
	SetAPen(w->RPort,2);
	RectFill(w->RPort, 2,1, w->Width-3, w->Height-2);

	draw_indent(w->RPort, 0, 0, w->Width-1,w->Height-1, default_colors.gadget_window.topleft_3d, default_colors.gadget_window.bottomright_3d);

	SetAPen(w->RPort,default_colors.gadget_window.bottomright_3d);
	if (flags&NOTICE_CENTER) Move(w->RPort,10+(((width-20)-(my_strlen(l1)*8))/2)+1, 12);
	else Move(w->RPort,10+1, 12);
	Text(w->RPort, l1, my_strlen(l1)); 
	if (flags&NOTICE_CENTER) Move(w->RPort,10+(((width-20)-(my_strlen(l2)*8))/2)+1, 23);
	else Move(w->RPort,10+1, 23);
	Text(w->RPort, l2, my_strlen(l2));

	SetAPen(w->RPort,default_colors.gadget_window.topleft_3d);
	if (flags&NOTICE_CENTER) Move(w->RPort,10+(((width-20)-(my_strlen(l1)*8))/2)-1, 11);
	else Move(w->RPort,10-1, 11);
	Text(w->RPort, l1, my_strlen(l1)); 
	if (flags&NOTICE_CENTER) Move(w->RPort,10+(((width-20)-(my_strlen(l2)*8))/2)-1, 22);
	else Move(w->RPort,10-1, 22);
	Text(w->RPort, l2, my_strlen(l2));

	if (flags&NOTICE_CANCEL)
		{
		if (w->Width > 200)
			{
			okbutn.box_x1=30;
			cancelbutn.box_x1=w->Width - 91;
			}
		else
			{
			okbutn.box_x1=9;
			cancelbutn.box_x1=w->Width - 70;
			}
		okbutn.box_x2=okbutn.box_x1+60;
	 	cancelbutn.box_x2=cancelbutn.box_x1+60;
		okbutn.next = &cancelbutn;
		}
	else 
		{
		okbutn.box_x1=(w->Width-50)/2;
		okbutn.box_x2=okbutn.box_x1+50;
		okbutn.next = NULL;
		}

	okbutn.notifyme=CreatePort(0,0);
	cancelbutn.notifyme=okbutn.notifyme;

	if (okbutn.notifyme) {
	   if (g=create_gadget_chain(w, &okbutn)) {
   	   WaitPort(okbutn.notifyme);
		 	msg=(struct GadMsg *)GetMsg(okbutn.notifyme);
			if (msg->g==okbutn.gad_ptr)
				return_code=TRUE;
			else
				return_code=FALSE;
			ReplyMsg(msg);
         delete_gadget_chain(w);
	      }
		DeletePort(okbutn.notifyme);
		}
	CloseFont(fnt);
   CloseWindow(w);
   }
lock_gadlib--;
if (flags&NOTICE_RETURN_NOW) {
	RemTask(0);
	Wait(0);
	}
return(return_code);
}

ptinbox(px,py,ble,bte,bw,bh)
{
return(px > ble && px < (ble+bw) && py > bte && py < (bte+bh));
}
/*############################################################
NAME	
	new_pointer -

FUNCTION
	Sets the mouse pointer to one of a few built in symbols,
	like "busy".
INPUTS
	short value of pointer to use (see gadlib.h)
	Example: new_pointer(PTR_BUSY);
				new_pointer(PTR_NORMAL);
RESULTS
	NONE
##############################################################*/

void new_pointer(mode)

SHORT mode;
{
static short lastmode = PTR_NORMAL;
static short thismode = PTR_NORMAL;
struct Window *w;
struct Screen *screen;
void setall();

static unsigned short chip paste[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x8000, 0x8000,
   0x7c00, 0x7c00,
   0x3e00, 0x3e00,
   0x1d00, 0x1f00,
   0x180, 0x280,
   0xc0, 0x140,
   0x60, 0xa0,
   0x30, 0x50,
   0x18, 0x28,
   0xc, 0x14,
   0x6, 0xa,
   0x2, 0x5,
   0x0, 0x3,
   0x0, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -1, YOFFSET = 0 */



static unsigned short chip busy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x380, 0x380,
   0x1FE0, 0x1FE0,
   0x3FFC, 0x3FFC,
   0x3FFE, 0x31FE,
   0x3FFE, 0x3DFE,
   0x7FFE, 0x7B1E,
   0x7FFE, 0x77DE,
   0x7FFC, 0x71BC,
   0x7FFC, 0x7F7C,
   0x3FFC, 0x3F1C,
   0x7FF8, 0x7FF8,
   0x7F00, 0x7F00,
   0x3BC0, 0x3BC0,
   0x1C0, 0x1C0,
   0x80, 0x80,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -1 */

static unsigned short chip crop[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x4000, 0x4000,
   0x6000, 0x6000,
   0x7000, 0x7000,
   0x7800, 0x7800,
   0x7C00, 0x7C00,
   0x7400, 0x7400,
   0xF00, 0xD00,
   0x780, 0x80,
   0xFC0, 0x40,
   0x7E0, 0x20,
   0x3F0, 0x10,
   0x1F8, 0x8,
   0xF0, 0x0,
   0x60, 0x0,
   0x0, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
/* WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = 0 */

static unsigned short chip hand[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x6c00, 0xc00,
   0x3680, 0x3680,
   0x1b40, 0x1b40,
   0x6da0, 0x6da0,
   0x36d0, 0x36d0,
   0x1b78, 0x1b78,
   0xdfc, 0xdfc,
   0x7fc, 0x7fc,
   0x63fe, 0x63fe,
   0x3bfe, 0x3bfe,
   0x1ffd, 0x1ffd,
   0xffb, 0xffb,
   0x3f6, 0x3f6,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -2, YOFFSET = -2 */

/****
static unsigned short chip copy[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x0, 0x0,
   0x6EEA, 0x0,
   0x4AAA, 0x0,
   0x6EEE, 0x0,
   0x84, 0x0,
   0x84, 0x0,
   0x80, 0x0,
   0x2A0, 0x0,
   0x1C0, 0x0,
   0x80, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
****/
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -14 */


static unsigned short chip cross[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x0, 0x0,
   0x7EBF, 0x80,
   0x0, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x80, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};

/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -7 */

/****
static unsigned short chip nothing[] = {
   0x0000, 0x0000,
   0x0, 0x0,
   0x0, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x0, 0x0,
   0x0000, 0x00,
   0x0, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x00, 0x0,
   0x0000, 0x0000,
   0x0000, 0x0000,
};
*****/
/*  WIDTH = 16, HEIGHT = 16
 XOFFSET = -9, YOFFSET = -7 */




if (mode==PTR_RESTORE) mode=lastmode;     /* restore previous mode */
lastmode=thismode;    /* need to remember last 2 modes for RESTORE */
thismode=mode;

/* if (!menus_on && mode!=BUSY) {
   setmenus(screen, TRUE);
   menus_on = TRUE;
   }  */

screen = IntuitionBase->ActiveScreen;

switch (mode) {

   case PTR_NORMAL:
      for (w=screen->FirstWindow;w!=0;w=w->NextWindow) {
         ClearPointer(w);
         }
      return;

   case PTR_BUSY:

      setall(screen,busy, 16, 16, -2, -1);
/*      setmenus(screen, FALSE);
      menus_on = FALSE; */
      break;


   case PTR_GRABIT:

      setall(screen,hand, 16, 16, -2, -2);
      break;


   case PTR_CROP:

      setall(screen,crop, 16, 16, -2, -0);
      break;


   case PTR_CROSS:

      setall(screen,cross, 16, 16, -9, -7);
      break;

   case PTR_PASTE:

      setall(screen,paste, 16, 16,-1, 0);
      break;

   }  /** end of switch **/
}

/************************************/
/** change pointers for all windows */
/************************************/

void setall(s,data,a,b,c,d)

struct Screen *s;
char *data;
int a,b,c,d;
{
struct Window *w;

for (w=s->FirstWindow; w!=0; w=w->NextWindow) {
   SetPointer(w,data,a,b,c,d-1);
   }
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
               inputHandler->is_Node.ln_Name="GadLibHandler";
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
	LockGadLibBase -
FUNCTION
	Obtain's a Semaphore lock on the gadlib's internal strucutres.
	No gadlib access to internal structures
	operations will occur while a lock is in place.
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void LockGadLibBase()
{
ObtainSemaphore(&baselock);
}

/*############################################################
NAME	
	UnLockGadLibBase() -
FUNCTION
	Free's the lock obtained by a call to LockGadLib().  Every
	call to LockGadLib() must be followed by a call to
	UnLockGadLib();
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void UnLockGadLibBase()
{
ReleaseSemaphore(&baselock);
}

/*############################################################
NAME
	LockGadLibRender -
FUNCTION
	Obtain's a Semaphore lock on the gadlib's render lock.
	No gadlib access to the graphics.library
	will occur while a lock is in place.
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void LockGadLibRender()
{
/*ObtainSemaphore(&renderlock);*/
ObtainSemaphore(&baselock);
WaitBlit();
}

/*############################################################
NAME	
	UnLockGadLibRender() -
FUNCTION
	Free's the lock obtained by a call to LockGadLibRender().  Every
	call to LockGadLibRender() must be followed by a call to
	UnLockGadLibRender();
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/
static int srkount;

void UnLockGadLibRender()
{
/*ReleaseSemaphore(&renderlock);*/
WaitBlit();
ReleaseSemaphore(&baselock);
}

/********************************************************************/


void SaveRast(rp)
struct RastPort *rp;
{
/*ObtainSemaphore(&renderlock);*/		/**!!!! I CHANGED ALL 4 rederlocks **/
/*ObtainSemaphore(&baselock);*/	/** to baselocks to prevent occasional lockups **/
LockGadLibRender();
}	/** caused by conflicting base/renderlocks.  Fixx beter when have time! ***/

/********************************************************************/

void RestoreRast()
{
/*ReleaseSemaphore(&renderlock);*/
/*ReleaseSemaphore(&baselock);*/
UnLockGadLibRender();
}

/*############################################################
NAME	set_default_colors -
FUNCTION
	Set's the colors to be used by such functions as telluser,
	askuser, & QuickWindow (when c==NULL)
INPUTS
	pointer to a GadgetColors struct
RESULTS
	NONE
##############################################################*/

void set_default_colors(c)
struct GadgetColors *c;
{
default_colors = *c;
}
