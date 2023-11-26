#include "exec/types.h"
#include "exec/exec.h"
#include "/include/CardCmds.h"
#include <libraries/dosextens.h>
#include "intuition/intuition.h"
#include "/include/psound.h"
#include "StandardCode.h"
#include "DelayCode.h"

extern struct StudioBase *StudioBase;
extern struct Window *wind;
extern USHORT *port;
/********************************************************************/
extern short se_cmd_trigger_record;
extern short se_cmd_trigger_playback;
/********************************************************************/
void SendW(z)
USHORT z;
{
ULONG x=0;

while (*(port+STATUS)&WROK68 && x<500000) x++;
if (x>=500000) telluser("AD1012 Handler:","SendW Write Not Recognized",0);
*(port+DATA)=z;
}
/********************************************************************/
/********************************************************************/
USHORT GetW()
{
USHORT y;
ULONG x=0;
while(*(port+STATUS)&RDOK68 && x<500000) x++;
if (x>=500000) telluser("AD1012 Handler:"," GetW Read Not Recognized",0);
y=*(port+DATA);

return(y);
}
/********************************************************************/
/********************************************************************/
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
	if (CL&0x01) SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.bottomright_3d);
	else SetAPen(wind->RPort,StudioBase->defaults.colors.gadget_window.topleft_3d);
	SetBPen(wind->RPort,StudioBase->defaults.colors.gadget_window.background);
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
telluser("AD1012 Handler:",c,0);
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
/********************************************************************/
int AHTriggerList(as,flags)
struct ActiveSample *as[];
{
int x=0;
if (!(flags & TRIGGER_LIST_MANUAL_SYNC)) SendW(CONVERSION|OFF);
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
if (!(flags & TRIGGER_LIST_MANUAL_SYNC)) SendW(CONVERSION|ON);
return(NULL);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
