#include "exec/types.h"
#include "intuition/intution.h"

unsigned short *bitmap;

main()
{
ULONG class;
USHORT code;
SHORT mousex,mousey;
struct Window *w;
openscr();
w=openwin();
bitmap = screen->RastPort.BitMap.Planes[0];

/** Open three test windows **/

w1=openwindow(0,0,50,200);
w2=openwindow(100,100,30,100);
w3=openwindow(20,100,40,125);

while (TRUE) {

   Wait(1<<w->UserPort->mp_SigBit);
   while ((message=GetMsg(w->UserPort))!=NULL) {
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      ReplyMsg(message);


      if (class==CLOSEWINDOW) {
         closewindow(w1);
         closewindow(w2);
         closewindow(w3);
         CloseWindow(w);
         closescr();
         exit(0);
         }

      system_event(mousex, mousey);
      }
   }
}

