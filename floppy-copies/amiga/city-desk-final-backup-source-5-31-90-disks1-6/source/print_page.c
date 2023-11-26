#include "stdio.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
     
#define XMARG 98
#define YMARG 288

extern COORD xoutlinespc;     
extern COORD youtlinespc;
extern struct printerprefs printerprefs;
extern struct Window *canwin;
extern SHORT xshadowsize;
extern SHORT yshadowsize;
extern UWORD yinch, xinch;
SHORT prt_canceled;
int plaser=FALSE;
FILE *fp;         /** if fp != 0, print to file. else, print to device
                      dependant location **/
short prt_passes;  /*** Number of passes the driver needs to make **/
     
int (*initprinter)();     /** this called before start of each page **/
int (*do_vline)();        /** draw a vertical line given width, pattrn **/
int (*do_hline)();        /** draw horiz. line given width and pattern **/
int (*do_graphic)();      /** send graphic object to printer **/
int (*do_text)();         /** send text object to printer **/
int (*do_bitmap)();       /** send a bitmap to printer **/
int (*do_box)();          /** draw a box **/
int (*move_x)();          /** move to x COORD **/
int (*move_y)();          /** move to y COORD **/
int (*prt_char)();        /** send a character at current x, y pos'n **/
int (*under_on)();        /** start underlining **/
int (*under_off)();       /** stop underlining **/
int (*setfont)();         /** switch to a given lfont **/
int (*print_part)();      /** called between prt_passes **/
int (*endpage)();         /** called when finished drawing on page **/
int (*do_block)();        /** renders a grayscale block **/
     
print_page(page)
     
struct pageinfo *page;
     
{
extern struct Window *clipboard;
int i;
short objkount,numobjs,pass_kount;
struct Node *node;
struct object *obj;
     
int hp_init();
int hp_vline();
int hp_hline();
int hp_box();
int hp_prtbitmap();
int hp_graphic();
int hp_movex();
int hp_movey();
int hp_prtchar();
int hp_underon();
int hp_underoff();
int hp_endpage();
int hp_setfont();
int hp_dotext();
int hp_doblock();
     
int ps_setfont();
int ps_movex();
int ps_movey();
int ps_prtchar();
int ps_init();
int ps_endpage();
int ps_hline();
int ps_vline();
int ps_box();
int ps_graphic();
int ps_dotext();
int ps_nada();
int ps_underon();
int ps_underoff();
int ps_doblock();
     
int pref_init();
int pref_doobj();
int pref_endpage();
int pref_hline();
int pref_vline();
int pref_box();
int pref_dumpchunk();
int pref_doblock();

int vp_init();
int vp_dumpchunk();
int vp_endpage();
     
numobjs=0;
for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ)
   numobjs++;
     
if (numobjs==0)         /** nothing on this page **/
   return(AOK);
     
if (printerprefs.printertype==HPLASER) {
     
   initprinter= hp_init;
   do_vline   = hp_vline;
   do_hline   = hp_hline;
   do_box     = hp_box;
   do_bitmap  = hp_prtbitmap;
   do_graphic = hp_graphic;
   move_x     = hp_movex;
   move_y     = hp_movey;
   prt_char   = hp_prtchar;
   under_on   = hp_underon;
   under_off  = hp_underoff;
   endpage    = hp_endpage;
   setfont    = hp_setfont;
   do_text    = hp_dotext;
   print_part = ps_nada;
   do_block   = hp_doblock;
   }
     
else if (printerprefs.printertype==POSTSCRIPT) {
     
   initprinter= ps_init;
   do_vline   = ps_vline;
   do_hline   = ps_hline;
   do_box     = ps_box;
   do_bitmap  = ps_nada;
   do_graphic = ps_graphic;
   move_x     = ps_movex;
   move_y     = ps_movey;
   prt_char   = ps_prtchar;
   under_on   = ps_underon;
   under_off  = ps_underoff;
   endpage    = ps_endpage;
   setfont    = ps_setfont;
   do_text    = ps_dotext;
   print_part = ps_nada;
   do_block   = ps_doblock;
   }
     
else if (printerprefs.printertype==PREFERENCES) {
     
   initprinter= pref_init;
   do_vline   = pref_vline;
   do_hline   = pref_hline;
   do_box     = pref_box;
   do_bitmap  = ps_nada;
   do_graphic = pref_doobj;
   move_x     = ps_nada;
   move_y     = ps_nada;
   prt_char   = ps_nada;
   under_on   = ps_nada;
   under_off  = ps_nada;
   endpage    = pref_endpage;
   setfont    = ps_nada;
   do_text    = pref_doobj;
   print_part = pref_dumpchunk;
   do_block   = pref_doblock;
   }

else if (printerprefs.printertype==VPAGE) {
     
   initprinter= vp_init;
   do_vline   = pref_vline;
   do_hline   = pref_hline;
   do_box     = pref_box;
   do_bitmap  = ps_nada;
   do_graphic = pref_doobj;
   move_x     = ps_nada;
   move_y     = ps_nada;
   prt_char   = ps_nada;
   under_on   = ps_nada;
   under_off  = ps_nada;
   endpage    = vp_endpage;
   setfont    = ps_nada;
   do_text    = pref_doobj;
   print_part = vp_dumpchunk;
   do_block   = pref_doblock;
   }
     
else {
   return(CANCEL);
   }
     
i=((*initprinter)(page));    /** call printer init code **/
if (i!=AOK) {
   return(i);
   }

sort_obj_list(page);    /** put graphics at head of list **/

for (pass_kount=0; pass_kount < prt_passes; pass_kount++) {

/* printf("pass: %d\n",pass_kount); */

   render_page_options(page);    /** outline and vert. rules **/

   numobjs=0;
   objkount=0;
   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ)
      numobjs++;
     
   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      if (checkcan()) {
         printer_cleanup();
         return(CANCEL);
         }
      obj=(struct object *)node;    /** render all screens first **/
      if (obj->density > 0 && (obj->flags&TOP)==0) {
         (*do_block)(obj);
         }
      }

   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      if (checkcan()) {
         printer_cleanup();
         return(CANCEL);
         }
      obj=(struct object *)node;
      if (!(obj->flags&INV) && !(obj->flags&TOP)) {
         showobjstat(canwin,++objkount,numobjs,page->pn);
         i=render_object(obj);
         if (i!=AOK) {
            printer_cleanup();
            return(i);
            }
         }
      }
     
   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      if (checkcan()) {
         printer_cleanup();
         return(CANCEL);
         }
      obj=(struct object *)node;
      if ((obj->flags&INV) || (obj->flags&TOP))  {
         showobjstat(canwin,++objkount,numobjs,page->pn);
         i=render_object(obj);
         if (i!=AOK) {
            printer_cleanup();
            return(i);
            }
         }
      }
   (*print_part)();      /** send this part to the printer **/
   }

printer_cleanup();     
return(AOK);
}

/***********************************************************************/
/*** printer_cleanup(): Called when done printing (either normally or **/
/*** because the print was canceled)                                  **/
/***********************************************************************/

printer_cleanup()

{
(*endpage)();      /** let printer driver cleanup **/
clearobjstat(canwin);
plaser=FALSE;
}
     
/*******************************************/
/*** given object, render it on the page ***/
/*******************************************/
     
render_object(obj)
     
struct object *obj;
     
{
int i;
struct rule *rule;
COORD start;

if (obj->type != RULE && (obj->flags&TOP)) {
   (*do_block)(obj);
   }

/** draw it if it is a line **/
     
if (obj->type==RULE) {
  rule=(struct rule *)obj->data;

   /** rule width is given in 1/576", not coords. So, convert **/

   if (rule->flags==HORIZ) {
      (*do_hline)(obj->x,obj->y,obj->w,rule->width*yinch/576,!(rule->pattern==0xFFFF));
      }
     
   else {
      (*do_vline)(obj->x,obj->y,obj->h,rule->width*xinch/576,!(rule->pattern==0xFFFF));
      }
   }
     
(*move_x)(obj->x);
(*move_y)(obj->y);

if (obj->type==GRAPHIC && !(printerprefs.flags&PROOF)) {
   i=(*do_graphic)(obj);
   if (i!=AOK)
      return(i);
   }

if (obj->type==TEXT) {
   (*do_text)(obj);        /** call correct text render function **/
   }

if (obj->flags & OUTLINEOBJ ||
   (obj->type==GRAPHIC && printerprefs.flags&PROOF)) {
   i=3;
   if (obj->flags & LINEWIDTH2)
      i = 8;
   else if (obj->flags & LINEWIDTH3)
      i = 16;
   else if (obj->flags & LINEWIDTH4)
      i = 24;

   (*do_box)(obj->x, obj->y, obj->h, obj->w, i ,obj->flags&DASHEDLINE);
   }
     
if (obj->flags & SHADOWRIGHT) {
   start=min(xshadowsize*2,(obj->w*15)/100);
   (*do_hline)(obj->x + start, /* shadow  85 % of box **/
      obj->y + obj->h,
      obj->w - start + xshadowsize,
      yshadowsize, FALSE);

   start=min(yshadowsize*2,(obj->h*15)/100);
   (*do_vline)(obj->x + obj->w,
      obj->y + start,
      obj->h - start,
      xshadowsize, FALSE);
   }

if (obj->flags & SHADOWLEFT) {
   start = min(xshadowsize,obj->w*15/100);
   (*do_hline)(obj->x,
      obj->y + obj->h,
      obj->w - start,
      yshadowsize, FALSE);

   start = min(yshadowsize,obj->h*15/100);
   (*do_vline)(obj->x - xshadowsize,
      obj->y + start,
      obj->h - start + yshadowsize,
      xshadowsize, FALSE);
   }

return(AOK);
}
     
     
/************************************************************************/
/** checkcan -- checks to see if the user has presed the cancel button **/
/************************************************************************/
     
checkcan()
     
{
extern struct Window *canwin;
struct IntuiMessage *GetMsg(),*msg;
     
if (canwin==0)
   return(FALSE);          /** make sure we are printing **/
     
if (prt_canceled)          /** This flag used so user only has to hit **/
   return(TRUE);            /** the cancel button once **/
     
if ((msg=GetMsg(canwin->UserPort))!=0) {
   ReplyMsg(msg);
   prt_canceled=TRUE;
   while ((msg=GetMsg(canwin->UserPort))!=0) {
      ReplyMsg(msg);
      }
   return(TRUE);
   }
return(FALSE);
}

/*********************************************************************/
/** Called before calling the printpage.  This code opens the files **/
/** that are printed to. IE, prt: ser: or file name                 **/
/*********************************************************************/
     
print_doc_init()

{
char fname[80];
short i;
static char savefile[34]={0};
static char drawer[35]={0};

prt_passes = 1;
prt_canceled = FALSE;
fp=0;
if (printerprefs.flags&USE_FILE) {
   if (printerprefs.printertype==PREFERENCES) {
      return(CANTPRINTTOFILE);
      }

   i=get_fname(clipboard,clipboard->WScreen,"Print to file...",savefile,drawer);
   if (i==NULL)
      return(CANCEL);

   fname[0]=0;
   add(fname,drawer);  /* connect drawer and filename into a path */
   add(fname,savefile);
   fp=fopen(fname,"w");
   if (fp==NULL) {
      return(CANTOPENPRINTER);
      }
   return(AOK);
   }
     
if (printerprefs.printertype==HPLASER) {
   if (printerprefs.flags&USE_PAR)
      fp=fopen("par:","w");
   else
      fp=fopen("ser:","w");
     
   if (fp==NULL) {
      return(CANTOPENPRINTER);
      }
   return(AOK);
   }
     
if (printerprefs.printertype==POSTSCRIPT) {
   if (printerprefs.flags&USE_PAR) {
      fp=fopen("par:","w");
      if (fp==NULL)
         return(CANTOPENPRINTER);
      }
   else {
      openpost();
      }
   }

return(AOK);
}

print_doc_end()

{
prt_canceled = FALSE;
if (fp==0 && printerprefs.printertype==POSTSCRIPT) {
   closepost();
   Delay(1000);   /** wait about 20 seconds... Give page time to print **/
   }
if (fp) {
   fclose(fp);
   fp = 0;
   }
}


/** Draw verticle rules between columns & outline page if requested **/

render_page_options(page)

struct pageinfo *page;

{
COORD columnwidth;
short i,j;

columnwidth = colwid(page->pn);
if ((page->flags & DRAWRULES) && (page->nc > 1)) {
   j=page->lm+columnwidth-(page->cg>>1);
   for (i=1; i < page->nc; i++) {
      (*do_vline)(j, page->tm, page->ph - page->tm - page->bm, 3, FALSE);
      j=j+columnwidth;
      }
   }

/** Outline the page if requested **/

if (page->flags & OUTLINEPAGE) {
   (*do_box)(page->lm-xoutlinespc, page->tm-youtlinespc,
             page->ph-page->tm-page->bm+(youtlinespc<<1),
             page->pw-page->lm-page->rm+(xoutlinespc<<1), 3 ,FALSE);
   }
}


