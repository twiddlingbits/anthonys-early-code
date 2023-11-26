#include "exec/types.h"
#include "exec/nodes.h"
#include "exec/lists.h"
#include "intuition/intuition.h"
#include "desk.h"

extern SHORT screensize;

/*************/
/** ctosx  **/
/*************/


/*** convert LCOORD to screen pix in x direction
     1 LCOORD == 1/576 in == 1/8 point

     x == an x coordinate in LCOORD's
     return's x in screen pixals **/

ctosx(x)

LCOORD x;
{
if (screensize >= 800)
   return((int)(x/10));

return((int)( ((x>>3)+1) >> 1 ) );
}

/************/
/** stocx  **/
/************/

stocx(x)
SHORT x;

{
if (screensize >= 800)       /* A2024 monitor! */
   return((int)(x*10));

return((int)(x<<4));
}

/************/
/** ctosy  **/
/************/

ctosy(y)
LCOORD y;
{

if (screensize>=800)
   return((int)((y+4)/9));

if (screensize < 400)
   return((int)((y+20)/40));

return((int)((y+10)/20));
}

/************/
/** stocy  **/
/************/

stocy(y)
SHORT y;
{
if (screensize>=800)
   return((int)(y*9));

if (screensize < 400)
   return((int)(y*40));

return((int)(y*20));
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

