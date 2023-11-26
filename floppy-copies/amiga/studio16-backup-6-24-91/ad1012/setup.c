#include "exec/types.h"
#include "exec/exec.h"
#include "/include/psound.h"
#include "libraries/configvars.h"
#include "/include/CardCmds.h"

#define Tperiod	0x0D00
#define Tscale	0x0080
/********************************************************************/
/********************************************************************/
extern void RemInt();
extern void AddInt();
extern void InitCard();
extern void SendW();
extern USHORT GetW();
extern int SendCode();
/********************************************************************/
/********************************************************************/
void setdown();
/********************************************************************/
/********************************************************************/
long Handler_sigbit;
USHORT *port,*ports[8],NumOfPorts;
/********************************************************************/
/********************************************************************/
struct ConfigDev *ConfigDev=NULL;
struct ConfigDev *FindConfigDev();
/********************************************************************/
void setup()
{
NumOfPorts=0;
while (ConfigDev = FindConfigDev( ConfigDev, 2127, 1 ))
	{
	ports[NumOfPorts]=(USHORT *)ConfigDev->cd_BoardAddr;
	NumOfPorts++;
	}
	if (NumOfPorts==0) {telluser("Cant Find AD1012 board","",0);exit(10);}

	port=ports[0];


if ( (Handler_sigbit=AllocSignal(-1))==-1) {telluser("Cant get SigBit","",0);exit(10);}

AddInt();

InitCard();

SetTaskPri(FindTask(NULL),5);
}
/********************************************************************/
void InitCard()
{
int x,c;

for(x=0;x<1000;x++)
c=*(port+DATA);
c=SendCode("RunTime:ad1012.o");
if (c) {telluser("Cant Download Dsp Code","",0);exit(10);}

SendW(ModeOn);

SendW(VAR0lo+((Tperiod & 0x00FF)<<8));
SendW(VAR0hi+(Tperiod & 0xFF00));
SendW(VAR1lo+((Tscale & 0x00FF)<<8));
SendW(VAR1hi+(Tscale & 0xFF00));
SendW(SetTimer);

SendW(GainUp+(50<<8));
SendW(PlayOn);
}
/********************************************************************/
void setdown()
{
SendW(HILOoff);
SendW(SMPTEoff);
SendW(ModeOff);
RemInt();
if (Handler_sigbit!=-1) FreeSignal(Handler_sigbit);
}
/********************************************************************/
