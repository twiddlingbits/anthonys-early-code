#include <exec/types.h>
#include <exec/exec.h>
#include <graphics/text.h>
#include <intuition/intuition.h>
#include "desk.h"
#include "text.h"
     
#define debug 0
#define xdebug 0
     
struct chardef {
   WORD charOffset;
   WORD charBitWidth;
};
     
/* Macro for rounding fixed point fractional coord into coord */
#define R_FP_TO_COORD(x)  (((x)+16)>>16)
#define FP_TO_COORD(x)  ((COORD)((x)>>16))
#define FUDGE 0x1000
     
#define MAX(a,b) ((a)>(b) ? (a) : (b))
     
extern int plaser;
extern SHORT texterrors;
/* extern FIXED cpertextpixx, cpertextpixy; */
extern SHORT inversetext;
extern struct text_state *first_state;
extern short this_page;
extern SHORT greeking;
     
short text_change;   /* TRUE if esc sequence caused font,mag,etc. change */
     
/* parameters for text_fit */
     
ULONG fitp_ystep, fitp_xstep;
LONG fitp_yfudge, fitp_xfudge;
PLANEPTR fitp_destdata;
UWORD fitp_desth;
UWORD fitp_destw;
UWORD fitp_destmodulo;
FIXED fitp_italicshift;

extern SHORT clipping;

static PLANEPTR bm;
static int xoff, grw, grh;
static struct text_line *line;
static FIXED xstep, ystep;
static FIXED x_opt1, y_opt1;
static struct text_ctrl tc;

#define  FIT         0
#define  ITFIT       1
#define  INVFIT      2
#define  INVITFIT    3
#define  STDFIT      4           /* standard (not text) fitting   */
#define  STDINVFIT   5           /* for underlines                */
typedef int (*FITPTR)();
extern int textfit(), itfit(), invtextfit(), invitfit();
extern int ctextfit(), citfit(), cinvtextfit(), cinvitfit();
extern int fit(), cfit(), invfit(), cinvfit();
static FITPTR fitfunc[6][2] = {textfit,    ctextfit,
                               itfit,      citfit,
                               invtextfit, cinvtextfit,
                               invitfit,   cinvitfit,
                               fit,        cfit,
                               invfit,     cinvfit};

/*********************************************************************/
/* status = draw_text (obj, bm, grw, grh, xoff, bmw);                */
/*                                                                   */
/* Draws text pointed to by obj into bitmap bm.  Destination area is */
/* of size grw, grh.  bm itself is of size bmw(bytes), grh.  Dest    */
/* area starts at x bit offset xoff in bm.  Note: bm is not a struct */
/* BitMap; it is a PLANEPTR.  Note that obj->data must point to a    */
/* previously initialized text_object.                               */
/* Status is FALSE if everything went OK, TRUE if out of memory.     */
/* If plaser == TRUE, things go slightly different.  First, the only */
/* argument used is obj, the others can be 0.  If a laser font is    */
/* enabled, characters go out through laser_charout instead of going */
/* into a bitmap.  If an amiga font is used, the characters still    */
/* stuffed into a temporary bitmap, but then laser_prtbitmap is      */
/* called on a character at a time basis, instead of filling one     */
/* large bitmap like if plaser == FALSE.                             */
/*********************************************************************/
     
draw_text (obj, pbm, pgrw, pgrh, pxoff, pbmw, pxstep, pystep)
struct object *obj;
PLANEPTR pbm;
int pgrw, pgrh, pxoff, pbmw;
FIXED pxstep, pystep;
{
COORD understart, understop;
short int underlining;
FIXED space_extra, char_extra;
FIXED fracx;
COORD w;
COORD vcent;
char ch;
int i, nc;
short int need_lasprint;
int temp_alloc;
struct text_state linestate;
struct text_object *tobj;
char hyphen = HYPHENCHAR;
COORD char_width();
SHORT savetexterrors;
int oldgrw, oldgrh;
short first;
struct macro *copy_macro_chain();

#if debug
printf("entering DRAWTEXT\n");
#endif
#if xdebug
printf("pgrw=%x,pgrh=%x,pxoff=%x,pbmw=%x,pxstep=%x,pystep=%x\n",
         pgrw,pgrh,pxoff,pbmw,pxstep,pystep);
#endif
bm = pbm;
fitp_destmodulo = pbmw;
xoff = pxoff;
grw = pgrw;
grh = pgrh;
oldgrw = 0;
oldgrh = 0;
xstep = pxstep;
ystep = pystep;
#if xdebug
printf("xstep=%x, ystep=%x\n", xstep, ystep);
#endif
     
first_state = &((struct text_object *)(obj->first->data))->state;
this_page=obj->pn;
savetexterrors = texterrors;
texterrors = FALSE;
     
tobj = (struct text_object *)obj->data;
tc.state = tobj->state;
tc.m = copy_macro_chain(tobj->macro);
tc.unget_macro = NULL;
tc.unget_okay = FALSE;
tc.x = 0;
tc.y = 0;
tc.maxw = 0;
tc.newx = 0;
tc.newy = 0;
tc.startx = 0;
tc.itkern = 0;
tc.ch = 0;
tc.prevch = 0;
tc.count = 0;

set_font(&tc.state);

understart = 0;
understop = 0;
underlining = FALSE;
     
line = tobj->line;
     
if (line)
   tc.y = line->maxh;
else
   tc.y = 0;

vcent = 0;
if (tobj->flags & VERTCENTER) {
   vcent = (obj->h - tobj->height) / 2;
   tc.y += vcent;
}

#if 0
x_opt1 = ((cpertextpixx>>4)      /*    4.12 */
              *xstep)>>16;       /*  * 0.16 = 4.28 -> 4.12 */
y_opt1 = ((cpertextpixy>>4)      /*    4.12 */
              *ystep)>>16;       /*  * 0.16 = 4.28 -> 4.12 */
#endif
x_opt1 = xstep>>4;         /* 4.12 */
y_opt1 = ystep>>4;         /* 4.12 */

#if xdebug
#if 0
printf("cpertextpixx = %x, cpertextpixy = %x\n", cpertextpixx,cpertextpixy);
#endif
printf("x_opt1 = %x, y_opt1 = %x\n", x_opt1, y_opt1);
#endif

opt_tptoc(&tc.state);
     
need_lasprint = FALSE;
temp_alloc = 0;
     
first = TRUE;
     
for ( ; line != NULL && !checkcan(); line = line->next) {

#if debug
   printf("drawtext, top of line loop\n");
#endif

   if (greeking) {
      greek_line(line, vcent, obj->w);
      goto lineloopend;          /* avoid re-indenting entire routine! */
   }

   tc.itkern = 0;
   tc.ch = '\0';
   ch = 'a';            /* anything non-0 for loop below */
   tc.x = 0;
   fracx = 0;

#if debug
   printf("tc.count=%d, line->startcnt=%d\n", tc.count, line->startcnt);
#endif
   while (tc.count < line->startcnt && ch != '\0')
      ch = get_ch(&tc);
#if debug
   printf("end of skipping\n");
#endif

   ch = get_ch(&tc);

   while (ch == CHAR_SOFTHYPHEN)
      ch = get_ch(&tc);

   linestate = tc.state;

   unget_ch(&tc);

   tc.x = 0;
   tc.startx = 0;

   if (first)
      tc.y += linestate.top_margin;
   first = FALSE;
     
   if (tc.y < line->maxh)
      tc.y = line->maxh;         /* don't go above top of box */

   if (plaser) {
      laser_movey (tc.y + tc.state.rel_vskip, obj);
     
      oldgrw = grw;
      oldgrh = grh;
      grw = ctolp(obj->w, obj->dpi);
      grh = ctolp(line->maxh+line->maxd, obj->dpi);
      fitp_destmodulo = RASSIZE(grw,1);
      if (temp_alloc != RASSIZE(grw, grh)) {
         if (temp_alloc)
            FreeMem(bm, temp_alloc);
         temp_alloc = RASSIZE(grw, grh);
         bm = (PLANEPTR) AllocMem(temp_alloc, MEMF_CLEAR);
      }
      else
         setmem (bm, temp_alloc, 0);
      if (bm == NULL) {
         grw = 0;
         grh = 0;
      }
      if (grw != oldgrw || grh != oldgrh) {
#if 0
         x_opt1 = ((cpertextpixx>>4)      /*    4.12 */
                       *xstep)>>16;       /*  * 0.16 = 4.28 -> 4.12 */
         y_opt1 = ((cpertextpixy>>4)      /*    4.12 */
                       *ystep)>>16;       /*  * 0.16 = 4.28 -> 4.12 */
#endif
         x_opt1 = xstep>>4;               /* 4.12 */
         y_opt1 = ystep>>4;               /* 4.12 */
      }
     
      need_lasprint = FALSE;
   }
     
   space_extra = 0;
   char_extra = 0;
   nc = line->stretch_chars-1;
   if (line->flags & TEXTL_HYPHEN)
       nc++;
   w = obj->w - line->right - line->w;
#if debug
   printf("nc=%d, ns=%d, obj->w=%x, line->w=%x, ww=%x\n", nc, line->num_spaces, obj->w, line->w, w);
   printf("num_chars=%d\n", line->num_chars);
#endif
   if (nc != 0) {
      if (!((line->flags & TEXTL_PAREND) ||
            (linestate.flags & (TEXTS_RAGRIGHT|TEXTS_RAGLEFT)))) {
         if (line->num_spaces) {
            space_extra = (w<<16) / line->num_spaces;
            /* To avoid bad spacing on very short lines: |The        cat| */
            /* add letter sp when interword sp too big:  |T h e    c a t| */
            if (space_extra > (linestate.max_space_stretch<<16)) {
               space_extra = linestate.max_space_stretch<<16;
               char_extra = ((w<<16)-(space_extra * line->num_spaces)) / nc;
            }
         }
         else
            char_extra = (w<<16) / nc;
      }
   }
#if debug
   printf("char_extra=%x, space_extra=%x\n", char_extra, space_extra);
#endif

   tc.x += line->left;         /* includes indent if needed */
   tc.startx += line->left;

   if ((linestate.flags&TEXTS_RAGRIGHT)&&(linestate.flags&TEXTS_RAGLEFT)) {
      tc.x += (w>>1);                  /* center justify */
      tc.startx += (w>>1);
   }
   else if (linestate.flags & TEXTS_RAGLEFT) {
      tc.x += w;                       /* right justify */
      tc.startx += w;
   }
     
   new_y();                /* calculate stuff for Y pos */
   text_change = FALSE;
     
   for (i=0; i < line->num_chars; i++) {
     
      ch = get_ch(&tc);
      while (ch == CHAR_SOFTHYPHEN)
         ch = get_ch(&tc);

      if (underlining) {
         if (!(tc.state.style & UNDERLINE)) {
            underlining = FALSE;
            do_underline (understart, understop, obj);
         }
      }
      else {
         if (tc.state.style&UNDERLINE) {
            underlining = TRUE;
            understart = tc.startx;
         }
      }
     
      if (ch == CHAR_VSKIP && plaser)
         laser_movey(tc.y + tc.state.rel_vskip, obj);
     
      if (ch != CHAR_HSKIP && ch != CHAR_VSKIP &&
          ch != CHAR_HPOS  && ch != CHAR_TAB) {
         if (plaser && tc.state.lfont) {
            laser_movex(tc.startx, obj);
            laser_charout(tc.state.lfont, ch);
         }
         else {
            mytext(ch);
            if (plaser)
               need_lasprint = TRUE;
         }
      }
      if (underlining)
         understop = tc.x;
      fracx += char_extra;
      if (spacechar(ch) || ch==CHAR_TIE && ch!=CHAR_HPOS && ch!=CHAR_TAB)
         fracx += space_extra;
      tc.x += fracx>>16;
      fracx &= 0xFFFF;
     
   }

   if (line->flags & TEXTL_HYPHEN) {

      force_get_ch(&tc, hyphen);

      if (plaser && tc.state.lfont) {
         laser_movex(tc.startx, obj);
         laser_charout(tc.state.lfont, hyphen);
      }
      else {
         mytext(hyphen);
         if (plaser)
            need_lasprint = TRUE;
      }
      if (text_change)
         opt_tptoc(&tc.state);
   }
     
   if (underlining) {
      do_underline (understart, understop, obj);
      underlining = FALSE;
   }
     
   if (plaser && need_lasprint) {
      laser_prtbitmap(bm,grw,grh,0,tc.y+tc.state.rel_vskip-line->maxh,obj);
      need_lasprint = FALSE;
   }
     
   if (linestate.flags & TEXTS_USELEADING)
      tc.y += linestate.leading;
   else {
      tc.y += line->maxd + linestate.spacing;
      if (line->next)
         tc.y += line->next->maxh;
   }
   if (line->flags & TEXTL_PAREND)
      tc.y += linestate.paragraph_space;

lineloopend:

}
     
if (temp_alloc)
   FreeMem(bm, temp_alloc);
     
texterrors = savetexterrors;

discard_text_ctrl(&tc);

return(FALSE);
}
     
     
static UCOORD yheight, ysize;
static int ccy;
COORD char_real_height(), char_ysize();
     
     
/*********************************************************************/
/* new_y();                                                          */
/*                                                                   */
/* Called every time something relevant to Y changes (pos,mag,font)  */
/*********************************************************************/
     
new_y()
{
COORD tempcy;
     
yheight = char_real_height(&tc.state);
ysize = char_ysize(&tc.state);
     
/* Clip in Y direction to avoid overwriting memory */
     
fitp_desth = MAX(1,FP_TO_COORD(ysize*ystep));   /** Round This? **/
tempcy = (plaser ? line->maxh : tc.y) + tc.state.rel_vskip;
ccy = FP_TO_COORD((tempcy-yheight)*(SFIXED)ystep);
     
if (grh < fitp_desth)      /* char won't fit in bitmap */
   return(0);
if (ccy > grh-fitp_desth)  /* pos too low */
   ccy = grh-fitp_desth;
if (ccy < 0)               /* pos too high */
   ccy = 0;
     
fitp_destdata = bm + fitp_destmodulo * ccy;
     
/* Set up ystep variable for fit */
     
if (tc.state.lfont) {
   fitp_ystep = ((  (ULONG)(tc.state.font->tf_YSize << 16)    /* 16.16 */
                  / ysize) <<12)             /* / 16.0 = 16.16 -> 4.28 */
                / (ystep>>4);                /* / 0.12 = 16.16 */
}
else {
   fitp_ystep = (ULONG)(1<<31) /               /* 1.31 */
                  ((y_opt1                     /* 4.12 */
                   * (tc.state.magy>>8)) >>5); /* * 8.8 = 12.20 -> 12.15 */
                                               /* 1.31 / 12.15 = 16.16 */
     
   fitp_xstep = (ULONG)(1<<31) /               /* 1.31 */
                  ((x_opt1                     /* 4.12 */
                   * (tc.state.magx>>8)) >>5); /* * 8.8 = 12.20 -> 12.15 */
}
fitp_yfudge = (1<<16) - ((fitp_ystep*fitp_desth) & 0xFFFF);
if (fitp_yfudge < 0 || fitp_yfudge == (1<<16))
   fitp_yfudge = 0;
#if xdebug
printf("fitp_ystep=%x, fitp_desth=%x, fitp_yfudge=%x, ysize=%x, grh=%x\n",
        fitp_ystep, fitp_desth, fitp_yfudge, ysize, grh);
#endif
}
     
/*********************************************************************/
/* mytext(ch)                                                        */
/*                                                                   */
/* Draw character ch into bitmap bm at position (cx>>16), cy.        */
/*********************************************************************/
     
mytext(ch)
char ch;
     
{
struct chardef *cd;
struct TextFont *font;
SCOORD kern, stpmagtocx();
COORD bs, xwc;
COORD tpmagtocy(), tpmagtocx();
COORD char_base_width();
FIXED ittotshift;
int xpos, cittotshift;
UBYTE chr;
WORD cbw;
int fittype;
     
if (ch == CHAR_HSKIP || ch == CHAR_VSKIP)
   return(0);
     
if (ch == CHAR_TIE)
   ch = ' ';
     
font = tc.state.font;
     
if (text_change) {
   new_y();
   text_change = FALSE;
}
     
cd = (struct chardef *)font->tf_CharLoc;
chr = ch;
if (chr < font->tf_LoChar || chr > font->tf_HiChar)
   chr = (font->tf_HiChar - font->tf_LoChar) + 1;
else
   chr -= font->tf_LoChar;
     
if (font->tf_CharKern) {
   kern = stpmagtocx((int)*(((WORD *)font->tf_CharKern) + chr),
                     tc.state.magx);
}
else
   kern = 0;
     
cbw = (cd+chr)->charBitWidth;
if (cbw) {
     
   xwc = char_base_width(ch, &tc.state, cbw);
/*   fitp_destw = MAX(FP_TO_COORD(xwc*xstep+FUDGE), 1); */
   fitp_destw = MAX(FP_TO_COORD(xwc*xstep), 1);  /** round in 1.1 **/
     
   if (tc.state.lfont) {
      fitp_xstep = ((  (ULONG)(cbw << 16)       /* 16.16 */
                     / xwc) <<12)               /* / 16.0 = 16.16 -> 4.28 */
                   / (xstep>>4);                /* / 0.12 = 16.16 */
   }
     
   fitp_xfudge = (1<<16) - ((fitp_xstep*fitp_destw) & 0xFFFF);
   if (fitp_xfudge < 0 || fitp_xfudge == (1<<16))
      fitp_xfudge = 0;
     
   if (tc.state.style & ITALIC && !(font->tf_Style & FSF_ITALIC)) {
      ittotshift = tpmagtocx(font->tf_YSize>>1,tc.state.magx);
      fitp_italicshift = (ittotshift*xstep) /
                         MAX(FP_TO_COORD(ysize*ystep), 1);
      cittotshift = FP_TO_COORD(ittotshift*xstep);
      if (inversetext) {
         fittype = INVITFIT;
         }
      else {
         fittype = ITFIT;
         }
   }
   else {
      ittotshift = 0;
      fitp_italicshift = 0;
      cittotshift = 0;
      if (inversetext)  {
         fittype = INVFIT;
         }
      else {
         fittype = FIT;
         }
   }
     
   xpos = FP_TO_COORD((tc.startx+kern+ittotshift)*xstep);
     
/* Clip in X direction to avoid overwriting memory */
     
   if (grw < fitp_destw + cittotshift) /* char won't fit in bitmap */
      return(0);
   if (xpos + fitp_destw > grw)        /* char too far right */
      xpos = grw - fitp_destw;
   if (xpos < cittotshift)             /* char too far left */
      xpos = cittotshift;

#if xdebug
printf("grw=%x, fitp_destw=%x, xpos=%x, xwc=%x, fitp_xstep=%x, fitp_xfudge=%x\n", grw,fitp_destw,xpos,xwc,fitp_xstep,fitp_xfudge);
#endif
     
   (*(fitfunc[fittype][clipping])) (
      font->tf_CharData,
      (cd+chr)->charOffset,
      font->tf_Modulo,
      xoff + xpos);
     
   if (tc.state.style & BOLD && !(font->tf_Style & FSF_BOLD)) {
      bs = tpmagtocx(font->tf_BoldSmear, tc.state.magx);
      xpos += R_FP_TO_COORD(bs*xstep);
     
/* Clip in X direction to avoid overwriting memory */
     
      if (xpos + fitp_destw > grw)        /* char too far right */
         xpos = grw - fitp_destw;
      if (xpos < 0)                       /* char too far left */
         xpos = 0;
     
      (*(fitfunc[fittype][clipping])) (
         font->tf_CharData,
         (cd+chr)->charOffset,
         font->tf_Modulo,
         xoff + xpos);
   }
}
}
     
     
/*********************************************************************/
/* do_underline(...)                                                 */
/*                                                                   */
/* Underline from x position 'start' to x position 'stop'.           */
/*********************************************************************/
     
do_underline(start, stop, obj)
COORD start, stop;
struct object *obj;
     
{
COORD ysize, tpmagtocy();
int xpos, xw;
int ys, ccy;
SHORT bit = -1;

#if debug
printf("underlining from %x to %x\n", start, stop);
#endif
if (plaser) {
   laser_movey(tc.y, obj);
   laser_movex(start, obj);
   laser_underon();
   laser_movex(stop, obj);
   laser_underoff();
   laser_movey(tc.y+tc.state.rel_vskip, obj);
   return(0);
}
     
ysize = tpmagtocy(1, 1);
     
/* Clip in Y direction to avoid overwriting memory */
     
ys = MAX(1,FP_TO_COORD(ysize*ystep));
ccy = FP_TO_COORD(tc.y*ystep) + ys;

#if debug
printf("orig ccy = %x, ys = %x\n", ccy, ys);
#endif
if (grh < ys)              /* char won't fit in bitmap */
   return(0);
if (ccy > grh-ys)          /* pos too low */
   ccy = grh-ys;
if (ccy < 0)               /* pos too high */
   ccy = 0;

#if debug
printf("final ccy = %x, ys = %x\n", ccy, ys);
#endif
xw = MAX(FP_TO_COORD((stop-start)*xstep), 1);
     
xpos = FP_TO_COORD(start*xstep);
     
/* Clip in X direction to avoid overwriting memory */

#if debug
printf("orig xpos = %x, xw = %x\n", xpos, xw);
#endif
if (xpos > grw)
   xpos = grw;
if (xpos < 0)
   xpos = 0;
if (xpos + xw > grw)
   xw = grw - xpos;
if (xw < 1)                   /* avoid divide by 0 */
   return 0;

#if debug
printf("final xpos = %x, xw = %x xoff = %x\n", xpos, xw, xoff);
#endif

if (inversetext)
   (*(fitfunc[STDINVFIT][clipping])) (&bit, 0, 1, 1, 0,
           bm+fitp_destmodulo*ccy, xoff+xpos, ys, xw, fitp_destmodulo);
else
   (*(fitfunc[STDFIT][clipping])) (&bit, 0, 1, 1, 0,
        bm+fitp_destmodulo*ccy, xoff+xpos, ys, xw, fitp_destmodulo);
     
}


/*********************************************************************/
/* greek_line(line, ycent, objw)                                     */
/*                                                                   */
/* Draw a greeking pattern for the line "line" with a vertical       */
/* offset of ycent (for vertical centering).  objw is the width of   */
/* the text_obj.  Must not be called when printing, especially to a  */
/* laser printer!!!                                                  */
/*********************************************************************/
     
greek_line(line, ycent, objw)
struct text_line *line;
COORD ycent, objw;

{
COORD ysize, tpmagtocy();
int xpos, xw;
int ys, ccy;

#if debug
printf("greeking line\n");
#endif
     
ysize = line->maxh;
     
/* Clip in Y direction to avoid overwriting memory */
     
ys = FP_TO_COORD(ysize*ystep);
ccy = FP_TO_COORD((line->ypos + ycent - line->maxh)*ystep);

#if debug
printf("orig ccy = %x, ys = %x\n", ccy, ys);
#endif
if (grh < ys)              /* char won't fit in bitmap */
   ys = grh;
if (ccy > grh-ys)          /* pos too low */
   ccy = grh-ys;
if (ccy < 0)               /* pos too high */
   ccy = 0;

#if debug
printf("final ccy = %x, ys = %x\n", ccy, ys);
#endif

if (!((line->flags & TEXTL_PAREND) ||
      (line->flags & (TEXTL_RAGRIGHT|TEXTL_RAGLEFT))))
   xw = objw - line->left - line->right;        /* fill justify */
else
   xw = line->w - line->left;                   /* not filled */

xpos = line->left;
if ((line->flags&TEXTL_RAGRIGHT)&&(line->flags&TEXTL_RAGLEFT))
   xpos += (objw - line->right - line->w)>>1;   /* center justify */
else if (line->flags & TEXTL_RAGLEFT)
   xpos += objw - line->right - line->w;        /* right justify */
     
xw = FP_TO_COORD(xw * xstep);

xpos = FP_TO_COORD(xpos * xstep);
     
/* Clip in X direction to avoid overwriting memory */

#if debug
printf("orig xpos = %x, xw = %x\n", xpos, xw);
#endif
if (xpos > grw)
   xpos = grw;
if (xpos < 0)
   xpos = 0;
if (xpos + xw > grw)
   xw = grw - xpos;
if (xw < 1)                /* avoid divide by 0 */
   return 0;

#if debug
printf("final xpos = %x, xw = %x xoff = %x\n", xpos, xw, xoff);
#endif

if (inversetext)
   invgreekfit(bm+fitp_destmodulo*ccy, xoff+xpos, ys, xw, fitp_destmodulo);
else
   greekfit(bm+fitp_destmodulo*ccy, xoff+xpos, ys, xw, fitp_destmodulo);

return 0;     
}

