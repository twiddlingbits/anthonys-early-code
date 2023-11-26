#include "exec/types.h"
#include <libraries/dosextens.h>
#include "libraries/configvars.h"

#define RDOK68 0x0200
#define WROK68 0x0100
#define CLEAR	0
#define STATUS	0
#define DATA	1


USHORT *port;
struct ExpansionBase *ExpansionBase=NULL;
struct ConfigDev *ConfigDev=NULL;
void SendW();

main(argc,argv)
int argc;
char *argv[];
{
if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0)
	return(40);

ConfigDev = FindConfigDev( ConfigDev, 2127, 1 );
	port=(USHORT *)ConfigDev->cd_BoardAddr;

SendCode(argv[1]);
if (ExpansionBase)  CloseLibrary(ExpansionBase);
}

void SendW(z)
USHORT z;
{
short x;

while (!((x=*(port+STATUS))&WROK68)) ;
*(port+DATA)=z;
}

SendCode(s)
char *s;
{
struct FileHandle *fp2;
extern struct FileHandle *Open();
USHORT *x,*x1;

SendW(0x003F);

if(!(x=(USHORT *)AllocMem(0x03f4<<2,NULL))) return(1);

x1=x;

if (!(fp2=Open(s,MODE_OLDFILE)))
	{
	printf("Could not open file");
	FreeMem(x,0x03f4<<2);
	return(2);
	}

if(Read(fp2,x,(0x03f3<<2))!=(0x03f3<<2))
	{
	printf("Could not read file");
	FreeMem(x,0x03f4<<2);
	Close(fp2);
	return(3);
	}

SendPgm(port,x1);

FreeMem(x,0x03f4<<2);
Close(fp2);
return(0);
}
