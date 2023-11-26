#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

extern struct studioBase *studioBase;

void main()
{
struct Window *this_window;
struct IntuiMessage *message,*get_all_msgs(),*GetMsg();
struct Module *find_module();
struct Module *m;
struct Gadget *g;
ULONG class;
USHORT code;
int x,y;
unsigned long mask;
unsigned long signals;
USHORT menunum,itemnum,subnum;

setup();

while (TRUE) {
   mask = 1<<studioBase->back_window->UserPort->mp_SigBit;
   for (m=studioBase->first_module; m; m=m->next)
      if (m->window)
         mask = mask | (1<<m->window->UserPort->mp_SigBit);
   while ((message=get_all_msgs())==0)
      signals=Wait(mask);
   class=message->Class;
   code=message->Code;
   y = message->MouseY;
   x = message->MouseX;
   g=(struct Gadget *)message->IAddress;
   this_window = message->IDCMPWindow;
   ReplyMsg(message);

/* if (class!=MOUSEMOVE) printf("class %x code %x\n",class,code);*/

   if (class == CLOSEWINDOW) {
      while ((message=GetMsg(this_window->UserPort))!=0)
         ReplyMsg(message);
      for (m=studioBase->first_module; m; m=m->next) {
         if (this_window==m->window) {
            if (m->close_window) {
               (*m->close_window)(m);
               }
            else {
               ClearMenuStrip(this_window);
               CloseWindow(this_window);
               }
            m->window = 0;
            break;
            }
         }
      }

   for (m=studioBase->first_module; m; m=m->next)
      if (this_window==m->window)
         (*m->do_event)(class,code,x,y,g);

   if (class == MENUPICK) {
      menunum=MENUNUM(code);
      itemnum=ITEMNUM(code);
      subnum=SUBNUM(code);

      if (menunum==0 && itemnum==5) {     /** quit **/
         setdown();  /** reverse off setup() **/
         exit(0);
         }

      if (menunum==2 && itemnum==0) {     /** show open list  **/
         m=find_module("OpenList");
         if (m)
            if (m->window==0)
               (*m->open_window)(m);
         }

      if (menunum==2 && itemnum==1) {     /** show cue list  **/
         m=find_module("CueList");
         if (m) 
            if (m->window==0)
               (*m->open_window)(m);
         }

      if (menunum==2 && itemnum==2) {     /** show smpte  **/
         m=find_module("SMPTE");
         if (m) 
            if (m->window==0)
               (*m->open_window)(m);
         }
      }
   }
}

struct IntuiMessage *get_all_msgs()
{
struct IntuiMessage *message,*GetMsg();
struct Module *m;

message=GetMsg(studioBase->back_window->UserPort);
for (m=studioBase->first_module; m; m=m->next)
   if (message==0 && m->window)
      message=GetMsg(m->window->UserPort);
return(message);
}

