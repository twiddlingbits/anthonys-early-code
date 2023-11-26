#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfx.h"
#include "graphics/rastport.h"
#include "stdio.h"
#include "desk.h"

#define TITLEWIDTH 10
#define EDGEWIDTH 4
#define BOTWIDTH 1

/*********************************************************************/
/** cropobject  -- let user change the "viewable area" of a graphic **/
/*********************************************************************/

cropobject(obj)

struct object *obj;

{
ULONG class;
USHORT code;
SHORT  cornerx,cornery;
SHORT x,y,oldx,oldy;
SHORT ww,wh,bpr,mag,wmag;
struct Window *w;
struct IntuiMessage *message,*GetMsg();
struct graphic *g;
struct click *click,*getclick2();
extern struct Window *clipboard;
FIXED temp;
UBYTE *src;

static struct NewWindow nw = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   0,0,   /* window width and height */
   0,1,   /* detail and block pens */
   CLOSEWINDOW|MOUSEBUTTONS|MOUSEMOVE|RAWKEY,   /* IDCMP flags */
   WINDOWCLOSE|ACTIVATE|WINDOWDRAG,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Crop This Graphic:",   /* window title */
   NULL,   /* custom screen */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

mag=obj->mag;
g=(struct graphic *)obj->data;
nw.Screen=clipboard->WScreen;

ww = min(stocx(633),obj->w);
temp=ww;
temp = temp << 16;
temp = temp/g->aspect;
wh = ctosy(temp);
ww = ctosx(ww);

if (wh > 200-TITLEWIDTH) {
   wh = min(stocy(200-TITLEWIDTH),obj->h);
   temp=wh;
   temp = temp*g->aspect;
   temp = temp << 16;
   ww = min(633,ctosx(temp));
   wh = ctosy(wh);
   }


/** double crop image if there is room for it **/

if ( (wh<<1) < (200-TITLEWIDTH) && (ww<<1) < 633) {
   wh=wh<<1;
   ww=ww<<1;
   wmag=1;
   }
else
   wmag=0;

wh += TITLEWIDTH;
ww += EDGEWIDTH;

nw.Width=ww;
nw.MinWidth=ww;
nw.MaxWidth=ww;
nw.Height=wh;
nw.MinHeight=wh;
nw.MaxHeight=wh;
nw.LeftEdge= (640-ww)>>1;
nw.TopEdge = (200-wh)>>1;
w=(struct Window *)OpenWindow(&nw);
if (w==0) return(FALSE);

newpointer(clipboard->WScreen,CROP);

bpr = w->RPort->BitMap->BytesPerRow;

fit (g->planes[g->numplanes-1],  0,  g->h,  g->w,  RASSIZE(g->w,1),  /** copy image **/
     w->RPort->BitMap->Planes[1] + bpr*(nw.TopEdge+TITLEWIDTH),
     nw.LeftEdge+EDGEWIDTH/2, wh - TITLEWIDTH-BOTWIDTH, ww-EDGEWIDTH,
     bpr);

if (g->numplanes > 1)
   src = g->planes[g->numplanes-2];
else
   src = g->planes[0];

fit (src,  0,  g->h,  g->w,  RASSIZE(g->w,1),  /** copy image **/
     w->RPort->BitMap->Planes[0] + bpr*(nw.TopEdge+TITLEWIDTH),
     nw.LeftEdge+EDGEWIDTH/2, wh - TITLEWIDTH-BOTWIDTH, ww-EDGEWIDTH,
     bpr);

newclick:      /** jump here if user wants to try again **/

do {
   click=getclick2(w);
   if (click==NULL) {      /** CLOSEWINDOW **/
      g->usex=0;
      g->usey=0;
      g->usew=g->w;
      g->useh=g->h;
      CloseWindow(w);
      return(TRUE);
      }
   } while (click->Window != w);    /** make sure they click in our window **/

cornerx=click->x;
cornery=click->y;
oldx=cornerx;
oldy=cornery;

ReportMouse(TRUE,w);
SetDrMd(w->RPort,COMPLEMENT);
drawbox(w->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);

loop:
while ((message=GetMsg(w->UserPort))!=0) {
   if(message->Class==MOUSEBUTTONS) break;
   ReplyMsg (message);   /* empty msg buffer, and wait */
   }
while (message==0) {
   Wait(1<<w->UserPort->mp_SigBit);
   message=GetMsg(w->UserPort);
   }

class=message->Class;
code=message->Code;
x=w->MouseX;
y=w->MouseY;
ReplyMsg(message);

if (x <= cornerx) {
   x=cornerx+2;   /** make sure we don't flip it **/
   }

if (y <= cornery) {
   y=cornery+2;
   }

drawbox(w->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);

if (class==MOUSEBUTTONS && code==SELECTUP) {
   ReportMouse(FALSE,w);
   CloseWindow(w);
/* printf("cx %d cy %d x %d y %d oldx %d oldy %d ww %d wh %d\n",
     cornerx,cornery,x,y,oldx,oldy,ww,wh); */


   temp = max(0,cornerx-EDGEWIDTH/2) << 16;
   g->usex = ((temp/(ULONG)(ww-EDGEWIDTH))*(ULONG)g->w) >> 16;

   temp = max(0,cornery - TITLEWIDTH) << 16;
   g->usey = ((temp/(ULONG)(wh - TITLEWIDTH-BOTWIDTH))*(ULONG)g->h) >> 16;

   temp = min(oldx - cornerx, ww-EDGEWIDTH) << 16;
   g->usew = ((temp/(ULONG)(ww-EDGEWIDTH))*(ULONG)g->w) >> 16;

   temp = min(oldy - cornery, wh - TITLEWIDTH) << 16;
   g->useh = ((temp/(ULONG)(wh - TITLEWIDTH-BOTWIDTH))*(ULONG)g->h) >> 16;

   return(TRUE);
   }

if (class==RAWKEY) {          /** space to abort **/
   goto newclick;
   }

oldx=x;
oldy=y;
drawbox(w->RPort,cornerx,cornery,oldy-cornery,oldx-cornerx);
goto loop;
}



/*******************************************************************/
/*** getclick()   Gets a mouse click and returns new window, x, y **/
/*** modified from normal get click to return if CLOSEWINDOW      **/
/*******************************************************************/

struct click *getclick2(Window)

struct Window *Window;

{
static struct click click;
struct IntuiMessage *message;
ULONG class;
USHORT code;

do {
   while((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
      Wait(1<<Window->UserPort->mp_SigBit);
      }
   class=message->Class;
   code=message->Code;
   click.x=message->MouseX;
   click.y=message->MouseY;
   click.Window=message->IDCMPWindow;
   if (click.Window->Flags&GIMMEZEROZERO) {
      click.x=click.Window->GZZMouseX;
      click.y=click.Window->GZZMouseY;
      }
   ReplyMsg(message);

   if (class==CLOSEWINDOW) {
      return(NULL);
      }

   } while (class!=MOUSEBUTTONS || code!=SELECTDOWN);
return(&click);
}
