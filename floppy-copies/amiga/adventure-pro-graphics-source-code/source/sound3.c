
#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "fcntl.h"
#include "df0:sed1.h"


extern struct ExecBase *SysBase;     /* for memory list searches */

/******************************************/
/**  allocate an audio channel          ***/
/******************************************/

allocate_channel (Window,IOB)
struct Window *Window;
struct IOAudio *IOB;
{
struct MsgPort *port;
int i;
char *x;
static UBYTE allocation_map[] = {1,2,4,8};

x=(char *)IOB;
for (i=0;i<sizeof(struct IOAudio);i++)
   x[i]=(char)0;

if (OpenDevice(AUDIONAME, 0, IOB, 0) != 0)
   printf ("can't open audio device");

/* now allocate a channel */

IOB->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port=(struct MsgPort *)CreatePort("samples", 0)) == 0)
   printf("can't open port");
IOB->ioa_Request.io_Message.mn_ReplyPort = port;
IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
IOB->ioa_Data = allocation_map;
IOB->ioa_Length = sizeof (allocation_map);

SendIO (IOB);
if (WaitIO (IOB))
   printf("error in allocation");

}

/****************************************/
/** deallocate an audio channel        **/
/****************************************/

deallocate_channel (IOB)
struct IOAudio *IOB;
{
struct Device *device;
struct MsgPort *port;

if (IOB != 0) {
   device = IOB->ioa_Request.io_Device;
   if (device != 0) {
      IOB->ioa_Request.io_Command = ADCMD_FREE;
      DoIO (IOB);
      CloseDevice (IOB);
   }
   port = IOB->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
}
}


/*************************************************/
/**  Play a sample on an alocated channel       **/
/*************************************************/

tochan (IOB, data, length, rate,cyc)
int rate,length,cyc;
char *data;
struct IOAudio *IOB;
{

IOB->ioa_Request.io_Command = CMD_WRITE;

IOB->ioa_Request.io_Flags = ADIOF_PERVOL;

IOB->ioa_Data = data;
IOB->ioa_Length = (ULONG)length;

IOB->ioa_Period = rate;

IOB->ioa_Cycles = cyc;

BeginIO (IOB);
}


