
/*** USE -ad WHEN COMPILING THIS FILE ***/

#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <fcntl.h>
#include "graphics/text.h"


beep(cyc,rate)
int cyc,rate;
{

struct Device *device;
struct MsgPort *port;
int i;
char *x;
static UBYTE allocation_map[] = {1,2,4,8};
int length=8;
static struct IOAudio IOB;
static UWORD sound[] =
{
0x7F7F,
0x7F7F,
0x8080,
0x8080
};


/******************************************/
/**  allocate an audio channel          ***/
/******************************************/

x=(char *)(&IOB);
for (i=0;i<sizeof(struct IOAudio);i++)
   x[i]=(char)0;

if (OpenDevice(AUDIONAME, 0, &IOB, 0) != 0) {
   return(FALSE);
   }

/* now allocate a channel */

IOB.ioa_Request.io_Message.mn_Node.ln_Pri = 0;
if ((port=(struct MsgPort *)CreatePort("samples", 0)) == 0)
   return(FALSE);
IOB.ioa_Request.io_Message.mn_ReplyPort = port;
IOB.ioa_Request.io_Command = ADCMD_ALLOCATE;
IOB.ioa_Data = allocation_map;
IOB.ioa_Length = sizeof (allocation_map);

SendIO (&IOB);
if (WaitIO (&IOB)) {
   if (port != 0)
      DeletePort (port);
   return(FALSE);
   }

/*************************************************/
/**  Play a sample on an allocated channel      **/
/*************************************************/


IOB.ioa_Request.io_Command=ADCMD_FINISH;
DoIO (&IOB);
IOB.ioa_Request.io_Command = CMD_WRITE;

IOB.ioa_Request.io_Flags = ADIOF_PERVOL;

IOB.ioa_Data = sound;
IOB.ioa_Length = (ULONG)length;

IOB.ioa_Period = rate;

IOB.ioa_Volume = 64;

IOB.ioa_Cycles = cyc;

BeginIO (&IOB);
WaitIO (&IOB);


/****************************************/
/** deallocate an audio channel        **/
/****************************************/


if (&IOB != 0) {
   device = IOB.ioa_Request.io_Device;
   if (device != 0) {
      IOB.ioa_Request.io_Command = ADCMD_FREE;
      DoIO (&IOB);
      CloseDevice (&IOB);
      }
   port = IOB.ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   }
return(TRUE);

}

