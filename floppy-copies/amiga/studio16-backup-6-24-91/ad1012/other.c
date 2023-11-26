#include "exec/types.h"
#include "exec/exec.h"
#include "/include/CardCmds.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
#include "/include/psound.h"

extern struct Window *wind;
extern USHORT *port;
/********************************************************************/
void SendW(z)
USHORT z;
{
short x;

while (!( (x=*(port+STATUS))&WROK68)) ;
*(port+DATA)=z;
}
/********************************************************************/
USHORT GetW()
{
USHORT y;
while(!(*(port+STATUS)&RDOK68)) ;
y=*(port+DATA);

return(y);
}
/********************************************************************/
/********************************************************************/
int SendCode(s)
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
	FreeMem(x,0x03f4<<2);
	return(2);
	}

if(Read(fp2,x,(0x03f3<<2))!=(0x03f3<<2))
	{
	FreeMem(x,0x03f4<<2);
	Close(fp2);
	return(3);
	}

SendPgm(port,x1);

FreeMem(x,0x03f4<<2);
Close(fp2);
return(0);
}
/********************************************************************/
/********************************************************************/
void prnt(c,e)
char *c;
struct StudioEvent *e;
{
static int Ypos=0;
static BYTE CL=0;
char	a[180];
if (wind)
	{
	if ((Ypos*8+20)>wind->Height) {Ypos=0; CL++;}
	sprintf(a,"%s   %08x  %08x  %08x  %08x  %08x  %08x                                                  ",c,e->arg1,e->arg2,e->arg3,e->arg4,e->arg4,e->flags);
	if (CL&0x01) SetAPen(wind->RPort, BLACK);
	else SetAPen(wind->RPort, WHITE);
	SetBPen(wind->RPort, BROWN);
	SetDrMd(wind->RPort, JAM2);
	Move(wind->RPort,8,Ypos*8+17);
	a[((wind->Width)>>3)-2]=0;
	Text(wind->RPort,a,((wind->Width)>>3)-2);
	Ypos++;
	}
}
/********************************************************************/
/********************************************************************/
