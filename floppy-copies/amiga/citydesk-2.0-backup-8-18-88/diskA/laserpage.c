#include "stdio.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
     
#define XMARG 98
#define YMARG 288
#define OUTLINESPC 100
     
#define UNDEROFF   "\033&d@"
#define UNDERON    "\033&dD"
#define FF         "\014"
#define RESET      "\033E"
#define GSTART     "\033*r1A"
#define GXFER      "\033*b50W"
#define GEND       "\033*rB"
     
     
static UBYTE onebits[]={0,128,128+64,128+64+32,128+64+32+16,128+64+32+16+8,
                  128+64+32+16+8+4,128+64+32+16+8+4+2};


extern struct printerprefs printerprefs;
extern struct Window *canwin;
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
     
laserpage(page)
     
struct pageinfo *page;
     
{
extern struct Window *clipboard;
int i,k;
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
   do_block   = ps_nada;
   }
     
else {
   return(CANCEL);
   }
     
newpointer(clipboard->WScreen,BUSY);
     
i=((*initprinter)(page));    /** call printer init code **/
if (i!=AOK) {
   newpointer(clipboard->WScreen,RESTORE);
   return(i);
   }
     
for (pass_kount=0; pass_kount < prt_passes; pass_kount++) {

printf("pass: %d\n",pass_kount);

   numobjs=0;
   objkount=0;
   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ)
      numobjs++;
     
   for (node=page->objects.lh_Head;node->ln_Succ;node=node->ln_Succ) {
      if (checkcan()) {
         printer_cleanup();
         return(CANCEL);
         }
      obj=(struct object *)node;
      if (obj->type==BLOCK) {
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
      if ((obj->type!=BLOCK) && (!(obj->flags&INV) && !(obj->flags&TOP))) {
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
      if (obj->type!=BLOCK && (obj->flags&INV || obj->flags&TOP))  {
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
newpointer(clipboard->WScreen,RESTORE);
}
     
/*******************************************/
/*** given object, render it on the page ***/
/*******************************************/
     
render_object(obj)
     
struct object *obj;
     
{
int i;
struct rule *rule;

if (obj->flags & OUTLINEOBJ) {
   i=8;
   if (obj->flags & LINEWIDTH2)
      i = 16;
   else if (obj->flags & LINEWIDTH3)
      i = 24;

   (*do_box)(obj->x, obj->y, obj->h, obj->w, i ,0xFFFF);
   }
     
if (obj->flags & SHADOWRIGHT) {
/*   i=obj->width + BOXGAP - 3; */
   i= BOXGAP - 3;
   (*do_hline)(obj->x + SHADOWSIZE,
      obj->y + obj->h + i,
      obj->w + i,
      SHADOWSIZE,(UWORD)0xFFFF);
     
   (*do_vline)(obj->x + obj->w +i,
      obj->y + SHADOWSIZE,
      obj->h - (SHADOWSIZE/2) + i,
      SHADOWSIZE,(UWORD)0xFFFF);
   }
     
if (obj->flags & SHADOWLEFT) {
/*   i=obj->width + BOXGAP - 3; */
   i = BOXGAP - 3;
   (*do_hline)(obj->x - i - SHADOWSIZE,
      obj->y + obj->h + i,
      obj->w + (i<<1) - SHADOWSIZE,
      SHADOWSIZE,(UWORD)0xFFFF);
     
   (*do_vline)(obj->x - i - SHADOWSIZE,
      obj->y + SHADOWSIZE,
      obj->h,
      SHADOWSIZE,(UWORD)0xFFFF);
   }
     
     
/** draw it if it is a line **/
     
if (obj->type==RULE) {
  rule=(struct rule *)obj->data;
     
   if (rule->flags==HORIZ) {
      (*do_hline)(obj->x,obj->y,obj->w,rule->width,rule->pattern);
      }
     
   else {
      (*do_vline)(obj->x,obj->y,obj->h,rule->width,rule->pattern);
      }
   }
     
   (*move_x)(obj->x);
   (*move_y)(obj->y);
     
if (obj->type==GRAPHIC) {
   i=(*do_graphic)(obj);
   if (i!=AOK);
      return(i);
   }

if (obj->type==BLOCK) {
   (*do_block)(obj);
   }
     
if (obj->type==TEXT) {
   (*do_text)(obj);        /** call correct text render function **/
   }
     
return(AOK);
}
     
     
/***************/
/*** ctolp *****/
/***************/
/* Must maintain in parallel to ctolplong! */
     
ctolp(val,dpi)
     
COORD val;
UWORD dpi;
{
FIXED fval;
FIXED inches;
ULONG i;
     
fval=val << 16;
inches=fval/576;
if (dpi==NULL) /** special case, use 720 dots/inch **/
   i=inches*720;
else
   i=inches*dpi;
return( (int) ((i + (1<<15)) >> 16) );
}
     
     
/*******************/
/*** ctolplong *****/
/*******************/
/* Must maintain in parallel with ctolp! */
     
ctolplong(val,dpi)
     
LCOORD val;
UWORD dpi;
{
FIXED inches;
ULONG i;
     
inches=val/576;
if (dpi==NULL) /** special case, use 720 dots/inch **/
   i=inches*720;
else
   i=inches*dpi;
return (i);
}
     
/*************************/
/***** laser move x ******/
/*************************/
     
     
laser_movex(x,obj)
COORD x;
struct object *obj;
{
extern FILE *fp;
extern int (*move_x)();
     
(*move_x)(x + obj->x);
}
     
/*************************/
/***** laser move y ******/
/*************************/
     
laser_movey(y,obj)
COORD y;
struct object *obj;
     
{
extern FILE *fp;
extern int (*move_y)();
     
(*move_y)(y + obj->y);
}
     
/***************************/
/***** laser_charout  ******/
/***************************/
     
laser_charout(lfont, ch)
struct laserfont *lfont;
char ch;
{
extern int (*prt_char)();
(*prt_char)(ch);
}
     
/***************************/
/***** laser_setfont  ******/
/***************************/
     
laser_setfont(lfont)
     
struct laserfont *lfont;
     
{
extern int (*setfont)();
(*setfont)(lfont);
}
     
     
/***************************/
/***** laser_underon   *****/
/***************************/
     
laser_underon()
{
extern int (*under_on)();
     
(*under_on)();
}
     
/***************************/
/***** laser_underoff  *****/
/***************************/
     
laser_underoff()
{
extern int (*under_off)();
     
(*under_off)();
}
     
     
/***************************/
/***** laser_prtbitmap *****/
/***************************/
     
laser_prtbitmap(bm,pix_x,pix_y,off_x,off_y,obj)
     
BYTE *bm;
int pix_x,pix_y;
COORD off_x,off_y;
struct object *obj;
     
{
extern int (*do_bitmap)();
     
(*do_bitmap)(bm,pix_x,pix_y,off_x,off_y,obj);
return(TRUE);
}
     
     
     
/*********************************************/
/** draw a vertical line on the HP laserjet **/
/*********************************************/
     
int hp_vline(x,y,h,thick,pat)
     
COORD x,y,h,thick;
UWORD pat;
{
SHORT dx,dy,dh,dthick;
     
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dh = ctolp(h,300);
dthick = ctolp(thick,300);
     
hp_dotvline(dx,dy,dh,dthick,pat);
}

/*************************************************************************/
/** This draws a vertical line on the hp LaserJet. Dimesions are passed **/
/** in dots (300 dpi).                                                  **/
/*************************************************************************/
     
int hp_dotvline(dx,dy,dh,dthick,pat)
     
UWORD pat;
{
SHORT i=0;
SHORT offset = 0;
SHORT tick;
     
if (dx < 0) dx=0;
if (dy < 0) dy=0;
if (dh < 0) dh=0;
     
if ( (pat&0xFF) == 0xFF) {
   hp_rule(dx, dy , dh, dthick);
   return(AOK);
   }
     
do {
   if (numdots(pat,&i,&tick)) {
      if (offset + tick > dh) {
         hp_rule(dx, dy + offset, dh - offset, dthick);
         }
      else {
         hp_rule(dx, dy + offset, tick, dthick);
         }
      }
   offset = offset + tick;
   } while (offset < dh);
}
     
/****************************************************/
/** Returns number of dots of same type in pattern **/
/****************************************************/
     
numdots(pat,i,tick)
SHORT pat,*i,*tick;
{
static USHORT pow2[]={128,64,32,16,8,4,2,1};
SHORT k=0;
     
if (pow2[(*i)&7]&pat) {
   while (pow2[(*i)&7]&pat) {
      k++;
      (*i)++;
      }
   }
else {
   while (!(pow2[(*i)&7]&pat)) {
      k++;
      (*i)++;
      }
   }
*tick = k*10;     /** 10 is tick size. **/
return (!(pow2[(*i)&7]&pat));
}
     
     
/********************************************************/
/** Give x,y,h,w (in dots) draw a sold rectangle/ rule **/
/********************************************************/
     
hp_rule(x, y, h, w)
int x,y,h,w;
{
fprintf(fp,"\033*p%dX",x);
fprintf(fp,"\033*p%dY",y);
fprintf(fp,"\033*c%dA",w);
fprintf(fp,"\033*c%dB",h);
fprintf(fp,"\033*c0P");
}

/********************************************************/
/** draw a gray rectangle on an HP laserjet            **/
/********************************************************/
     
hp_doblock(obj)
struct object *obj;
{
fprintf(fp,"\033*c%dH", ctolp(obj->w,0));       /** set width decipoints **/
fprintf(fp,"\033*c%dV", ctolp(obj->h,0));       /** set height **/
fprintf(fp,"\033*c%dG", (int)obj->density);        /** set gray level **/
fprintf(fp,"\033*c2P");                         /** draw block **/
}
     
/***********************************************/
/** draw a horizontal line on the HP laserjet **/
/***********************************************/
     
int hp_hline(x,y,w,thick,pat)
     
COORD x,y,w,thick;
UWORD pat;
     
{
SHORT dx,dy,dw,dthick;
     
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dw = ctolp(w,300);
dthick = ctolp(thick,300);
     
hp_dothline(dx,dy,dw,dthick,pat);
}
     
/*************************************************************************/
/** This draws a horizon. line on the hp LaserJet. Dimesions are passed **/
/** in dots (300 dpi).                                                  **/
/*************************************************************************/
     
int hp_dothline(dx,dy,dw,dthick,pat)
     
UWORD pat;
{
SHORT i=0;
SHORT offset = 0;
SHORT tick;
     
if (dx < 0) dx=0;
if (dy < 0) dy=0;
if (dw < 0) dw=0;
     
if ( (pat&0xFF) == 0xFF) {
   hp_rule(dx, dy , dthick, dw);
   return(AOK);
   }
     
do {
   if (numdots(pat,&i,&tick)) {
      if (offset + tick > dw) {
         hp_rule(dx + offset, dy, dthick, dw - offset);
         }
      else {
         hp_rule(dx + offset, dy, dthick, tick);
         }
      }
   offset = offset + tick;
   } while (offset < dw);
}
     
     
     
/**********************************/
/*** LaserJet box draw function ***/
/**********************************/
     
int hp_box(x,y,h,w,thick,pat)
     
COORD x,y,h,w,thick;
UWORD pat;
     
{
SHORT dx,dy,dh,dw,dthick;
     
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dh = ctolp(h,300);
dw = ctolp(w,300);
dthick = ctolp(thick,300);
     
/** dimentions of the box are OUTSIDE dimentions **/
     
hp_dothline(dx, dy, dw, dthick, pat);
hp_dothline(dx, dy - dthick + dh, dw, dthick, pat);
hp_dotvline(dx, dy, dh, dthick, pat);
hp_dotvline(dx + dw - dthick, dy, dh, dthick, pat);
return(AOK);
}
     
/**********************************/
/*** LaserJet "init" subroutine ***/
/**********************************/
     
int hp_init()
{
extern FILE *fp;
     
plaser=TRUE;
/* fprintf(fp,"\033E"); */  /** master reset. Top margin now .5 inch */
fprintf(fp,"\0339");        /** clear margins **/
fprintf(fp,"\033&l%dX",printerprefs.copies);
printf("copies=%d\n",printerprefs.copies);
return(AOK);
}
     
/********************************/
/*** LaserJet move_x function ***/
/********************************/
     
int hp_movex(x)
     
COORD x;
{
extern FILE *fp;
     
fprintf(fp,"\033&a%dH",max(0,ctolp(x - XMARG,NULL)));
}
     
/********************************/
/*** LaserJet move_y function ***/
/********************************/
     
int hp_movey(y)
     
COORD y;
{
extern FILE *fp;
     
fprintf(fp,"\033&a%dV",max(0,ctolp(y - YMARG,NULL)));
}
     
     
/**************************************/
/*** LaserJet graphic dump function ***/
/**************************************/
     
int hp_graphic(obj)
     
struct object *obj;
     
{
extern FILE *fp;
struct graphic *g;
SHORT bytesperrow,numrows;
UBYTE *bitmap;
SHORT i,k;
     
/** set printer resolution **/
     
fprintf(fp,"\033*t%dR",obj->dpi);
     
g=(struct graphic *)obj->data;
bytesperrow=RASSIZE(ctolp(obj->w,obj->dpi),1);
numrows=ctolp(obj->h,obj->dpi);
bitmap=(BYTE *)AllocMem(bytesperrow*numrows,MEMF_CLEAR);
if (bitmap==NULL) {
   return(OUTOFMEM);
   }


printf("starting dither\n");
     
dither(g, bitmap, 0,
        ctolp(obj->w,obj->dpi),  /** dest w **/
        numrows,                 /** dest h **/
        bytesperrow,             /** dest width in bytes (mod) **/
        printerprefs.flags);     /** dither method **/

#if 0
fit(g->image+RASSIZE(g->w,g->usey),
    g->usex,             /* srcoff */
    g->useh,             /* srch */
    g->usew,             /* srcw */
    RASSIZE(g->w,1),     /* srcmod */
    bitmap,              /* dest */
    0,
    numrows,       /* desth */
    ctolp(obj->w,obj->dpi),       /* destw */
    bytesperrow);    /* destmod == len in bytes of dest area */
#endif

printf("starting print\n");

fprintf(fp,GSTART);   /* graphic dump start cmd */
     
for (i=0; i<numrows; i++) {
     
   fprintf(fp,"\033*b%dW",bytesperrow);
   for (k=0;k<bytesperrow;k++)
      fprintf(fp,"%c",*(bitmap+i*bytesperrow+k));
     
   if (checkcan())
      break;
   }
     
fprintf(fp,GEND);
FreeMem(bitmap,bytesperrow*ctolp(obj->h,obj->dpi));
return(AOK);
}
     
     
/*******************************/
/** LaserJet "endpage" code **/
/*******************************/
     
int hp_endpage()
{
extern FILE *fp;
     
fprintf(fp,FF);
plaser=FALSE;
}
     
/*************************************************/
/***** LaserJet print character at cursor pos ****/
/*************************************************/
     
int hp_prtchar(ch)
UBYTE ch;
{
extern FILE *fp;
extern UBYTE *translate;
     
fprintf(fp,"%c",translate[ch]);
     
}
     
/************************/
/*** LaserJet setfont ***/
/************************/
     
int hp_setfont(lfont)
struct laserfont *lfont;
{
extern FILE *fp;
fprintf(fp,"%s",lfont->fonton);
}
     
     
/************************************/
/***** LaserJet turn on underline ***/
/************************************/
     
int hp_underon()
{
extern FILE *fp;
fprintf(fp, "%s", UNDERON);
}
     
/*************************************/
/***** LaserJet turn off underline ***/
/*************************************/
     
int hp_underoff()
{
extern FILE *fp;
fprintf(fp, "%s", UNDEROFF);
}
     
     
/****************************/
/***** LaserJet prtbitmap ***/
/****************************/
     
int hp_prtbitmap(bm,pix_x,pix_y,off_x,off_y,obj)
     
UBYTE *bm;
int pix_x,pix_y;
COORD off_x,off_y;
struct object *obj;
     
{
extern FILE *fp;
int bytesperrow;
int i,k;
     
bytesperrow=RASSIZE(pix_x,1);
     
     
/** move laserPrinter cursor to this boxes x/y **/
     
fprintf(fp,"\033&a%dH",ctolp(obj->x+off_x - XMARG,NULL));
fprintf(fp,"\033&a%dV",ctolp(obj->y+off_y - YMARG,NULL));
     
fprintf(fp,"\033*t%dR",150); /* set resolution */
fprintf(fp,GSTART);   /* graphic dump start cmd */
     
for (i=0; i<pix_y; i++) {
     
   fprintf(fp,"\033*b%dW",bytesperrow);
   for (k=0; k<bytesperrow; k++)
      fprintf(fp,"%c",*(bm+i*bytesperrow+k));
   }
     
fprintf(fp,GEND);
return(AOK);
}
     
/******************************************/
/*** HP LaserJet text rendering function **/
/******************************************/
     
hp_dotext(obj)
     
struct object *obj;
     
{
draw_text(obj,0,0,0,0,0, ctolplong(1<<16, obj->dpi),
                         ctolplong(1<<16, obj->dpi));
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
   return(TRUE);
   }
return(FALSE);
}

/*********************************************************************/
/** Called before calling the laserpage.  This code opens the files **/
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
   openpost();
   }

return(AOK);
}

print_doc_end()

{
prt_canceled = FALSE;
if (fp)
   fclose(fp);
fp = 0;
}

