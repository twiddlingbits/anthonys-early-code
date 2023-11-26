#include "exec/types.h"
#include "exec/ports.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "/include/psound.h"
#include "libraries/configvars.h"
#include "exec/interrupts.h"
#include <hardware/intbits.h>
#include "/include/CardCmds.h"

#define BLOCKWORDS 512

#define Tperiod	0x0a00
#define Tscale	0x0080
char _ProgramName[] = {"HANDLER PROGRAM"};

void HandleGraph();

USHORT asd[10000];
/****************************************************************/
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

int type=0;
/****************************************************************/
int III=0;
USHORT Pw2[]    = { 0x0001,0x0002,0x0004,0x0008};
USHORT NegPw2[] = { 0xFFFE,0xFFFD,0xFFFB,0xFFF7};
/****************************************************************/
int Cutoff[28] =
	{ 33333,22222,20833,16667,13889,13333,11111,10417,9524,8333,
	7407,6944,6667,6068,5952,5556,5208,5128,4762,4630,4444,4167,
	3718,3472,3205,2976,2778,2604};
int FREQval[28] = 
	{0x1100,0x1200,0x0100,0x1300,0x0200,0x1400,0x1500,0x0300,0x1600,0x1700,0x1800,0x0500,0x1900,0x1A00,0x0600,0x1B00,0x0700,0x1C00,0x1D00,0x0800,0x1E00,0x1F00,0x0A00,0x0B00,0x0C00,0x0D00,0x0E00,0x0F00};
/****************************************************************/
void SendW();
USHORT GetW();
void RemInt();
void AddInt();

struct Interrupt *DspInt;
extern struct Task *FindTask();
struct Task *tc_Handler;
long Handler_sigbit;

USHORT *port,*ports[8],NumOfPorts;
struct ExpansionBase *ExpansionBase=NULL;

/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/

ULONG DOSBase;
ULONG SysBase;

struct MsgPort *child_play_port;
struct MsgPort *parent_play_port;

void setup();
struct play_msg *get_free_msg();
void init_send_msg();
void free_send_msg();
void send_msg();

#define NUM_MSGS 30
static struct play_msg msg_array[NUM_MSGS];

void _main()
{
struct play_msg *GetMsg();
struct play_msg *msg;
int unit_num,rate,vol;
int sigbits;

USHORT	Incoming=0;
USHORT	z;
int x,c;

int	WordsInBuff[4]	= {0,0,0,0};
int	PlayFlags[4]	= {0,0,0,0};
USHORT	 *buff[4]	= {0,0,0,0};

int RecBuffSize=0,RecBuffNum=0,RecdWords=0;
USHORT *RecBuff[2]={0,0},*Rbuff=0;

setup(); 
for(x=0;x<10000;x++)
c=*(port+DATA);

SendW(VAR0lo+((Tperiod & 0x00FF)<<8));
SendW(VAR0hi+(Tperiod & 0xFF00));
SendW(VAR1lo+((Tscale & 0x00FF)<<8));
SendW(VAR1hi+(Tscale & 0xFF00));
SendW(SetTimer);

SendW(PlayOn);


/** Now wait for a command from the Main (parent) perfect sound **/

for(;;)
{
sigbits=Wait( (1<<Handler_sigbit) | (1<<child_play_port->mp_SigBit) );

if (sigbits&(1<<Handler_sigbit))  
	{
	while (ID.CmdBuffPtr != DCmdPtr)
	{
		DCmdPtr=(DCmdPtr+1) & 0x00FF;
		Incoming=DCmd[DCmdPtr];

		switch (Incoming & 0x000F)
			{
			case 1:		/* HI info */
			case 7:		/* LO info */
		/*******************************************************/
			HandleGraph(Incoming);
			break;

			case 3:		/* Seconds + Frame */
			case 3+8:
		/*******************************************************/
			send_msg(MSG_SMPTE_SF, Incoming, 0, 0,0,0,0);
			break;

			case 5:		/* Hours + Minutes */
			case 5+8:
		/*******************************************************/
			send_msg(MSG_SMPTE_HM, Incoming, 0, 0,0,0,0);
			break;

			case 9:		/* Record Send Request */
		/*******************************************************/
			x=((BLOCKWORDS)<<1)*((Incoming & 0xFFF0)>>4);

			if (RecdWords >= RecBuffSize)
				{
				send_msg(MSG_HERES_DATA, RecBuff[RecBuffNum],RecdWords, 0,0,0,0);
				RecBuffNum=(RecBuffNum+1)&1;
				Rbuff=RecBuff[RecBuffNum];
				RecdWords=(x>>1);
				}

			/*Rbuff=(USHORT *)*/RecData(port,Rbuff,Rbuff+x);
			break;

			case 15:	/* Play Data Request */
		/*******************************************************/
			for(x=0;x<3;x++)
			{
			while (ID.PlayCount[x]!=0 && WordsInBuff[x]!=0)
				{
				ID.PlayCount[x]--;
				buff[x]=(USHORT *)(SendBlock(port,buff[x],buff[x]+BLOCKWORDS,&WordsInBuff[x], (x<<1)) );

				if (WordsInBuff[x]==0)
					{
					if (PlayFlags[x] & LAST_BLOCK)
						{
						ID.SoundOn=ID.SoundOn & NegPw2[x];
						ID.PlayCount[x]=0;
						}
					else
						{
						send_msg(MSG_READY_FOR_DATA, x, 0, 0,0,0,0);
						}
					}
				}
			} /* end for */
			break;
		}
	}
	}

if (sigbits&(1<<child_play_port->mp_SigBit))
	while (msg=(struct play_msg *)GetMsg(child_play_port))
	{
	if (msg->msgtype >= 100) free_send_msg(msg);	/** all CMD_ < 100 **/
	else
		{
		switch (msg->msgtype)
			{
		case CMD_TEST:
		/*******************************************************/
			msg->return_error = msg->arg1;
			ReplyMsg(msg);
			break;

		case CMD_CLOSEDOWN:
		/*******************************************************/
			SendW(ModeOff);
			SendW(HILOoff);
			SendW(SMPTEoff);
			DeletePort(child_play_port);
			msg->return_error = 0;
			if (ExpansionBase)  CloseLibrary(ExpansionBase);
			RemInt();
			if (Handler_sigbit!=-1) FreeSignal(Handler_sigbit);
			ReplyMsg(msg);
			Wait(0);
			break;
/*||PLAY||*/
		case CMD_BEGIN_PLAYING:
		/*******************************************************/
			msg->return_error = 0;
			rate = msg->arg2;
			unit_num=Pw2[msg->arg1];

			z=HALF_CLK/msg->arg2;
			SendW(VAR0lo+((z & 0x00FF)<<8));
			SendW(VAR0hi+(z & 0xFF00));
			SendW(SetRate);
			vol  = msg->arg3;
			ReplyMsg(msg);

			if (unit_num&0x0001) {ID.SoundOn=ID.SoundOn|0x0001;ID.PlayCount[0]=3;SendW(MovePlayPtr0);}
			if (unit_num&0x0002) {ID.SoundOn=ID.SoundOn|0x0002;ID.PlayCount[1]=3;SendW(MovePlayPtr1);}
			if (unit_num&0x0004) {ID.SoundOn=ID.SoundOn|0x0004;ID.PlayCount[2]=3;SendW(MovePlayPtr2);}
			if (unit_num&0x0008) {ID.SoundOn=ID.SoundOn|0x0008;ID.PlayCount[3]=3;SendW(MovePlayPtr3);}
			break;

		case CMD_DATA:
		/*******************************************************/
			unit_num = msg->arg3;

			buff[unit_num]=(USHORT *)msg->arg1;
			WordsInBuff[unit_num]=msg->arg2;
			PlayFlags[unit_num]=msg->flags;

			ReplyMsg(msg);
			break;

		case CMD_STOP_PLAYING:
		/*******************************************************/
			unit_num=Pw2[msg->arg3];
			ReplyMsg(msg);
			if (unit_num&0x0001) {ID.SoundOn=ID.SoundOn&0xFFFE;ID.PlayCount[0]=0;}
			if (unit_num&0x0002) {ID.SoundOn=ID.SoundOn&0xFFFD;ID.PlayCount[1]=0;}
			if (unit_num&0x0004) {ID.SoundOn=ID.SoundOn&0xFFFB;ID.PlayCount[2]=0;}
			if (unit_num&0x0008) {ID.SoundOn=ID.SoundOn&0xFFF7;ID.PlayCount[3]=0;}
			break;

/*||RECORD||*/
		case CMD_BEGIN_RECORDING:
		/*******************************************************/
			/* channel#=msg->arg1 */
			z=5000000/msg->arg2;
			RecBuff[0]=(USHORT *)msg->arg3;
			RecBuff[1]=(USHORT *)msg->arg4;
			RecBuffSize=msg->arg5;

			SendW(VAR0lo+((z & 0x00FF)<<8));
			SendW(VAR0hi+(z & 0xFF00));
			SendW(SetRate);

			SendW(RecordOn);
			ReplyMsg(msg);

			RecBuffNum=0;
			Rbuff=RecBuff[RecBuffNum];
			RecdWords=0;
			break;

		case CMD_STOP_RECORDING:
		/*******************************************************/
			SendW(RecordOff);
			ReplyMsg(msg);
			send_msg(MSG_HERES_DATA, RecBuff[RecBuffNum], RecdWords, 0,0,0,LAST_BLOCK);
			break;

/*||OTHER||*/
		case CMD_SET_INPUT_GAIN:  
		/*******************************************************/
		/* msg->arg1 = volume, quiet 0x00-0xFF loud            */ 
		/*******************************************************/
			SendW(SetVolume+((msg->arg1)<<8));
			ReplyMsg(msg);
			break;

		case CMD_SET_CUTOFF_FREQ: 
		/*******************************************************/
		/* msg->arg1 = Cutoff frequency in Hz                  */ 
		/*******************************************************/
			x=msg->arg1;
			ReplyMsg(msg);
			for(c=0;c!=27 && x<Cutoff[c];c++) ;
			SendW(SetFreq+FREQval[c]);
			break;

		case CMD_SET_SAMPLING_RATE: 
		/*******************************************************/
		/* msg->arg1 = Sampling Rate in samps/sec              */ 
		/*******************************************************/
			x=HALF_CLK/msg->arg1;
			ReplyMsg(msg);
			SendW(VAR0lo+((x & 0x00FF)<<8));
			SendW(VAR0hi+(x & 0xFF00));
			SendW(SetRate);
			break;

		case CMD_SET_CHANNEL_VOLUME: 
		/*******************************************************/
		/* msg->arg1 = CHANNEL#   msg->arg2=VALUE		*/ 
		/*******************************************************/
			x=msg->arg2;
			z=msg->arg1;
			ReplyMsg(msg);
			SendW(VAR0lo+((x & 0x00FF)<<8));
			SendW(VAR0hi+(x & 0xFF00));
			SendW(SetChannelVol+(z<<8));
			break;

		case CMD_MONITOR_ON:
		/*******************************************************/
			SendW(MonitorOn);
			ReplyMsg(msg);
			break;

		case CMD_MONITOR_OFF:
		/*******************************************************/
			SendW(MonitorOff);
			ReplyMsg(msg);
c=*(port+DATA);

			break;

		case CMD_HILO_ON:
		/*******************************************************/
			SendW(HILOon);
			ReplyMsg(msg);
			break;

		case CMD_HILO_OFF:
		/*******************************************************/
			SendW(HILOoff);
			ReplyMsg(msg);
			break;

		case CMD_SMPTE_OFF:
		/*******************************************************/
			SendW(SMPTEoff);
			ReplyMsg(msg);
			break;

		case CMD_SMPTE_ON:
		/*******************************************************/
			SendW(SMPTEon);
/*			SendW(SMPTEreq);*/
			ReplyMsg(msg);
			break;

		case CMD_DELAY_ON:
		/*******************************************************/
			ReplyMsg(msg);
/*8			SendW(DelayOn);*/
			break;

		case CMD_DELAY_OFF:
		/*******************************************************/
			ReplyMsg(msg);
/*			SendW(DelayOff);*/
			break;

		case CMD_RESET_DSP:
		/*******************************************************/
			SendW(DspReset);
			ReplyMsg(msg);
			break;

		case CMD_SET_VAR0:
		/*******************************************************/
			x=msg->arg1;
			ReplyMsg(msg);
			SendW(VAR0lo+((x & 0x00FF)<<8));
			SendW(VAR0hi+(x & 0xFF00));
			break;

		case CMD_SET_VAR1: 
		/*******************************************************/
			x=msg->arg1;
			ReplyMsg(msg);
			SendW(VAR1lo+((x & 0x00FF)<<8));
			SendW(VAR1hi+(x & 0xFF00));
			break;

		case CMD_SET_VAR2: 
		/*******************************************************/
			x=msg->arg1;
			ReplyMsg(msg);
			SendW(VAR2lo+((x & 0x00FF)<<8));
			SendW(VAR2hi+(x & 0xFF00));
			break;

		case CMD_SET_VAR3: 
		/*******************************************************/
			x=msg->arg1;
			ReplyMsg(msg);
			SendW(VAR3lo+((x & 0x00FF)<<8));
			SendW(VAR3hi+(x & 0xFF00));
			break;

		case CMD_SENDCMD:
		/*******************************************************/
			if(msg->arg1) SendW(msg->arg1);
			if(msg->arg2) SendW(msg->arg2);
			if(msg->arg3) SendW(msg->arg3);
			if(msg->arg4) SendW(msg->arg4);
			if(msg->arg5) SendW(msg->arg5);
			ReplyMsg(msg);
			break;

		default: 
			ReplyMsg(msg);
			}
		}
	}

}
}

struct ConfigDev *ConfigDev=NULL;
struct ConfigDev *FindConfigDev();

/**********************************************************/

void setup()
{
struct play_msg *GetMsg();

SysBase = *((ULONG *)4);

if ((parent_play_port = (struct MsgPort *)FindPort("ParentPlayPort"))==0) Wait(0);
if ((child_play_port=(struct MsgPort *)CreatePort(0,0))==0) Wait(0);

init_send_msg();

send_msg(MSG_I_AM_ALIVE, child_play_port, 0, 0,0,0,0);

if ((ExpansionBase = (struct ExpansionBase *)OpenLibrary("expansion.library",0))==0)
	send_msg(MSG_CAN_NOT_OPEN_EXPANSIONBASE,0, 0, 0,0,0,0);

NumOfPorts=0;
while (ConfigDev = FindConfigDev( ConfigDev, 2127, 1 ))
	{
	ports[NumOfPorts]=(USHORT *)ConfigDev->cd_BoardAddr;
	NumOfPorts++;
	}
	if (NumOfPorts==0) send_msg(MSG_CAN_NOT_FIND_BORED,0, 0, 0,0,0,0);

	port=ports[0];

tc_Handler=FindTask(NULL);

SetTaskPri(FindTask(NULL),4);

if ( (Handler_sigbit=AllocSignal(-1))==-1) ; /* {R} */ 
AddInt();
SendW(ModeOn);
}


/*############################################################
NAME  - send_msg

FUNCTION
	This function sends the request message to the studio16.
	The message is sent as a message via PutMsg() and a reply is
	expected.  However, this command returns immediately.  Replys
	are processed seperately.  send_msg() picks a free message
	struct from its pool of stuct play_msg's that are pre-allocated.
	Thus, multiple send_msg() calls can be made without waiting for
	replys.
INPUTS 
	cmd a command to send (eg. CMD_STOP_PLAYING)
	arg 1 - arg5 comand dependent arguments
	flags - command dependent flags (eg. LAST_BLOCK)

RESULTS
	NONE
##############################################################*/

void send_msg(cmd, arg1, arg2, arg3, arg4, arg5, flags)
ULONG cmd, arg1, arg2, arg3, arg4, arg5;
{
struct play_msg *msg;
extern struct MsgPort *parent_play_port;

if (msg=get_free_msg()) {

	msg->msgtype = cmd; 
	msg->arg1 = arg1;
	msg->arg2 = arg2;
	msg->arg3 = arg3;
	msg->arg4 = arg4;
	msg->arg5 = arg5;
	msg->flags |= flags;

	PutMsg(parent_play_port, msg);
	}
}

/*############################################################
NAME  - init_send_msg

FUNCTION
	initilizes stuff needed before you can call send_msg()
INPUTS
	NONE
RESULTS
	NONE
##############################################################*/

void init_send_msg()
{
int i;

for (i=0; i < NUM_MSGS; i++) {
	setmem(&msg_array[i], sizeof(struct play_msg), 0);
	msg_array[i].flags=FREE_MSG;
	msg_array[i].message.mn_Node.ln_Type = NT_MESSAGE;
	msg_array[i].message.mn_ReplyPort = child_play_port;
	}
}

/*************************************************************/

struct play_msg *get_free_msg()
{
int i;
for (i=0; i < NUM_MSGS; i++) {
	if (msg_array[i].flags&FREE_MSG) {
		msg_array[i].flags=0;
		return(&msg_array[i]);
		}
	}
return(NULL);
}

void free_send_msg(m)
struct play_msg *m;
{
m->flags=FREE_MSG;
}

/*****************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/*****************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/

void AddInt()
{
void RemInt();
extern void DspXferInt();
extern struct Interrupt *MakeInt();

ID.CardAddr=port;
ID.CmdBuffPtr=0;
ID.DspCmd=DCmd;
ID.SoundOn=0;
ID.PlayCount[0]=0;
ID.PlayCount[1]=0;
ID.PlayCount[2]=0;
ID.PlayCount[3]=0;

DspInt = MakeInt("Studio16.DataXfer",0,DspXferInt,&ID);
if (DspInt==0) {RemInt();exit(0);}
AddIntServer(INTB_EXTER, DspInt); 
}
/*****************************************************************/

void RemInt()
{
void FreeInt();

RemIntServer(INTB_PORTS,DspInt);
FreeInt(DspInt);
}
/*****************************************************************/

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
/*****************************************************************/

void FreeInt(intr)
struct Interrupt *intr;
{
if (intr==0) return;
if (intr->is_Node.ln_Type != NT_INTERRUPT) return;
intr->is_Node.ln_Type=0;
FreeMem(intr,sizeof(*intr));
}

/*****************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
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
/********************************************************************/
/********************************************************************/
BYTE  samp,hi,lo,vuh;
void HandleGraph(Incoming)
USHORT Incoming;
{

samp=(Incoming&0xff00)>>8;

if (Incoming & 0x0006)
	{
	lo=(127-samp)>>2;
	if (vuh > -samp)
		send_msg(MSG_HILO_INFO,hi, lo, vuh,0,0,0);
	else
		send_msg(MSG_HILO_INFO,hi, lo,-samp,0,0,0);
	}
else
	{
	hi=(127-samp)>>2;
	vuh=samp;
	}
}
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/
/********************************************************************/

/*	send_msg(MSG_PRINTME, Incoming, 0, 0,0,0,0);*/

/****************************************************************/

/** Child process to be lauched by Perfect Sound for Amiga builtin voices

lc -b0 -v handler.c

Player Handler sequence goes like this:

1. handler code is loaded and launched as a process by PS12
2. handler allocates itself a private port
2. PS12 waits for an I_AM_ALIVE msg to arrive at it's public port.
3. PS12 extracts the handlers port address from msg
4. handler goes into endless loop waiting for a command

play sequence goes like this:

1. PS12 sends CMD_BEGIN_PLAYING
2. handler sends MSG_READY_FOR_DATA
3. PS12 sends DATA command
4. handler begins playing, and keeps sending SEND_DATA msgs until its
   internal buffers are full
5. PS12 send CMD_STOP_PLAYING


COMANDS (CMD_) are sent by PS12. MESSAGES (MSG_) are sent by handler to PS12

commands:

CMD_BEGIN_PLAYING

Tells the player handler to start playing a digital sound.  The player
handler should immediately start sending MSG_READY_FOR_DATA msgs to the
main program.

arg1:	channel number
arg2:	sampling rate
arg3:	volume
arg4:	sample length

CMD_QUE_PLAY

Tells the player handler to begin playing when the specified time code
is reached.  Player should send MSG_READY_FOR_DATA immediately so it can
buffer data for immediate playback when the time code is reached.

Arguments are the same as CMD_BEGIN_PLAYING

CMD_DATA

PS12 is sending a block of data. arg1 points to 512 words of 16 bit data.
arg2 contains the length of the data.  This will be equal to BLOCK_SIZE,
unless this is the last block (indicated by LAST_BLOCK being set in flags.)

flags: LAST_BLOCK
arg1: pointer to buffer full of data
arg2: length of data (will be <= BLOCK_SIZE on last block,
	and ==BLOCK_SIZE for all other blocks

CMD_STOP_PLAYING

arg1: unit (channel) number

PS12 is telling handler to quit playing and flush buffers.

CMD_CLOSEDOWN

no arguments

Handler should de-allocate all resources then Replay(); Wait(0);
PS12 will remove the process after it gets a reply.

MSG_READY_FOR_DATA

This msg is sent by the play handler when it wants to recieve some more data
for playing.  PS12 will respond by sending a CMD_DATA.  THe play handler
should only send one MSG_READY_FOR_DATA at a time, waiting for CMD_DATA
between msgs.

no argumants

MSG_I_AM_ALIVE

This msg is send by the play handler after it allocates its required memory
and ports.  This informs PS12 that the handler has been successfully
launched and is now ready to recieve cmds.

arg1: pointer to play handlers MsgPort.

MSG_FATEL_ERROR

arg1: error code

*/

void _ONBREAK()
{
}
void _oserr()
{
}
void _OSERR()
{
}
void XCEXIT()
{
}
