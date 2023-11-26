
#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <fcntl.h>
#include "graphics/text.h"
#include "df0:sed1.h"

WORD diskchange=TRUE;


/***********************************/
/* Play a digitized sample         */
/***********************************/

play(Window,IOB,start,len,samp,cyc)
struct Samp *samp;
struct IOAudio *IOB;
struct Window *Window;
int len,start,cyc;

{
struct MsgPort *port;
struct IntuiMessage *mess;
int k,i,firstloop;
int unit_num=1;
int off,thislen;
if (len==0) return(0);


IOB->ioa_Request.io_Command=ADCMD_FINISH;
DoIO (IOB);

tochan (IOB,samp->lmem,len,samp->rate,cyc);

}



