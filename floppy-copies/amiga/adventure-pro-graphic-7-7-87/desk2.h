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

#define RULE 2

/** object flags ***/

#define OUTLINEOBJ  1 
#define SHADOWRIGHT 2
#define SHADOWLEFT  4
#define TOP         8
#define INV         16

