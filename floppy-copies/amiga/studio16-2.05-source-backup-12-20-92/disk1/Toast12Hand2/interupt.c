#include "exec/types.h"
#include "exec/exec.h"
#include "exec/interrupts.h"
#include <hardware/intbits.h>
#include "/include/studio16.h"
#include "/include/cardcmds.h"
#include "handler.data"

#include "clib/macros.h"

char cstr[120];
/**********************************/
extern struct StudioBase *StudioBase;
extern struct ChanKey *key[4],*tkey;
extern struct AudioHandler *hand;
extern short SMPTERRORcnt,SMPTEcnt,HILOcnt;
/**********************************/
extern short se_info_channel_done;
extern short se_info_block_played;
/**********************************/
struct HLinfo	HL,hl;
USHORT *HH=(USHORT *)&HL;
/**********************************/
#define BLOCKWORDS 0x00002000
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
long Handler_sigbit,Hand_Signal;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern USHORT *port;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct IntData ID;

USHORT DCmd[0x1001],DCmdPtr=0;
USHORT SigbitFlag=0;

struct Interrupt *DspInt;
struct Task *tc_Handler;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
USHORT *Rbuff=0;

USHORT Pw2[]    = { 0x0001,0x0002,0x0004,0x0008};
USHORT NegPw2[] = { 0xFFFE,0xFFFD,0xFFFB,0xFFF7};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern struct Window *wind;
extern struct ChanData CD[4];
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
BYTE t=0;
void handle_interuptevents()
{
USHORT	Incoming;
int x;
short *data;
LONG ActualLen;
int ASHandlerQueryPlayData(),status;

void WaitErrorFunc();

while (ID.CmdBuffPtr != DCmdPtr)
	{
	DCmdPtr=(DCmdPtr+1) & 0x0FFF;
	Incoming=DCmd[DCmdPtr];


	switch (Incoming & 0x000F)
		{
		case 1:		/* HILO info */
	/*******************************************************/
		if (HILOcnt)
			{
			GetHiLoData();
			*((SHORT *)(&hand->channel[0].hilo_left))=HL.input;
			*((SHORT *)(&hand->channel[1].hilo_left))=HL.chan[0];
			*((SHORT *)(&hand->channel[2].hilo_left))=HL.chan[1];
			*((SHORT *)(&hand->channel[3].hilo_left))=HL.chan[2];
			}
		break;
/* 3 & 5 = SMPTE */

		case 9:		/* Record Send Request */
	/*******************************************************/
		if (CD[3].PS)
			{
			while (STDERR_DATA_NOT_AVAILABLE_YET==(status=ASHandlerQueryRecordData(CD[3].PS,BLOCKWORDS,&ActualLen,&Rbuff)))
				WaitErrorFunc();
			GetRecData();	/* Sends ReqDataReq, Modifies _Rbuff, Uses _port */
			}
		else	/** Handle case when '9' is in circ. buffer after stop recording **/
			{
			EmptyRecData();
			}
		break;

		case 15:	/* Play Data Request */
	/*******************************************************/
		while (ID.PlayCount[0] || ID.PlayCount[1])
		{
		for(x=0;x<2;x++)
			{
			if((ID.SoundOn & (1<<x)) && ID.PlayCount[x])
				{
				if(ID.PlayCount[x]>2) WaitTOF();
				ID.PlayCount[x]--;
				while (STDERR_DATA_NOT_AVAILABLE_YET==(status=ASHandlerQueryPlayData(CD[x].PS,BLOCKWORDS,&ActualLen,&data))) WaitErrorFunc();
				SendBlock(port,data,x,ActualLen);
				if (status==STDERR_DATA_EOF)
					{
					ID.SoundOn=ID.SoundOn & NegPw2[x];
					ID.PlayCount[x]=0;
					BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key, 0,0,CD[x].PS->region.user_data,0);
					CD[x].PS->key->as=NULL;
					}
				}
			}
		}
		break;
		}
	}
SigbitFlag=0;
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void WaitErrorFunc()
{
USHORT clr;
int x;

SendW(CONVERSION|OFF);
clr=GetRGB4(StudioBase->screen->ViewPort.ColorMap,0);
clr=clr^0x0FFF;
SetRGB4(&StudioBase->screen->ViewPort,0,(clr&0x0F00)>>8,(clr&0x00F0)>>4,clr&0x000F);
SetTaskPri(FindTask(NULL),-2);
for(x=0;x<60;x++)
	WaitTOF();
clr=clr^0x0FFF;
SetTaskPri(FindTask(NULL),51);
SetRGB4(&StudioBase->screen->ViewPort,0,(clr&0x0F00)>>8,(clr&0x00F0)>>4,clr&0x000F);
SendW(CONVERSION|ON);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void AddInt()
{
void RemInt();
extern void DspXferInt();
struct Interrupt *MakeInt();

tc_Handler=(struct Task *)FindTask(NULL);

ID.CardAddr=port;
ID.CmdBuffPtr=0;
ID.DspCmd=DCmd;
ID.SoundOn=0;
ID.PlayCount[0]=0;
ID.PlayCount[1]=0;
ID.PlayCount[2]=0;
ID.PlayCount[3]=0;

DspInt = MakeInt("Studio16.DataXfer",64,DspXferInt,&ID);
if (DspInt==0) Fail("Can't add interupt 1",3);
AddIntServer(INTB_EXTER, DspInt);
if ( (Handler_sigbit=AllocSignal(-1))==-1) Fail("Cant get SigBit",4);
Hand_Signal=1<<Handler_sigbit;
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct Interupt *MakeInt(name,pri,code,data)
char *name;
int pri;
void (*code)();
APTR data;
{
struct Interrupt *intr;

intr=(struct Interrupt *)AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR);
if (intr==0) return(0);

intr->is_Node.ln_Pri=pri;
intr->is_Node.ln_Type=NT_INTERRUPT;
intr->is_Node.ln_Name=name;
intr->is_Data = data;
intr->is_Code = code;
return(intr);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void RemInt()
{
void FreeInt();
if (DspInt)
	{
	RemIntServer(INTB_PORTS,DspInt);
	FreeInt(DspInt);
	}
if (Handler_sigbit!=-1) FreeSignal(Handler_sigbit);
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void FreeInt(intr)
struct Interrupt *intr;
{
if (intr==0) return;
if (intr->is_Node.ln_Type != NT_INTERRUPT) return;
intr->is_Node.ln_Type=0;
FreeMem(intr,sizeof(*intr));
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
