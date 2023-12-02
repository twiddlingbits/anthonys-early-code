#include <exec/types.h>
#include <intuition/intuition.h>
#include <graphics/gfx.h>
#include "scan.h"

extern struct Screen *is_screen;
extern struct Window *is_window;
extern struct config config;

void change_screen()

{
struct Screen *openscreen();
struct Window *openwindow();
void refresh_screen();
short refresh;

if (is_screen)
   if (is_screen->Width==config.sw && is_screen->Height==config.sh)
      return;

if (is_screen) {
   CloseWindow(is_window);
   CloseScreen(is_screen);
   refresh=TRUE;
   }
else {
   refresh = FALSE;
   }

is_screen = openscreen(config.sw,config.sh,4);
is_window = openwindow(is_screen);

if (is_window==0) {
   closelibraries();
   exit(10);
   }

if (refresh) {
   refresh_screen();
   }
else {
   clr();   /*** clear screen **/
   }
}

void refresh_screen()

{
struct BitMap *bm;

clr();

if (config.actual_height==0)
   return;

bm = is_window->RPort->BitMap;
newpointer(is_screen, BUSY);

if (config.flags & IMG_NOLACE) {    /** raw_bug_size = 128000 **/
   if (config.sw==320) {      /** sh always 200 in IMG_NOLACE **/
      show_raw_320_400(config.rawdata, bm->Planes[0], bm->Planes[1],
              bm->Planes[2], bm->Planes[3],&config.translate[0],config.actual_height-1);
   }
   else {      /** must be 640 X 200 **/
      show_raw_640_400(config.rawdata, bm->Planes[0], bm->Planes[1],
              bm->Planes[2], bm->Planes[3],&config.translate[0],config.actual_height-1);
      }
   newpointer(is_screen, NORMAL);
   return;
   }

if (config.sw==320 && config.sh==200)
   show_raw_320_200(config.rawdata, bm->Planes[0], bm->Planes[1],
           bm->Planes[2],bm->Planes[3],&config.translate[0],config.actual_height/2-1);

else if (config.sw==320 && config.sh==400)
   show_raw_320_400(config.rawdata, bm->Planes[0], bm->Planes[1],
           bm->Planes[2], bm->Planes[3],&config.translate[0],config.actual_height-1);

else if (config.sw==640 && config.sh==200)
   show_raw_640_200(config.rawdata, bm->Planes[0], bm->Planes[1],
           bm->Planes[2], bm->Planes[3],&config.translate[0],config.actual_height/2-1);

else if (config.sw==640 && config.sh==400)
   show_raw_640_400(config.rawdata, bm->Planes[0], bm->Planes[1],
           bm->Planes[2], bm->Planes[3],&config.translate[0],config.actual_height-1);

newpointer(is_screen, NORMAL);
}

void refresh_buffer()

{
struct BitMap *bm;

bm = is_window->RPort->BitMap;
newpointer(is_screen, BUSY);

if (config.flags*IMG_NOLACE) {
   if (config.sw==320)
      pack_screen2_320_200(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);

   else
      pack_screen2_640_200(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);
   config.actual_height = 200;
   newpointer(is_screen, NORMAL);
   return;
   }

if (config.sw==320 && config.sh==200)
      pack_screen_320_200(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);

else if (config.sw==320 && config.sh==400)
      pack_screen_320_400(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);

else if (config.sw==640 && config.sh==200)
      pack_screen_640_200(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);

else if (config.sw==640 && config.sh==400)
      pack_screen_640_400(config.rawdata, bm->Planes[0], bm->Planes[1], bm->Planes[2], bm->Planes[3]);

config.actual_height = 400;
newpointer(is_screen, NORMAL);
}

clr()
{
SetAPen(is_window->RPort,4);
RectFill(is_window->RPort,0,0,is_window->Width-1,is_window->Height-1);
}
