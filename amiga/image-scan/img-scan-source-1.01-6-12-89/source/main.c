#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include "scan.h"

#define DEMO FALSE

struct Window *is_window;
struct Screen *is_screen;
struct config config;

void main()

{
struct IntuiMessage *message,*GetMsg();
struct Window *openwindow();
struct Screen *openscreen();
ULONG class;
USHORT code,menunum,itemnum,subnum;
int i,j;

openlibraries();
open_config();
change_screen();
show_title();

while (TRUE) {

   Wait( 1<< (is_window->UserPort->mp_SigBit));
   while( (message=GetMsg(is_window->UserPort))!=0) {

      class=message->Class;
      code=message->Code;
      ReplyMsg(message);

      menunum=MENUNUM(code);
      itemnum=ITEMNUM(code);
      subnum=SUBNUM(code);

      if (class==MENUPICK && menunum==0 && itemnum==0) {    /** load **/
         if (subnum==0) {
            load_raw();
            }
         else {
            load_iff();
            }
         }

      if (class==MENUPICK && menunum==0 && itemnum==1) {    /** save **/
#if DEMO
         telluser2(is_screen,"Save does not work with this demo.","",FALSE);
#else
         if (subnum==0) {
            save_raw();
            }
         else {
            save_iff();
            }
#endif
         }

      if (class==MENUPICK && menunum==0 && itemnum==2) {    /** about **/
         telluser2(is_screen,
                  "IMG Scan Amiga Version 1.01",
                  "Software by Anthony J. Wood",FALSE);
         }

      if (class==MENUPICK && menunum==0 && itemnum==4) {    /** quit **/
         closewindow(is_window);
         CloseScreen(is_screen);
         closelibraries();
         exit(0);
         }

      if (class==MENUPICK && menunum==1 && itemnum==0) {    /** scan **/
#if !DEMO
         newpointer(is_screen, BUSY);
         do_scan(0);
         newpointer(is_screen, NORMAL);
#endif
         }

      if (class==MENUPICK && menunum==1 && itemnum==1) {    /** cont scan **/
#if !DEMO
         newpointer(is_screen, BUSY);
         do_scan(config.actual_height);
         newpointer(is_screen, NORMAL);
#endif
         }

#if 0
      if (class==MENUPICK && menunum==1 && itemnum==2) {    /** rev scan **/
         newpointer(is_screen, BUSY);
         for (i=0; i<num_v_feeds()*config.sh; i++) {
            for (j=0; j<config.rev_feed_len; j++) {
               send_byte(config.rev_feed[j]);
               }
            }
         newpointer(is_screen, NORMAL);
         }
#endif

      if (class==MENUPICK && menunum==1 && itemnum==2) {
         edit_pic();
         }

      if (class==MENUPICK && menunum==1 && itemnum==3) {    /** show raw **/
         show_raw_data();
         }

      if (class==MENUPICK && menunum==2 && itemnum==0) {    /** screen res **/
         get_screen_size();
         change_screen();
         }

      if (class==MENUPICK && menunum==2 && itemnum==1) {    /** scan size **/
         scan_size();
         }
      }
   }
}


