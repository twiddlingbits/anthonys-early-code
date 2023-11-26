#include "exec/types.h"
#include "exec/exec.h"
#include "exec/interrupts.h"
#include <hardware/intbits.h>
#include "/include/psound.h"
/**********************************/
/**********************************/

extern short se_dELAYbUFF;
extern short se_delay_off;
extern short se_delay_on;
extern short se_hilo_info;
extern short se_hilo_off;
extern short se_hilo_on;
extern short se_io_begin_playing;
extern short se_io_begin_recording;
extern short se_io_chan_done;
extern short se_io_play_data;
extern short se_io_ready_for_data;
extern short se_io_record_data;
extern short se_io_stop_playing;
extern short se_io_stop_recording;
extern short se_kill_module;
extern short se_module_selected;
extern short se_monitor_off;
extern short se_monitor_on;
extern short se_set_channel_volume;
extern short se_set_cutoff_freq;
extern short se_set_input_gain;
extern short se_set_sampling_rate;
extern short se_smpte_hm;
extern short se_smpte_off;
extern short se_smpte_on;
extern short se_smpte_sf;

/**********************************/
/**********************************/

#define BLOCKWORDS 512
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern void RemInt();
extern void AddInt();
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
extern USHORT *port;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
struct IntData
	{
	USHORT *CardAddr;
	USHORT CmdBuffPtr;
	USHORT *DspCmd;
	USHORT SoundOn;
	SHORT PlayCount[4];
	};
USHORT DCmd[256],DCmdPtr=0;

struct IntData ID;

struct Interrupt *DspInt;
struct Task *tc_Handler;
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
USHORT	 *buff[4]	= {0,0,0,0};
int	ChanCtr[4]	= {0,0,0,0};
int	WordsInBuff[4]	= {0,0,0,0};
int	PlayFlags[4]	= {0,0,0,0};
int RecBuffSize=0,RecBuffNum=0;
USHORT *RecBuff[2]={0,0},*Rbuff=0,*RbuffEND=0;

USHORT Pw2[]    = { 0x0001,0x0002,0x0004,0x0008};
USHORT NegPw2[] = { 0xFFFE,0xFFFD,0xFFFB,0xFFF7};
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void handle_interuptevents()
{
USHORT	Incoming=0;
int x;

while (ID.CmdBuffPtr != DCmdPtr)
	{
	DCmdPtr=(DCmdPtr+1) & 0x00FF;
	Incoming=DCmd[DCmdPtr];

	switch (Incoming & 0x000F)
		{
		case 1:		/* HI info */
		case 7:		/* LO info */
	/*******************************************************/
		ProcHiLoInfo();
		break;

		case 3:		/* Seconds + Frame */
		case 3+8:
	/*******************************************************/
		BroadcastEventParms(se_smpte_sf, Incoming, 0, 0,0,0,0);
		break;

		case 5:		/* Hours + Minutes */
		case 5+8:
	/*******************************************************/
		BroadcastEventParms(se_smpte_hm, Incoming, 0, 0,0,0,0);
		break;

		case 9:		/* Record Send Request */
	/*******************************************************/
		Rbuff=(USHORT *)RecData(port,Rbuff,BLOCKWORDS);

		if (Rbuff == RbuffEND)
			{
			BroadcastEventParms(se_io_record_data,RecBuff[RecBuffNum],RecBuffSize, 0,0,0,0);
			RecBuffNum=(RecBuffNum+1)&1;
			Rbuff=RecBuff[RecBuffNum];
			RbuffEND=Rbuff+RecBuffSize;
			}
		break;

		case 15:	/* Play Data Request */
	/*******************************************************/
		for(x=0;x<4;x++)
		{
		while (ID.PlayCount[x]!=0 && WordsInBuff[x]!=0)
			{
			ID.PlayCount[x]--;
			buff[x]=(USHORT *)(SendBlock(port,buff[x],buff[x]+BLOCKWORDS,&WordsInBuff[x], (x<<1)) );

			if (WordsInBuff[x]==0)
				{
				if (PlayFlags[x] & LAST_BLOCK)
					{
					ChanCtr[x]=0;
					ID.SoundOn=ID.SoundOn & NegPw2[x];
					ID.PlayCount[x]=0;
					BroadcastEventParms(se_io_chan_done, x, 0, 0,0,0,0);
					}
				else
					{
					BroadcastEventParms(se_io_ready_for_data, x, 0, 0,0,0,0);
					}
				}
			}
		} /* end for */
		break;
		}
	}
}
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
/*­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­·­*/
void AddInt()
{
void RemInt();
extern void DspXferInt();
struct Interrupt *MakeInt();

tc_Handler=FindTask(NULL);

ID.CardAddr=port;
ID.CmdBuffPtr=0;
ID.DspCmd=DCmd;
ID.SoundOn=0;
ID.PlayCount[0]=0;
ID.PlayCount[1]=0;
ID.PlayCount[2]=0;
ID.PlayCount[3]=0;

DspInt = MakeInt("Studio16.DataXfer",64,DspXferInt,&ID);
if (DspInt==0) {RemInt();exit(0);}
AddIntServer(INTB_EXTER, DspInt); 
}

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

RemIntServer(INTB_PORTS,DspInt);
FreeInt(DspInt);
}

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
