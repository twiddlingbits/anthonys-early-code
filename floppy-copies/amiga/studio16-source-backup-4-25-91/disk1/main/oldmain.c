#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

extern struct studioBase *studioBase;
extern struct MsgPort *parent_play_port; /*** CHANGE TO A MODULE!! ***/
extern struct EditInstance EditInstance[32];

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
struct MinNode *node;
int i;

setup();

while (TRUE) {
   mask = 1<<studioBase->back_window->UserPort->mp_SigBit;
   for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
      m=(struct Module *)node;
      if (m->window)
         mask |= (1<<m->window->UserPort->mp_SigBit);
      }

	for (i=0; i < 32; i++) {
		if (EditInstance[i].edit_win!=0) {
         mask |= (1<<EditInstance[i].edit_win->UserPort->mp_SigBit);
			}
		}									

   mask |= (1<<parent_play_port->mp_SigBit);

   signals = Wait(mask);

   if (signals&(1<<parent_play_port->mp_SigBit)) {
      handle_io_msg();
      }

   while ((message=get_all_msgs())!=0) {
      class=message->Class;
      code=message->Code;
      y = message->MouseY;
      x = message->MouseX;
      g=(struct Gadget *)message->IAddress;
      this_window = message->IDCMPWindow;
      ReplyMsg(message);

 if (class!=MOUSEMOVE) printf("class %x code %x\n",class,code);

      if (class == CLOSEWINDOW) {
         while ((message=GetMsg(this_window->UserPort))!=0)
            ReplyMsg(message);
         for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
            m=(struct Module *)node;
            if ((this_window==m->window) || (m->flags&MOD_CALL_ALL_WINDOWS)) {
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

      for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
         m=(struct Module *)node;
         if ((this_window==m->window) || (m->flags&MOD_CALL_ALL_WINDOWS)) {
            (*m->do_event)(class,code,x,y,g);
				}
         }

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

/*         if (menunum==2 && itemnum==1) {     
            m=find_module("CueList");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
            }*/

         if (menunum==2 && itemnum==1) {     /** show cue list  **/
            m=find_module("Record");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
		}

         if (menunum==2 && itemnum==2) {     /** show cue list  **/
            m=find_module("Delay");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
		}

         if (menunum==2 && itemnum==3) {     /** show cue list  **/
            m=find_module("Equalizer");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
		}

         if (menunum==2 && itemnum==4) {     /** show cue list  **/
            m=find_module("SmpteMonitor");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
		}

         if (menunum==2 && itemnum==5) {     /** show cue list  **/
            m=find_module("Mixer");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
		}

/*         if (menunum==2 && itemnum==2) {     
            m=find_module("SMPTE");
            if (m) 
               if (m->window==0)
                  (*m->open_window)(m);
            }*/
         }
      }
   }
}

struct IntuiMessage *get_all_msgs()
{
struct IntuiMessage *message,*GetMsg();
struct Module *m;
struct MinNode *node;

message=GetMsg(studioBase->back_window->UserPort);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   m=(struct Module *)node;
   if (message==0 && m->window)
      message=GetMsg(m->window->UserPort);
   }
return(message);
}

