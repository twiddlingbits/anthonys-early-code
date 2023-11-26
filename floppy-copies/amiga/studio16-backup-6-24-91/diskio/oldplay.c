#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include <intuition/intuition.h>
#include "/include/psound.h"

extern struct studioBase *studioBase;

static struct VirtualChan {
	unsigned long data_index;
	unsigned long data_start;
	unsigned long data_len;
	struct Disk_Samp *playing_samp;
	USHORT bigblockptr;
	int fp;
	struct EditInstance *ei;
	int oldindex;
	} vc[4];

void msg_ready_for_data();
void FreeChan();
void UpdateEditLine();
void StartEditLine();
void EndEditLine();

/***********************************************************/

void main()
{
openlibraries();
void InitModDiskIO()

while (TRUE) {
   Wait(1<<thismod->notifyme->mp_SigBit);
	handle_studioevents();
	}
}

/***********************************************************/

void InitModDiskIO()
{
struct MinNode *node;
struct Module *mod;
static struct NewModule mod_data =
			{"DiskIO",0};

if ((thismod=(struct Module *)AddModule(&mod_list_data))==0) {
   telluser("Can't add mod_list","");
	closelibraries();
   exit(10);
   }

/** Turn on the events we want to hear about **/

NotifyMeOnEvent(SE_PLAY_SAMPLE, thismod);
NotifyMeOnEvent(SE_KILL_MODULE, thismod);
NotifyMeOnEvent(SE_IO_READY_FOR_DATA, thismod);
}

/***********************************************************/

void handle_studioevents()
{
struct StudioEvent *event;
struct Module *mod;

while ((event=(struct StudioEvent *)GetMsg(thismod->notifyme))!=0) {
	switch (event->type) {
		case SE_PLAY_SAMPLE:
			play(event->arg1, event->arg2, event->arg3);
			break;

		case SE_KILL_MODULE:
			DeleteModule(thismod);
			closelibraries();
			exit(0);

		case SE_IO_READY_FOR_DATA:
			msg_ready_for_data(event,0);
			break;
		}
	ReplyMsg(event);
	}
}

/***********************************/
/* Play a digitized sample         */
/***********************************/

void play(samp,start,len)
int len,start;
struct Disk_Samp *samp;

{
struct play_msg *GetMsg();
struct MsgPort *CreatePort();
int unit_num;


/*printf("play: %s start %d len %d\n",samp->name,start,len);*/

unit_num = AllocChan();
if (unit_num==-1)
	return;						/** ADD ERROR CODES!!!! ***/

/** PUT IN ERROR DETECTION STUFF ***/

vc[unit_num].data_index = start;
vc[unit_num].data_start = start;
vc[unit_num].data_len = len;
vc[unit_num].playing_samp = samp;
vc[unit_num].fp = ReadLockSamp(samp);
vc[unit_num].ei = (struct EditInstance *)GetEditInstance(samp);

StartEditLine(unit_num,start);

Seek(vc[unit_num].fp,start,OFFSET_BEGINNING);
quick_read(vc[unit_num].fp, bigblock[0][unit_num]);
vc[unit_num].bigblockptr=0;
msg_ready_for_data(0,unit_num);
BroadcastEventParms(SE_IO_BEGIN_PLAYING,unit_num,samp->rate,64,len,0,0);
}

/*******************************************************/
/*** cancel a playing channel 								**/
/*******************************************************/

void stop_playing(unit_num)
int unit_num;
{
if (vc[unit_num].playing_samp) {
	BroadcastEventParms(SE_IO_STOP_PLAYING,unit_num,0,0,0,0,0);
	ReadUnLockSamp(vc[unit_num].playing_samp,vc[unit_num].fp);
	vc[unit_num].playing_samp=0;
	}
}


/*******************************************************/
/*** called every time there is a MSG_READY_FOR_DATA ***/
/*******************************************************/

void msg_ready_for_data(msgtorep,chan)
struct play_msg *msgtorep;
int chan;
{
int blklen;
int unit_num;
unsigned long last_block;

/*printf("Ready 4 Data");*/


if(msgtorep) {
	unit_num=msgtorep->arg1;
	if (vc[unit_num].playing_samp==0)
		return;
 	/*printf("MESSAGE\n");*/
	}
else {
	unit_num=chan;
	/*printf("Start\n");*/
	}

vc[unit_num].data_index=vc[unit_num].data_index+BIG_BLOCK_SIZE;

if (vc[unit_num].data_index >= (vc[unit_num].data_start+vc[unit_num].data_len)) {
   last_block = LAST_BLOCK;
   blklen = vc[unit_num].data_start + vc[unit_num].data_len - (vc[unit_num].data_index - BIG_BLOCK_SIZE);
	ReadUnLockSamp(vc[unit_num].playing_samp,vc[unit_num].fp);
   }
else {
   last_block = 0;
   blklen = BIG_BLOCK_SIZE;
   }

BroadcastEventParms(SE_IO_PLAY_DATA, bigblock[vc[unit_num].bigblockptr][unit_num], blklen, unit_num, 0, 0, last_block);

vc[unit_num].bigblockptr=(vc[unit_num].bigblockptr+1)&1;

if (last_block!=LAST_BLOCK) {
   quick_read(vc[unit_num].fp,bigblock[vc[unit_num].bigblockptr][unit_num]);
	}
}

void msg_chan_done(msg)
struct play_msg *msg;
{
EndEditLine(msg->arg1);
vc[msg->arg1].playing_samp=0;
FreeChan(msg->arg1);
}

/**************************************************************/

void UpdateEditLine(unit_num, index)
int unit_num;
int index;
{
struct RastPort *rport;
int x;

if (vc[unit_num].playing_samp && vc[unit_num].ei) {
	index = index + vc[unit_num].data_start;
	rport = vc[unit_num].ei->edit_win->RPort;
   SetDrMd(rport, COMPLEMENT);

   x = vc[unit_num].oldindex;
   Move(rport,x,GRAPH_Y);
   Draw(rport,x,GRAPH_Y+vc[unit_num].ei->graph_height-1);

   x = (index*vc[unit_num].ei->graph_width)/vc[unit_num].playing_samp->length + GRAPH_X;
   Move(rport,x,GRAPH_Y);
   Draw(rport,x,GRAPH_Y+vc[unit_num].ei->graph_height-1);
	vc[unit_num].oldindex=x;
   }
}

/**************************************************************/

void StartEditLine(unit_num,index)
int unit_num;
{
struct RastPort *rport;
int x;

if (vc[unit_num].playing_samp && vc[unit_num].ei) {
	rport = vc[unit_num].ei->edit_win->RPort;
   SetDrMd(rport, COMPLEMENT);
   x = (index*vc[unit_num].ei->graph_width)/vc[unit_num].playing_samp->length + GRAPH_X;
   Move(rport,x,GRAPH_Y);
   Draw(rport,x,GRAPH_Y+vc[unit_num].ei->graph_height-1);
	vc[unit_num].oldindex=x;
   }
}

/**************************************************************/

void EndEditLine(unit_num)
int unit_num;
{
struct RastPort *rport;
 int x;

if (vc[unit_num].ei) {
	rport = vc[unit_num].ei->edit_win->RPort;
   SetDrMd(rport, COMPLEMENT);
   x = vc[unit_num].oldindex;
   Move(rport,x,GRAPH_Y);
   Draw(rport,x,GRAPH_Y+vc[unit_num].ei->graph_height-1);
	vc[unit_num].ei=0;
   }
}

/**************************************************************/

/** Handle channel allocation/ deallocations **/

static ChansInUse = 0;

int AllocChan()
{
short i;
short ChanBit;

for (i=0; i < 4 ; i++)	{
	ChanBit=(1<<i);
	if ((ChansInUse&ChanBit)==0) {		/** Free chanel !! **/
		ChansInUse |= ChanBit;
		return((int)i);
		}
	}
return(-1);
}

void FreeChan(ChanNum)
int ChanNum;
{
ChansInUse &= (~(1<<ChanNum));
}

void debug_samp_print(s)
struct Disk_Samp *s;
{
printf("Debug Print: samp %x '%s'\n",s,s->name);
printf("wfp %x rfp0 %x\n",s->wfp,s->rfp[0]);
printf("len %d rate %d type %d flags %x\n",s->length,s->rate,s->type,s->flags);
printf("version %d\n",s->version);
}
