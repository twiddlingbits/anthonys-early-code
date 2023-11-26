#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "intuition/intuition.h"
#include "psound.h"

/***********************************/
/* Play a digitized sample         */
/***********************************/

play(Window,IOB,start,len,samp,it)
struct Samp *samp;
struct IOAudio *IOB;
struct Window *Window;
int len,start,it;

{
extern char *bufL[2],*bufR[2];
struct IOAudio *IOBlks[4];
struct MsgPort *port;
int k,i,firstloop;
int unit_num=0;
int off,thislen;
ULONG class;
struct Gadget *gad;
struct IntuiMessage *message;


if (len==0) return(0);
if (len<2) {
   msg(Window,"Invalid Range... Check Your Markers.");
   return(0);
   }

if (samp->type[it]==LEFT || samp->type[it]==STEREO) unit_num|=1;
if (samp->type[it]==RIGHT || samp->type[it]==STEREO) unit_num|=2;

/** allocate memory for Audio IOB's **/

for (k=0;k<4;k++) {
   if ((IOBlks[k]=AllocMem(sizeof(struct IOAudio),0))==0) {
      nomem(Window);
      while(k-->0)
         FreeMem(IOBlks[k],sizeof(struct IOAudio));
      return(0);
      }
   if ((port=(struct MsgPort *)CreatePort("Rize-samps", 0)) == 0) {
      msg (Window,"can't open port");
      return(0);
      }
   movmem(IOB,IOBlks[k],sizeof(struct IOAudio));
   IOBlks[k]->ioa_Request.io_Message.mn_ReplyPort = port;
   }

IOB->ioa_Request.io_Unit=3;
IOB->ioa_Request.io_Command=CMD_RESET;
DoIO (IOB);


k=0;      /* indicates which of the two buffers to use */
off=0;    /* our position in the sample relative to start */
thislen=BUFSIZE;   /* length of current chuck of sample we are playing */


IOB->ioa_Request.io_Command=CMD_STOP;
DoIO(IOB);

firstloop=TRUE;


do {

if (off+BUFSIZE>len) thislen=len-off;
if (thislen==0) thislen=2;

if ((unit_num&2)==2) {
   movmem(samp->rmem[it]+off+start,bufR[k],BUFSIZE);
   IOBlks[2+k]->ioa_Request.io_Unit=2;
   tochan (IOBlks[2+k],bufR[k],thislen,toperiod(samp->rate[it]),1);
   }

if ((unit_num&1)==1) {
   movmem(samp->lmem[it]+off+start,bufL[k],BUFSIZE); /* set 1st buffer up */
   IOBlks[k]->ioa_Request.io_Unit=1;
   tochan (IOBlks[k],bufL[k],thislen,toperiod(samp->rate[it]),1);
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
   message=(struct IntuiMessage *)GetMsg(Window->UserPort);
   if (message !=0 ) {
      class=message->Class;
      gad=(struct Gadget *)message->IAddress;
      ReplyMsg(message);
      if (class==GADGETUP && gad->GadgetID==STOP)
         goto stopplay;
      }
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

/* free iob's memory */

   for (i=0;i<4;i++) {
      port = IOBlks[i]->ioa_Request.io_Message.mn_ReplyPort;
      if (port != 0)
         DeletePort (port);
      FreeMem(IOBlks[i],sizeof(struct IOAudio));   /* finished with this IOB */
      }
}
