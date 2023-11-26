#include "exec/types.h"
#include "exec/nodes.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "desk.h"

/** on 2-25-89 the 'COORD' def'n was changed from 1/8th pt to 1 printer **/
/** pixel. Thus the old coordpertextpix is now 1, and xinch & yinch     **/
/** (the printer dpi) needs to be looked at **/


extern SHORT screensize;
extern USHORT xinch, yinch;

/*************/
/** ctosx  **/
/*************/


/*** convert LCOORD to screen pix in x direction
     1 LCOORD == 1/576 in == 1/8 point

     x == an x coordinate in LCOORD's
     return's x in screen pixals **/

/** x / xinch          == x in inches **/
/** (x / xinch) * 576  == x in old coords **/

ctosx(x)

LCOORD x;
{
if (screensize >= 800)
   return((int)((x*58+(int)xinch/2)/(int)xinch));

return((int)((x*36+(int)xinch/2)/(int)xinch));
}

ctosxnr(x)  /** no rounding **/

LCOORD x;
{
/* return(ctosx(x)); */
if (screensize >= 800)
   return((int)((x*58)/(int)xinch));

return((int)((x*36)/(int)xinch));
}

/************/
/** stocx  **/
/************/

stocx(x)
SHORT x;

{
if (screensize >= 800)       /* A2024 monitor! */
   return((int)(x*(int)xinch/58));

return((int)(x*(int)xinch/36));
}

/************/
/** ctosy  **/
/************/

ctosy(y)
LCOORD y;
{

if (screensize>=800)
   return((int)((y*64+(int)yinch/2)/(int)yinch));

if (screensize < 400)
   return((int)((y*14+(int)yinch/2)/(int)yinch));

return((int)((y*29+(int)yinch/2)/(int)yinch));
}


ctosynr(y)        /** no rounding **/
LCOORD y;
{
/* return(ctosy(y)); */
if (screensize>=800)
   return((int)((y*64)/(int)yinch));

if (screensize < 400)
   return((int)((y*14)/(int)yinch));

return((int)((y*29)/(int)yinch));
}

/************/
/** stocy  **/
/************/

stocy(y)
SHORT y;
{
if (screensize>=800)
   return((int)(y*(int)yinch/64));

if (screensize < 400)
   return((int)(y*(int)yinch/14));

return((int)(y*(int)yinch/29));
}

/*************************************************************/
/*** given page and x coord in LCOORD return screen positon ***/
/*************************************************************/

ctomousex(view,x)
struct pageview *view;
LCOORD x;
{
return(ctosx(x*(LCOORD)view->mag) - ctosx((LCOORD)view->x)+XBORDER);
}

/*************************************************************/
/*** given page and y coord in LCOORD return screen positon ***/
/*************************************************************/

ctomousey(view,y)
struct pageview *view;
LCOORD y;
{
return(ctosy(y*(LCOORD)view->mag) - ctosy((LCOORD)view->y)+YBORDER);
}


/************************************************************/
/* given a page and mousex on that page, return x in coords */
/************************************************************/


mousetocx(page,x)

struct pageinfo *page;
SHORT x;

{
extern SHORT snaptogrid;
extern COORD x_gridsize;

x = x - XBORDER;
return(XSNAPGRID((stocx(x) + page->view->x)/page->view->mag));
}

/************************************************************/
/* given a page and mousey on that page, return y in coords */
/************************************************************/

mousetocy(page,y)

struct pageinfo *page;
SHORT y;

{
extern SHORT snaptogrid;
extern COORD y_gridsize;

y = y - YBORDER;
return(YSNAPGRID((stocy(y) + page->view->y)/page->view->mag));
}

