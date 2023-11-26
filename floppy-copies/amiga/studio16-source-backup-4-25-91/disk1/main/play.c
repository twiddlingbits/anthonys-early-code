#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <devices/audio.h>
#include <intuition/intuition.h>
#include "/include/psound.h"

extern struct MsgPort *parent_play_port;
extern struct MsgPort *child_play_port;
extern unsigned char *bigblock[2][4];

static struct VirtualChan {
	unsigned long data_index;
	unsigned long data_start;
	unsigned long data_len;
	struct Disk_Samp *playing_samp;
	USHORT bigblockptr;
	} vc[4];

void xor_graph_line();
void msg_ready_for_data();
void FreeChan();


/***********************************/
/* Play a digitized sample         */
/***********************************/

void play(Window,start,len,samp)
struct Window *Window;
int len,start;
struct Disk_Samp *samp;

{
struct play_msg *GetMsg();
struct MsgPort *CreatePort();
void send_msg();
int unit_num;


printf("play: %s start %d len %d\n",samp->name,start,len);

unit_num = AllocChan();
if (unit_num==-1)
	return;						/** ADD ERROR CODES!!!! ***/

/** PUT IN ERROR DETECTION STUFF ***/

vc[unit_num].data_index = start;
vc[unit_num].data_start = start;
vc[unit_num].data_len = len;
vc[unit_num].playing_samp = samp;

update_block1(vc[unit_num].playing_samp, vc[unit_num].data_index);		/** write last dirty block to disk **/

Seek(vc[unit_num].playing_samp->lfp,vc[unit_num].data_index,OFFSET_BEGINNING);
quick_read(vc[unit_num].playing_samp, bigblock[0][unit_num]);
vc[unit_num].bigblockptr=0;
xor_graph_line(unit_num);    /** erase old line **/
msg_ready_for_data(0,unit_num);
send_msg(CMD_BEGIN_PLAYING, unit_num, samp->rate, 64, len, 0,0);
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

printf("Ready 4 Data");
if(msgtorep) {unit_num=msgtorep->arg1; printf("MESSAGE\n");}
else {unit_num=chan;printf("Start\n");}

xor_graph_line(unit_num);    /** erase old line **/
vc[unit_num].data_index=vc[unit_num].data_index+BIG_BLOCK_SIZE;

if (vc[unit_num].data_index >= (vc[unit_num].data_start+vc[unit_num].data_len)) {
   last_block = LAST_BLOCK;
   blklen = vc[unit_num].data_start + vc[unit_num].data_len - (vc[unit_num].data_index - BIG_BLOCK_SIZE);
	FreeChan(unit_num);
   }
else {
   last_block = 0;
   blklen = BIG_BLOCK_SIZE;
   xor_graph_line(unit_num);    /** draw new line **/
   }

send_msg(CMD_DATA, bigblock[vc[unit_num].bigblockptr][unit_num], blklen, unit_num, 0, 0, last_block);

vc[unit_num].bigblockptr=(vc[unit_num].bigblockptr+1)&1;

if (msgtorep)				/** i can call this with NULL to skip reply **/
   ReplyMsg(msgtorep);
if (last_block!=LAST_BLOCK) {
   quick_read(vc[unit_num].playing_samp,bigblock[vc[unit_num].bigblockptr][unit_num]);
	}
}

void xor_graph_line(unit_num)
int unit_num;
{
extern struct Disk_Samp *cur;        /** currently selected sample **/
int x;

#if 0
if (show_play && edit_win && cur==vc[unit_num].playing_samp) { 
   SetDrMd(edit_win->RPort, COMPLEMENT);
   x = (vc[unit_num].data_index*graph_width)/vc[unit_num].playing_samp->length + GRAPH_X;
   Move(edit_win->RPort,x,GRAPH_Y);
   Draw(edit_win->RPort,x,GRAPH_Y+graph_height-1);
   }
#endif
}

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
printf("alloced %d\n",i);
		return((int)i);
		}
	}
printf("nochanels!\n");
return(-1);
}

void FreeChan(ChanNum)
int ChanNum;
{
printf("freed chan %d\n",ChanNum);
ChansInUse &= (~(1<<ChanNum));
}

void debug_samp_print(s)
struct Disk_Samp *s;
{
printf("Debug Print: samp %x '%s'\n",s,s->name);
printf("lfp %x rfp %x\n",s->lfp,s->rfp);
printf("len %d rate %d type %d flags %x\n",s->length,s->rate,s->type,s->flags);
printf("version %d block1_start %d block2_start %d\n",s->version,s->block1_start,s->block2_start);
}
