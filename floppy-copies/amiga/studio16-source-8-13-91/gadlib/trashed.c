clude "exec/types.h"�#include "exec/exec.h" �8 #include "intuition/intuition.h"   �T#include "stdio.h
#include "/include/studio16.h" ��#include "/include/gadlib.h"   ��    ��void draw_knob();  ��void drag_knob();  
d draw_indent();�void refresh_knob();   �,void calc_knob_size(); �H    �d@/***** Knob Handelin
nes ********************************/   �l    ��struct knob *create_knob(nk)   ��struct NewKnob *
{  ��struct knob *k;�     �=k = (struct knob *)AllocMem(sizeof(struct knob), MEMF_CLEAR);  �	if (k==0)  �`   return(0);  �p    ��k->box_x1=nk->box_x1;  ��k->box_y1=nk->box_y1;  ��k->box_x2=nk->box_x
k->box_y2=nk->box_y2;  ��k->flags = nk->flags;  ��k->kw = nk->window;�    �0if (k->flags&BORDER_3D) {  �8   k->box_x1 += 2; �X   k->box_x2 -= 2; �p   k->box_y1 += 1; ��   
2 -= 1; ��   }   ��    ��k->show_size = nk->show_size;  ��k->list_size = nk->list_size;  ��k->top_element =
p;  �k->APen = nk->apen;�4k->BPen = nk->bpen;�Lk->func = nk->func;�d;k->CPen = LIGHT_BROWN;     /**
 when selected color **/�|calc_knob_size(k); ��if (!(k->flags&AUTO_KNOB)) {   ��   if (nk
_height!=-1)��%      k->knob_height=nk->knob_height;  �   if (nk->knob_width!=-1) �D#     
ob_width=nk->knob_width;�d   }   ��    ��refresh_knob(k);   ��
return(k); ��}  �� /**��struc
 *create_button(nb) ��struct NewButton *nb;���{  �struct knob *k;�     �4=k = (stru
b *)AllocMem(sizeof(struct knob), MEMF_CLEAR);  �<	if (k==0)  ��   return(0);  ��    ��k->box_x1=nb->
1;  ��k-k->box_y1=nk->box_y1;�nk->box_y2;  ��k->flags = nk->flags;  ��k->kw = nk->window;�                
it_knob(window,k,l, box_x1, box_y1, box_x2, box_y2, show_size, list_size, flags, apen, bpen, top)             
uct Window *window;     struct knob *k;    struct list *l;    int box_x1,box_y1;     int box_x2,box_y2;     i
w_size, list_size;      int flags,top;     int apen, bpen;    {      k->box_x1=box_x1;      k->box_y1=box_

box_x2=box_x2;      k->box_y2=box_y2;      if (show_size > l
ze)     $   k->knob_height = box_y2-box_y1+1;       else       <   k->knob_height = ((box_y2-box_y1+1)*s
e)/list_size;       k->knob_y = box_y1;    k->kw = window;    k->list = l;       k->show_size = show_size;      k->li
e = list_size;      k->top_element = top;      k->APen = apen;    k->BPen = bpen;    k->flags = flags;              d
ob(k,k->APen);      }              void refresh_knob(k)               struct knob *k;    {              i
list) {     #   k->show_size=k->list->show_size;    #   k->lis
=k->list->list_size;    )   k->top_element = k->list->top_element;         calc_knob_size(k);      


APen(k->kw->RPort, k->BPen);    CRectFill(k->kw->RPort, k->box_x1, k->box_y1, k->box_x2, k->box_y2);            if 
s&BORDER_3D) {      Q   draw_indent(k->kw->RPort, k->box_x1-2, k->box_y1-1, k->box_x2+2, k->box_y2+1); 


w_knob(k,k->APen);      if (k->func)          (*k->func)(k);      }              8/** calculat
width, knob_height, knob_x, knob_y */               void calc_knob_size(k)     strructtnob *k


flags&FREE_VERT) {      %   if (k->show_size > k->list_size) {

->knob_height = k->box_y2-k->box_y1+1;            k->knob_y = k->box_y1;             k->top_element = 0; 

    	   else {      K      k->knob_height = ((k->box_y2-k->box_y1+1)*k->show_size)/k->list_size;    r      k->knob_y=k->box_y
box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_
->show_size)+1;           }    E   if (!(k->flags&FREE_HORIZ)) {  /** only moving in one dimension?*/     
   k->knob_x = k->box_x1+1;     
->knob_width = k->box_x2-k->box_x1-1;             }       }       else if (k->flags&FREE_HORIZ) {    %  
k->show_size > k->list_size) {      ,   
ob_width = k->box_x2-k->box_x1+1;             k->knob_x = k->box_x1;             k->top_element = 0;       
    	   else {      J      k->knob_width = ((k->box_x2-k->box_x1+1)*k->show_size)/k->list_size;     r      k->knob_x =k->box_
>box_x2 - k->box_x1-k->knob_width+1)*k->top_e
/(k->list_size-k->show_size)+1;         

!(k->flags&FREE_VERT)) {  /** only movin
 dimension?*/             k->knob_y = k->box_y1+1;     -      k->knob_height = k->box_y2-k->box_y1-1

       }       }              void draw_knob(k,color)    struct knob *k;    
int color;     {      struct RastPort *rp;


>kw->RPort;             3if (k->knob_y > k->b
&& k->flags&FREE_VERT) {       
rp, k->BPen);       M   RectFill(rp,k->k
->box_y1, k->knob_x+k->knob_width-1, k->knob_y-1);         }               3if (k->knob_x > k->box_x1 && k
gs&FREE_HORIZ) {       SetAPen(rp, k->BPen);       N   RectFill(rp,k->box_x1, k->knob_y, k->knob
->knob_y+k->knob_height-1);        }               SetAPen(rp, color);    if (k->flags&KNOB_3D)      `  
Fill(rp,k->knob_x+2, k->knob_y+1, k->knob_x+k->knob_width-3, k->knob_y+k->knob_height-2);       else       \   Rect
k->knob_x, k->knob_y, k->knob_x+k->knob_widt
knob_y+k->knob_height-1);       if (k->flags&KNOB_3D) {     
Pen(rp, WHITE);     2   Move(rp, k->knob_x+k->knob_width-2, k->kn

Draw(rp, k->knob_x, k->knob_y); 
Draw(rp, k->knob_x, k->knob_y+k-
ight-1);    $   Move(rp, k->knob_x+1, k->knob_y);       5   Draw(rp, k->knob_x+1, k->knob_y+k->knob_h
);                 SetAPen(rp, DARK_BROWN);    2
(rp, k->knob_x+k->knob_width-1, k->knob_y);     C   Draw(rp, k->knob_x+k->knob_width-1, k->knob_y+k->
ight-1);    2   Move(rp, k->knob_x+k->knob_width-2, k->knob_y);     C   Draw(rp, k->knob_x+k->knob_wi
k->knob_y+k->knob_height-1);    
(rp, k->knob_x+1, k->knob_y+k->knob_height-1);         }               Bif (k->knob_y+k->knob_height <= k->box_y2 && k
gs&FREE_VERT) {        SetAPen(rp, k->BPen);       3   RectFill(rp,k->knob_x, k->knob_y+k->knob_
    2            k->knob_x+k->knob_width-1, 
2);        }               Bif (k->knob_x+k->knob_width <= k->box_x2 && k->flags&FREE_HORIZ) {        SetAPen(rp, k->BP

RectFill(rp,k->knob_x+k->knob_wi
knob_y,     3            k->box_x2, k->knob_y+k->knob_height-1




d handle_knob(k, class, code, x, y)     struct knob *k;    ULONG class;       USHORT code;       int 


d refresh_list();       short refresh;             /if (class==MOUSEBUTTONS  && code==SELECTDOWN) {    
hit_knob(k, x, y)) {          drag_knob
          return;            }               if (k->flag
E_VERT) {             refresh = FALSE;     !      if (hit_knob_up(k,
 {               k->top_element--;     4
  if (k->top_element < 0) k->top
 = 0;                refresh = TRUE;       
         }     #      if (hit_knob_down(k, x, y)) {             k->top
++;     M         if (k->top_element >= k->list_size) k->top_element = k->list_size-1;               refresh = TRUE

  }           if (refresh) {   
      if (k->show_size > k->list

         k->knob_y=k->box_y1;   
      else      x            k->
->box_y1+((k->box_y2 - k->box_y1-k->knob_height+1)*k->top_element)/(k->list_size-k->show_size)+1;       1  
 if (k->knob_y+k->knob_height > 
2)      5            k->knob_y = k->box_y2 - k->knob_height+1;               draw_knob(k,k->APen);              if (k->
    9            if (k->list->top_element != k->top_element) {      5               k->list->top_element = k->top_element; 

        refresh_list(k->list);  
           }                }  
      if (k->func)             
->func)(k);     	        }            return;            }       if (k->flags&FREE_HORIZ) {            
sh = FALSE;     #      if (hit_knob_left(k, x, y)) {             k->top_element--;     4         if 
_element < 0) k->top_element = 0;                refresh = TRUE;       
         }     $      if (hit_knob_righ
y)) {                k->top_ele

      if (k->top_element >= k->l
) k->top_element = k->list_size-1;               refresh = TRUE;       
         }           if (re
{       )         if (k->show_si
list_size)                   k->knob_x=k->box_x1;            
se      w            k->knob_x=k->box_x1+((k->box_x2 - k->box_x1-k->knob_width+1)*k->top_element)/(k->list_s
how_size)+1;    1         if (k->knob_x+k->knob_width > k->box_x2)      4            k->knob_x = k->box_x2 - k->kno
th+1;                draw_knob(k,k->APen);              if (k->list) {    9            if (k->list
ement != k->top_element) {      5               k->list->top_element = k->top_element;      %               
list(k->list);                    }                }               if (k->func)            
 (*k->func)(k);     
         } 
   }       }       }          

 hit_knob(k, x, y)              struct knob *k;    int x,y;       {      ireturn(x>=k->knob_x && x< (k->knob_x+k-
dth) && y >= k->knob_y && y < (k->knob_y+k->knob_height));      }              hit_knob_up(k, x, y)    

uct knob *k;    int x,y;       {      Hreturn(x>=k->box_x1 && x<=k->box_x2 && y >= k->box_y1 && y < k->knob_y); 



b_down(k, x, y)             struct knob *k;    int x,y;       {      Yreturn(x>=k->box_x1 && x<=k->box_x2 &&
(k->knob_y+k->knob_height) && y 
x_y2);      }              hit_knob_left(k, x, y)             struct knob *k;    int x,y;       {      Hre
y>=k->box_y1 && y<=k->box_y2 && x >= k->
& x < k->knob_x);       }            
_knob_right(k, x, y)            struct knob *k;    int x,y;       {      Xreturn(y>=k->box_y1 && y<=k->box_y2 && 
knob_x+k->knob_width) && x <= k->box_x2);       }            
d drag_knob(k, x, y)    struct knob *k;    int y;     int x;     {      void refresh_list();       (
IntuiMessage *message, *GetMsg()

lass;       USHORT code;       int newy, newx;    short old_flag;    int old_top;       int ol
b_y;    int old_knob_x;            $old_flag = k->kw->Flags&REPORTMOUSE;       old_top = k->top_element;     
ortMouse(TRUE,k->kw);       Eif (k->CPen!=k->APen) { /** if highlight color is != normal color **/         draw
k->CPen);          if (k->func)          (
)(k);          }       while (TRUE) {     '   Wait(1<<k->kw->UserPort->mp_SigBit);    1   while (
age=GetMsg(k->kw->UserPort))!=0) {            class=message->Class;          code=message->Code;            newy
ssage->MouseY;            newx = message->MouseX;            ReplyMsg(message);               3    
lass==MOUSEBUTTONS  && code==SELECTUP) {    %         ReportMouse(old_flag,k->kw);      N         i
>CPen!=k->APen) { /** if highlight color is !
l color **/     G            draw_knob(k,k->APen);   /** restore knob to correct color*/        
 if (k->func)                  
>func)(k);                  }              
;       
         }           }               if (class==MOUSEMOVE) {           if (k->flags&FREE_VERT) {  
      old_knob_y = k->knob_y;   
      k->knob_y += (newy - y);  
      if (k->knob_y < k->box_y1)
         k->knob_y = k->box_y1; 
      if (k->knob_y+k->knob_heig
box_y2)     5            k->knob_y = k->box_y2 - k->knob_heigh


      SetBPen(k->kw->RPort, k->B

      if (k->knob_y < old_knob_y
* moving up **/     ?            ScrollRas
>kw->RPort, 0, old_knob_y-k->knob_y,    M    
    k->box_x1, k->knob_y, k->box
_knob_y+k->knob_height-1);                   }              else {    ?            ScrollRaster(k-
rt, 0, old_knob_y-k->knob_y,    M               k->box_x1, old_knob_y, k->box_x2, k->knob_y+k->knob_height-1); 

     }               y = newy; 

      old_top = k->top_element; 
      if (k->show_size >= k->lis

         k->top_element = 0;    
  else      x            k->top_
= ((k->list_size-k->show_size)*(
y-k->box_y1))/(k->box_y2-k->box_y1-k->knob_height+1);                        if (k->list) {   
         if (k->list->top_elemen
top_element) {      5               k->list->top_element = k->
ement;      %               refresh_list(k->list);                     }                   }              
  if (old_top!=k->top_element) {
         if (k->func)          
    (*k->func)(k);             

      }     %      else if (k->f
E_HORIZ) {                old_knob_x = k->knob_x;       !         k->
+= (newx - x);      #         if
b_x < k->box_x1)    "            k->knob_x = k->box_x1;     1         if (k->knob_x+k->knob_width > k->box_x2) 

     k->knob_x = k->box_x2 - k->
th+1;               (         SetBPen(k->kw->RPort, k->BPen);       <         if (k->knob_x < old_kn
  /** moving left **/       ?   
 ScrollRaster(k->kw->RPort, old_
->knob_x, 0,    ]               k->knob_x, k->knob_y, old_knob_x+k->knob_width-1, k->knob_y+k->knob_height-

          }              else {
         ScrollRaster(k->kw->RPo
knob_x-k->knob_x, 0,    ]               old_knob_x, k->knob_y, k->knob_x+k->knob_width-1, k->kn
k->knob_height-1);                  }     
      x = newx;             "   
d_top = k->top_element;     *         if (k->show_size >= k->list_size)                 k->top_element = 0;
      else      w            k->
ent = ((k->list_size-k->show_size)*(k->knob_x-k->box_x1))/(k->box_x2-k->box_x1-k->knob_width+1);            
  if (k->list) {    9           
list->top_element != k->top_element) {      5               k-
>top_element = k->top_element;      %        
refresh_list(k->list);         
    }                   }      

  if (old_top!=k->top_element) {
         if (k->func)          
    (*k->func)(k);             

      }           }       }   


*** List Handeling Routines **************
************/                       !handl
(l, class, code, x, y)              struct list *l;    ULONG class;       USHORT code;       int x,y;       {  
d refresh_list();       int select;    int return_code;       int select_len;    return_code = NO_COD


knob) {     +   handle_knob(l->knob, class, code, x, y);    M 
l->top_element!=l->knob->top_element) {/
 isnt necessary ??/ */      ,      l->top_element = l->knob->top_element;             refresh_list(l);           



(class==MOUSEBUTTONS  && code==S
N) {       select = (y-l->box_y-2)/9;      #   if (select >= 0 && select < 7) {    4  
elect_len =  my_strlen(l->list_text[select]);       )      if (select_len > l->max_show_chars)      ,         selec
 = l->max_show_chars - 1;       9      if (x > l->box_x && x < l->box_x+10+8*select_len) {      *         sele
elect + l->top_element;     %         if (l->current != select ) {                   l->current = select;         
     refresh_list(l);       %   
 return_code = NEW_SELECT;      
     }               else {    
     return_code = DOUBLE_SELECT

     }      
         }        


urn(return_code);       }              Pvoid init_list(window,l,k, box_x, box_y, show_size, list_size, top, current

uct Window *window;     struct list *l;    struct knob *k;    int box_x, box_y;      int list_size;     int s
ze;     int top;       int current;       int msc;       {      l->box_x = box_x;      l->box_y = b

show_size = show_size;      l->list_size = list_size;      l
element = top;      l->current = current;      l->knob = k;       l->lw = window;    l->APe
HITE;       l->BPen = BROWN;       l->max_show_chars = msc;       }              vo
fresh_list(l)       struct list *l;    {      int i;     static int max_size=0;     int len;    
uct RastPort *rp;               rp = l->lw->RPort;             SetAPen(rp, l->APen);      SetBPen(rp, l->BPen);

(rp, JAM2);              for (i=0; i < l->list_size; i++)       1   if ((len=my_strlen(l->list_text[
_size)            max_size = len;              @for (i=l->top_element; i < (l->top_element+l->show_size); i++) {       
i==l->current) {          SetAPen(rp, l
          SetBPen(rp, l->APen);          }    7   Move(rp
box_x, l->box_y+(i-l->top_element)*9+8);       if (i < l->lis
) {     %      len=my_strlen(l->list_text[i]);      "      if (len > l->max_show_chars)     !        
l->max_show_chars;      %      T
l->list_text[i], len);         

          len = 0;        if (i==l->current) {          SetAPen(rp, l->APen);          SetBPen(rp
en);          }    0   len = min(max_size, l->max_show_chars) - len;       a   
 "                                          
                           ",len




d add_list(l, c)            struct list *l;    char *c;       {      void refresh_list();       
char

 len;               len = my_strlen(c)+1;      mem = (char *)AllocMem(len, 0);    
if (mem) {        strncpy(mem,
;       $   l->list_text[l->list_size] = 

l->list_size++;        }       
_list(l);       }              int my_strlen(c)       char *c;       {      if (c)        return(s
);      else       
return(0);     }   

******************************************
*********/              &/**!!!!!! make co
arameters!! **/             $void draw_indent(rp, x1, y1, x2, y2)               struct RastPort *rp;       
int

 y1,y2;     {      SetAPen(rp, DARK_BROWN);       Move(rp, x2, y1);      Draw(rp, x1, y1);      Draw(rp, x1, y2

(rp, WHITE);    Draw(rp, x2, y2);      Draw(rp, x2, y1);              Move(rp, x2-1,y1);     Draw(rp, x2-1

APen(rp, DARK_BROWN);       Move(rp, x1+1,y1);     Draw(rp, x1+1,y2-1);       }              =/******************
*********************************/  ******
