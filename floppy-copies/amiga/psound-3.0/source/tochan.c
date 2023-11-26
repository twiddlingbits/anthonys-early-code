#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"

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
IOB->ioa_Volume = 64;
IOB->ioa_Cycles = cyc;

BeginIO (IOB);
}

