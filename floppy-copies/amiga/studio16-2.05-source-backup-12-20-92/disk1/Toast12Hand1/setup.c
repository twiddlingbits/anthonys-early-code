#include "exec/types.h"
#include "exec/exec.h"
#include "/include/psound.h"
#include "libraries/configvars.h"
#include "/include/CardCmds.h"

#define Tperiod	0x2000
#define Tscale	0x0080
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

void GetCard()
{
Disable();
while ((ConDev = FindConfigDev( ConDev, 2127, 1 )) && port==NULL)
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
if (port==NULL) {Fail("Can not find a FREE AD1012 board",2);}
}

void setup()
{
hand->class_id=CardNum;
NewSMPTEsrc.unit=CardNum;

sprintf(SrcName,"AD1012 #%d   ",CardNum);

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
SendW(MODE|OFF);
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
void InitCard();
int x,c;
int GetFreqVal();

for(x=0;x<2050;x++) {*(port+DATA)=0x003B;c=*(port+DATA);} /* Flush out any half sent buffer */
SendW(GetRev);
CardSoftRev=GetW();
CardSoft=GetW();
CardSerialNum=(GetW()<<8)+GetW();
CardRev=GetW();
CardPal=GetW();
CardFlags=GetW();
CardMemory=GetW(); 

if ((CardMemory+1)*8 != 128)
	{
	telluser("This Handler requires 128K of DSP memory.  Contact SunRize for","information on how to add the additional memory.");
	Fail("Not Enough DSP Memory.",5);
	}
if (!(CardFlags&0x03)) hand->number_of_filter_settings=28;
else	hand->number_of_filter_settings=15;

if (SendCode(0)) Fail("Cant Download Dsp Code",5);

InitCard();
SendW(MODE|ON);
SendW(SMPTE|ON);
SendW(PLAY_BLOCK|ON);
}
/********************************************************************/
/********************************************************************/
