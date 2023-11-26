#include "exec/types.h"
#include "exec/exec.h"
#include "/include/psound.h"
#include "libraries/configvars.h"
#include "/include/CardCmds.h"

#define Tperiod	0x2000
#define Tscale	0x0080
extern struct Task *tc_Handler;
extern long Handler_sigbit;
extern struct StudioBase *StudioBase;

/********************************************************************/
struct SmpteSource *SMPTEsrc=NULL;
char SrcName[20];
struct NewSmpteSource NewSMPTEsrc={SrcName,0,NULL};
/********************************************************************/
extern void RemInt();
extern void AddInt();
extern void InitCardSware();
extern void SendW();
extern USHORT GetW();
extern int SendCode();
/********************************************************************/
/********************************************************************/
void setdown();
/********************************************************************/
/********************************************************************/
USHORT *port=NULL;
USHORT CardNum=1;
/********************************************************************/
extern struct Module *mod;
extern struct AudioHandler *hand;
/********************************************************************/
struct ConfigDev *ConDev=NULL,*ConfigDev=NULL;
struct ConfigDev *FindConfigDev();
/********************************************************************/
struct SmpteSource *AddSmpteSource();
void SetSmpteSource();

void setup()
{

Disable();
while ((ConDev = FindConfigDev( ConDev, 2127, 2 )) && port==NULL)
	{
	if (ConDev->cd_Driver==NULL)
		{
		ConfigDev=ConDev;
		ConfigDev->cd_Driver=(APTR)mod;
		port=(USHORT *)ConfigDev->cd_BoardAddr;
		}
	else
		{
		CardNum++;
		}
	}
Enable();
if (port==NULL) {Fail("Can not find a FREE AD516 board",13);}

hand->class_id=CardNum;
NewSMPTEsrc.unit=CardNum;

sprintf(SrcName,"AD516 #%d    ",CardNum);

SMPTEsrc=AddSmpteSource(&NewSMPTEsrc);
SetSmpteSource(SMPTEsrc);

AddInt();

InitCardSware();
}
/********************************************************************/
void setdown()
{
void StopTimer();

SendW(PLAY_BLOCK|OFF);
SendW(SMPTE|OFF);
SendW(MODE|OFF);
RemInt();
DeleteSmpteSource(SMPTEsrc);
StopTimer();
ConfigDev->cd_Driver=NULL;
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
USHORT CardRev,CardMemory,CardPal,CardFlags,CardSoftRev,CardSoft;
ULONG  CardSerialNum;
/********************************************************************/
void InitCardSware()
{
int x,c;
extern int OldGainVal;
extern struct Handler12 *state;

for(x=0;x<2049;x++) {c=*(port+DATA);c=*(port+FIFO);SendW(0x003B);} /* Flush out any half sent buffer */
SendW(GetRev);
CardSoftRev=GetW();
CardSoft=GetW();
CardSerialNum=(GetW()<<8)+GetW();
CardRev=GetW();
CardPal=GetW();
CardFlags=GetW();
CardMemory=GetW(); 
if (SendCode("Studio16:RunTime/dsp/AD516.o")) Fail("Cant Download Dsp Code",5);
for(x=0;x<100000;x++) c=*(port+DATA);

SendW(MODE|ON);


SendW(ChanVol+(1<<8));
SendW(0x8000);
SendW(ChanVol+(2<<8));
SendW(0x8000);
SendW(ChanVol+(3<<8));
SendW(0x8000);
SendW(ChanVol+(4<<8));
SendW(0x8000);

SendW(SMPTE|ON);
/*SendW(GainUp+(state->input_gain[0]<<8));
OldGainVal=state->input_gain[0];
*/
SendW(PLAY_BLOCK|ON);
}
/********************************************************************/
/********************************************************************/
USHORT Tflag=0;
/********************************************************************/
/********************************************************************/
struct IntData
	{
	USHORT *CardAddr;
	USHORT CmdBuffPtr;
	USHORT *DspCmd;
	USHORT SoundOn;
	SHORT PlayCount[4];
	};
void HiloTimer()
{
struct MsgPort *timerport;
struct timerequest *timermsg;
extern USHORT DCmd[0x1001];
extern struct IntData ID;
Tflag=1;

timerport = (struct MsgPort *) CreatePort( 0, 0 );
timermsg = (struct timerequest *) CreateExtIO( timerport, sizeof( struct timerequest ) );
OpenDevice( TIMERNAME, UNIT_VBLANK ,(struct IORequest *) timermsg, 0L );

while (Tflag==1)
	{
	timermsg->tr_time.tv_secs = 0;
	timermsg->tr_time.tv_micro = StudioBase->defaults.MeterSpeed*1000;
	timermsg->tr_node.io_Command = TR_ADDREQUEST;
	SendIO((struct IORequest *) timermsg );

	Wait(1<<timerport->mp_SigBit);
	Disable();
	ID.CmdBuffPtr=(ID.CmdBuffPtr+1) & 0x0FFF;
	DCmd[ID.CmdBuffPtr]=1;
	Enable();
	Signal(tc_Handler,1<<Handler_sigbit);
	}
DeletePort(timerport);
CloseDevice( (struct IORequest *) timermsg );
DeleteExtIO( (struct IORequest *) timermsg );
Tflag=0;
}
/********************************************************************/
/********************************************************************/
void StopTimer()
{
if (Tflag==1) Tflag=2;
while (Tflag!=0) WaitTOF();
}
/********************************************************************/
/********************************************************************/
void StartTimer()
{
void HiloTimer();
if (Tflag==0)
	{
	CreateTask("STUDIO_HILO_TIMER",0,HiloTimer,2000);
	Tflag=1;
	}
}
/********************************************************************/
/********************************************************************/
