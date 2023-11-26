/** AudioHandler Demo using the Amiga's four voice internal **/
/** sound channels.  Also includes a SmpteHandler timed off the **/
/** vertical blanking interupt.  Compiled using lattice 'C' 5.1 **/
/** lc -v -b0 -r0 -fim													  	**/

#include <exec/types.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <hardware/intbits.h>
#include <stdio.h>
#include "/include/studio16.h"

/* Local defines **/

#define NUMBUFFERS 4
#define NUMCHANS 4
#define BLOCK_SIZE_8BIT 512

/** used for sending misc. commands to Audio Device **/

struct IOAudio IOB;

/** circular buffers in chip ram for Audio Device data **/

unsigned char chip buf0[BLOCK_SIZE_8BIT];
unsigned char chip buf1[BLOCK_SIZE_8BIT];
unsigned char chip buf2[BLOCK_SIZE_8BIT];
unsigned char chip buf3[BLOCK_SIZE_8BIT];

unsigned char chip buf4[BLOCK_SIZE_8BIT];
unsigned char chip buf5[BLOCK_SIZE_8BIT];
unsigned char chip buf6[BLOCK_SIZE_8BIT];
unsigned char chip buf7[BLOCK_SIZE_8BIT];

unsigned char chip buf8[BLOCK_SIZE_8BIT];
unsigned char chip buf9[BLOCK_SIZE_8BIT];
unsigned char chip buf10[BLOCK_SIZE_8BIT];
unsigned char chip buf11[BLOCK_SIZE_8BIT];

unsigned char chip buf12[BLOCK_SIZE_8BIT];
unsigned char chip buf13[BLOCK_SIZE_8BIT];
unsigned char chip buf14[BLOCK_SIZE_8BIT];
unsigned char chip buf15[BLOCK_SIZE_8BIT];

/** Variables global to this file **/

struct HLinfo HLinfo;

struct MsgPort *audio_dev_port;		/* Audio Device msg's reply here */
struct Module *thismod;					/* AudioHandler studio 16 module */
short HiLowOn;								/* Broadcast hilow events? */
short SmpteOn;								/* Broadcast smpte events? */
short output_hilo;						/* Summation of all playing channels */
short output_hilo_k;						/* Kount of sum's (for average) */
short num_channels;						/* Available Amiga channels */
struct AudioHandler *thishandler;	/* Pointer to handler struct */
struct Task *tc_handler;				/* Handler task pointer */
unsigned long hilo_sigmask;			/* Used by hilow timer */
struct Interrupt *hilo_int;			/* VertBlank hilo timer */
unsigned long smpte_sigmask;			/* Signal from smpte timer */
struct Interrupt *smpte_int;			/* VertBlank Smpte timer */
struct TimeCode internal_smpte;		/* Studio 16 structure */

/** Studio 16 event's assigned at run time **/

short se_info_block_played;
short se_info_channel_done;
short se_cmd_kill_module;
short se_cmd_trigger_playback;
short se_cmd_stop_playing;
short se_info_hilo;
short se_info_smpte;
short se_cmd_set_channel_volume;
short se_info_event_count;

/** Updated in the hilow interupt **/

short count;

/* Tag each Audio Device message with it's studio 16 channel id */

struct IOAudioPlus {
	struct IOAudio iob;
	short un;				/* unit number := chan_num */
	};

/* All handler specific information for a playing channel is here */

struct PlayChan {
	struct IOAudioPlus IOBlks[NUMBUFFERS];
	unsigned char *bufptr[NUMBUFFERS];
	short empty_buffers;
	short *bigbufptr;
	int bigbuflen;
	int more;
	int bufindex;
	int rate;							/** Amiga Period value **/
	int vol;								/** 0 - 63 (6 bits) **/	
	int hilowindex;
	struct PlayingSample *ps;
	int samples_to_play;
	char hi_sample;					/** split copy calc's this */
	} pc[NUMCHANS];

/** Delclare subroutine types **/

void PlayChipBlock();
void FreeChannel();
void Setup();
struct StudioEvent *get_free_msg();
void try_fill_buffers();
struct AudioHandler *AddAudioHandler();
void RemInt();
struct Interupt *MakeInt();
void FreeInt();
void CloseDown();
void MyDoIO();

/***********************************************************/

void main()
{
struct StudioEvent *msg;
struct IOAudioPlus *audiomsg;
unsigned long sigbits;
short i;
short un;
struct PlayingSample *tps;
unsigned long waitmask;
unsigned long studio_sigmask;
unsigned long audio_sigmask;

Setup();

/* Wait for an event from four sources:	**/
/* 1) Audio Device (CMD_WRITE's finishing) **/
/* 2) Studio 16 (StudioEvent's) */
/* 3) HiLo Vert Blank interupt (timer ticks used to xmit hilo info) */
/* 4) Smpte Vert Blank Interupt (time to xmit smpte info) */


audio_sigmask=1<<audio_dev_port->mp_SigBit;
studio_sigmask=1<<thismod->notifyme->mp_SigBit;

waitmask=audio_sigmask|studio_sigmask|hilo_sigmask|smpte_sigmask;

while (TRUE) {
   sigbits=Wait(waitmask);

	if (sigbits&hilo_sigmask) {
		if (HiLowOn) {
			for (i=0; i < 4; i++) {
				HLinfo.chan[i].high=abs(pc[i].hi_sample);
				HLinfo.chan[i].low=-abs(pc[i].hi_sample);
				pc[i].hi_sample = 0;
				}
			if (output_hilo_k==0)
				output_hilo=0;
			else
				output_hilo=output_hilo/output_hilo_k;
			HLinfo.output.high=abs(output_hilo);
			HLinfo.output.low=-abs(output_hilo);
			output_hilo=0;
			output_hilo_k=0;
			BroadcastEventParms(se_info_hilo,&HLinfo,0,0,0,0,0);
			}
		}

	if (sigbits&smpte_sigmask) {
		if (SmpteOn) {
			BroadcastEventParms(se_info_smpte,&internal_smpte,0,0,0,0,0);
			}
		}

/*** Handle any buffers that are done playing ***/

   if (sigbits&audio_sigmask) {
      while (audiomsg=(struct IOAudioPlus *)GetMsg(audio_dev_port)) { /** get AudioDevice CMD_WRITE replys **/
			un=audiomsg->un;
			pc[un].empty_buffers++;
			pc[un].hilowindex += BLOCK_SIZE_8BIT;

			if (pc[un].more==TRUE) {
				try_fill_buffers(un,TRUE);		/** fill only one buffer **/
				}
			else {
				if (pc[un].empty_buffers==4) {	/** Finished playing a channel? **/
					BroadcastEventParms(se_info_channel_done, pc[un].ps,0,0,0,0,0);
					pc[un].empty_buffers=0;
					pc[un].bigbuflen=0;
					pc[un].ps=0;
					}
				}
	      }
		}
   
/*** now handle any messages recieved from studio16 ***/

   if (sigbits&studio_sigmask) 
	while (msg=(struct StudioEvent *)GetMsg(thismod->notifyme)) {
		if (msg->type==se_cmd_set_channel_volume && (struct AudioHandler *)msg->arg4==thishandler) {
			un=msg->arg1&3;	/** ignore monitor channel **/
			pc[un].vol=msg->arg2>>10;	/* 16 bits --> 6 bits */
			if (pc[un].more) {	/* channel playing? **/
				pc[un].IOBlks[0].iob.ioa_Volume = pc[un].vol;
				pc[un].IOBlks[1].iob.ioa_Volume = pc[un].vol;
				pc[un].IOBlks[2].iob.ioa_Volume = pc[un].vol;
				pc[un].IOBlks[3].iob.ioa_Volume = pc[un].vol;
   	  	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
				IOB.ioa_Period = pc[un].rate;
				IOB.ioa_Volume = pc[un].vol;
   	      IOB.ioa_Request.io_Command=ADCMD_PERVOL;
  	   	   MyDoIO (&IOB);
				}
         ReplyMsg(msg);
         }
				
		else if (msg->type==se_cmd_kill_module && (struct Module *)msg->arg1==thismod) {
         ReplyMsg(msg);
	      IOB.ioa_Request.io_Command=CMD_FLUSH;
			for (un=0; un < num_channels; un++) {
				if (pc[un].ps) {		/** cancel any playing blocks **/
printf("AmigaAudioHandler: flushed one!\n");
	   	  	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
					MyDoIO(&IOB);
					BroadcastEventParms(se_info_channel_done, pc[un].ps,0,0,0,0,0);
					}
				}
			CloseDown();
         exit(0);
			}

		else if (msg->type==se_cmd_trigger_playback) {
			tps = (struct PlayingSample *)msg->arg1;
			if (tps->key->handler==thishandler) {
				un = tps->key->chan_num;
/*printf("trigger un %d\n",un);*/
				pc[un].ps=tps;
      	   pc[un].rate = toperiod(tps->playing_samp->rate);
	         pc[un].vol  = 64;
				pc[un].bigbufptr=tps->big_block[tps->bb_handler_index];
				pc[un].samples_to_play = tps->load_len;
				pc[un].bigbuflen=min(tps->bb_len,pc[un].samples_to_play);
				pc[un].hilowindex=0;
      	   pc[un].bufindex=0;      /* indicates which of the buffers to use */
	         pc[un].empty_buffers = NUMBUFFERS;
				pc[un].more=TRUE;

#if 0
      	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
	         IOB.ioa_Request.io_Command=CMD_RESET;
   	      MyDoIO (&IOB);
#endif

			
/* printf("trigger: un %d rate= %d stp %d\n",un,pc[un].rate,pc[un].samples_to_play); */

				try_fill_buffers(un,FALSE);

#if 0
      	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
      	   IOB.ioa_Request.io_Command=CMD_START;
         	MyDoIO(&IOB);
#endif
				}
         ReplyMsg(msg);
			}

		else if (msg->type==se_cmd_stop_playing) {
			tps = (struct PlayingSample *)msg->arg1;
/*printf("stop playing un %d\n",tps->key->chan_num);*/
			if (tps->key->handler==thishandler)
	      	pc[tps->key->chan_num].more = FALSE;
         ReplyMsg(msg);
			}

		else if (msg->type==se_info_event_count) {
      	if (msg->arg1==se_info_smpte) {
				if (msg->arg2==0)
					SmpteOn=FALSE;
				else
					SmpteOn=TRUE;
				}
			else if (msg->arg1==se_info_hilo) {
				if (msg->arg2==0)
					HiLowOn=FALSE;
				else
					HiLowOn=TRUE;
				}
         ReplyMsg(msg);
			}

		else {
      	ReplyMsg(msg);
			}
      }
   }
}

/*************************************************/

void try_fill_buffers(un,only_one_buffer)
{
short thislen;

while (pc[un].empty_buffers && pc[un].bigbuflen > 0) {
/*printf("try fill buffers: un %d empty_buffers %d bigbuflen %d h_i %d\n",un,pc[un].empty_buffers, pc[un].bigbuflen,pc[un].ps->bb_handler_index);*/
	pc[un].bigbuflen -= BLOCK_SIZE_8BIT;
	if (pc[un].bigbuflen < 0) /**!!!! THIS IS WIERD: ONLY NEEDED IF BLOCK_SIZE 8!= MULT. OF BIGBLOCKSIZE?? **/
		thislen=pc[un].bigbuflen+BLOCK_SIZE_8BIT;
	else
		thislen=BLOCK_SIZE_8BIT;

/******** split_copy is roughly equvilant to this code:
	for (j=0; j<thislen; j++) {
		pc[un].bufptr[pc[un].bufindex][j] = pc[un].bigbufptr[j]>>8;
		}
************/

	split_copy(pc[un].bufptr[pc[un].bufindex],pc[un].bigbufptr, &pc[un].hi_sample, thislen);
	output_hilo += pc[un].hi_sample;
	output_hilo_k++;

	pc[un].bigbufptr += thislen;
	pc[un].empty_buffers--;

/**** SHOULDN'T I MAKE SURE THIS IS ALWAYS EVENT? ***/

	PlayChipBlock(&pc[un].IOBlks[pc[un].bufindex],pc[un].bufptr[pc[un].bufindex],thislen,pc[un].rate,1,pc[un].vol);
	pc[un].bufindex++;
	if (pc[un].bufindex==NUMBUFFERS) pc[un].bufindex = 0;
	if (pc[un].bigbuflen <= 0) {
		pc[un].samples_to_play -= pc[un].ps->bb_len;	/** I COULD USE LAST_BLOCK TO KEEP TRACK OF THIS SAME INFO! **/
		if (pc[un].ps->flags&LAST_BLOCK && pc[un].ps->bb_diskio_index==pc[un].ps->bb_handler_index) {
			pc[un].more=FALSE;
			}
		else {
printf("waiting\n");	
			while(pc[un].ps->bb_diskio_index==pc[un].ps->bb_handler_index && !(pc[un].ps->flags&LAST_BLOCK))
				WaitTOF();
printf("done waiting\n");	
			pc[un].ps->bb_handler_index = (pc[un].ps->bb_handler_index+1)&1;
			pc[un].bigbufptr=pc[un].ps->big_block[pc[un].ps->bb_handler_index];
			pc[un].bigbuflen=min(pc[un].ps->bb_len,pc[un].samples_to_play);
			BroadcastEventParms(se_info_block_played, pc[un].ps, 0, 0,0,0,0);
			}
		}
	if (only_one_buffer)
		break;
	}
}

/******************************************/
/**  allocate all audio channel         ***/
/******************************************/

AllocChannel (iob)
struct IOAudio *iob;
{
unsigned char allocation_map;   /**!!!! was 3 for stereo **/
short i;
short j;

setmem(iob, sizeof(struct IOAudio), 0);
if (OpenDevice(AUDIONAME, 0, iob, 0) != 0)
   return(FALSE);

iob->ioa_Request.io_Message.mn_Node.ln_Pri = 127; /** max **/
if ((audio_dev_port=(struct MsgPort *)CreatePort(0,0)) == 0) {
   CloseDevice(iob);
	setmem(iob, sizeof(struct IOAudio), 0);
   return(FALSE);
   }

iob->ioa_Request.io_Message.mn_ReplyPort = audio_dev_port;
iob->ioa_Request.io_Command = ADCMD_ALLOCATE;
iob->ioa_Request.io_Flags=ADIOF_NOWAIT;
iob->ioa_Data = &allocation_map;
iob->ioa_Length = sizeof (allocation_map);

/** Try and allocate each channel individually. **/

for (i=0; i<4; i++) {
	allocation_map = 1<<i;
	MyDoIO(iob);
	if (iob->ioa_Request.io_Error==0) { /** Allocated channel? **/
		for (j=0; j < NUMBUFFERS; j++) {
			pc[num_channels].IOBlks[j].iob=*iob;
			pc[num_channels].IOBlks[j].un=num_channels;
			}
		num_channels++;
		}
/*	printf("allocated chanes so far: %d %x\n",num_channels,iob->ioa_Request.io_Unit);
	printf("allockey = %x\n",iob->ioa_AllocKey); */
	}

/** Reset Audio Device **/

j=0;
for (i=0; i < num_channels; i++)
	j |= (1<<i);

iob->ioa_Request.io_Unit=(struct Unit *)j;
iob->ioa_Request.io_Flags=0;
iob->ioa_Request.io_Command=CMD_RESET;
MyDoIO (iob);
iob->ioa_Request.io_Command=CMD_START;
MyDoIO (iob);

return(TRUE);
}


/****************************************/
/** deallocate all audio channel      **/
/****************************************/

void FreeChannel(iob)
struct IOAudio *iob;
{
struct Device *device;
struct MsgPort *port;

if (iob != 0) {
   device = iob->ioa_Request.io_Device;
   if (device != 0) {
      iob->ioa_Request.io_Command = ADCMD_FREE;
      MyDoIO (iob);
      CloseDevice (iob);
   	}

   port = iob->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   	}
}

/*************************************************/
/**  Play a sample on an alocated channel       **/
/*************************************************/

void PlayChipBlock (iob, data, length, rate,cyc,vol)
int rate,length,cyc,vol;
char *data;
struct IOAudio *iob;
{

iob->ioa_Request.io_Command = CMD_WRITE;
iob->ioa_Request.io_Flags = ADIOF_PERVOL;
iob->ioa_Data = data;
iob->ioa_Length = (ULONG)length;
iob->ioa_Period = rate;
iob->ioa_Volume = vol;
iob->ioa_Cycles = cyc;

BeginIO(iob);
}


/**************************************************************/
/* convert samples per second to amiga hardware period value **/
/**************************************************************/

int vid_clock=3579545;  /** NTSC, see p. 621 of RKM 1.3 man for PAL info **/

toperiod(ss)
long ss;
{
int j;
j=vid_clock/ss;
return(j);
}

/**********************************************************/


void CloseDown()
{
RemInt();
if (thishandler)
	DeleteAudioHandler(thishandler);
if (thismod)
	DeleteModule(thismod);
FreeChannel(&IOB);
closelibraries();
}

/**********************************************************/

void Setup()
{
void FreeChannel();
static struct NewModule mod_data ={"AmigaHandler",0,sizeof(struct StandardModState)};
struct NewAudioHandler me = {"Amiga8Bit",0,0,0,4,0,8,-10};
int j;

openlibraries();

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	CloseDown();
   exit(10);
   }

/*** fill in variables used by vertical blanking interupts **/

tc_handler=(struct Task *)FindTask(0);

if ((j=AllocSignal(-1))==-1)  {
	printf("Cant alloc sigbit!!\n");
	CloseDown();
	exit(10);
	}

hilo_sigmask = 1L<<j;

if ((j=AllocSignal(-1))==-1)  {
	printf("Cant alloc sigbit!!\n");
	CloseDown();
	exit(10);
	}

smpte_sigmask = 1L<<j;

if (AddMyTwoInts()==0) {
	printf("AddMyTwoInts error!!!!\n");
	CloseDown();
	exit(10);
	}

/**** Assign "double buffers" for audio device ****/

pc[0].bufptr[0]=buf0;      /** change if NUMBUFFERS != 4 **/
pc[0].bufptr[1]=buf1;
pc[0].bufptr[2]=buf2;
pc[0].bufptr[3]=buf3;

pc[1].bufptr[0]=buf4;      /** change if NUMBUFFERS != 4 **/
pc[1].bufptr[1]=buf5;
pc[1].bufptr[2]=buf6;
pc[1].bufptr[3]=buf7;

pc[2].bufptr[0]=buf8;      /** change if NUMBUFFERS != 4 **/
pc[2].bufptr[1]=buf9;
pc[2].bufptr[2]=buf10;
pc[2].bufptr[3]=buf11;

pc[3].bufptr[0]=buf12;      /** change if NUMBUFFERS != 4 **/
pc[3].bufptr[1]=buf13;
pc[3].bufptr[2]=buf14;
pc[3].bufptr[3]=buf15;

/* allocate any available audio channels */

if (AllocChannel(&IOB)==FALSE) {
	CloseDown();
   exit(0);
   }

for (j=0; j < NUMCHANS; j++) {
	pc[j].bufindex=0;      /* indicates which of the buffers to use */
	pc[j].bigbuflen=0;
	pc[j].bigbufptr=0;
	pc[j].rate=0;
	pc[j].vol=64;
	pc[j].empty_buffers=0;     /** get rid of compiler warnings **/
	pc[j].more=FALSE;
	}

HiLowOn=FALSE;			
SmpteOn=FALSE;

/* Get event id's */

se_info_block_played			=GetEventID("SE_INFO_BLOCK_PLAYED");
se_info_channel_done			=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_kill_module			=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_trigger_playback		=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_cmd_stop_playing			=GetEventID("SE_CMD_STOP_PLAYING");
se_info_hilo					=GetEventID("SE_INFO_HILO");
se_info_smpte					=GetEventID("SE_INFO_SMPTE");
se_cmd_set_channel_volume	=GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_event_count			=GetEventID("SE_INFO_EVENT_COUNT");

/* Specify which events we want to hear about */

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_set_channel_volume, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);
NotifyMeOnEvent(se_cmd_stop_playing, thismod);

/** Specify events we want to know how many modules are listening too */

NotifyMeOnCount(se_info_smpte, thismod);
NotifyMeOnCount(se_info_hilo, thismod);

/** Tell everyone about our new audio handler **/

me.handler_mod=thismod;
me.play_channels=num_channels;	/** determined at run time **/
thishandler=AddAudioHandler(&me);
}

/**************************************************************/

AddMyTwoInts()
{
extern void HiLoTimer();
extern void SmpteTimer();
extern struct Interrupt *MakeInt();

hilo_int = MakeInt("HandlerTimer",0,HiLoTimer,0);
if (hilo_int==0) {
	return(0);
	}

AddIntServer(INTB_VERTB, hilo_int); 

smpte_int = MakeInt("HandlerTimer",0,SmpteTimer,&internal_smpte);
if (smpte_int==0) {
	return(0);
	}
AddIntServer(INTB_VERTB, smpte_int); 
return(1);
}

/*****************************************************************/

void RemInt()
{
void FreeInt();

if (hilo_int) {
	RemIntServer(INTB_VERTB,hilo_int);
	FreeInt(hilo_int);
	}

if (smpte_int) {
	RemIntServer(INTB_VERTB,smpte_int);
	FreeInt(smpte_int);
	}
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
FreeMem(intr,sizeof(struct Interrupt));
}

/**************************************************************/

void MyDoIO(iob)
struct IOAudio *iob;
{
struct MsgPort *port;

port=iob->ioa_Request.io_Message.mn_ReplyPort;
BeginIO(iob);
do {
	WaitPort(audio_dev_port);
	} while ((struct IOAudio *)GetMsg(port)!=iob) ;		/** Discard **/
}

/**************************************************************/
