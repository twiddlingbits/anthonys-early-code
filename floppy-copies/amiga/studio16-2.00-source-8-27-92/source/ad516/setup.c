#include "exec/types.h"
#include "exec/exec.h"
#include "/include/psound.h"
#include "libraries/configvars.h"
#include "/include/CardCmds.h"

#define Tperiod	0x2000
#define Tscale	0x0080
extern struct StudioBase *StudioBase;
int ErrorLevel;

/********************************************************************/
void ModeOn();
void ModeOff();
/**********************************/
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

void GetCard()
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
if (port==NULL) {Fail("Can not find a FREE AD516 board",2);}
}

void setup()
{

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
SendW(PLAY_BLOCK|OFF);
SendW(SMPTE|OFF);
ModeOff();
RemInt();
DeleteSmpteSource(SMPTEsrc);
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

ErrorLevel=1;
for(x=0;x<2049;x++) {c=*(port+DATA);c=*(port+FIFO);*(port+DATA)=0x003B;/*SendW(0x003B);*/} /* Flush out any half sent buffer */
ErrorLevel=2;
SendW(GetRev);
CardSoftRev=GetW();
CardSoft=GetW();
CardSerialNum=(GetW()<<8)+GetW();
CardRev=GetW();
CardPal=GetW();
CardFlags=GetW();
CardMemory=GetW(); 

WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
ErrorLevel=3;

if (SendCode("Studio16:RunTime/dsp/AD516.o")) Fail("Cant Download Dsp Code",5);

WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();

for(x=0;x<2049;x++) {c=*(port+DATA);c=*(port+FIFO);*(port+DATA)=0x003B;} /* Flush out any half sent buffer */

/*WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();*/
ErrorLevel=4;

ModeOn();
ErrorLevel=6;

WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();

SendW(SMPTE|ON);
ErrorLevel=7;

WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();

SendW(PLAY_BLOCK|ON);
ErrorLevel=10;
InitCard();
}
/********************************************************************/
/********************************************************************/
