#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

static struct Window *z_w;
static short z_p;

void show_info(screen)

struct Screen *screen;

{

static struct NewWindow NewWindowStructure1 = {
   198,46,   /* window XY origin relative to TopLeft of screen */
   198,92,   /* window width and height */
   WHITE,GRAY1,   /* detail and block pens */
   CLOSEWINDOW|INTUITICKS,   /* IDCMP flags */
   ACTIVATE+WINDOWDRAG+WINDOWCLOSE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Status:",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   198,92,   /* minimum width and height */
   198,92,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

char z[40];
long *t;
long class;
struct IntuiMessage *message,*GetMsg();
void dis(), dis_time();
extern UWORD xinch,yinch;
extern char printername[60];

NewWindowStructure1.Screen = screen;
z_w = OpenWindow(&NewWindowStructure1);
z_p = 23;

sprintf(z,"Chip Ram:   %d", AvailMem(MEMF_CHIP));
dis(z);

sprintf(z,"Fast Ram:   %d", AvailMem(MEMF_FAST));
dis(z);

sprintf(z,"Page Count: %d",count_pages());
dis(z);

sprintf(z,"Last Page:  %d",last_page());
dis(z);

sprintf(z,"Printer:    %s",printername);
dis(z);

sprintf(z,"X Y dpi:    %d %d",xinch,yinch);
dis(z);

while (TRUE) {
   do {
      Wait( 1<< (z_w->UserPort->mp_SigBit));
      message=GetMsg(z_w->UserPort);
      } while (message==0);

   class = message->Class;
   ReplyMsg(message);

   time(&t);
   dis_time(ctime(&t));

   if (class==CLOSEWINDOW) {
      CloseWindow(z_w);
      return;
      }
   }
}

void dis(z)

char *z;

{
Move(z_w->RPort,10,z_p);
Text(z_w->RPort,z,strlen(z));
z_p = z_p + 9;
}

void dis_time(z)

char *z;

{
Move(z_w->RPort,20,z_p+5);
Text(z_w->RPort,z,19);
}

