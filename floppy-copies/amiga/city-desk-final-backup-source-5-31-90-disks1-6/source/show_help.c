/** USE -b0 when compiling this module to reserve b0 memory space **/

#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "desk.h"

static struct Window *z_w;
static short z_p;

void show_help(screen)

struct Screen *screen;

{

static struct NewWindow NewWindowStructure1 = {
   0,0,   /* window XY origin relative to TopLeft of screen */
   640,200,   /* window width and height */
   WHITE,GRAY1,   /* detail and block pens */
   CLOSEWINDOW,   /* IDCMP flags */
   ACTIVATE+WINDOWDRAG+WINDOWCLOSE+NOCAREREFRESH,   /* other window flags */
   NULL,   /* first gadget in gadget list */
   NULL,   /* custom CHECKMARK imagery */
   "Embeded Command List",   /* window title */
   NULL,   /* custom screen pointer */
   NULL,   /* custom bitmap */
   198,92,   /* minimum width and height */
   198,92,   /* maximum width and height */
   CUSTOMSCREEN   /* destination screen type */
};

struct IntuiMessage *message,*GetMsg();
void help_dis();

NewWindowStructure1.Screen = screen;
z_w = OpenWindow(&NewWindowStructure1);
z_p = 23;

help_dis("autothresh       bd             bm             bold           break");
help_dis("cj               compress       def            default        define");
help_dis("fj               font           fsize          hskip          hyphenoff");
help_dis("hyphenon         ifeq           ifge           ifgt           ifle");
help_dis("iflt             ifne           indent         irrmargin      irrminwidth");
help_dis("it               italic         leading        lj             lm");
help_dis("mag              magx           magy           newcolumn      newpage");
help_dis("nobd             nobold         nobreak        nocj           nocompress");
help_dis("noit             noitalic       noun           nounder        nounderlined");
help_dis("p                parspace       pl             plain          print");
help_dis("printdec         printdol       printint       printrom       printromlc");
help_dis("restorefont      rj             rm             savefont       sentspace");
help_dis("set              setendchar     setpendchar    setpstartchar  setsofthyphenchar");
help_dis("setstartchar     settabchar     settiechar     spacestretch   spacing");
help_dis("tab              tabset         tm             un             under");
help_dis("underlined       vcenter        vskip          widowoff       widowon");
help_dis("");
help_dis("[ ]     Enclose an embedded command   @   Same as [tab]");
help_dis("T       Toolbox font/back             F1  Outline all objects on page");
help_dis("<space> Space bar cancels most functions");

do {
   Wait( 1<< (z_w->UserPort->mp_SigBit));
   message=GetMsg(z_w->UserPort);
   } while (message==0);

ReplyMsg(message);
CloseWindow(z_w);
}

void help_dis(z)

char *z;

{
Move(z_w->RPort,5,z_p);
Text(z_w->RPort,z,strlen(z));
z_p = z_p + 9;
}

