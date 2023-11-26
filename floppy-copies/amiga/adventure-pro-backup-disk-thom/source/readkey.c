

#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "adventure.h"


main()
{

struct IntuiMessage *mess;
LONG mclass;
USHORT mcode[2],mqual;
SHORT Mx,My;

struct Window *win1;
struct Screen *screen;

openlibs();
screen = openscreen(320,200,1);
win1 = nwwin(screen,0,0,320,200);

ReportMouse(TRUE,win1);
ModifyIDCMP(win1, RAWKEY);

do {

   Wait(1<<win1->UserPort->mp_SigBit);
   while(mess = GetMsg(win1->UserPort)) {

      mclass = mess->Class;
      mcode[0] = mess->Code;
      mqual = mess->Qualifier;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == RAWKEY) {
         printf("> %c : %d/%d <\n",mcode[0],mcode[0],mqual);
         if (mcode[0] == 10 || mcode[0] == 13) {
            CloseWindow(win1);
            CloseScreen(screen);
            goto ret1;
            }
         }

      }
   } while(TRUE) ;
   ret1:
   ReportMouse(FALSE,win1);

}



