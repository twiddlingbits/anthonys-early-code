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

/***********************************/
/* Play a digitized sample         */
/***********************************/

play(Window,start,len,samp)
struct Window *Window;
int len,start;
struct Disk_Samp *samp;

{
struct play_msg *GetMsg();
struct MsgPort *CreatePort();
void send_msg();
extern struct Menu Titles[];
ULONG class,code;
struct IntuiMessage *message;
int x,i, unit_num, type;
int blklen;
struct play_msg *msg;
                              /** TAKE OFF STACK **/
unsigned long last_block;

if (len==0) return(0);
if (len<2) {
   show_msg(Window,183);
   return(0);
   }


    /** DEBUG!!! delete **/
/* do {
   printf("test getmsg: %x\n",msg=GetMsg(parent_play_port));
   } while(msg);  */

printf("play: %s start %d len %d\n",samp->name,start,len);

unit_num = 0;
if (samp->type==LEFT || samp->type==STEREO) unit_num|=1;
if (samp->type==RIGHT || samp->type==STEREO) unit_num|=2;

ClearMenuStrip(Window,Titles);
if (edit_win)
   SetDrMd(edit_win->RPort, COMPLEMENT);

/** PUT IN ERROR DETECTION STUFF ***/

send_msg(CMD_BEGIN_PLAYING, unit_num, samp->rate, 64, len, 0,0);

i = start;
last_block = 0;
while (i < (start+len)) {
   if (show_play && edit_win) {
      x = (i*graph_width)/samp->length + GRAPH_X;
      Move(edit_win->RPort,x,GRAPH_Y);
      Draw(edit_win->RPort,x,GRAPH_Y+graph_height-1);
      }
   update_block1(samp, i);
   WaitPort(parent_play_port);
   msg=GetMsg(parent_play_port);
   type = msg->msgtype;
   ReplyMsg(msg);
 /* printf("recieved msg of type %d (=? %d) %d %d\n",type,MSG_READY_FOR_DATA,msg->arg1,msg->arg2); */
   i=i+BLOCK_SIZE;
   if (type==MSG_READY_FOR_DATA) {
      if (i >= (start+len)) {
         last_block = LAST_BLOCK;
         blklen = start + len - (i - BLOCK_SIZE);
        /*  printf("lb!\n"); */
         }
      else {
         blklen = BLOCK_SIZE;
         }

      /** Is user canceling playback?? **/
      /** Note: CMD_STOP_PLAYING must be sent in replace of CMD_DATA.
          sending it when the play handler is not expecting a cmd, can
          cause a lock up **/

      while (message=(struct IntuiMessage *)GetMsg(Window->UserPort)) {
         class=message->Class;
         code=message->Code;
         ReplyMsg(message);
         if (class==MOUSEBUTTONS && code==SELECTDOWN) {
            if (show_play && edit_win) {
               Move(edit_win->RPort,x,GRAPH_Y);        /** erase old line **/
               Draw(edit_win->RPort,x,GRAPH_Y+graph_height-1);
               }
            send_msg(CMD_STOP_PLAYING, unit_num,0, 0, 0, 0, 0);
            while (msg=GetMsg(parent_play_port))  /** flush msgs **/
            ReplyMsg(msg);
            goto stopplay;
            }
         }
      send_msg(CMD_DATA, samp->blockl1, blklen, 0, 0, 0, last_block);
      }
   else {
      printf("play: bad packet!!!????\n");
      }
   if (show_play && edit_win) {
      Move(edit_win->RPort,x,GRAPH_Y);
      Draw(edit_win->RPort,x,GRAPH_Y+graph_height-1);
      }
   }
stopplay:
SetMenuStrip(Window,Titles);   /* turn on menus */
/* DeletePort(playreps); */
}


#if 0
/***!!!!!!!! obsolete see handler_io.c ***/
void send_msg(msg, cmd, arg1, arg2, arg3, arg4, arg5, flags)
struct play_msg *msg;
ULONG cmd, arg1, arg2, arg3, arg4, arg5;
{
struct play_msg *GetMsg();
struct play_msg *rep_msg;
struct MsgPort *msg_rep_port;

msg->msgtype = cmd;
msg->arg1 = arg1;
msg->arg2 = arg2;
msg->arg3 = arg3;
msg->arg4 = arg4;
msg->arg5 = arg5;
msg->flags = flags;

msg_rep_port=msg->message.mn_ReplyPort;

/* printf("sending cmd %d...",cmd); */

PutMsg(child_play_port, msg);
WaitPort(msg_rep_port);
if ((rep_msg=GetMsg(msg_rep_port))!=msg)
   printf("ERROR--reply type %d ",rep_msg->msgtype);
/*   printf("Replyed\n"); */
}

#endif