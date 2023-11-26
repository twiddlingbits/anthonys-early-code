#include "exec/types.h"
#include "intuition/intuition.h"
#include "vid.h"

extern short lace;
extern short show_mode;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct Window *main_window;
extern struct Window *menu_window;
extern struct RastPort *rp;
extern UBYTE  *grab_buffer;

short red_tint = 0;
short green_tint = 0;
short blue_tint = 0;
short do_false_color = FALSE;

option_menu()
{
int menunum;

while(TRUE) {
   show_option_menu();
   menunum=wait_menu();
   switch (menunum) {
      case 0:
         break;

      case 1:
         return(0);

      case 2:
         newpointer(menu_screen,BUSY);
         toggle_lace();
         newpointer(menu_screen,NORMAL);
         break;

      case 3:
         if (lace)
            palette(main_screen,15,30,160,200);
         else
            palette(main_screen,15,15,160,100);
         break;

      case 4:

         if (do_false_color) {
            do_false_color = FALSE;
            setrp3(red_tint,green_tint,blue_tint);
            }
         else {
            do_false_color = TRUE;
            set_false();
            }
         break;

      case 5:
         if (red_tint==0)
            red_tint = 4;
         else
            red_tint = 0;
         setrp3(red_tint,green_tint,blue_tint);
         do_false_color = FALSE;
         break;

      case 6:
         if (green_tint==0)
            green_tint = 4;
         else
            green_tint = 0;
         setrp3(red_tint,green_tint,blue_tint);
         do_false_color = FALSE;
         break;

      case 7:
         if (blue_tint==0)
            blue_tint = 4;
         else
            blue_tint = 0;
         setrp3(red_tint,green_tint,blue_tint);
         do_false_color = FALSE;
         break;

      case 8:
         if (show_mode == ASM)
            show_mode = BLIT;
         else
            show_mode = ASM;

         show_option_menu();
         break;
      }
   }
}

show_option_menu()

{
char *t[10];

t[0] = "Exit";
t[1] = "Lace";
t[2] = "Palette";
if (do_false_color)
   t[3] = "Color*";
else
   t[3] = "Color";
if (red_tint)
   t[4] = "R*";
else
   t[4] = "R";
if (green_tint)
   t[5] = "G*";
else
   t[5] = "G";
if (blue_tint)
   t[6] = "B*";
else
   t[6] = "B";

if (show_mode == ASM)
   t[7] = "Slow";
else
   t[7] = "Fade";

t[8] = 0;

create_menu(t);
}


toggle_lace()
{
extern SHORT lace;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct Window *main_window;
extern struct Window *menu_window;

CloseWindow(main_window);
CloseScreen(main_screen);
main_window = 0;
main_screen = 0;

if (lace) {
   lace = FALSE;
   }
else {
   lace = TRUE;
   }

main_screen = openmain();  /** This exits program if no memory!!!!! **/
ScreenToFront(menu_screen);

if (main_screen == 0) {
   show_error(OUTOFMEM);
   close_all();
   exit(0);
   }

rp = &main_screen->RastPort;  /** Reset this global variable **/
show(grab_buffer, NORMAL, 0,FALSE);

if (do_false_color)
   set_false();
else
   setrp3(red_tint, green_tint, blue_tint);
}

