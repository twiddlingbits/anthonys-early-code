#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "vid.h"

extern short fourbits;
extern short lace;
extern short show_mode;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;

static short show_hist = FALSE;

UBYTE bug0,bug1;
UBYTE lbug0,lbug1;

cycle_menu()
{
static short show_size = NORMAL;
short menunum;
ULONG i;
struct BitMap bm;
struct BitMap *bmp;

show_cycle_menu();
if (pack_grab(grab_buffer)==0) { /** Timed out waiting for sync **/
   pop_up();
   telluser2(menu_screen,"No Video Signal","Present.",FALSE);
   pop_down();
   return(0);
   }

pack_grab(grab_buffer);    /** Remove old frame garbage **/

bmp = 0;

if (show_mode != ASM) {
   bmp = &bm;
   bm = *rp->BitMap;
   i=InitBS();              /** Allocate blitter memory **/
   if (i!=0) {
      FiniBS();
      show_error(OUTOFMEM);
      return(0);
      }
   }

while(TRUE) {

   Delay(30);

   bug0=grab_buffer[0];          /** Save these bytes to allow bugfix **/
   bug1=grab_buffer[1];          /** to fix the blitscreen bug **/
   lbug0=grab_buffer[32000];
   lbug1=grab_buffer[32001];

   if (fourbits) {
      pack_grab(grab_buffer);
      pack_grab(grab_buffer+32000);
      }
   else {
      grab_frame(grab_buffer);
      grab_frame(grab_buffer+64000);
      }

/*   show (grab_buffer, show_size, bmp, show_hist); */

   menunum=check_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:     /** Stop **/

         newpointer(menu_screen,BUSY);
         if (show_mode!=ASM) {
            Delay(90);        /** Give Blits a chance to finish **/
            FiniBS();
            }
                     /*** Restore current screen into buffer ***/

         if (show_size==NORMAL) {
            if (fourbits)
               packit(grab_buffer);    /** see show.c **/
            fixbug();
            }
         newpointer(menu_screen,NORMAL);
         return(0);

      case 2:     /** One **/
         show_size = NORMAL;
         bm.BytesPerRow = 40;
         break;

      case 3:     /** Four **/

         show_size = QUARTER;
         bm.BytesPerRow = 20;
         break;

      case 4:     /** Grab a sequence of pictures **/
         record(bmp);
         show_cycle_menu();
         break;

      case 5:     /** Toggle Histogram on/off **/
         show_hist = ~(show_hist);
         show_cycle_menu();
         break;
      }
   show (grab_buffer, show_size, bmp, show_hist);
   }
}


show_cycle_menu()

{
char *t[10];

t[0] = "Exit";
t[1] = "One";
t[2] = "Four";
t[3] = "Record";
if (show_hist)
   t[4] = "Histogram*";
else
   t[4] = "Histogram";
t[5] = 0;

create_menu(t);
}

/** Hardware has bug that causes 1st pixel to always be zero **/
/** This attempts to hide bug by copying 2nd pixel to 1st on exit **/

fixbug()
{
short y,lines;
int off=0;
UBYTE x;

grab_buffer[0] = bug0;
grab_buffer[1] = bug1;

SetAPen(rp, bug0&15);                  /** Blitscreen has a bug that **/
WritePixel(rp,1,0);                    /** causes 1st 4 pixs not to show */

SetAPen(rp, bug1>>4);
WritePixel(rp,2,0);

SetAPen(rp, bug1&15);
WritePixel(rp,3,0);

if (lace) {

grab_buffer[32000] = lbug0;
grab_buffer[32001] = lbug1;

SetAPen(rp, lbug0&15);                  /** Blitscreen has a bug that **/
WritePixel(rp,1,1);                    /** causes 1st 4 pixs not to show */

SetAPen(rp, lbug1>>4);
WritePixel(rp,2,1);

SetAPen(rp, lbug1&15);
WritePixel(rp,3,1);
}

if (lace)
   lines = 400;
else
   lines = 200;

for (y=0; y<lines; y++) {
   SetAPen(rp, ReadPixel(rp,1,y));
   WritePixel(rp,0,y);
   }

for (y=0; y<400; y++) {
   x = grab_buffer[off];
   x = x<<4;
   grab_buffer[off] = x|(x>>4);
   off=off+160;
   }
}

