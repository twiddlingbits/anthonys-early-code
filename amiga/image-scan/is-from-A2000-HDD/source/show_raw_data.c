#include <exec/types.h>
#include <intuition/intuition.h>
#include "scan.h"

extern struct Screen *is_screen;
extern struct config config;


void show_raw_data()

{
static struct NewWindow NewWindowStructure1 = {
   0,25,   /* window XY origin relative to TopLeft of screen */
   65,150,   /* window width and height */
   15,6,   /* detail and block pens */
   CLOSEWINDOW,   /* IDCMP flags */
   WINDOWDRAG+ACTIVATE+WINDOWCLOSE+NOCAREREFRESH,   /* other window flags REPORTMOUSE */
   0,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Data",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct Window *window;
struct IntuiMessage *message,*GetIntuiMessage(), *GetMsg();
char buf[10];
void stat();

NewWindowStructure1.LeftEdge = (config.sw-60)/2;
NewWindowStructure1.Screen = is_screen;
window = (struct Window *)OpenWindow(&NewWindowStructure1);
if (window == 0){
   return(OUTOFMEM);
   }

SetRGB4(&is_screen->ViewPort, 1, 13, 13, 13);    /** make gray black temp. **/
init_scanner();
while (TRUE) {
   if ((message=GetMsg(window->UserPort))!=0) {
      ReplyMsg(message);
      CloseWindow(window);
      SetRGB4(&is_screen->ViewPort, 1, 1, 1, 1);
      return(AOK);
      }

   sprintf(buf,"%d",get_byte());
   stat(window,buf);
   Delay(1);
   }
}


void stat(w,s)

struct Window *w;
char *s;

{
ScrollRaster(w->RPort, 0, 9, 5,10, w->Width-17, w->Height-2);
SetAPen(w->RPort, 15);
Move(w->RPort,18,w->Height-4);
Text(w->RPort,s,strlen(s));
}
