#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "vid.h"

#define HISTX 0
#define HISTY 0
#define HISTH 50
#define HISTW 136    /** Should be divisable by 17 for best results **/

extern short lace;
extern short show_mode;
extern struct Window *menu_window;
extern struct Screen *main_screen;
extern struct Screen *menu_screen;
extern struct RastPort *rp;


/*** Puts nyble backed data onto the screen.
   data == array of data to put on screen
   size == NORMAL for full screen or QUARTER to put only 1/4 of picture
   bm   == 0 if we should use Assembly routines to display data,
        == pointer to bitmap if should blit. See cycle_menu for calling
        == sequence. ***/


show (data, size, bm, hist)

UBYTE *data;
int size,hist;
struct BitMap *bm;

{
extern short fourbits;
static short off=0;
short hx,hy;

if (!fourbits) {
   if (lace) {
      big_lace_show(data,
              rp->BitMap->Planes[0],
              rp->BitMap->Planes[1],
              rp->BitMap->Planes[2],
              rp->BitMap->Planes[3]);
      }
   else {
      big_show_frame(data,
         rp->BitMap->Planes[0],
         rp->BitMap->Planes[1],
         rp->BitMap->Planes[2],
         rp->BitMap->Planes[3]);
      }
      return;
   }


if (bm && size == NORMAL) {
   BlitScreen(bm, data);
   if (hist) {
      if (lace) {
         do_hist(rp,HISTX,HISTY,HISTH*2,HISTW,data,FALSE);
         }
      else {
         do_hist(rp,HISTX,HISTY,HISTH,HISTW,data,FALSE);
         }
      }
   return;
   }

if (lace) {
   if (size == NORMAL) {
      lace_show(data,
              rp->BitMap->Planes[0],
              rp->BitMap->Planes[1],
              rp->BitMap->Planes[2],
              rp->BitMap->Planes[3]);
      if (hist) do_hist(rp,HISTX,HISTY,HISTH*2,HISTW,data,FALSE);
         }
   else {
      ql_show(data,
           rp->BitMap->Planes[0]+off,
           rp->BitMap->Planes[1]+off,
           rp->BitMap->Planes[2]+off,
           rp->BitMap->Planes[3]+off);

      hx = 0;
      hy = 0;
      if (off == 0) {
         off = 20;
         }
      else if (off == 20) {
         hx = 160;
         off = 8000;
         }
      else if (off == 8000) {
         hy = 200;
         off = 8020;
         }
      else {
         off = 0;
         hx = 160;
         hy = 200;
         }
      if (hist) do_hist(rp,hx,hy,HISTH,HISTW/2,data,FALSE);
      }
   return;
   }

if (size == NORMAL) {
   show_frame(data,
      rp->BitMap->Planes[0],
      rp->BitMap->Planes[1],
      rp->BitMap->Planes[2],
      rp->BitMap->Planes[3]);
   if (hist) do_hist(rp,HISTX,HISTY,HISTH,HISTW,data,FALSE);
   }
else {
   quarter_show(data,
        rp->BitMap->Planes[0]+off,
        rp->BitMap->Planes[1]+off,
        rp->BitMap->Planes[2]+off,
        rp->BitMap->Planes[3]+off);

      hx = 0;
      hy = 0;
   if (off == 0) {
      off = 20;
      }
   else if (off == 20) {
      hx = 160;
      off = 4000;
      }
   else if (off == 4000) {
      hy = 100;
      off = 4020;
      }
   else {
      off = 0;
      hx = 160;
      hy = 100;
      }
   if (hist) do_hist(rp,hx,hy,HISTH/2,HISTW/2,data,FALSE);
   }
}

packit(data)
UBYTE *data;
{
if (lace) {
  lpack_screen(data,
           rp->BitMap->Planes[0],
           rp->BitMap->Planes[1],
           rp->BitMap->Planes[2],
           rp->BitMap->Planes[3]);
}
else {
  pack_screen(data,
           rp->BitMap->Planes[0],
           rp->BitMap->Planes[1],
           rp->BitMap->Planes[2],
           rp->BitMap->Planes[3]);
   }
}
