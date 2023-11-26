#ifndef GADLIB_H
#define GADLIB_H TRUE

/** handle_list() return codes **/

#define NO_CODE       0    /** 0 returned if nothing interesting happened **/
#define DOUBLE_SELECT 1    /** user clicked on selected item in list **/
#define NEW_SELECT    2    /** user clicked on new list entry **/

/** knob flags **/

#define KNOB_3D    (1<<0)
#define BORDER_3D  (1<<1)
#define FREE_HORIZ (1<<2)  /* knob can move left to right */
#define FREE_VERT  (1<<3)  /* knob can move up and down */
#define AUTO_KNOB  (1<<4)  /* knob changes size dynamicaly */

struct NewKnob {
   void (*func)();            /** call after every mouse move **/
   struct Window *window;
   short box_x1;
   short box_y1;
   short box_x2;
   short box_y2;
   short knob_width;          /* used if not autoknob */
   short knob_height;
   short show_size;
   short list_size;
   short top;
   short apen;
   short bpen;
   unsigned long flags;
   };

struct knob {
   short knob_x;           /* size & pos (inclusive) of knob */
   short knob_y;
   short knob_width;
   short knob_height;
   short box_x1,box_y1;    /* size (inclusive) of container */
   short box_x2,box_y2;
   struct Window *kw;      /* window this knob is in */
   short list_size;
   short show_size;
   short top_element;
   unsigned short flags;
   unsigned char APen;
   unsigned char BPen;
   unsigned char CPen;
   struct list *list;
   void (*func)();         /** call this after top_element changes */
   APTR userdata;
   };


/**IDEAS!!! add function that creates everyhing from  a text
string.   Eg. button=AddTextButton(XPOS, YPOS, "TEST",&func()); also
add button create_button(NewButton);
what about general purpose gadget?

struct knob {
   struct mygadget;   * alternte:  struct location loc; *
   .....rest }


struct button {
   struct mygadget;
   ....rest
   }
!!!!!!*****/



struct NewButton {
   short box_x1,box_y1;    /* size (inclusive) of container */
   short box_x2,box_y2;
   struct Window *window;      /* window this button is in */
   unsigned short flags;
   char *text;             /* center this in button */
   void (*func)();         /* call this after button release */
   APTR userdata;
   };


struct button {
   short box_x1,box_y1;    /* size (inclusive) of container */
   short box_x2,box_y2;
   struct Window *bw;      /* window this button is in */
   unsigned short flags;
   char *text;             /* center this in button */
   void (*func)();         /* call this after button release */
   APTR userdata;
   };


#define MAX_LIST_SIZE 100

struct list {
   short current;
   short box_x;
   short box_y;
   struct Window *lw;
   short list_size;
   short show_size;
   short top_element;
   short max_show_chars;
   struct knob *knob;
   unsigned char APen;
   unsigned char BPen;
   char *list_text[MAX_LIST_SIZE];
   };

#endif

