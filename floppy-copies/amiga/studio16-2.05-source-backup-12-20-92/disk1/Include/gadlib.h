#ifndef GADLIB_H
#define GADLIB_H TRUE

/* COPYRIGHT 1992 SunRize Industries */

/********** START FLAGS ***********/

/*** Gadget Type ***/

#define GAD_LIST (1<<0)
#define GAD_SLIDER (1<<1)
#define GAD_BUTTON (1<<2)
#define GAD_DROPLIST (1<<3)
#define GAD_LOOPLIST (1<<4)
#define GAD_BOX		(1<<25)

/** General Gadget Qualiiers ***/

#define KNOB_3D    (1<<5)
#define BORDER_3D  (1<<6)
#define BUTN_HIT    (1<<7)					/* Should be called GAD_HIT */
#define MANUAL_XOR (1<<8)
#define KEEP_ONE_ACTIVE (1<<9)			/* for butn or list */	
#define HIDE_GADGET (1<<10)
#define GAD_SIBLING (1<<11)
#define RENDER_ONLY (1<<12)
#define NOT_IN_CHAIN (1<<13)
#define TEXT_ONLY_FILE_NAME		(1<<28)
#define TEXT_ONLY_REGION_NAME		(1<<29)


/** Slider Flags (also used in lists for setting list sliders) **/

#define FREE_HORIZ (1<<14)				/* knob can move left to right */
#define FREE_VERT  (1<<15)				/* knob can move up and down */
#define AUTO_KNOB  (1<<16)				/* knob changes size dynamicaly */
#define SLIDER_ARROWS  (1<<17)	
#define KNOB_TODDISH (1<<18)

/* List Flags **/

#define LIST_NEWSELECT (1<<19)
#define LIST_DOUBLESELECT (1<<20)
#define LIST_USEKEYS (1<<21)
#define LIST_SINGLESELECT (1<<22)		/* only one allowed active */
#define LIST_SORTED (1<<23)
#define LIST_DRAGABLE (1<<27)

/** Drop list flags **/

#define DROPLIST_SELECTED (1<<24)

/** Button Flags (values duplicate list values to make flag room) **/

#define BUTN_TOGGLE (1<<17) 
#define BUTN_UP_ARROW (1<<18)
#define BUTN_DOWN_ARROW (1<<19)
#define BUTN_LEFT_ARROW (1<<20)
#define BUTN_RIGHT_ARROW (1<<21)
#define BUTN_X  (1<<22)
#define BUTN_DROP_ARROW (1<<23)
#define BUTN_LOOP (1<<24)

/** Box Gadget Qualifiers **/

#define BOX_NORENDER		(1<<26)

/********** END FLAGS ***********/

/** define the flag configuration that looks the best */

#define SLIDER_COOL (GAD_SLIDER|KNOB_3D|BORDER_3D)
#define BUTN_COOL (GAD_BUTTON|BORDER_3D)
#define LIST_COOL (GAD_LIST|KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH)
#define DROPLIST_COOL (GAD_DROPLIST|KNOB_3D|BORDER_3D|SLIDER_ARROWS|KNOB_TODDISH|KEEP_ONE_ACTIVE)
#define LOOPLIST_COOL (GAD_LOOPLIST|BORDER_3D)
#define BOX_COOL (GAD_BOX|BORDER_3D)

/**** misc defines ***/

#define STD_V_ARROW_SIZE 8
#define STD_H_ARROW_SIZE 16
#define STD_SLIDER_WIDTH 17
#define STD_SLIDER_GAP 3
#define KNOB_TODDISH_SIZE 2
#define STD_DROP_ARROW_HEIGHT 12
#define STD_DROP_ARROW_WIDTH 17
#define MAX_NOTICES 32

/**** MSG's sent by gadlib to notifyme port *****/

#define GADMSG_ACTIVE 1
#define GADMSG_RELEASED 2
#define GADMSG_RAWKEY 3
#define GADMSG_DRAG_AND_DROP 4

/**** The actual message sent to notifyme port ****/

struct GadMsg {
	struct Message message;
	struct GadgetHeader *g;
	unsigned long type;
	unsigned long flags;
	char *text;							/** from->text **/
   APTR user_data;					/** from->userdata **/
	struct GadgetHeader *from;		/** source of drag&drop **/
	struct list_entry *le;			/** g->le droped on **/
	short shift_down;					/** Shift key held down? **/
	};

/**** telluser and askuser flags *****/

#define NOTICE_CANCEL (1<<0)
#define NOTICE_RETURN_NOW (1<<1)
#define NOTICE_CENTER (1<<2)

/**** gadget structures ****/

struct HitAreaColor {
	unsigned char background;	/** SHOULD gadget_window REPLACE THIS!!?? **/
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
	struct HitAreaColor gadget_window;
	};

/** Used to request information about new key presses **/

struct NewKeyNotice {
	struct Window *w;		/* This window must be active or NULL for any active window **/
   void (*func)();      /* call this when key press occurs && w active **/
   APTR user_data;
	};

struct key_notice {
	struct MinNode node;
	struct Window *w;
   void (*func)();      /* call this when key press occurs && w active **/
   APTR user_data;
	};
	
/** All new gadget structures start out like this **/

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
	struct MsgPort *notifyme;
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
	struct MsgPort *notifyme;
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
	struct MsgPort *notifyme;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
   long list_size;
   long show_size;
   long top_element;
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
	struct MsgPort *notifyme;
   struct Window *kw;      /* window this slider is in */
   long list_size;
   long show_size;
   long top_element;
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
	struct MsgPort *notifyme;
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
	struct MsgPort *notifyme;
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
	struct MsgPort *notifyme;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
	char **e_text;
	unsigned long *e_user_data; 
	long initial_value;
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
	struct MsgPort *notifyme;
   struct Window *lw;		      	/* window this list is in */
   short list_size;
   short show_size;
   short top_element;
   short current;							/* index of active_entry */
   struct list_entry  *active_entry;/* entry selected */	   
   short max_show_chars;	/* width for displaying list entries */
   struct slider *slider;				/* for scrolling through list */
   struct MinList entries;				/* chain of list_entry */
	struct key_notice *kn;				/* is LIST_USEKEYS set **/
   };


/*** FLAGS used IN list_entry ****/

#define LE_SELECTED 	(1<<0)
#define LE_HIGHLIGHT (1<<1)

struct list_entry {
   struct MinNode e_node;
	struct MinNode a_node;	/** NOT USED!!?? REMOVE!!! ***/
   char *text;
   unsigned long user_data;
	unsigned long flags;
	unsigned char highlight_color;	/** Non zero over rides default text color **/
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
	struct MsgPort *notifyme;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
	char **e_text;
	unsigned long *e_user_data; 
	long initial_value;
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
	struct MsgPort *notifyme;
   struct Window *dw;      /* window this gadget is in */
	struct list *l;			/* actual list */
	struct button *b;			/* button that triggers this droplist */
 };

struct NewLoopList {
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
	struct MsgPort *notifyme;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
	char **e_text;
	unsigned long *e_user_data; 
	long initial_value;
   };

struct looplist {         /* all gadget structs start this way */
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
	struct MsgPort *notifyme;
   struct Window *llw;      /* window this gadget is in */
	struct list *l;			/* actual list */
	struct button *b;			/* button that triggers this droplist */
 };

struct NewBox {
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
	struct MsgPort *notifyme;
	struct GadgetHeader *gad_ptr;	/* filled in by create_gadget */
   char *text;             /* center this in box */
   };

struct box {
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
	struct MsgPort *notifyme;
   struct Window *bw;      /* window this button is in */
   char *text;             /* center this in button */
   };


/***********************************/
/** input.device handler defines ***/
/***********************************/

#define GE_LBUTTONDOWN 1
#define GE_LBUTTONUP 2
#define GE_DELTAMOVE 3
#define GE_RAWKEY 4

struct GadEvent {
   struct Window *window;     /** active window **/
   short x;                   /** xpos or delta x */
   short y;
   short type;                /** BUTTONDOWN or BUTTONUP **/
	short code;						/** For RAWKEY **/
	short qualifier;        
	unsigned long secs;		/** time stamp, mouse down  **/
	unsigned long micro;
   };

/** DEFINES for changing pointer new_pointer() ***/

#define PTR_RESTORE 0
#define PTR_NORMAL 1
#define PTR_BUSY 2
#define PTR_GRABIT 3
#define PTR_CROP 4
#define PTR_CROSS 5
#define PTR_PASTE 6
#define PTR_NOTHING 7

/*** This define should match the one in Studio16.h! ****/

#define REGION_DELIMITER_CHAR '>'

#endif

