#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "intuition/intuition.h"
#include "psound.h"

struct IOAudio *IOBlks[4];
extern char *bufL[2],*bufR[2];
extern short show_play;

/***********************************/
/* Play a digitized sample         */
/***********************************/

play(Window,IOB,start,len,samp)
struct IOAudio *IOB;
struct Window *Window;
int len,start;
struct Samp *samp;

{
extern struct Screen *screen;
extern struct Menu Titles[];
int k,i,firstloop;
int unit_num=0;
int off,thislen;
int x;
ULONG class,code;
struct IntuiMessage *message;


if (len==0) return(0);
if (len<2) {
   show_msg(Window,183);
   return(0);
   }
if (samp->rate > 28867) {  /** DMA limited to 28,867 samp/sec **/
   show_msg(Window,252);
   return(0);
   }

if (samp->type==LEFT || samp->type==STEREO) unit_num|=1;
if (samp->type==RIGHT || samp->type==STEREO) unit_num|=2;

if (alloc_ioblocks(Window,IOB))
   return(0);

IOB->ioa_Request.io_Unit=3;
IOB->ioa_Request.io_Command=CMD_RESET;
DoIO (IOB);

k=0;      /* indicates which of the two buffers to use */
off=0;    /* our position in the sample relative to start */
thislen=BUFSIZE;   /* length of current chuck of sample we are playing */

IOB->ioa_Request.io_Command=CMD_STOP;
DoIO(IOB);

firstloop=TRUE;

ClearMenuStrip(Window);
SetDrMd(&screen->RastPort, COMPLEMENT);

do {

if (off+BUFSIZE>len) thislen=len-off;
if (thislen==0) thislen=2;

x = ((off+start)*GRAPH_W)/samp->length + GRAPH_X;
if (show_play) {
   Move(&screen->RastPort,x,GRAPH_Y);
   Draw(&screen->RastPort,x,GRAPH_Y+GRAPH_H-1);
   }

if ((unit_num&2)==2) {
   movmem(samp->rmem+off+start,bufR[k],BUFSIZE);
   IOBlks[2+k]->ioa_Request.io_Unit=2;
   tochan (IOBlks[2+k],bufR[k],thislen,toperiod(samp->rate),1);
   }

if ((unit_num&1)==1) {
   movmem(samp->lmem+off+start,bufL[k],BUFSIZE); /* set 1st buffer up */
   IOBlks[k]->ioa_Request.io_Unit=1;
   tochan (IOBlks[k],bufL[k],thislen,toperiod(samp->rate),1);
   }

if (k==0) k=1; else k=0;

off=off+BUFSIZE;

if (firstloop==TRUE) {
   IOB->ioa_Request.io_Unit=unit_num;
   IOB->ioa_Request.io_Command=CMD_START;
   DoIO(IOB);
   firstloop=FALSE;
   }

else {
   if ((unit_num&1)==1) WaitIO (IOBlks[k]);
   if ((unit_num&2)==2) WaitIO (IOBlks[2+k]);
   while (message=(struct IntuiMessage *)GetMsg(Window->UserPort)) {
      class=message->Class;
      code=message->Code;
      ReplyMsg(message);
      if (class==MOUSEBUTTONS && code==SELECTDOWN) {
         if (show_play) {
            Move(&screen->RastPort,x,GRAPH_Y);        /** erase old line **/  
            Draw(&screen->RastPort,x,GRAPH_Y+GRAPH_H-1);
            }
         goto stopplay;
         }
      }
   }

if (show_play) {
   Move(&screen->RastPort,x,GRAPH_Y);        /** erase old line **/
   Draw(&screen->RastPort,x,GRAPH_Y+GRAPH_H-1);
   }

} while (thislen==BUFSIZE);

if (k==0) k=1;    /* wait for last request to finish */
else k=0;
if ((unit_num&1)==1) WaitIO (IOBlks[k]);
if ((unit_num&2)==2) WaitIO (IOBlks[2+k]);

stopplay:    /* here if user presses the stop button */

IOB->ioa_Request.io_Unit=3;
IOB->ioa_Request.io_Command=CMD_RESET;
DoIO (IOB);

free_ioblocks();
SetMenuStrip(Window,Titles);   /* turn on menus */
}

alloc_ioblocks(Window,IOB)
struct Window *Window;
struct IOAudio *IOB;
{
short k;
struct MsgPort *port;

/** allocate memory for Audio IOB's **/

for (k=0;k<4;k++) {
   if ((IOBlks[k]=AllocMem(sizeof(struct IOAudio),0))==0) {
      nomem(Window);
      while(k-->0)
         FreeMem(IOBlks[k],sizeof(struct IOAudio));
      return(TRUE);
      }
   if ((port=(struct MsgPort *)CreatePort("Rize-samps", 0)) == 0) {
      show_msg (Window,184);
      return(TRUE);
      }
   movmem(IOB,IOBlks[k],sizeof(struct IOAudio));
   IOBlks[k]->ioa_Request.io_Message.mn_ReplyPort = port;
   }
return(FALSE);
}

free_ioblocks()
{
/* free iob's memory */
short i;
struct MsgPort *port;

for (i=0;i<4;i++) {
   port = IOBlks[i]->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   FreeMem(IOBlks[i],sizeof(struct IOAudio));   /* finished with this IOB */
   }
}

