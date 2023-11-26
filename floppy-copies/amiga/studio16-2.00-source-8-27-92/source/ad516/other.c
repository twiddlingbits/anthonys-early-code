#include "exec/types.h"
#include "exec/exec.h"
#include "/include/CardCmds.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "StandardCode.h"
#include "DelayCode.h"

extern int ErrorLevel;
extern struct Window *wind;
extern USHORT *port;
/********************************************************************/
extern short se_cmd_trigger_record;
extern short se_cmd_trigger_playback;
extern short se_cmd_pause_playback;
extern short se_cmd_set_sampling_rate;
/********************************************************************/
void SendW(z)
USHORT z;
{
ULONG x=0;
char a[50];

while (*(port+STATUS)&WROK68 && x<500000) x++;
if (x>=500000) {sprintf(a,"AD516 Handler: Level(%d )",ErrorLevel);telluser(a,"SendW Write Not Recognized",0);}
*(port+DATA)=z;
/*sprintf(a,"SendW(%x): Level(%d)",z,ErrorLevel);kprintf("%s\n",a);*/
}
/********************************************************************/
/********************************************************************/
USHORT GetW()
{
USHORT y;
ULONG x=0;
char a[50];

while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) {sprintf(a,"AD516 Handler: Level(%d )",ErrorLevel);telluser(a,"GetW Write Not Recognized",0);}
y=*(port+DATA);
/*sprintf(a,"%x=GetW(): Level(%d)",y,ErrorLevel);kprintf("%s\n",a);*/
return(y);
}
/********************************************************************/
/********************************************************************/
/**********************USHORT GetSerialTest()
{
USHORT y;
ULONG x=0;
extern long _priority;

OverHere:
SetTaskPri(FindTask(NULL),-1);
while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) {telluser("AD516 Handler:"," DSP Serial Port Initializtion Failed, Retrying ",0); return;}

y=*(port+DATA);
SetTaskPri(FindTask(NULL),_priority);

return(y);
}***********

*/
/********************************************************************/
/********************************************************************/
void ModeOn()
{
USHORT y;
ULONG x=0;
extern long _priority;
extern USHORT DataParms[];

SendW(MODE|ON);
x=0;
SetTaskPri(FindTask(NULL),-1);
while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) {telluser("AD516 Handler:"," DSP Serial Port Initializtion Failed, MODE ON",0);return;}

y=*(port+DATA);

SetTaskPri(FindTask(NULL),_priority);

WaitTOF();WaitTOF();

#if CRYSTAL
DataParms[0]&=0x40C0;
DataParms[0]|=0x4000;
#else
DataParms[0]&=0xC0C0;
DataParms[0]|=0xC000;
#endif
SendW(DataRegAdj);
SendW(DataParms[0]);
SendW(DataParms[1]);
}
/********************************************************************/
/********************************************************************/
void ModeOff()
{
USHORT y;
ULONG x=0;
extern long _priority;
extern USHORT DataParms[];

#if CRYSTAL
DataParms[0]&=0x3FFF;
DataParms[0]|=0x3F3F;
#else
DataParms[0]&=0x3FFF;
DataParms[0]|=0x3F3F;
#endif
SendW(DataRegAdj);
SendW(DataParms[0]);
SendW(DataParms[1]);
WaitTOF();WaitTOF();


SendW(MODE|OFF);
x=0;
SetTaskPri(FindTask(NULL),-1);
while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) {telluser("AD516 Handler:"," DSP Serial Port Initializtion Failed, MODE OFF ",0);return;}

y=*(port+DATA);
SetTaskPri(FindTask(NULL),_priority);
}
/********************************************************************/

int SendCode(x)
int x;
{
SendW(0x003F);
SendW(0x0000);
SendW(0x03F1);
if (x==1) SendPgm(port,Delay);
else SendPgm(port,Standard);
return(0);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
int Ypos=0;
/********************************************************************/
void prnt(c)
char *c;
{
static BYTE CL=0;
char	a[180];
if (wind)
	{
	if ((Ypos*8+22)>wind->Height) {Ypos=0; CL++;}
	sprintf(a,"%s                                                                               ",c);
	if (CL&0x01) SetAPen(wind->RPort, BLACK);
	else SetAPen(wind->RPort, WHITE);
	SetBPen(wind->RPort, BROWN);
	SetDrMd(wind->RPort, JAM2);
	Move(wind->RPort,8,Ypos*8+20);
	a[((wind->Width)>>3)-2]=0;
	Text(wind->RPort,a,((wind->Width)>>3)-2);
	Ypos++;
	}
}
/********************************************************************/
/********************************************************************/
void Fail(c,e)
char *c;
int e;
{
extern struct Module *mod;
extern struct AudioHandler *hand;

BroadcastEventParms(GetEventID("SE_INFO_INITILIZATION_FAILED"), 0, 0,0,0,0,0);
telluser("AD516 Handler:",c,0);
if (wind) HideWind();
if (e>3) RemInt(); 
if (e>2) DeleteAudioHandler(hand);
if (e>1) DeleteModule(mod);
if (e>0) closelibraries();
exit(0);
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
int AHTriggerList(as,flags)
struct ActiveSample *as[];
int flags;
{
extern BOOL PAUSED;
int x;
int rat[32],rrat;

if (!(flags & TRIGGER_LIST_MANUAL_SYNC)) 
	{
	x=0;
	rrat=as[0]->region.parms.rate;
	BroadcastEventParms(se_cmd_set_sampling_rate, hand,rrat,0,0,0,0);
	while (as[x])
		{
		rat[x]=as[x]->region.parms.rate;
		as[x]->region.parms.rate=rrat;
		x++;
		}
	BroadcastEventParms(se_cmd_pause_playback, hand, TRUE,0,0,0,0);
	}
x=0;
while (as[x])
	{
	if (as[x]->key->handler==hand)
		{
		if (as[x]->flags&AS_RECORD)
			{
			BroadcastEventParms(se_cmd_trigger_record,as[x],NULL,0,0,0,0);
			}
		else	/** assume AS_PLAY **/
			{
			BroadcastEventParms(se_cmd_trigger_playback,as[x],NULL,0,0,0,0);
			}
		}
	x++;
	}

if (!(flags & TRIGGER_LIST_MANUAL_SYNC))
	{
	BroadcastEventParms(se_cmd_pause_playback, hand, FALSE,FLUSH_PLAY_BUFFERS,0,0,0);
	x=0;
	while (as[x])
		{
		as[x]->region.parms.rate=rat[x];
		x++;
		}
	}
return(NULL);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/********************************************************************/
/********************************************************************/
