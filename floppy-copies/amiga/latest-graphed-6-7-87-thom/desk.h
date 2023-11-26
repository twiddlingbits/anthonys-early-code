#include "exec/exec.h"

#define DATASIZE(obj) (RASSIZE(ctosx(obj->mag*obj->w),ctosy(obj->mag*obj->h))*obj->numplanes)
#define SNAPGRID(x) (snaptogrid ? (((x)+(gridsize>>1))/gridsize)*gridsize : (x) )

/** Make printer drivers broken? **/

#define DEMO 0

/** Standard Subroutine Return codes **/

#define CANCEL 0
#define OUTOFMEM 1
#define AOK 2
#define FILENOTFOUND 3
#define DISKERROR 4
#define CANTOPENFILE 5

/**** stack size for new tasks ***/

#define STACKSIZE 2048

/*** gadget ID's to identify gadget structures  ***/

#define GADUD 0
#define GADRL 1

/*** define clipboard location  **/

#define CB 0         /** use page zero as a clipboard **/

/*** object shadow and outline paramaters **/

#define BOXGAP 24
#define SHADOWSIZE 48

/**** modes-- you're in a new mode when the pointer changes **/

#define NORMAL 0       /* allows movement of objects on page only */
#define SIZE 1
#define CROP 2
#define FLOW 3         /* waiting for user to pick place to put text */
#define BUSY 4
#define LINE 5
#define BOX 6
#define MAGNIFY 7      /* this mode doesn't have a pointer */
#define ABCEDIT 8      /* This mode doesn't have a pointer */
#define GRABIT 9       /* for moving objects from one window to another */
#define NEWPAGE 10     /* no pointer. Wants to open new page */
#define DUPOBJECT 11   /* make a copy of the next object picked */
#define INVERT 12      /* invert a graphics image-- make a "Negative" */
#define CHEAPEDIT 13   /* cheapedit a text block */
#define EDITFORMAT 14  /* wants to edit text attributes */
#define PLACEGRAPHIC 15   /* PIC icon, like text (FLOW) but for graphics */
#define CROSS 16
#define RESTORE 99     /* command to newpointer says "use last mode" */
#define ASPLOW 0
#define ASPMED 0
#define ASPHI 0
#define ASPLOWLACE 0

/** Page info Flags  **/

#define PORT 1
#define LAND 2
#define BOOK 4
#define DRAWRULES 8        /** draw rules between columns **/
#define OUTLINEPAGE 16     /** draw a box around page **/


/** graphic resize directions **/

#define ANYWAY 0
#define XONLY 1
#define YONLY 2

/** flowtext type parameteres **/

#define UNITSCOORD 0
#define UNITSMOUSE 1

/** color definitions **/

#define WHITE 0
#define BLACK 1
#define BROWN 2
#define RED 3

#define COLOROBJECT BLACK
#define COLORBACK   WHITE
#define COLORDESK   BLACK
#define COLORDETAIL BROWN


/**** viewmsg commands ****/

#define MOVED 1          /* the object pointed to was moved, possibly */
                         /* to another page */
#define SIZED 2          /* the object pointed to was re-sized */
#define REFRESH 3        /* redraw entire page */
#define DRAWIT 4         /* put this object into this page */
#define MOVEDONPAGE   5  /* object moved but should stay in this window */
#define CLEAR 6          /* remove this object from viewport */

/** text_object flags **/

#define FIRSTOBJECT 1    /* first text_object marker. Also start of text */
#define NEEDSFLOWING 2   /* Set on last text_object when more text left */


/** misc. parameters **/

#define MAXPAGES 3    /* num. of pages to define lists for */
#define XBORDER 22
#define YBORDER 20

/*** structure definitions ***/

typedef short COORD;
typedef short SCOORD;
typedef ULONG FIXED;
typedef LONG  SFIXED;

   
/** "object.type" definitions  **/

#define TEXT 0
#define GRAPHIC 1
#define RULE 2

/** object flags ***/

#define OUTLINEOBJ  1 
#define SHADOWRIGHT 2
#define SHADOWLEFT  4
#define TOP         8
#define INV         16

struct object {
   struct Node node;
   UBYTE *screendata;   /* object scaled to fit onto screen */
   UBYTE numplanes;     /* must be one in this version */
   UBYTE type;          /* GRAPHIC, TEXT, etc. */
   UBYTE flags;         /* outline, shadow, etc */
   COORD x, y;          /* Position of this object rel upper Left of page */
   COORD h, w;          /* Height and width of this object */
   SHORT angle;
   APTR  data;          /* original object data in full resolution */
   SHORT mag;           /* magnification of current *screendata info */
   UWORD pattern;       /* outline pattern **/
   COORD width;         /* outline width **/
   UWORD dpi;           /* if laserprinter, show at this resolution **/
   };


struct graphic {
   UBYTE *image;       /* original graphic data stored here */
   SHORT h,w;
   SHORT usex,usey;    /* defines area user wants to see */
   SHORT useh,usew;    /* ditto */
   FIXED aspect;       /* Width / Height */
   UBYTE numplanes;    /* currently must be one */
   };

#define HORIZ 0       /** This is a horiz. rule **/
#define VERT  1       /** This is a vertic. rule **/

struct rule {
   UWORD pattern;      /** pattern to use when drawing this line **/
   COORD width;        /** width of line. if zero, draw "thinline" **/
   UWORD flags;
   };

struct taskplus {
   struct Task task;
   struct MsgPort port;
   };

struct pageview {
   struct Window *window;        /* window to display this page into  */
   struct taskplus *tp;          /* task & port that updates this page */
   COORD x,y;                    /* UL corner of area of page being viewed */
   COORD h,w;                    /* dimensions of area being viewed */
   SHORT mag;                    /* page display magnification */
   struct Gadget *ud;            /* Rifht-Left prop gadget */
   struct Gadget *rl;            /* Up-Down prop gadget */   
   };


struct pageinfo {
   SHORT pn;              /* page number */
   COORD pw,ph;
   COORD rm,lm,tm,bm;      /* margins */
   SHORT nc;
   COORD cg;               /* num of cols, col gutter */
   USHORT flags;           /* orientation PORT, LAND, BOOK */
   struct List objects;    /* objects on this page */
   struct pageview *view;  /* zero if page not displayed, or *pageview */
   };

struct viewmsg {
   struct Message Msg;
   struct pageinfo *page;
   struct object  *obj;     /* object to update */
   SHORT command;           /* MOVE,SIZE,REFRESH etc.. */
   SHORT newx,newy;         /* obj position after move (screen coords) */
   COORD oldx,oldy;         /* quick fix, change later !!!! */
   COORD oldh,oldw;         /* obj size before resize */
   };

struct click {              /* getclick() returns this info */
   struct Window *Window;   /* window that was clicked */
   SHORT x,y;               /* cursor x,y position  */
   };
