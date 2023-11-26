#include "stdio.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "desk.h"
#include "text.h"
#include "laser.h"
     
/* #define XMARG 51
#define YMARG 150 */
#define XMARG 65     /** was 85 **/
#define YMARG 1
#define OUTLINESPC 100
#define BLACK_DASH_SIZE 40      /** nominal dash size in 1/300" **/
#define WHITE_DASH_SIZE 20      /** size of space between dashes **/

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
extern int plaser;
extern UWORD xinch, yinch;
     
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
inches=fval/xinch;
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
     
inches=val/xinch;
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
     
int hp_vline(x,y,h,thick,dashed)
     
COORD x,y,h,thick;
int dashed;
{
SHORT dx,dy,dh,dthick;
     
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dh = ctolp(h,300);
dthick = ctolp(thick,300);
     
hp_dotvline(dx,dy,dh,dthick,dashed);
}

/*************************************************************************/
/** This draws a vertical line on the hp LaserJet. Dimesions are passed **/
/** in dots (300 dpi).                                                  **/
/*************************************************************************/
     
int hp_dotvline(dx,dy,dh,dthick,dashed)

int dx, dy, dh, dthick, dashed;
{
SHORT num_dashes;
SHORT first_dash_length;
SHORT dash_size;
     
if (dx < 0) dx=0;
if (dy < 0) dy=0;
if (dh < 0) dh=0;
     
if (!dashed) {
   hp_rule(dx, dy , dh, dthick);
   return(AOK);
   }

/** BLACK_DASH_SIZE is the exact length of the black line,         **/
/** WHITE_DASH_SIZE is a nominal size that is adjusted to make the **/
/**                 dashes look evenly spaced **/

num_dashes = dh/(WHITE_DASH_SIZE+BLACK_DASH_SIZE);  /** pick reasonable **/
dash_size  = (dh - BLACK_DASH_SIZE)/num_dashes;     /** get w+b to use **/

first_dash_length = dh - num_dashes*dash_size;

hp_rule(dx, dy , first_dash_length, dthick);
dy += first_dash_length+dash_size-BLACK_DASH_SIZE;

while (num_dashes) {
   hp_rule(dx, dy, BLACK_DASH_SIZE, dthick);
   dy += dash_size;
   num_dashes--;
   }
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
hp_movex(obj->x);
hp_movey(obj->y);
fprintf(fp,"\033*c%dH", ctolp(obj->w,0));       /** set width decipoints **/
fprintf(fp,"\033*c%dV", ctolp(obj->h,0));       /** set height **/
fprintf(fp,"\033*c%dG", (int)obj->density);     /** set gray level **/
fprintf(fp,"\033*c2P");                         /** draw block **/
}
     
/***********************************************/
/** draw a horizontal line on the HP laserjet **/
/***********************************************/
     
int hp_hline(x,y,w,thick,dashed)
     
COORD x,y,w,thick;
int dashed;
     
{
SHORT dx,dy,dw,dthick;
     
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dw = ctolp(w,300);
dthick = ctolp(thick,300);
     
hp_dothline(dx,dy,dw,dthick,dashed);
}
     
/*************************************************************************/
/** This draws a horizon. line on the hp LaserJet. Dimesions are passed **/
/** in dots (300 dpi).                                                  **/
/*************************************************************************/
     
int hp_dothline(dx,dy,dw,dthick,dashed)
     
int dx, dy, dw, dthick, dashed;
{
SHORT num_dashes, dash_size;
SHORT first_dash_length;
     
if (dx < 0) dx=0;
if (dy < 0) dy=0;
if (dw < 0) dw=0;
     
if (!dashed) {
   hp_rule(dx, dy , dthick, dw);
   return(AOK);
   }

/** BLACK_DASH_SIZE is the exact length of the black line,         **/
/** WHITE_DASH_SIZE is a nominal size that is adjusted to make the **/
/**                 dashes look evenly spaced **/

num_dashes = dw/(WHITE_DASH_SIZE+BLACK_DASH_SIZE);  /** pick reasonable **/
dash_size  = (dw - BLACK_DASH_SIZE)/num_dashes;     /** get w+b to use **/

first_dash_length = dw - num_dashes*dash_size;

hp_rule(dx, dy , dthick, first_dash_length);
dx += first_dash_length+dash_size-BLACK_DASH_SIZE;

while (num_dashes) {
   hp_rule(dx, dy, dthick, BLACK_DASH_SIZE);
   dx += dash_size;
   num_dashes--;
   }
}
     
     
     
/**********************************/
/*** LaserJet box draw function ***/
/**********************************/
     
int hp_box(x,y,h,w,thick,dashed)
     
COORD x,y,h,w,thick;
int dashed;

{
SHORT dx,dy,dh,dw,dthick;

/*
dx = ctolp(x-XMARG,300);
dy = ctolp(y-YMARG,300);
dh = ctolp(h,300);
dw = ctolp(w,300);
dthick = ctolp(thick,300);
*/

dx = x-XMARG;
dy = y-YMARG;
dh = h;
dw = w;
dthick = max(1,thick*300/576);      /** old coords to new coords **/

/** dimentions of the box are OUTSIDE dimentions **/

hp_dothline(dx, dy, dw, dthick, dashed);
hp_dothline(dx, dy - dthick + dh, dw, dthick, dashed);
hp_dotvline(dx, dy, dh, dthick, dashed);
hp_dotvline(dx + dw - dthick, dy, dh, dthick, dashed);
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
fprintf(fp,"\0339\033&a0L\033&l0E");        /** clear side/top margins **/
fprintf(fp,"\033&l%dX",printerprefs.copies);
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
LONG i,k,pre_calc;
     
g=(struct graphic *)obj->data;
bytesperrow=RASSIZE(ctolp(obj->w,obj->dpi),1);
numrows=ctolp(obj->h,obj->dpi);
bitmap=(BYTE *)AllocMem(bytesperrow*numrows,MEMF_CLEAR);
if (bitmap==NULL) {
   return(OUTOFMEM);
   }

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

fprintf(fp,"\033*t%dR",obj->dpi);  /** set printer resolution **/
fprintf(fp,GSTART);   /* graphic dump start cmd */
     
for (i=0; i<numrows; i++) {
   fprintf(fp,"\033*b%dW",bytesperrow); /** bytes per row is even **/
   pre_calc = i*bytesperrow;
   fwrite(bitmap+pre_calc, bytesperrow, 1, fp);
/*   for (k=0;k<bytesperrow;k++) */
/*      fputchar(*(bitmap+pre_calc+k),fp); */
     
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
     
