#include "exec/types.h"
#include "devices/audio.h"
#include "psound.h"

/******************************************/
/**  allocate an audio channel          ***/
/******************************************/

allocate_channel (IOB)
struct IOAudio *IOB;
{
struct MsgPort *port;
int i;
char *x;
static UBYTE allocation_map[] = {3};

x=(char *)IOB;
for (i=0;i<sizeof(struct IOAudio);i++)
   x[i]=(char)0;

if (OpenDevice(AUDIONAME, 0, IOB, 0) != 0)
   return(FALSE);

/* now allocate a channel */

IOB->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port=(struct MsgPort *)CreatePort("samples", 0)) == 0) {
   CloseDevice(IOB);
   return(FALSE);
   }
IOB->ioa_Request.io_Message.mn_ReplyPort = port;
IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
IOB->ioa_Request.io_Flags=ADIOF_NOWAIT;
IOB->ioa_Data = allocation_map;
IOB->ioa_Length = sizeof (allocation_map);

BeginIO (IOB);
if (WaitIO (IOB)) {
   DeletePort(port);
   CloseDevice(IOB);
   return(FALSE);
   }
return(TRUE);
}

