#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "stdio.h"
#include "/include/psound.h"


/***** Knob Handeling Routines ********************************/

init_knob(window,k,l, box_x1, box_y1, box_x2, box_y2, show_size, list_size, flags, apen, bpen, top)

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
if (show_size > list_size)
   k->knob_height = box_y2-box_y1+1;
else
   k->knob_height = ((box_y2-box_y1+1)*show_size)/list_size;
k->knob_y = box_y1;
k->kw = window;
k->list = l;
k->show_size = show_size;
k->list_size = list_size;
k->top_element = top;
k->APen = apen;
k->BPen = bpen;
k->flags = flags;

draw_knob(k,k->APen);
}

refresh_knob(k)

struct knob *k;
{

k->show_size=k->list->show_size;
k->list_size=k->list->list_size;
k->top_element = k->list->top_element;
if (k->show_size > k->list_size) {
   k->knob_height = k->box_y2-k->box_y1+1;
   k->knob_y = k->box_y1;
   k->top_element = 0;
   }
else {
   k->knob_height = ((k->box_y2-k->box_y1+1)*k->show_size)/k->list_size;
   }
draw_knob(k,k->APen);
}

draw_knob(k,color)
struct knob *k;
int color;
{
struct RastPort *rp;

rp = k->kw->RPort;

if (k->knob_y > k->box_y1) {
   SetAPen(rp, k->BPen);
   RectFill(rp,k->box_x1+1, k->box_y1, k->box_x2-1, k->knob_y-1);
   }

SetAPen(rp, color);
if (k->flags&KNOB_3D)
   RectFill(rp,k->box_x1+3, k->knob_y+1, k->box_x2-3, k->knob_y+k->knob_height-2);
else
   RectFill(rp,k->box_x1+1, k->knob_y, k->box_x2-1, k->knob_y+k->knob_height-1);
if (k->flags&KNOB_3D) {
   SetAPen(rp, WHITE);
   Move(rp, k->box_x2-2, k->knob_y);
   Draw(rp, k->box_x1+1, k->knob_y);
   Draw(rp, k->box_x1+1, k->knob_y+k->knob_height-1);
   Move(rp, k->box_x1+2, k->knob_y);
   Draw(rp, k->box_x1+2, k->knob_y+k->knob_height-1);

   SetAPen(rp, DARK_BROWN);
   Move(rp, k->box_x2-1, k->knob_y);
   Draw(rp, k->box_x2-1, k->knob_y+k->knob_height-1);
   Move(rp, k->box_x2-2, k->knob_y);
   Draw(rp, k->box_x2-2, k->knob_y+k->knob_height-1);
   Draw(rp, k->box_x1+2, k->knob_y+k->knob_height-1);
   }

if (k->knob_y+k->knob_height < k->box_y2) {
   SetAPen(rp, k->BPen);
   RectFill(rp,k->box_x1+1, k->knob_y+k->knob_height, k->box_x2-1, k->box_y2);
   }
}

handle_knob(k, class, code, x, y)
struct knob *k;
ULONG class;
USHORT code;
int x,y;
{
void refresh_list();
short refresh;

if (class==MOUSEBUTTONS  && code==SELECTDOWN) {
   if (hit_knob(k, x, y)) {
      drag_knob(k,y);
      }
   else {
      refresh = FALSE;
      if (hit_knob_up(k, x, y)) {
         k->top_element--;
         if (k->top_element < 0) k->top_element = 0;
         refresh = TRUE;
         }
      else if (hit_knob_down(k, x, y)) {
         k->top_element++;
         if (k->top_element >= k->list_size) k->top_element = k->list_size-1;
         refresh = TRUE;
         }
      if (refresh) {
         if (k->show_size > k->list_size)
            k->knob_y=k->box_y1;
         else
            k->knob_y=k->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_size-k->show_size)+1;
         if (k->knob_y+k->knob_height > k->box_y2)
            k->knob_y = k->box_y2 - k->knob_height+1;
         draw_knob(k,k->APen);
         if (k->list) {
            if (k->list->top_element != k->top_element) {
               k->list->top_element = k->top_element;
               refresh_list(k->list);
               }
            }
         }
      }
   }
}

hit_knob(k, x, y)

struct knob *k;
int x,y;
{
return(x>=k->box_x1 && x<=k->box_x2 && y >= k->knob_y && y <=(k->knob_y+k->knob_height));
}

hit_knob_up(k, x, y)

struct knob *k;
int x,y;
{
return(x>=k->box_x1 && x<=k->box_x2 && y >= k->box_y1 && y < k->knob_y);
}

hit_knob_down(k, x, y)

struct knob *k;
int x,y;
{
return(x>=k->box_x1 && x<=k->box_x2 && y > (k->knob_y+k->knob_height) && y <= k->box_y2);
}


drag_knob(k, y)
struct knob *k;
int y;
{
void refresh_list();
struct IntuiMessage *message, *GetMsg();
ULONG class;
USHORT code;
int newy;
short old_flag;

old_flag = k->kw->Flags&REPORTMOUSE;
ReportMouse(TRUE,k->kw);
draw_knob(k,LIGHT_BROWN);
while (TRUE) {
   Wait(1<<k->kw->UserPort->mp_SigBit);
   while ((message=GetMsg(k->kw->UserPort))!=0) {
      class=message->Class;
      code=message->Code;
      newy = message->MouseY;
      ReplyMsg(message);

      if (class==MOUSEBUTTONS  && code==SELECTUP) {
         ReportMouse(old_flag,k->kw);
         draw_knob(k,k->APen);
         if (k->show_size >= k->list_size)
            k->top_element = 0;
         else
            k->top_element = ((k->list_size-k->show_size)*(k->knob_y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);
         return;
         }
      }

   k->knob_y += (newy - y);
   if (k->knob_y < k->box_y1)
      k->knob_y = k->box_y1;
   if (k->knob_y+k->knob_height > k->box_y2)
      k->knob_y = k->box_y2 - k->knob_height+1;
   draw_knob(k,LIGHT_BROWN);
   y = newy;

   if (k->list) {
      if (k->show_size >= k->list_size)
         k->top_element = 0;
      else
         k->top_element = ((k->list_size-k->show_size)*(k->knob_y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);
      if (k->list->top_element != k->top_element) {
         k->list->top_element = k->top_element;
         refresh_list(k->list);
         }
      }
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

init_list(window,l,k, box_x, box_y, show_size, list_size, top, current,msc)
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

add_list(l, c)

struct list *l;
char *c;
{
void refresh_list();
char *mem,*AllocMem();
int len;

len = my_strlen(c)+1;
mem = AllocMem(len, 0);
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

