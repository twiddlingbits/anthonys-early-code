#include "exec/types.h"
#include "intuition/intuition.h"

/**************************************/
/** setup -- opens librarys, windows **/
/**************************************/

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;

struct Window *setup()

{
struct Window *w;
static struct NewWindow NewWindowStructure = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   0,1,   /* detail and block pens */
   RAWKEY,   /* IDCMP flags */
   WINDOWSIZING+WINDOWDRAG+WINDOWDEPTH+ACTIVATE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Space Invaders © 1987 Anthony Wood",   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   WBENCHSCREEN   /* destination screen type */
};


IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   printf("Can't open Intuition!\n");
   return(0);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!\n");
   CloseLibrary(IntuitionBase);
   return(0);
   }

w = OpenWindow(&NewWindowStructure);
if (w==0) {
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   return(0);
   }

hit_setup();
return(w);
}


/**********************************/
/** closes libraries and windows **/
/**********************************/

void closedown(w)

struct Window *w;

{

CloseWindow(w);
CloseLibrary(IntuitionBase);
CloseLibrary(GfxBase);
}

