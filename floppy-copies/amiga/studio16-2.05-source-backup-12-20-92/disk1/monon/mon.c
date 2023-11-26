#include "exec/types.h"
#include "exec/exec.h"
#include "libraries/configvars.h"
#include "prog:include/CardCmds.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
struct ExpansionBase *ExpansionBase=NULL;

USHORT CardRev,CardMemory,CardPal,CardFlags,CardSoftRev,CardSoft;
ULONG  CardSerialNum;
/********************************************************************/
/********************************************************************/
void InitCard();
void SendW();
USHORT GetW();
int SendCode();
/********************************************************************/
/********************************************************************/
void setdown();
/********************************************************************/
/********************************************************************/
USHORT *port,*ports[8],NumOfPorts;
/********************************************************************/
/********************************************************************/
struct ConfigDev *ConfigDev=NULL;
struct ConfigDev *FindConfigDev();
/********************************************************************/
void main(argc,argv)
int argc;
char *argv[];

{
if (argc!=2) {printf("USAGE:%s [ON/OFF]\n",argv);exit();}

if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0)
	{printf("Cant open Expansion Lib!");exit(10);}

NumOfPorts=0;
while (ConfigDev = FindConfigDev( ConfigDev, 2127, 2 ))
	{
	ports[NumOfPorts]=(USHORT *)ConfigDev->cd_BoardAddr;
	NumOfPorts++;
	}
	if (NumOfPorts==0) {printf("Cant Find AD516 board");exit(10);}

	port=ports[0];


if (stricmp(argv[1],"on")) SendW(0x47);
else SendW(0x45);

if (ExpansionBase)	CloseLibrary(ExpansionBase);

}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
void SendW(z)
USHORT z;
{
short x=0;

while (*(port+STATUS)&WROK68 && x<500000) x++;
if (x>=500000) printf("Card Comunications Error #1 - Write Not Recognized\n");
*(port+DATA)=z;
}
/********************************************************************/
USHORT GetW()
{
USHORT y;
ULONG x=0;
while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) printf("Card Comunications Error #2 - Read Not Recognized\n");
y=*(port+DATA);

return(y);
}

/********************************************************************/
/********************************************************************/
