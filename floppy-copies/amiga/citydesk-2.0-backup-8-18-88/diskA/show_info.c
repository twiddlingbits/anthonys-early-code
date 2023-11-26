#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"

static struct Window *z_w;
static short z_p;

void show_info(screen)

struct Screen *screen;

{

static struct NewWindow NewWindowStructure1 = {
   198,46,   /* window XY origin relative to TopLeft of screen */
   261,98,   /* window width and height */
   0,1,   /* detail and block pens */
   CLOSEWINDOW,   /* IDCMP flags */
   ACTIVATE+WINDOWDRAG+WINDOWCLOSE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Status:",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   5,5,   /* minimum width and height */
   640,200,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

char z[40];
struct IntuiMessage *message,*GetMsg();
void dis();
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


do {
   Wait( 1<< (z_w->UserPort->mp_SigBit));
   message=GetMsg(z_w->UserPort);
   } while (message==0);

ReplyMsg(message);
CloseWindow(z_w);
}

void dis(z)

char *z;

{
Move(z_w->RPort,10,z_p);
Text(z_w->RPort,z,strlen(z));
z_p = z_p + 9;
}

