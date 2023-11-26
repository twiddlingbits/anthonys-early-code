#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphics/regions.h"
#include "desk.h"

#define IDCMP ACTIVEWINDOW|GADGETUP|MENUPICK|CLOSEWINDOW|MOUSEBUTTONS|MOUSEMOVE|RAWKEY

/******************/
/** createview  ***/
/******************/

createview(screen,page)

struct Screen *screen;      /* City Desk custom screen */
struct pageinfo *page;      /* pageinfo of the page to view */

{
extern SHORT screensize;
extern SHORT std_width,std_height;
char title[50];
extern struct Window *clipboard;

static struct NewWindow nwcb = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   WHITE,GRAY1,   /* detail and block pens */
   IDCMP,    /* idcmp flags */
   BACKDROP|BORDERLESS|ACTIVATE|NOCAREREFRESH,
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   640,200,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

static struct PropInfo prl = {
   AUTOKNOB|FREEHORIZ,
   0,0,
   0xFFFF,0xFFFF,
   0,0,0,0,0,0
};

static struct Gadget rightleft = {
   NULL,
   1,-8,-16,9,
   GADGHNONE|GRELBOTTOM|GRELWIDTH,
   BOTTOMBORDER|RELVERIFY,
   PROPGADGET,
   NULL,
   NULL,
   NULL,
   NULL,
   &prl,
   GADRL,0
 };


static struct PropInfo pud = {
   AUTOKNOB|FREEVERT,
   0,0,
   0xFFFF,0xFFFF,
   0,0,0,0,0,0
};

static struct Gadget updown = {
   &rightleft,
   -14,9,16,-18,
   GADGHNONE|GRELHEIGHT|GRELRIGHT,
   RIGHTBORDER|RELVERIFY,
/*   PROPGADGET|GZZGADGET, */
   PROPGADGET,
   NULL,
   NULL,
   NULL,
   NULL,
   &pud,
   GADUD,0
 };

static struct NewWindow nw = {
   122,12,   /* window XY origin relative to TopLeft of screen */
   0,0,   /* window width and height */
   WHITE,GRAY1,   /* detail and block pens */
   NULL,    /* idcmp flags */
/*   NOCAREREFRESH|WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|ACTIVATE|GIMMEZEROZERO,*/
   NOCAREREFRESH|WINDOWSIZING|WINDOWDRAG|WINDOWDEPTH|WINDOWCLOSE|ACTIVATE,
   NULL,    /* next window */
   NULL,   /* custom CHECKMARK imagery */
   NULL,   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   20,20,   /* minimum width and height */
   350,190,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

nw.Screen=screen;
nwcb.Screen=screen;
nwcb.Height=screensize;
nwcb.MinHeight=screensize;
nwcb.MaxHeight=screensize;

nwcb.Width=std_width;
nwcb.MinWidth=std_width;
nwcb.MaxWidth=std_width;

/* printf("in createview: std_width = %d std_height = %d screensize = %d",
   std_width, std_height, screensize); */
/** open the window ***/

page->view=(struct pageview *)AllocMem(sizeof(struct pageview),MEMF_CLEAR);
if (page->view==0)
   return(OUTOFMEM);

if (page->pn==CB) {
   page->view->window=OpenWindow(&nwcb);
   }

else {
   nw.Width=ctosx(page->pw)+XBORDER_AREA; /* 16 is border width. */
   nw.Height=ctosy(page->ph)+YBORDER_AREA; /* 18 is borders. 16 & 22 should be var? */
   nw.MaxWidth=nw.Width;
   nw.MaxHeight=nw.Height;
   page->view->rl  = rightleft;
   page->view->prl = prl;
   page->view->ud  = updown;
   page->view->pud = pud;
   page->view->ud.NextGadget   = &page->view->rl;
   page->view->rl.SpecialInfo  = &page->view->prl;
   page->view->ud.SpecialInfo  = &page->view->pud;
   page->view->rl.GadgetRender = &page->view->i1;  /* image structure */
   page->view->ud.GadgetRender = &page->view->i2;
   nw.FirstGadget = &page->view->ud;

   if (page->pn == DEFAULTODD)
      sprintf(title,"Default Odd Page");
   else if (page->pn == DEFAULTEVEN)
      sprintf(title,"Default Even Page");
   else 
#if GERMAN
   sprintf(title,"Seite %d",page->pn);
#else
   sprintf(title,"Page number %d",page->pn);
#endif
   nw.Title=AllocMem(strlen(title)+1,0);
   if (nw.Title == 0) {
      FreeMem(page->view,sizeof(struct pageview));
      return(OUTOFMEM);
      }
   strcpy(nw.Title,title);
   page->view->window=OpenWindow(&nw);
   add_clip(page->view);
   }

if (page->view->window==0) {
   FreeMem(page->view,sizeof(struct pageview));
   page->view=0;
   return(OUTOFMEM);
   }

if (page->pn==CB)   {
   clipboard=page->view->window;      /* clipboard is a global var. */
   SetDrMd(clipboard->RPort,JAM1);    /* draw clipboard brown */
   SetAPen(clipboard->RPort,GRAY2);
   RectFill(clipboard->RPort,0,0,
         clipboard->Width-1,
         clipboard->Height-1);
   }

else   {
   page->view->window->UserPort=clipboard->UserPort;
   ModifyIDCMP(page->view->window,IDCMP|NEWSIZE);
   }

/** set misc. parameters **/

page->view->mag=1;
page->view->h=page->ph;   /** view entire page ***/
page->view->w=page->pw;

return (AOK);
}
