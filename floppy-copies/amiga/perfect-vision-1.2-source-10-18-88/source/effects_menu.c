#include "exec/types.h"
#include "intuition/intuition.h"
#include "vid.h"

extern short lace;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;
static UBYTE *new_buffer;
static short thresh=7;


UBYTE (*pf)();

effects_menu()
{
int menunum,i;
UBYTE edge_kern(),sharp_kern(),edge_v_kern(),edge_h_kern();
UBYTE avg(),slice();
int pix(),lace_pix();
void do_kern();

new_buffer = AllocMem(64000,0);
if (new_buffer==0) {
   show_error(OUTOFMEM);
   return;
   }

if (lace) {
   pf = lace_pix;
   }
else {
   pf = pix;
   }


show_effects_menu();
grabtonew();

while(TRUE) {
   menunum=wait_menu();
   switch (menunum) {
      case 0:
            break;

      case 1:
         newtograb();
         FreeMem(new_buffer,64000);
         return(0);

      case 2:
         edge_menu();
         show_effects_menu();
         break;

      case 3:
         newpointer(menu_screen,BUSY);
         newtograb();
         do_kern(sharp_kern);
         newpointer(menu_screen,NORMAL);
         break;

      case 4:
         newpointer(menu_screen,BUSY);
         newtograb();
         do_kern(avg);
         newpointer(menu_screen,NORMAL);
         break;

      case 5:
         pop_up();
         thresh=getint(menu_screen,thresh,0,15,"Slice Threshold",170);
         pop_down();
         newpointer(menu_screen,BUSY);
         newtograb();
         do_kern(slice);
         newpointer(menu_screen,NORMAL);
         break;

      case 6:
         do_hist(rp,10,17,140,300,new_buffer,TRUE);
         show_exit_menu();
         while (check_menu()!=1) ;
         newpointer(menu_screen,BUSY);
         show(new_buffer,NORMAL,0,FALSE);
         show_effects_menu();
         newpointer(menu_screen,NORMAL);
         break;

      case 7:        /*** UNDO ***/
         newpointer(menu_screen,BUSY);
         grabtonew();
         show(grab_buffer,NORMAL,0,FALSE);
         newpointer(menu_screen,NORMAL);
         break;
      }
   }
}


show_edge_menu()
{
char *t[10];

t[0] = "Exit";
t[1] = "Horiz";
t[2] = "Vert";
t[3] = "All";
t[4] = 0;

create_menu(t);
}

show_exit_menu()
{
char *t[10];

t[0] = "Exit";
t[1] = 0;

create_menu(t);
}

show_effects_menu()

{
char *t[10];

t[0] = "Exit";
t[1] = "Edge";
t[2] = "Sharp";
t[3] = "Avg";
t[4] = "Slice";
t[5] = "Hist";
t[6] = "Undo";
t[7] = 0;

create_menu(t);
}


void do_kern(fun)
UBYTE (*fun)();
{
int x,y,i;
UBYTE z;

if (lace) {
   i=0;
   for (y=0; y < 400; y=y+2) {
      for (x=0; x<319; x=x+2) {
         z = ((*fun)(x,y))<<4;
         new_buffer[i] = z | ((*fun)(x+1,y));
         i++;
         }
      }
   for (y=1; y < 400; y=y+2) {
      for (x=0; x<319; x=x+2) {
         z = ((*fun)(x,y))<<4;
         new_buffer[i] = z | ((*fun)(x+1,y));
         i++;
         }
      }
   }

else {
   i=0;
   for (y=0; y < 200; y++) {
      for (x=0; x<319; x=x+2) {
         z = ((*fun)(x,y))<<4;
         new_buffer[i] = z | ((*fun)(x+1,y));
         i++;
         }
      }
   }
show(new_buffer,NORMAL,0,FALSE);
}

UBYTE sharp_kern(x,y)
int x,y;
{
int z;

z = ((*pf)(grab_buffer,x,y)<<3) + (*pf)(grab_buffer,x,y);
z = z - (*pf)(grab_buffer,x-1,y-1) -
        (*pf)(grab_buffer,x,y-1) -
        (*pf)(grab_buffer,x+1,y-1) -

        (*pf)(grab_buffer,x-1,y) -
        (*pf)(grab_buffer,x+1,y) -

        (*pf)(grab_buffer,x-1,y+1) -
        (*pf)(grab_buffer,x,y+1) -
        (*pf)(grab_buffer,x+1,y+1);

if (z<0) z=0;
if (z>15) z = 15;

return(z);
}

UBYTE edge_kern(x,y)
int x,y;
{
int z;

z = ((*pf)(grab_buffer,x,y)<<3);
z = z - (*pf)(grab_buffer,x-1,y-1) -
        (*pf)(grab_buffer,x,y-1) -
        (*pf)(grab_buffer,x+1,y-1) -

        (*pf)(grab_buffer,x-1,y) -
        (*pf)(grab_buffer,x+1,y) -

        (*pf)(grab_buffer,x-1,y+1) -
        (*pf)(grab_buffer,x,y+1) -
        (*pf)(grab_buffer,x+1,y+1);

if (z<0) z=0;
if (z>15) z = 15;

return(z);
}

UBYTE edge_h_kern(x,y)
int x,y;
{
int z;

z =    - (*pf)(grab_buffer,x-1,y-1) -
        (*pf)(grab_buffer,x,y-1) -
        (*pf)(grab_buffer,x+1,y-1) +


        (*pf)(grab_buffer,x-1,y+1) +
        (*pf)(grab_buffer,x,y+1) +
        (*pf)(grab_buffer,x+1,y+1);

if (z<0) z=0;
if (z>15) z = 15;

return(z);
}

UBYTE edge_v_kern(x,y)
int x,y;
{
int z;

z =     -(*pf)(grab_buffer,x-1,y-1) +
        (*pf)(grab_buffer,x+1,y-1) -

        (*pf)(grab_buffer,x-1,y) +
        (*pf)(grab_buffer,x+1,y) -

        (*pf)(grab_buffer,x-1,y+1) +
        (*pf)(grab_buffer,x+1,y+1);

if (z<0) z=0;
if (z>15) z = 15;

return(z);
}

UBYTE avg(x,y)
int x,y;
{
int z;

z   =   (*pf)(grab_buffer,x-1,y-1) +
        (*pf)(grab_buffer,x,y-1) +
        (*pf)(grab_buffer,x+1,y-1) +

        (*pf)(grab_buffer,x-1,y) +
        (*pf)(grab_buffer,x+1,y) +

        (*pf)(grab_buffer,x-1,y+1) +
        (*pf)(grab_buffer,x,y+1) +
        (*pf)(grab_buffer,x+1,y+1);
z = z>>3;

return(z);
}

UBYTE slice(x,y)
int x,y;
{
int z;

if ((*pf)(grab_buffer,x,y) > thresh)
   return(15);
else
   return(0);
}

rp_to_buf()
{
int x,y,i;
unsigned short val;
packit(grab_buffer);
}

/******************************************************************/
/** Draws a histogram, given rastport, position and hight/widht **/
/******************************************************************/

do_hist(rp,x,y,h,w,data,do_label)

UBYTE *data;

{
char *numlab = "BK  2   4   6   8   10  12    WT";
UWORD k[16],maxval,i;
int bar,xpos,this_height;

histogram(data, k);

maxval = 0;
for (i=0; i<16; i++)
   if (k[i] > maxval)
      maxval = k[i];


SetDrMd(rp,JAM1);
SetAPen(rp,15);         /** white **/

if (do_label) i=10;
else i = 1;

RectFill(rp, x, y, x+w-1, y+h+i-1);

bar = w/34;

SetAPen(rp, 0);      /** Black **/

for (i=0; i<16; i++) {
   xpos = x+(w*(i+1))/17;
   this_height = k[i]*h/(maxval+1);
   if (do_label) {
      if (i>0 && i<10)
         Move(rp, xpos-4, y+h+9);
      else 
         Move(rp, xpos-8, y+h+9);
      Text(rp, &numlab[i<<1],2);
      }

/* printf("xpos %d bar %d y %d h %d this_height %d\n",
   xpos,bar,y,h,this_height); */

   RectFill(rp, xpos-bar+1, y+h-this_height, xpos+bar-1, y+h);
   }
}

edge_menu()
{

show_edge_menu();

while (TRUE) {
   switch (check_menu()) {
      case 0:
         break;

      case 1:
         return;

      case 2:
         newtograb();
         newpointer(menu_screen,BUSY);
         do_kern(edge_h_kern);
         newpointer(menu_screen,NORMAL);
         break;

      case 3:
         newtograb();
         newpointer(menu_screen,BUSY);
         do_kern(edge_v_kern);
         newpointer(menu_screen,NORMAL);
         break;

      case 4:
         newtograb();
         newpointer(menu_screen,BUSY);
         do_kern(edge_kern);
         newpointer(menu_screen,NORMAL);
         break;
      }
   }
}

newtograb()
{
movmem(new_buffer,grab_buffer,64000);
}

grabtonew()
{
movmem(grab_buffer,new_buffer,64000);
}


#if 0

cpix(data,x,y)
UBYTE *data;
{
short val,val0,val1,top,bot;

val = data[y*160+(x/2)];
val0 = data[y*160+(x/2)-1];
val1 = data[y*160+(x/2)+1];

top = data[y*160+(x/2)-160];
bot = data[y*160+(x/2)+160];

/* printf("at %d %d val = %x %x %x tb = %x %x\n",x,y,val0,val,val1,top,bot);
*/
return(val);
}
#endif
