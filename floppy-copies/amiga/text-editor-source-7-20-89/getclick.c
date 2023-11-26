
#include <exec/types.h>
#include <exec/exec.h>
#include <intuition/intuition.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include "desk.h"
#include "texted.h"


getclick (twin,c1,c2,curblock,ld)
struct textwin *twin;
int *c1,*c2;
struct linedata **curblock,*ld;
{
struct Window *win;
struct linedata *curblock2,*lastblock,*curblock3;
extern struct screenline line[50];
struct RastPort *rp;
int tx,ty;
LONG mclass;
USHORT mcode;
SHORT Mx,My;
SHORT mn1,mn2,mn3;
struct IntuiMessage *mess,bogusmess;
int down,offset,oldy,fstline,oldfl;
int status,ysize,bogusflag;

bogusflag = 0;
ysize = twin->maxy;
win = twin->win;

ReportMouse(TRUE,win);
ModifyIDCMP(win, MOUSEBUTTONS);

rp = win->RPort;
down = 0;
while (TRUE) {

   Wait(1<<win->UserPort->mp_SigBit);
   do {
      mess = (struct IntuiMessage *)GetMsg(win->UserPort);
      mclass = mess->Class;
      mcode = mess->Code;
      Mx = mess->MouseX;
      My = mess->MouseY;
      ReplyMsg(mess);

      if (mclass == MOUSEBUTTONS) {
         if (mcode == SELECTDOWN) {
            findpointer(twin,Mx,My,&tx,&ty);
            if (tx>-1 &&ty>-1) {

               }
            }
         }

      } while (TRUE);  /* end of while(IntuiMessage) */
   endwhile:
   } /* end of while(TRUE) */

end:
if (*c1 > *c2) {
   ty = *c2;
   *c2 = *c1;
   *c1 = ty;
   }

return(status);
}




setline(win,line1,line2)
struct Window *win;
int line1,line2;
{

extern struct screenline line[50];
int x,y,h,w,line3;

SetAPen(win->RPort,GRAY1);
SetDrMd(win->RPort,COMPLEMENT);

if (line1 > line2) {
   line3 = line1;
   line1 = line2;
   line2 = line3;
   }

x = line[line1].pos[0];
y = line[line1].h;
w = win->Width - RIGHTOFFSET;
h = line[line2].d;

RectFill(win->RPort,x,y,w,h);

SetDrMd(win->RPort,JAM2);

}
