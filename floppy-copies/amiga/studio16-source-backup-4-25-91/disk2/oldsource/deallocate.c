#include "exec/types.h"
#include "devices/audio.h"
#include "psound.h"


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

