


#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"

clearmess(win,kitwin,tmess)
struct Window *win,*kitwin;
struct IntuiMessage tmess[];
{

struct IntuiMessage *mess;
int i;

/*   while(mess = GetMsg(kitwin->UserPort))
   ReplyMsg(mess);   */
i = 0;
tmess[1].Class = 0;

while(mess = GetMsg(win->UserPort)) {
   if (mess->Class != MOUSEMOVE) {
      if (i == 1) {
         movmem(&tmess[0],&tmess[1],sizeof(struct IntuiMessage));
         }
      movmem(mess,&tmess[0],sizeof(struct IntuiMessage));
      ReplyMsg(mess);
      return(i+1);
      }
   movmem(mess,&tmess[0],sizeof(struct IntuiMessage));
   i = 1;
   ReplyMsg(mess);
   }
return(i);
}


