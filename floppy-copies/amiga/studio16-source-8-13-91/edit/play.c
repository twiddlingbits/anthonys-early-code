#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

extern int graph_width;
extern int graph_height;
extern short show_play;
extern struct MsgPort *parent_play_port;
extern struct MsgPort *child_play_port;
extern struct Window *edit_win;

static unsigned long data_index;
static unsigned long data_start;
static unsigned long data_len;
static struct Disk_Samp *playing_samp;
static unsigned long last_block;
static short playing=FALSE;
void xor_graph_line();


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

unit_num = 0;
if (samp->type==LEFT || samp->type==STEREO) unit_num|=1;
if (samp->type==RIGHT || samp->type==STEREO) unit_num|=2;

if (playing) {
   playing=FALSE;
   send_msg(CMD_STOP_PLAYING, unit_num,0, 0, 0, 0, 0);
   xor_graph_line();    /** erase old line **/
   }
else {
   playing = TRUE;

/** PUT IN ERROR DETECTION STUFF ***/

   send_msg(CMD_BEGIN_PLAYING, unit_num, samp->rate, 64, len, 0,0);
   
   data_index = start;
   data_start = start;
   data_len = len;
   last_block = 0;
   playing_samp = samp;
   update_block1(playing_samp, data_index);  /** preload block **/
   xor_graph_line();    /** draw new line **/
   }
}

/*******************************************************/
/*** called every time there is a MSG_READY_FOR_DATA ***/
/*******************************************************/

void msg_ready_for_data(msgtorep)
struct play_msg *msgtorep;
{
int blklen;

/*printf("msg _read\n");*/
if (playing)
   xor_graph_line();    /** erase old line **/
data_index=data_index+BLOCK_SIZE;
if (data_index >= (data_start+data_len)) {
   playing=FALSE;
   last_block = LAST_BLOCK;
   blklen = data_start + data_len - (data_index - BLOCK_SIZE);
  /*  printf("lb!\n"); */
   }
else {
   blklen = BLOCK_SIZE;
   if (playing)
      xor_graph_line();    /** draw new line **/
   }

send_msg(CMD_DATA, playing_samp->blockl1, blklen, 0, 0, 0, last_block);
ReplyMsg(msgtorep);
update_block1(playing_samp, data_index);  /** fetch next block !!!!! MAKE THIS MORE EFFECIENT **/
}

void xor_graph_line()
{
int x;
if (show_play && edit_win) { 
   SetDrMd(edit_win->RPort, COMPLEMENT);
   x = (data_index*graph_width)/playing_samp->length + GRAPH_X;
   Move(edit_win->RPort,x,GRAPH_Y);
   Draw(edit_win->RPort,x,GRAPH_Y+graph_height-1);
   }
}


debug_samp_print(s)
struct Disk_Samp *s;
{
printf("Debug Print: samp %x '%s'\n",s,s->name);
printf("lfp %x rfp %x\n",s->lfp,s->rfp);
printf("len %d rate %d type %d flags %x\n",s->length,s->rate,s->type,s->flags);
printf("version %d block1_start %d block2_start %d\n",s->version,s->block1_start,s->block2_start);
}
