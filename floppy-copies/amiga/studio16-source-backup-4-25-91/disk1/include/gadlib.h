#ifndef GADLIB_H
#define GADLIB_H TRUE

/** flags **/

#define KNOB_3D    (1<<0)
#define BORDER_3D  (1<<1)
#define FREE_HORIZ (1<<2)				/* knob can move left to right */
#define FREE_VERT  (1<<3)				/* knob can move up and down */
#define AUTO_KNOB  (1<<4)				/* knob changes size dynamicaly */
#define BUTN_TOGGLE (1<<5) 
#define BUTN_HIT    (1<<6)
#define BUTN_UP_ARROW (1<<7)
#define BUTN_DOWN_ARROW (1<<8)
#define BUTN_LEFT_ARROW (1<<9)
#define BUTN_RIGHT_ARROW (1<<10)
#define BUTN_X  (1<<11)
#define SLIDER_ARROWS  (1<<12)
#define GAD_LIST (1<<13)
#define GAD_SLIDER (1<<14)
#define GAD_BUTTON (1<<15)
#define LIST_NEWSELECT (1<<16)
#define LIST_DOUBLESELECT (1<<17)
#define KNOB_NOGAP (1<<18)
#define MANUAL_XOR (1<<19)
#define KNOB_TODDISH (1<<20)
#define KEEP_ONE_ACTIVE (1<<21)			/* for butn or list */	
#define HIDE_GADGET (1<<22)
#define BUTN_DROP_ARROW (1<<23)
#define GAD_DROPLIST (1<<24)
#define GAD_SIBLING (1<<25)
#define LIST_SINGLESELECT (1<<26)		/* only one allowed active */


/** define the flag configuration that looks the best */

#define SLIDER_COOL (GAD_SLIDER|KNOB_NOGAP|KNOB_3D|BORDER_3D)
#define BUTN_COOL (GAD_BUTTON|BORDER_3D)
#define LIST_COOL (GAD_LIST|KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH|KNOB_NOGAP)
#define DROPLIST_COOL (GAD_DROPLIST|KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH|KNOB_NOGAP)

/**** misc defines ***/

#define STD_V_ARROW_SIZE 8
#define STD_H_ARROW_SIZE 16
#define STD_SLIDER_WIDTH 17
#define STD_SLIDER_GAP 3
#define KNOB_TODDISH_SIZE 2
#define STD_DROP_ARROW_HEIGHT 12
#define STD_DROP_ARROW_WIDTH 17

/**** gadget structures ****/

struct HitAreaColor {
	unsigned char background;
	unsigned char topleft_3d;
	unsigned char bottomright_3d;
	unsigned char hitbox_unselected;
	unsigned char hitbox_selected;
	unsigned char draw_unselected;
	unsigned char draw_selected;
	};

struct GadgetColors {
	struct HitAreaColor slider_box;
	struct HitAreaColor slider_knob;
	struct HitAreaColor slider_button;
	struct HitAreaColor button;
	struct HitAreaColor list;
	struct HitAreaColor droplist;
	};

#if 0
struct GadgetColors {
	unsigned char topleft_3d;
	unsigned char bottomright_3d;
	unsigned char background;
	unsigned char global_text;		/** !!! NOT USED - REMOVE? **/
	unsigned char global_image;
	unsigned char slider_hitbox;
	unsigned char knob_unselected;
	unsigned char knob_selected;
	unsigned char butn_unselected_hitbox;
	unsigned char butn_selected_hitbox;
	unsigned char butn_unselected_text;	/** DIFFERENTIATE BUTN_X?? **/
	unsigned char butn_selected_text;
	unsigned char butn_arrow_image;	/** ADD SELECTED & UNSEL?? **/
	unsigned char list_hitbox;
	unsigned char list_text;
	};
#endif
	
struct NewGadgetHeader {   /* all New gadget structs start this way */
   struct NewGadgetHeader *next;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();         /* call this after top_element changes */
   APTR user_data;
	struct GadgetColors *c;
	short id;					/** mutual exclude gadgets with same id */
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
   };

struct GadgetHeader {         /* all gadget structs start this way */
   struct MinNode node;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();
   APTR user_data;
	struct GadgetColors *c;
	short id;
   struct Window *w;       /* window this gadget is in */
 };
 
struct NewSlider {
   struct NewGadgetHeader *next;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();         /** call this after top_element changes */
   APTR user_data;
	struct GadgetColors *c;
	short id;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
   short list_size;
   short show_size;
   short top_element;
   short knob_width;          /* used if not autoknob */
   short knob_height;
   };

struct slider {
   struct MinNode node;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();         /** call this after top_element changes */
   APTR user_data;
	struct GadgetColors *c;
	short id;
   struct Window *kw;      /* window this slider is in */
   short list_size;
   short show_size;
   short top_element;
   short knob_x;           /* size & position (inclusive) of knob */
   short knob_y;
   short knob_width;
   short knob_height;
   struct button *b1;      /* used if arrow buttons attached */
   struct button *b2;   
   };


/**IDEAS!!! add function that creates everyhing from  a text
string.   Eg. button=AddTextButton(XPOS, YPOS, "TEST",&func()); also
OR QuickButton(), QuickWindow(),
!!!!!!*****/

struct NewButton {
   struct NewGadgetHeader *next;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();
   APTR user_data;
	struct GadgetColors *c;
	short id;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
   char *text;             /* center this in button */
   };

struct button {
   struct MinNode node;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();
   APTR user_data;
	struct GadgetColors *c;
	short id;
   struct Window *bw;      /* window this button is in */
   char *text;             /* center this in button */
   };

struct NewList {
   struct NewGadgetHeader *next;
   short box_x1;
   short box_y1;
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();       
   APTR user_data;
	struct GadgetColors *c;
	short id;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
	char **e_text;
	unsigned long *e_user_data; 
   };

struct list {
   struct MinNode node;
   short box_x;
   short box_y;							/* upper left corner of list box */
   short box_x2;
   short box_y2;
   unsigned long flags;	
   void (*func)();    
   APTR user_data;
	struct GadgetColors *c;
	short id;
   struct Window *lw;		      	/* window this list is in */
   short list_size;
   short show_size;
   short top_element;
   short current;							/* index of active_entry */
   struct list_entry  *active_entry;/* entry selected */	   
   short max_show_chars;	/* width for displaying list entries */
   struct slider *slider;				/* for scrolling through list */
   struct MinList entries;				/* chain of list_entry */
   };


/*** FLAGS used IN list_entry ****/

#define LE_SELECTED 1

struct list_entry {
   struct MinNode e_node;
	struct MinNode a_node;
   char *text;
   unsigned long user_data;
	unsigned long flags;
   };

struct NewDropList {
	struct NewGadgetHeader *next;
	short box_x1;
	short box_y1;
	short box_x2;
	short box_y2;
	unsigned long flags;
	void (*func)();       
	APTR user_data;
	struct GadgetColors *c;
	short id;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
	char **e_text;
	unsigned long *e_user_data; 
   };

struct droplist {         /* all gadget structs start this way */
   struct MinNode node;
   short box_x1;
   short box_y1;           /* size (inclusive) of container */
   short box_x2;
   short box_y2;
   unsigned long flags;
   void (*func)();
   APTR user_data;
	struct GadgetColors *c;
	short id;
   struct Window *dw;      /* window this gadget is in */
	struct list *l;			/* actual list */
	struct button *b;			/* button that triggers this droplist */
 };


/***********************************/
/** input.device handler defines ***/
/***********************************/

#define GE_LBUTTONDOWN 1
#define GE_LBUTTONUP 2
#define GE_DELTAMOVE 3

struct GadEvent {
   struct Window *window;     /** active window **/
   short x;                   /** xpos or delta x */
   short y;
   short type;                /** BUTTONDOWN or BUTTONUP **/
   };

#endif

