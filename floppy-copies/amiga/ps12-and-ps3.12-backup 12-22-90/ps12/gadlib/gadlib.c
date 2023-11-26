#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/studio16.h"
#include "/include/gadlib.h"

void draw_knob();
void drag_knob();
void draw_indent();
void refresh_knob();
void calc_knob_size();
void calc_knob_xy();
void calc_top_element();
void scroll_knob();

/***** Knob Handeling Routines ********************************/

struct knob *create_knob(nk)
struct NewKnob *nk;
{
struct knob *k;

k = (struct knob *)AllocMem(sizeof(struct knob), MEMF_CLEAR);
if (k==0)
   return(0);

k->box_x1=nk->box_x1;
k->box_y1=nk->box_y1;
k->box_x2=nk->box_x2;
k->box_y2=nk->box_y2;
k->flags = nk->flags;
k->kw = nk->window;

if (k->flags&BORDER_3D) {
   k->box_x1 += 2;
   k->box_x2 -= 2;
   k->box_y1 += 1;
   k->box_y2 -= 1;
   }

k->show_size = nk->show_size;
k->list_size = nk->list_size;
k->top_element = nk->top;
k->APen = nk->apen;
k->BPen = nk->bpen;
k->CPen = LIGHT_BROWN;
k->func = nk->func;
calc_knob_size(k);

if (!(k->flags&AUTO_KNOB)) {
   if (nk->knob_width!=-1) 
      k->knob_width = nk->knob_width;	
	if (nk->knob_height!=-1)
		k->knob_height = nk->knob_height;
	}
refresh_knob(k,k->APen);
return(k);
}

/**********************************************************************/

void init_knob(window,k,l, box_x1, box_y1, box_x2, box_y2, show_size, list_size, flags, apen, bpen, top)

struct Window *window;
struct knob *k;
struct list *l;
int box_x1,box_y1;
int box_x2,box_y2;
int show_size, list_size;
int flags,top;
int apen, bpen;
{
k->box_x1=box_x1;
k->box_y1=box_y1;
k->box_x2=box_x2;
k->box_y2=box_y2;
k->knob_y = box_y1;
k->kw = window;
k->list = l;
k->show_size = show_size;
k->list_size = list_size;
k->top_element = top;
k->APen = apen;
k->BPen = bpen;
calc_knob_size(k);

draw_knob(k,k->APen);
}

/****************************************************************/

void refresh_knob(k)   /** change to refresh_knob_from_list() ??!!! **/

struct knob *k;
{

if (k->list) {
   k->top_element = k->list->top_element;
   k->show_size=k->list->show_size;
   k->list_size=k->list->list_size;
	calc_knob_size(k);
	}

SetAPen(k->kw->RPort, k->BPen);
RectFill(k->kw->RPort,k->box_x1, k->box_y1, k->box_x2, k->box_y2);
if (k->flags&BORDER_3D) {
   draw_indent(k->kw->RPort, k->box_x1-2, k->box_y1-1, k->box_x2+2, k->box_y2+1);
   } 
draw_knob(k,k->APen);

}
/****************************************************************/

void calc_knob_size(k)
struct knob *k;
{

if (k->flags&FREE_VERT) { 
   if (k->show_size > k->list_size) {
      k->knob_height = k->box_y2-k->box_y1+1;
      k->top_element = 0;
      }
   else {
      k->knob_height = ((k->box_y2-k->box_y1+1)*k->show_size)/k->list_size;
      }
   k->knob_width = k->box_x2 - k->box_x1-1;
   }
if (k->flags&FREE_HORIZ) {
   if (k->show_size > k->list_size) {
      k->knob_width = k->box_x2-k->box_x1+1;
      k->top_element = 0;
      }
   else {
      k->knob_width = ((k->box_x2-k->box_x1+1)*k->show_size)/k->list_size;
      }
   k->knob_height = k->box_y2 - k->box_y1-1;
   }
calc_knob_xy(k);
}

/****************************************************************/
/** calcuate knob->x and knob->y from top_element, show_size,  **/
/** and list_size                                              **/
/****************************************************************/

void calc_knob_xy(k)
struct knob *k;
{
if (k->flags & FREE_VERT) {
   if (k->show_size > k->list_size)
      k->knob_y=k->box_y1;
   else
      k->knob_y=k->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_size-k->show_size)+1;
   if (k->knob_y+k->knob_height > k->box_y2)
      k->knob_y = k->box_y2 - k->knob_height+1;
   k->knob_x = k->box_x1+1;
   }

if (k->flags & FREE_HORIZ) {
   if (k->show_size > k->list_size)
      k->knob_x=k->box_x1;
   else
      k->knob_x=k->box_x1+((k->box_x2 - k->box_x1-k->knob_width+1)*k->top_element)/(k->list_size-k->show_size)+1;
   if (k->knob_x+k->knob_width > k->box_x2)
      k->knob_x = k->box_x2 - k->knob_width+1;
   k->knob_y = k->box_y1+1;
   }
}

/****************************************************************/
/** calc top_element from show knob_x & knob_y                ***/
/****************************************************************/

void calc_top_element(k)
struct knob *k;
{
if (k->show_size >= k->list_size) {
   k->top_element = 0;
   }
else {
   if (k->flags&FREE_VERT)
      k->top_element = ((k->list_size-k->show_size)*(k->knob_y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);
   if (k->flags&FREE_HORIZ)
      k->top_element = ((k->list_size-k->show_size)*(k->knob_x-k->box_x1))/(k->box_x2-k->box_x1-k->knob_width+1);
   }
}

/****************************************************************/

void draw_knob(k,color)
struct knob *k;
int color;
{
struct RastPort *rp;

rp = k->kw->RPort;
SetAPen(rp, color);
if (k->flags&KNOB_3D)
   RectFill(rp,k->knob_x+2, k->knob_y+1, k->knob_x+k->knob_width-3, k->knob_y+k->knob_height-2);
else
   RectFill(rp,k->knob_x, k->knob_y, k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1);

if (k->flags&KNOB_3D) {
   SetAPen(rp, WHITE);
   Move(rp, k->knob_x+k->knob_width-2, k->knob_y);
   Draw(rp, k->knob_x, k->knob_y);
   Draw(rp, k->knob_x, k->knob_y+k->knob_height-1);
   Move(rp, k->knob_x+1, k->knob_y);
   Draw(rp, k->knob_x+1, k->knob_y+k->knob_height-1);

   SetAPen(rp, DARK_BROWN);
   Move(rp, k->knob_x+k->knob_width-1, k->knob_y);
   Draw(rp, k->knob_x+k->knob_width-1, k->knob_y+k->knob_height-1);
   Move(rp, k->knob_x+k->knob_width-2, k->knob_y+1);
   Draw(rp, k->knob_x+k->knob_width-2, k->knob_y+k->knob_height-1);
   Draw(rp, k->knob_x+1, k->knob_y+k->knob_height-1);
   }

if (k->func)	/* call user set function when knob is re-drawn */
   (*k->func)(k);

}

/****************************************************************/

void handle_knob(k, class, code, x, y)
struct knob *k;
ULONG class;
USHORT code;
int x,y;
{
void refresh_list();
short refresh;
int old_knob_y, old_knob_x;

if (class==MOUSEBUTTONS  && code==SELECTDOWN) {
   if (hit_knob(k, x, y)) {
      drag_knob(k, x, y);
      }
   else {
      refresh = FALSE;
      if (hit_knob_up(k, x, y) || hit_knob_left(k, x, y)) {
         k->top_element--;
         if (k->top_element < 0) k->top_element = 0;
         refresh = TRUE;
         }
      else if (hit_knob_down(k, x, y) || hit_knob_right(k, x, y)) {
         k->top_element++;
         if (k->top_element >= k->list_size) k->top_element = k->list_size-1;
         refresh = TRUE;
         }
      if (refresh) {
         old_knob_y = k->knob_y;
         old_knob_x = k->knob_x;
         calc_knob_xy(k);
         scroll_knob(k, old_knob_x, old_knob_y);
         if (k->func)
            (*k->func)(k);
         if (k->list) {/**!!!! change list so the use func() !!**/
            if (k->list->top_element != k->top_element) {
               k->list->top_element = k->top_element;
               refresh_list(k->list);
               }
            }
         }
      }
   }
}

/****************************************************************/

int hit_knob(k, x, y)

struct knob *k;
int x,y;
{
return(x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->knob_y && y <=(k->knob_y+k->knob_height));
}

/****************************************************************/

hit_knob_up(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y >= k->box_y1 && y < k->knob_y);
}

/****************************************************************/

hit_knob_down(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_VERT && x>=k->knob_x && x<= (k->knob_x+k->knob_width-1) && y > (k->knob_y+k->knob_height) && y <= k->box_y2);
}
/****************************************************************/

hit_knob_left(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_HORIZ && y>=k->knob_y && y<= (k->knob_y+k->knob_height-1) && x >= k->box_x1 && x < k->knob_x);
}

/****************************************************************/

hit_knob_right(k, x, y)

struct knob *k;
int x,y;
{
return(k->flags&FREE_HORIZ && y>=k->knob_y && y<= (k->knob_y+k->knob_height-1) && x > (k->knob_x+k->knob_width) && x <= k->box_x2);
}

/*******************************************************************/

void drag_knob(k, x, y)
struct knob *k;
int y;
{
void refresh_list();
struct IntuiMessage *message, *GetMsg();
ULONG class;
USHORT code;
int newy, newx;
short old_flag;
int old_knob_y, old_knob_x;
int old_top;

old_flag = k->kw->Flags&REPORTMOUSE;
old_knob_x = k->knob_x;
old_knob_y = k->knob_y;
old_top = k->top_element;
ReportMouse(TRUE,k->kw);
if (k->CPen != k->APen)
   draw_knob(k,k->CPen);   /* CPen is knob color when being draged */
while (TRUE) {
   Wait(1<<k->kw->UserPort->mp_SigBit);
   while ((message=GetMsg(k->kw->UserPort))!=0) {
      class=message->Class;
      code=message->Code;
      newy = message->MouseY;
      newx = message->MouseX;
      ReplyMsg(message);

      if (class==MOUSEBUTTONS  && code==SELECTUP) {
         ReportMouse(old_flag,k->kw);
         if (k->CPen != k->APen)
            draw_knob(k,k->APen);   /* CPen is knob color when being draged */
         return;
         }
      }

   if (class==MOUSEMOVE) {
      if (k->flags & FREE_VERT) {
         k->knob_y += (newy - y);
         if (k->knob_y < k->box_y1)
            k->knob_y = k->box_y1;
         if (k->knob_y+k->knob_height > k->box_y2)
            k->knob_y = k->box_y2 - k->knob_height+1;
         }

      if (k->flags & FREE_HORIZ) {
         k->knob_x += (newx - x);
         if (k->knob_x < k->box_x1)
            k->knob_x = k->box_x1;
         if (k->knob_x+k->knob_width > k->box_x2)
            k->knob_x = k->box_x2 - k->knob_width+1;
         }
      
      scroll_knob(k, old_knob_x, old_knob_y);
      
	   old_knob_y = k->knob_y;
      old_knob_x = k->knob_x;
      y = newy;
      x = newx;
      calc_top_element(k);
      if (old_top!=k->top_element)
         if (k->func)
            (*k->func)(k);
      old_top = k->top_element;

      if (k->list) { /**!!!!! replace this with func !! **/
         if (k->list->top_element != k->top_element) {
            k->list->top_element = k->top_element;
            refresh_list(k->list);
				}
         }
      }
   }
}

/************************************************************/

void scroll_knob(k, old_knob_x, old_knob_y)
struct knob *k;
int old_knob_x;
int old_knob_y;
{

SetBPen(k->kw->RPort,k->BPen);

if (k->flags&FREE_VERT) { 
   if (k->knob_y < old_knob_y)    /** dragging up? **/
      ScrollRaster(k->kw->RPort, 0, old_knob_y-k->knob_y, 
         k->box_x1, k->knob_y, k->box_x2, old_knob_y+k->knob_height-1);   
   else 
      ScrollRaster(k->kw->RPort, 0, old_knob_y-k->knob_y, 
         k->box_x1, old_knob_y, k->box_x2, k->knob_y+k->knob_height-1);
   }

if (k->flags&FREE_HORIZ) { 
   if (k->knob_x < old_knob_x)    /** dragging left? **/
      ScrollRaster(k->kw->RPort, old_knob_x-k->knob_x,0, 
          k->knob_x, k->box_y1, old_knob_x+k->knob_width-1, k->box_y2);   
   else 
      ScrollRaster(k->kw->RPort, old_knob_x-k->knob_x, 0,
          old_knob_x, k->box_y1, k->knob_x+k->knob_width-1, k->box_y2);
   }  
}

/***** List Handeling Routines ********************************/


handle_list(l, class, code, x, y)

struct list *l;
ULONG class;
USHORT code;
int x,y;
{
void refresh_list();
int select;
int return_code;
int select_len;
return_code = NO_CODE;

if (l->knob) {
   handle_knob(l->knob, class, code, x, y);
   if (l->top_element!=l->knob->top_element) {
      l->top_element = l->knob->top_element;
      refresh_list(l);
      }
   }

if (class==MOUSEBUTTONS  && code==SELECTDOWN) {
   select = (y-l->box_y-2)/9;
   if (select >= 0 && select < 7) {
      select_len =  my_strlen(l->list_text[select]);
      if (select_len > l->max_show_chars)
         select_len = l->max_show_chars - 1;
      if (x > l->box_x && x < l->box_x+10+8*select_len) {
         select = select + l->top_element;
         if (l->current != select ) {
            l->current = select;
            refresh_list(l);
            return_code = NEW_SELECT;
            }
         else {
            return_code = DOUBLE_SELECT;
            }
         }
      }
   }
return(return_code);
}

/****************************************************************/

void init_list(window,l,k, box_x, box_y, show_size, list_size, top, current,msc)
struct Window *window;
struct list *l;
struct knob *k;
int box_x, box_y;
int list_size;
int show_size;
int top;
int current;
int msc;
{
l->box_x = box_x;
l->box_y = box_y;
l->show_size = show_size;
l->list_size = list_size;
l->top_element = top;
l->current = current;
l->knob = k;
l->lw = window;
l->APen = WHITE;
l->BPen = BROWN;
l->max_show_chars = msc;
}

/****************************************************************/

void refresh_list(l)
struct list *l;
{
int i;
static int max_size=0;
int len;
struct RastPort *rp;

rp = l->lw->RPort;

SetAPen(rp, l->APen);
SetBPen(rp, l->BPen);
SetDrMd(rp, JAM2);

for (i=0; i < l->list_size; i++)
   if ((len=my_strlen(l->list_text[i]))>max_size)
      max_size = len;

for (i=l->top_element; i < (l->top_element+l->show_size); i++) {
   if (i==l->current) {
      SetAPen(rp, l->BPen);
      SetBPen(rp, l->APen);
      }
   Move(rp, l->box_x, l->box_y+(i-l->top_element)*9+8);
   if (i < l->list_size) {
      len=my_strlen(l->list_text[i]);
      if (len > l->max_show_chars)
         len = l->max_show_chars;
      Text(rp, l->list_text[i], len);
      }
   else
      len = 0;
   if (i==l->current) {
      SetAPen(rp, l->APen);
      SetBPen(rp, l->BPen);
      }
   len = min(max_size, l->max_show_chars) - len;
   Text(rp, "                                                                             ",len);
   }
}

/****************************************************************/

void add_list(l, c)

struct list *l;
char *c;
{
void refresh_list();
char *mem;
int len;

len = my_strlen(c)+1;
mem = (char *)AllocMem(len, 0);
if (mem) {
   strncpy(mem, c, len);
   l->list_text[l->list_size] = mem;
   l->list_size++;
   }
refresh_list(l);
}

int my_strlen(c)
char *c;
{
if (c)
   return(strlen(c));
else
return(0);
}

/***********************************************************/

/**!!!!!! make colors parameters!! **/

void draw_indent(rp, x1, y1, x2, y2)

struct RastPort *rp;
int x1,x2;
int y1,y2;
{
SetAPen(rp, DARK_BROWN);
Move(rp, x2, y1);
Draw(rp, x1, y1);
Draw(rp, x1, y2);
SetAPen(rp, WHITE);
Draw(rp, x2, y2);
Draw(rp, x2, y1);

Move(rp, x2-1,y1);
Draw(rp, x2-1,y2);
SetAPen(rp, DARK_BROWN);
Move(rp, x1+1,y1);
Draw(rp, x1+1,y2-1);
}

/***********************************************************/
