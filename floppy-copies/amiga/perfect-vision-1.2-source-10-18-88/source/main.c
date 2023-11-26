#include "exec/types.h"
#include "intuition/intuition.h"
#include "vid.h"

short fourbits = TRUE;
short lace = FALSE;
short show_mode = BLIT;        /** ASM, BLIT, NICEBLIT **/
short swap_cs_bits = FALSE;    /** causes set_cs_bits to switch **/
struct Window *menu_window;    /** when users gives 'i' flag **/
struct Window *main_window;
struct Screen *main_screen;
struct Screen *menu_screen;
struct RastPort *rp;
UBYTE  *grab_buffer;
struct Window *window;   /** HAM and 32 color window/screen ptr **/
struct Screen *screen;

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct LayersBase *LayersBase;
struct MathBase *MathBase;

void main(argc,argv)
int argc;
char *argv[];
{
void grab_frame(), show_frame(), cycle_frame(), show_main_menu();
void lace_cycle(), cycle();
int menunum;

if (argc > 1) {
   if ( *argv[1]=='i')
      swap_cs_bits=TRUE;
   else
      fourbits = FALSE;
   }

/* printf("swap_cs_bits %x\n",swap_cs_bits);
printf("fourbits %x\n",fourbits); */

setup();
about();
show_main_menu();

while(TRUE) {
   menunum=wait_menu();

   switch (menunum) {
      case 0:
         break;

      case 1:     /** Cycle **/
         cycle_menu();
         show_main_menu();
         break;

      case 2:     /** effects **/
         effects_menu();
         show_main_menu();
         break;

      case 3:
         save_menu();
         show_main_menu();
         break;

      case 4:        /** HAM **/
         ham_menu();
         show_main_menu();
         break;

      case 5:        /** Options Menu **/
         option_menu();
         show_main_menu();
         break;

      case 6:
         close_all();
         exit(0);
      }
   }
}


/**************************/
/** Set up the main menu **/
/**************************/

void show_main_menu()

{
char *t[8];

t[0] = "Cycle";
t[1] = "Effects";
t[2] = "File";
t[3] = "Color";
t[4] = "Options";
t[5] = "Quit";
t[6] = 0;

create_menu(t);
}

