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
extern short se_info_hilo;
extern BOOL PAUSED;
/**********************************/
USHORT	HL[20];
/**********************************/
#define BLOCKWORDS 1024L
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
long Handler_sigbit,Hand_Signal;
long rate_sigbit;
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
USHORT *Rbuff=0,*RbuffEND=0;

USHORT Pw2[]    = { 0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080};
USHORT NegPw2[] = { 0xFFFE,0xFFFD,0xFFFB,0xFFF7,0xFFEF,0xFFDF,0xFFBF,0xFF7F};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern struct Window *wind;
extern struct ChanData CD[16];
extern int lasttc;
extern struct ActiveSample *lastAS;

extern int CRecSamps,CRecFrames,CRecTc;
extern int DrecSamps,DrecFrames,DrecTc;

/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern ULONG RecSamps;
extern ULONG RecFrames;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
BYTE t=0;
USHORT *HH=(USHORT *)&HL;

void handle_interuptevents()
{
USHORT	Incoming=0;
int x;
short *data;
LONG ActualLen;
int ASHandlerQueryPlayData(),status;
short flag;
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
			*((SHORT *)(&hand->channel[0].hilo_left))=HL[0];
			*((SHORT *)(&hand->channel[1].hilo_left))=HL[1];
			*((SHORT *)(&hand->channel[2].hilo_left))=HL[2];
			*((SHORT *)(&hand->channel[3].hilo_left))=HL[3];
			*((SHORT *)(&hand->channel[4].hilo_left))=HL[4];
			*((SHORT *)(&hand->channel[5].hilo_left))=HL[5];
			*((SHORT *)(&hand->channel[6].hilo_left))=HL[6];
			*((SHORT *)(&hand->channel[7].hilo_left))=HL[7];
			*((SHORT *)(&hand->channel[8].hilo_left))=HL[8];
			*((SHORT *)(&hand->channel[11].hilo_left))=HL[9];
			}
		break;
/* 3 & 5 = SMPTE */
case 3:
/*kprintf("QQQ:%x%x  %x%x\n",RecSamps,RecFrames);*/
break;

		case 7:		/* Record Send Request Right */
	/*******************************************************/
		if (CD[15].PS)
			{
			while (STDERR_DATA_NOT_AVAILABLE_YET==(status=ASHandlerQueryRecordData(CD[15].PS,BLOCKWORDS,&ActualLen,&Rbuff))) WaitErrorFunc();
			GetRecData();	/* Sends ReqDataReq, Modifies _Rbuff, Uses _port */
			DrecSamps+=ActualLen;
			DrecTc=QuerySmpteTime();
			}
		else	/** Handle case when '7' is in circ. buffer after stop recording **/
			{
			EmptyRecData();
/*			kprintf("RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR\n");*/
			}
		break;

		case 9:		/* Record Send Request Left */
	/*******************************************************/
		if (CD[14].PS)
			{
			while (STDERR_DATA_NOT_AVAILABLE_YET==(status=ASHandlerQueryRecordData(CD[14].PS,BLOCKWORDS,&ActualLen,&Rbuff))) WaitErrorFunc();
			GetRecData();	/* Sends ReqDataReq, Modifies _Rbuff, Uses _port */
			CRecSamps+=ActualLen;
			CRecTc=QuerySmpteTime();
			}
		else	/** Handle case when '9' is in circ. buffer after stop recording **/
			{
			EmptyRecData();
/*			kprintf("RRRRRRLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL\n");*/
			}
		break;

		case 11:			/* TRACE PRINT 1 */
	/*******************************************************/
		/*kprintf("TRACE %x%04x\n",Incoming);*/
		break;

		case 13:			/* TRACE PRINT 2 */
	/*******************************************************/
		/*kprintf("TRACE #13\n");*/
		break;


		case 15:	/* Play Data Request */
	/*******************************************************/
		flag=ID.SoundOn;
/*		while (ID.PlayCount[0] || ID.PlayCount[1] || ID.PlayCount[2] || ID.PlayCount[3])*/
		while(flag)
		{
		flag=0;
		for(x=0;x<8;x++)
			{
			if(ID.SoundOn & (1<<x) && ID.PlayCount[x])
				{
				Disable();ID.PlayCount[x]--;Enable();
				if (ID.PlayCount[x]) flag++;
				while (STDERR_DATA_NOT_AVAILABLE_YET==(status=ASHandlerQueryPlayData(CD[x].PS,BLOCKWORDS,&ActualLen,&data))) WaitErrorFunc();

/* 8 Chan Statement!! (8-x)*/	SendBlock(port,data,8-x,ActualLen);
				if (status==STDERR_DATA_EOF)
					{
					ID.SoundOn=ID.SoundOn & NegPw2[x];
					ID.PlayCount[x]=0;
					if (lastAS==CD[x].PS) lasttc=NULL;
					BroadcastEventParms(se_info_channel_done, CD[x].PS, CD[x].PS->key, NULL,0,CD[x].PS->region.user_data,0);
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

PAUSED=TRUE;
SendW(PINT|OFF);
SetTaskPri(FindTask(NULL),-1);
clr=GetRGB4(StudioBase->screen->ViewPort.ColorMap,0);
clr=clr^0x0FFF;
SetRGB4(&StudioBase->screen->ViewPort,0,(clr&0x0F00)>>8,(clr&0x00F0)>>4,clr&0x000F);
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();WaitTOF();
clr=clr^0x0FFF;
SetRGB4(&StudioBase->screen->ViewPort,0,(clr&0x0F00)>>8,(clr&0x00F0)>>4,clr&0x000F);
SetTaskPri(FindTask(NULL),51);
SendW(PINT|ON);
PAUSED=FALSE;
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
if ( (rate_sigbit=AllocSignal(-1))==-1) Fail("Cant get SigBit",4);
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
if (rate_sigbit!=-1) FreeSignal(rate_sigbit);
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
