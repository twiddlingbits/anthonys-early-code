#include "exec/exec.h"

#ifndef INTUITION_INTUITION_H
#include "intuition/intuition.h"
#endif

#ifndef GRAPHICS_GFX_H
#include "graphics/gfx.h"
#endif

#define GERMAN 0     /** show text in german or english ? **/

#define DATASIZE(obj) (RASSIZE(ctosx(obj->mag*obj->w),ctosy(obj->mag*obj->h))*obj->numplanes)
#define XSNAPGRID(x) (snaptogrid ? (((x)+(x_gridsize>>1))/x_gridsize)*x_gridsize : (x) )
#define YSNAPGRID(x) (snaptogrid ? (((x)+(y_gridsize>>1))/y_gridsize)*y_gridsize : (x) )

/** Make printer drivers broken? **/

#define DEMO 0

/** Standard Subroutine Return codes **/

#define CANCEL 0
#define OUTOFMEM 1
#define AOK 2
#define FILENOTFOUND 3
#define DISKERROR 4
#define CANTOPENFILE 5
#define NOTIFF 6
#define BADIFF 7
#define CANTOPENPRINTER 8
#define NOTCDPROJECT 9
#define NOTAFM 10
#define BADAFM 11
#define BADFILEFORMAT 12
#define BADCD 13
#define NOTPREFS 14
#define TOOSMALL 15
#define NOTENUF 16
#define CANTPRINTTOFILE 17
#define CANTLINK 18

/*** aspect rations ***/

#define ASPLOW      ((10<<16)/11)
#define ASPMED      ((5<<16)/11)
#define ASPLOWLACE  ((5<<16)/11)
#define ASPHI       ((10<<16)/11)

/**** stack size for new tasks ***/

#define STACKSIZE 2048

/*** gadget ID's to identify gadget structures  ***/

#define GADUD 0
#define GADRL 1


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
#define PLACEGRAPHIC 15 /* PIC icon, like text (FLOW) but for graphics */
#define CROSS  16      /* graphed crosshair **/
#define SAVEOBJ 17     /* save picked object to disk **/
#define REFLOWTEXT 18  /* reflow and format entire text block **/
#define TRASHCAN 19    /* trashcan tool **/
#define BREAKCHAIN 20  /* Next Text Object clicked, make new chain */
#define PICKTEXTA 21   /* pick block of text to conect to */
#define PICKTEXTB 22   /* pick block of text to add to "A" */
#define LINK      23    /* link objects into a group */
#define UNLINKALL 24    /* remove all links in this objects chain */
#define UNLINK  25      /* remove this one object from chain */
#define DOBLOCK   26      /* Draw a filled object */
#define RESTORE 99     /* command to newpointer says "use last mode" */


/** Page info Flags  **/

#define PORT 1
#define LAND 2
#define BOOK 4
#define DRAWRULES 8        /** draw rules between columns **/
#define OUTLINEPAGE 16     /** draw a box around page **/
#define PAGEINUSE 32       /** This page has never been opened before **/


/** graphic resize directions **/

#define ANYWAY 0
#define XONLY 1
#define YONLY 2

/** flowtext type parameteres **/

#define UNITSCOORD 0
#define UNITSMOUSE 1

/** color definitions **/

/*
#define WHITE 0
#define BLACK 1
#define BROWN 2
#define RED 3

#define COLOROBJECT BLACK
#define COLORBACK   WHITE
#define COLORDESK   BLACK
#define COLORDETAIL BROWN
*/

#define BLACK 3
#define GRAY1 2
#define GRAY2 1
#define WHITE 0

#define COLOROBJECT BLACK
#define COLORBACK   WHITE
#define COLORDESK   GRAY2
#define COLORDETAIL GRAY1

/**** viewmsg commands ****/

#define MOVED 1          /* the object pointed to was moved, possibly */
                         /* to another page */
#define SIZED 2          /* the object pointed to was re-sized */
#define REFRESH 3        /* redraw entire page */
#define DRAWIT 4         /* put this object into this page */
#define MOVEDONPAGE   5  /* object moved but should stay in this window */
#define CLEAR 6          /* remove this object from viewport */


/** misc. parameters **/

#define MAXPAGES 100           /* num. of pages to reserve memory for */
#define LASTPAGE MAXPAGES - 4  /* Last page number used by user */
#define XBORDER 2
#define YBORDER 10
#define XBORDER_AREA (XBORDER + 15)
#define YBORDER_AREA (YBORDER + 9)

/*** pages used for specific purposes ***/

#define CB 0                      /** use page zero as a clipboard **/
#define DEFAULTEVEN MAXPAGES-1    /** use these two pages for default **/
#define DEFAULTODD  MAXPAGES-2    /** pages  **/
#define CUTPAGE     MAXPAGES-3    /** This is where the page is moved to **/
                                  /** when it is cut or copied **/


/*** structure definitions ***/

typedef short COORD;
typedef short SCOORD;
typedef UWORD UCOORD;
typedef LONG  LCOORD;         /** need them bits! **/
typedef ULONG FIXED;
typedef LONG  SFIXED;

   
/** "object.type" definitions  **/

#define TEXT 0
#define GRAPHIC 1
#define RULE 2
#define BLOCK 3

/** object flags ***/

#define OUTLINEOBJ  1 
#define SHADOWRIGHT 2
#define SHADOWLEFT  4
#define TOP         8
#define INV         16
#define MORETEXT    32     /** this text object has more text to flow **/
#define STAMPED     64     /** Can't grab or layer (always on bottom) **/
#define TRANSPARENT 128    /** Flowing text ignores this object **/

struct object {
   struct Node node;
   UBYTE *screendata;   /* object scaled to fit onto screen */
   UBYTE numplanes;     /* one or two in version 2.0 **/
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
   struct object *first; /* first object in this chain */
   SHORT pn;            /* page that contains this object */
   struct object *top_link; /** First in the link **/
   struct object *next;     /** Next object in this link **/
   SHORT pad;           /* reserved for latter use */
   };


struct graphic {
   PLANEPTR planes[6];  /* points to data bit planes (AllocRast'd) */
   SHORT h,w;           /* hight & width of original B&W data */
   SHORT usex,usey;     /* defines area user wants to see */
   SHORT useh,usew;     /* for croping purposes */
   FIXED aspect;        /* Width / Height */
   SHORT numplanes;     /* Number of planes in original B&W data */
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
   struct Region *rgn;           /* Clipping Region for this window */
   COORD x,y;                    /* UL corner of area of page being viewed */
   COORD h,w;                    /* dimensions of area being viewed */
   SHORT mag;                    /* page display magnification */
   struct Gadget ud;             /* Up - down slider gadget */
   struct PropInfo pud;          /* Prop info for updown gadget */
   struct Gadget rl;             /* right - left gadget */
   struct PropInfo prl;          /* Prop info for right left gadget */
   struct Image i1;              /* Image for autoknob sliders */
   struct Image i2;
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

