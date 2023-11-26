#include "exec/types.h"
#include "intuition/intuition.h"
#include "libraries/dos.h"
#include "stdio.h"
#include "fcntl.h"
#include "ilbm.h"
#include "readpict.h"
#include "vid.h"

extern struct Screen *main_screen;
extern struct Screen *menu_screen;


about()

{

int i,f,cancel;
struct BitMap bitmap0;
ILBMFrame iFrame;
extern ChipAlloc();

if ((f=Open("title.pic",MODE_OLDFILE))==0) {
  if ((f=Open("PVision:Title.pic",MODE_OLDFILE))==0) {
      pop_up();
      telluser2(menu_screen,"Please put the file Title.pic",
         "in the directory with PVision.",FALSE);
      close_all();
      exit(0);
      }
   }

newpointer(menu_screen, BUSY);
i=ReadPicture(f, &bitmap0, &iFrame, ChipAlloc);
Close(f);
newpointer(menu_screen, NORMAL);

if (i!=IFF_DONE) {
   return(FALSE);
   }

i = min(200,iFrame.bmHdr.h);

BltBitMap(&bitmap0,0,0, &main_screen->BitMap,0,0,320,i, 0xC0,0xFF,0);
RemFree(bitmap0.Planes[0]);
}

