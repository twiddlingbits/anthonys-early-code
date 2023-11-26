#include "exec/types.h"
#include "exec/exec.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "prog:include/smpte.h"
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/

#define PERIOD 746

/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
void __asm FillTimeCodeBuffer(register __a0 BYTE *,register __d0 ULONG);
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
struct IOAudio sound1,sound2;
BYTE           *buff1=NULL,*buff2=NULL;
struct MsgPort *port1=NULL,*port2=NULL;
UBYTE whichannel[]={2,8};
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
void main(argc,argv)
int argc;
char *argv[];
{
void openlibraries();
void closelibraries();
void OutputTimeCode();

openlibraries();
OutputTimeCode(0,30);
closelibraries();
}
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
void OutputTimeCode(tc,mode)
ULONG tc,mode;
{
int OpenAudDevice();
void CloseAudDevice();
UBYTE *pa;

if (OpenAudDevice())
	{
	SetTaskPri(FindTask(NULL),21);

	FillTimeCodeBuffer(buff1,tc);
	tc=IncTimeCode(tc,mode);
	FillTimeCodeBuffer(buff2,tc);

	BeginIO(&sound1);
	BeginIO(&sound2);
	pa=(UBYTE *)0xbfe001;
	while ((*pa&64)!=0)
		{
		tc=IncTimeCode(tc,mode);
		Wait(1<<port1->mp_SigBit);
		while(GetMsg(port1)) ;
		FillTimeCodeBuffer(buff1,tc);
		BeginIO(&sound1);

		tc=IncTimeCode(tc,mode);
		Wait(1<<port2->mp_SigBit);
		while(GetMsg(port2)) ;
		FillTimeCodeBuffer(buff2,tc);
		BeginIO(&sound2);
		}

	SetTaskPri(FindTask(NULL),0);
	}
CloseAudDevice();
}
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
int OpenAudDevice()
{
ULONG device;

setmem(&sound1,sizeof(struct IOAudio),0);
setmem(&sound2,sizeof(struct IOAudio),0);

port1=(struct MsgPort *)CreatePort(0,0);
port2=(struct MsgPort *)CreatePort(0,0);

if (!port1 || !port2) {telluser("Smpte Out:","Could not create MsgPorts",NULL);return(0);}

buff1=(BYTE *)AllocMem(80*2+1,MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR);
buff2=(BYTE *)AllocMem(80*2+1,MEMF_PUBLIC|MEMF_CHIP|MEMF_CLEAR);
if (!buff1 || !buff2) {telluser("Smpte Out:","Could not alloc buffer mem",NULL);return(0);}

sound1.ioa_Request.io_Message.mn_ReplyPort=port1;
sound1.ioa_Request.io_Message.mn_Node.ln_Pri=127;
sound1.ioa_Request.io_Command=ADCMD_ALLOCATE;
sound1.ioa_Request.io_Flags=/*ADIOF_NOWAIT*/NULL;
sound1.ioa_AllocKey=0;
sound1.ioa_Data=whichannel;
sound1.ioa_Length=sizeof(whichannel);

device=OpenDevice(AUDIONAME,0L,&sound1,0L);
if (0!=device) {telluser("Smpte Out:","Could not open device",NULL);return(0);}

sound1.ioa_Cycles=1;
sound1.ioa_Length=80*2;
sound1.ioa_Period=PERIOD;
sound1.ioa_Volume=127;
sound1.ioa_Request.io_Command=CMD_WRITE;
sound1.ioa_Request.io_Flags=ADIOF_PERVOL/*|IOF_QUICK*/;

sound2=sound1;

sound2.ioa_Request.io_Message.mn_ReplyPort=port1;
sound2.ioa_Request.io_Message.mn_ReplyPort=port2;
sound2.ioa_Data=buff2;
sound1.ioa_Data=buff1;
return(1);
}
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
void CloseAudDevice()
{
Forbid();

sound1.ioa_Request.io_Command=ADCMD_FINISH;
sound1.ioa_Request.io_Flags=IOF_QUICK;
BeginIO(&sound1);
WaitIO(&sound1);
if(port1!=NULL) {DeletePort(port1);port1=NULL;}

sound2.ioa_Request.io_Command=ADCMD_FINISH;
sound2.ioa_Request.io_Flags=IOF_QUICK;
BeginIO(&sound2);
WaitIO(&sound2);
if(port2!=NULL) {DeletePort(port2);port2=NULL;}
CloseDevice(&sound1);

if(buff1) {FreeMem(buff1,80*2+1);buff1=NULL;}
if(buff2) {FreeMem(buff2,80*2+1);buff2=NULL;}

Permit();
}
/*©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©©*/
