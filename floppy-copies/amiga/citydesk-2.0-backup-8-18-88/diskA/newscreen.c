#include "exec/types.h"
#include "intuition/intuition.h"
#include "desk.h"


/*************************************/
/** Attempt to open a new screen... **/
/*************************************/

newscreen(screen,newsize,pg,toolbox)

struct Screen **screen;
struct Window **toolbox;
SHORT newsize;
int *pg;

{
struct Screen *newscreen;
struct Node *node;
struct object *obj;
extern struct Window *clipboard;
extern struct pageinfo page[MAXPAGES];
extern SHORT std_width;
struct viewmsg msg;
SHORT oldsize;
extern SHORT screensize;
int i;

if (screensize==newsize)
   return(CANCEL);

if (AvailMem(MEMF_CHIP) < 80*newsize+16000)
   return(OUTOFMEM);

setmenus(*screen,FALSE);        /* turn off menus */

newscreen=(struct Screen *)openscreen(std_width,newsize,2);

if (newscreen==NULL)
   return(OUTOFMEM);

closetoolbox(*toolbox);

for (i=MAXPAGES-1; i >= 0; i--) {   /* delete all objects and windows */
   if (page[i].flags&PAGEINUSE) {
      deleteview(&page[i]);
      for (node=page[*pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
         obj=(struct object *)node;
         freedata(obj);    /* free memory used by *screendata */
         obj->mag=1;
         }
      }
   }

/** set up clipboard **/
oldsize=screensize;
screensize=newsize;
if (createview(newscreen,&page[CB])!=AOK) {
   screensize=oldsize;
   CloseScreen(newscreen);
   if (createview(*screen,&page[CB])==AOK) {
      *toolbox=opentoolbox(*screen,page[CB].view->window->UserPort);
      }
   return(OUTOFMEM);
   }

CloseScreen(*screen);
*screen=newscreen;

*toolbox=opentoolbox(*screen,page[CB].view->window->UserPort);

/* msg.page=&page[CB]; This doesn't do anything
msg.command=REFRESH;
viewmonitor(&msg); */


/**** set up current page if opened ****/

if (*pg > 0) {
   if (createview(*screen,&page[*pg])!=AOK) {
      *pg=0;
      return(OUTOFMEM);
      }

   newpointer(*screen,BUSY);

   for (node=page[*pg].objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      obj=(struct object *)node;
      makedisplay(obj);
      }
   msg.page=&page[*pg];
   msg.command=REFRESH;
   viewmonitor(&msg);
   }

return(AOK);
}

