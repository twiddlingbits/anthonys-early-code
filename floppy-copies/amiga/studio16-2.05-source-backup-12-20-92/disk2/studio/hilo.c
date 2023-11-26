#include "exec/types.h"
#include "exec/exec.h"
#include "/include/psound.h"
/********************************************************************/
/********************************************************************/
USHORT Tflag=0;
short se_info_hilo_tick;
extern struct StudioBase *StudioBase;
/********************************************************************/
/********************************************************************/
void HiloTimer()
{
struct MsgPort *timerport;
struct timerequest *timermsg;
int x=0;
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
	BroadcastEventParms(se_info_hilo_tick,x++,NULL,NULL,NULL,NULL,NULL);
	}
DeletePort(timerport);
CloseDevice( (struct IORequest *) timermsg );
DeleteExtIO( (struct IORequest *) timermsg );
Tflag=0;
}
/********************************************************************/
/********************************************************************/
void StopHILOTimer()
{
if (Tflag==1) Tflag=2;
while (Tflag!=0) WaitTOF();
}
/********************************************************************/
/********************************************************************/
void StartHILOTimer()
{
void HiloTimer();

se_info_hilo_tick = GetEventID("SE_INFO_HILO_TICK");
while (Tflag==2) WaitTOF();
if (Tflag==0)
	{
	CreateTask("STUDIO_HILO_TIMER",-2,HiloTimer,2000);
	Tflag=1;
	}
}
/********************************************************************/
/********************************************************************/
