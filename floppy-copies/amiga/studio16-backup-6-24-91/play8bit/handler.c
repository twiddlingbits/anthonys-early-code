#include <exec/types.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <hardware/intbits.h>
#include <stdio.h>
#include "/include/studio16.h"

#define NUMBUFFERS 4
#define NUMCHANS 4
#define BLOCK_SIZE_8BIT 512

struct IOAudio IOB;

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

struct MsgPort *audio_dev_port;
struct Module *thismod;
short HiLowOn;
short SmpteOn;
short output_hilo;
short output_hilo_k;
short num_channels;
struct AudioHandler *thishandler;
struct Task *tc_handler;
unsigned long hilo_sigmask;
struct Interrupt *hilo_int;

unsigned long smpte_sigmask;
struct Interrupt *smpte_int;

struct TimeCode internal_smpte;

short count;

short se_info_block_played;
short se_info_channel_done;
short se_cmd_kill_module;
short se_cmd_trigger_playback;
short se_cmd_stop_playing;
short se_info_hilo;
short se_info_smpte;
short se_cmd_set_channel_volume;
short se_info_event_count;

struct IOAudioPlus {
	struct IOAudio iob;
	short un;
	};

struct PlayChan {
	struct IOAudioPlus IOBlks[NUMBUFFERS];
	unsigned char *bufptr[NUMBUFFERS];
	short empty_buffers;
	short *bigbufptr;
	int bigbuflen;
	int more;
	int bufindex;
	int rate;
	int vol;
	int hilowindex;
	struct PlayingSample *ps;
	int samples_to_play;
	char hi_sample;
	} pc[NUMCHANS];

void tochan ();
void FreeChannel();
void setup();
struct StudioEvent *get_free_msg();
void try_fill_buffers();
struct AudioHandler *AddAudioHandler();
void RemInt();
void AddInt();
struct Interupt *MakeInt();
void FreeInt();

/***********************************************************/

void main()
{
unsigned short *hardvol[] = {
		(unsigned short *)0xdff0a8,
		(unsigned short *)0xdff0b8,
		(unsigned short *)0xdff0c8,
		(unsigned short *)0xdff0d8};
struct StudioEvent *GetMsg();
struct StudioEvent *msg;
struct IOAudioPlus *audiomsg;
unsigned long sigbits;
short j,i;
short un;
struct PlayingSample *tps;
unsigned long waitmask;

setup();

while (TRUE) {
	waitmask=(1<<audio_dev_port->mp_SigBit)|(1<<thismod->notifyme->mp_SigBit)|hilo_sigmask|smpte_sigmask;
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

/*** first handle any buffers that are done playing ***/

   if (sigbits&(1<<audio_dev_port->mp_SigBit)) {
      while (audiomsg=(struct IOAudioPlus *)GetMsg(audio_dev_port)) { /** get AudioDevice CMD_WRITE replys **/
			un=audiomsg->un;
			pc[un].empty_buffers++;
			pc[un].hilowindex += BLOCK_SIZE_8BIT;

			if (pc[un].more==TRUE) {
				try_fill_buffers(un,TRUE);		/** fill only one buffer **/
				}
			else {
				if (pc[un].empty_buffers==4) {	/** Finished playing a channel? **/
					pc[un].empty_buffers=0;
					pc[un].bigbuflen=0;
					BroadcastEventParms(se_info_channel_done, pc[un].ps,0,0,0,0,0);
					}
				}
	      }
		}
   
/*** now handle any messages recieved from studio16 ***/

   if (sigbits&(1<<thismod->notifyme->mp_SigBit)) 
	while (msg=(struct StudioEvent *)GetMsg(thismod->notifyme)) {
		if (msg->type==se_cmd_set_channel_volume) {
			j=msg->arg1&3;
			pc[j].vol=msg->arg2>>10;
			*(hardvol[j])=pc[j].vol;
			pc[j].IOBlks[0].iob.ioa_Volume = pc[j].vol;
			pc[j].IOBlks[1].iob.ioa_Volume = pc[j].vol;
			pc[j].IOBlks[2].iob.ioa_Volume = pc[j].vol;
			pc[j].IOBlks[3].iob.ioa_Volume = pc[j].vol;
         ReplyMsg(msg);
         }
				
		else if (msg->type==se_cmd_kill_module && (struct Module *)msg->arg1==thismod) {
         ReplyMsg(msg);
			RemInt();
			DeleteAudioHandler(thishandler);
			DeleteModule(thismod);
         FreeChannel(&IOB);
			closelibraries();
         exit(0);
			}

		else if (msg->type==se_cmd_trigger_playback) {
			tps = (struct PlayingSample *)msg->arg1;
			if (tps->key->handler==thishandler) {
				un = tps->key->chan_num;
				pc[un].ps=tps;
      	   pc[un].rate = tps->playing_samp->rate;
	         pc[un].vol  = 64;
				pc[un].bigbufptr=tps->big_block[tps->bb_handler_index];
				pc[un].samples_to_play = tps->load_len;
				pc[un].bigbuflen=min(tps->bb_len,pc[un].samples_to_play);
				pc[un].hilowindex=0;
      	   pc[un].bufindex=0;      /* indicates which of the buffers to use */
	         pc[un].empty_buffers = NUMBUFFERS;
				pc[un].more=TRUE;

      	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
	         IOB.ioa_Request.io_Command=CMD_RESET;
   	      DoIO (&IOB);
			
/* printf("trigger: un %d rate= %d stp %d\n",un,pc[un].rate,pc[un].samples_to_play); */

				try_fill_buffers(un,FALSE);

      	   IOB.ioa_Request.io_Unit=pc[un].IOBlks[0].iob.ioa_Request.io_Unit;
      	   IOB.ioa_Request.io_Command=CMD_START;
         	DoIO(&IOB);
				}
         ReplyMsg(msg);
			}

		else if (msg->type==se_cmd_stop_playing) {
			tps = (struct PlayingSample *)msg->arg1;
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

/**** SHOUULDN'T I MAKE SURE THIS IS ALWAYS EVENT? ***/
	tochan (&pc[un].IOBlks[pc[un].bufindex],pc[un].bufptr[pc[un].bufindex],thislen,toperiod(pc[un].rate),1,pc[un].vol);
	pc[un].bufindex++;
	if (pc[un].bufindex==NUMBUFFERS) pc[un].bufindex = 0;
	if (pc[un].bigbuflen <= 0) {
		pc[un].samples_to_play -= pc[un].ps->bb_len;	/** I COULD USE LAST_BLOCK TO KEEP TRACK OF THIS SAME INFO! **/
/*printf("stp %d\n",pc[un].samples_to_play);*/
		if (pc[un].ps->flags&LAST_BLOCK && pc[un].ps->bb_diskio_index==pc[un].ps->bb_handler_index) {
			pc[un].more=FALSE;
			}
		else {	
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
/**  allocate an audio channel          ***/
/******************************************/

AllocChannel (AC_IOB)
struct IOAudio *AC_IOB;
{
unsigned char allocation_map;   /**!!!! was 3 for stereo **/
short i;
short j;

setmem(AC_IOB, sizeof(struct IOAudio), 0);
if (OpenDevice(AUDIONAME, 0, AC_IOB, 0) != 0)
   return(FALSE);

AC_IOB->ioa_Request.io_Message.mn_Node.ln_Pri = 127; /** max **/
if ((audio_dev_port=(struct MsgPort *)CreatePort(0,0)) == 0) {
   CloseDevice(AC_IOB);
   return(FALSE);
   }

AC_IOB->ioa_Request.io_Message.mn_ReplyPort = audio_dev_port;
AC_IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
AC_IOB->ioa_Request.io_Flags=ADIOF_NOWAIT;
AC_IOB->ioa_Data = &allocation_map;
AC_IOB->ioa_Length = sizeof (allocation_map);

/** Try and allocate each channel individually. **/

for (i=0; i<4; i++) {
	allocation_map = 1<<i;
	BeginIO(AC_IOB);
	WaitPort(audio_dev_port);
	GetMsg(audio_dev_port);		/** Discard **/
	if (AC_IOB->ioa_Request.io_Error==0) { /** Allocated channel? **/
		for (j=0; j < NUMBUFFERS; j++) {
			pc[num_channels].IOBlks[j].iob=*AC_IOB;
			pc[num_channels].IOBlks[j].un=num_channels;
			}
		num_channels++;
		}
	printf("allocated chanes so far: %d %x\n",num_channels,AC_IOB->ioa_Request.io_Unit);
	printf("allockey = %x\n",AC_IOB->ioa_AllocKey);
	}
return(TRUE);
}


/****************************************/
/** deallocate an audio channel        **/
/****************************************/

void FreeChannel(DC_IOB)
struct IOAudio *DC_IOB;
{
struct Device *device;
struct MsgPort *port;

if (DC_IOB != 0) {
   device = DC_IOB->ioa_Request.io_Device;
   if (device != 0) {
      DC_IOB->ioa_Request.io_Command = ADCMD_FREE;
      DoIO (DC_IOB);
      CloseDevice (DC_IOB);
   }
   port = DC_IOB->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   }
}

/*************************************************/
/**  Play a sample on an alocated channel       **/
/*************************************************/

void tochan (TC_IOB, data, length, rate,cyc,vol)
int rate,length,cyc,vol;
char *data;
struct IOAudio *TC_IOB;
{

TC_IOB->ioa_Request.io_Command = CMD_WRITE;
TC_IOB->ioa_Request.io_Flags = ADIOF_PERVOL;
TC_IOB->ioa_Data = data;
TC_IOB->ioa_Length = (ULONG)length;
TC_IOB->ioa_Period = rate;
TC_IOB->ioa_Volume = vol;
TC_IOB->ioa_Cycles = cyc;

BeginIO (TC_IOB);
}

/** convert samples per second to amiga hardware period value **/

int vid_clock=3579545;  /** NTSC, see p. 621 of RKM 1.3 man for PAL info **/

toperiod(ss)
long ss;
{
int j;
j=vid_clock/ss;
return(j);
}


/**********************************************************/

void setup()
{
void FreeChannel();
struct StudioEvent *GetMsg();
static struct NewModule mod_data ={"AmigaHandler",0};
struct NewAudioHandler me = {"Amiga8Bit",0,0,0,4,0,8,-10};
int j;

openlibraries();

if ((thismod=(struct Module *)AddModule(&mod_data))==0) {
	closelibraries();
   exit(10);
   }


/*** fill in variables used by vertical blanking interupts **/

tc_handler=(struct Task *)FindTask(0);

if ((j=AllocSignal(-1))==-1)  {
	printf("Cant alloc sigbit!!\n");
	exit(0);			/**FIX THIS!! **/
	}
hilo_sigmask = 1L<<j;

if ((j=AllocSignal(-1))==-1)  {
	printf("Cant alloc sigbit!!\n");
	exit(0);			/**FIX THIS!! **/
	}
smpte_sigmask = 1L<<j;

AddInt();

/**** allocate "double buffers" for audio device ****/

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

/* allocate audio channels 0,1,2,3 */

if (AllocChannel(&IOB)==FALSE) {
	DeleteModule(thismod);
	closelibraries();
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

HiLowOn=FALSE;			/** SHOULD BE FALSE!!! or (COUNT!) **/
SmpteOn=FALSE;

se_info_block_played			=GetEventID("SE_INFO_BLOCK_PLAYED");
se_info_channel_done			=GetEventID("SE_INFO_CHANNEL_DONE");
se_cmd_kill_module			=GetEventID("SE_CMD_KILL_MODULE");
se_cmd_trigger_playback		=GetEventID("SE_CMD_TRIGGER_PLAYBACK");
se_cmd_stop_playing			=GetEventID("SE_CMD_STOP_PLAYING");
se_info_hilo					=GetEventID("SE_INFO_HILO");
se_info_smpte					=GetEventID("SE_INFO_SMPTE");
se_cmd_set_channel_volume	=GetEventID("SE_CMD_SET_CHANNEL_VOLUME");
se_info_event_count			=GetEventID("SE_INFO_EVENT_COUNT");

NotifyMeOnEvent(se_cmd_kill_module, thismod);
NotifyMeOnEvent(se_cmd_set_channel_volume, thismod);
NotifyMeOnEvent(se_cmd_trigger_playback, thismod);
NotifyMeOnEvent(se_cmd_stop_playing, thismod);

NotifyMeOnCount(se_info_smpte, thismod);
NotifyMeOnCount(se_info_hilo, thismod);

me.handler_mod=thismod;
me.play_channels=num_channels;	/** determined at run time **/
thishandler=AddAudioHandler(&me);
}
/**************************************************************/

void AddInt()
{
extern void HiLoTimer();
extern void SmpteTimer();
extern struct Interrupt *MakeInt();

hilo_int = MakeInt("HandlerTimer",0,HiLoTimer,0);
if (hilo_int==0) {
	printf("AddInt error!!!!\n");		/**!!!! FIX THIS!!!! ***/	
	}
AddIntServer(INTB_VERTB, hilo_int); 

smpte_int = MakeInt("HandlerTimer",0,SmpteTimer,&internal_smpte);
if (smpte_int==0) {
	printf("AddInt smpte error!!!!\n");		/**!!!! FIX THIS!!!! ***/	
	}
AddIntServer(INTB_VERTB, smpte_int); 
}

/*****************************************************************/

void RemInt()
{
void FreeInt();

RemIntServer(INTB_VERTB,hilo_int);
FreeInt(hilo_int);

RemIntServer(INTB_VERTB,smpte_int);
FreeInt(smpte_int);
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
