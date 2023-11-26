#include <exec/types.h>
#include <exec/ports.h>
#include "/include/psound.h"

/* create the 'play' AmigaDos Process */
/* Creates a "parent_play_port", loads the "play.exe" code, and creates */
/* an AmigaDos Proc to execuate it in.  Then waits for a message to arrive*/
/* at the recentely created parent_play_port.  This is a "hello" type msg */
/* from the recently created child Proc signiling that is is ok and ready */
/* to recieve commands */

struct MsgPort *parent_play_port;
struct MsgPort *child_play_port;

struct MsgPort *FindPort();
static int play_seg;
static struct MsgPort *dfltport;

load_play()
{
struct MsgPort *CreatePort();
struct MsgPort *CreateProc();
struct play_msg *GetMsg();
struct play_msg *msg;

parent_play_port = CreatePort("ParentPlayPort",0);
if (parent_play_port==0) {
   printf("cant create playport!!!!!\n"); /** FIX ERROR CLEAN UP !!!!! **/
   return(-1);
   }

play_seg = LoadSeg("play.exe");
if (play_seg==0) {
   DeletePort(parent_play_port);
   printf("Cant Load segment!!!!!!!!!!\n");
   return(-1);
   }

dfltport=CreateProc("PlayProc",0,play_seg,8000);
if (dfltport==0) {
   DeletePort(parent_play_port);
   UnLoadSeg(play_seg);
   printf("Can't create process!!!\n");
   return(-1);
   }

/** Need to add a timeout if no message recieved. Then unload seg!!!! **/


WaitPort(parent_play_port);      /** Wait for MSG_I_AM_ALIVE **/
printf("alive: %x\n",msg=GetMsg(parent_play_port));    /** throw away msg **/
child_play_port = (struct MsgPort *)msg->arg1;
ReplyMsg(msg);

printf("Play server process successfully launched!\n");

/*******
printf("parentport %x\n",parent_play_port);
send_test(1);
send_test(3);
send_test(5);
send_test(7);
printf("parentport %x\n",parent_play_port);
printf("getmsg %x\n",GetMsg(parent_play_port)); *******/

return(0);
}

send_test(val)
int val;
{
extern struct MsgPort *parent_play_port;
extern struct MsgPort *child_play_port;
struct play_msg msg;
struct play_msg *rep_msg;
struct play_msg *GetMsg();

setmem(&msg, sizeof(struct play_msg), 0);
msg.message.mn_Node.ln_Type = NT_MESSAGE;
msg.message.mn_ReplyPort = parent_play_port;
msg.msgtype = CMD_TEST;
msg.arg1 = val;
PutMsg(child_play_port, &msg);
WaitPort(parent_play_port);      /** Wait for reply **/
rep_msg=GetMsg(parent_play_port);        /** get and ignore reply **/
printf("&msg %x rep_msg %x msg %x\n",&msg, rep_msg, msg);
if (rep_msg!=(&msg)) printf("test reply error!\n");
printf("return error %d\n",rep_msg->return_error);
return((int)rep_msg->return_error);
}

void unload_play()
{
extern struct MsgPort *parent_play_port;
extern struct MsgPort *child_play_port;
struct play_msg msg;
struct play_msg *rep_msg, *GetMsg();

/* printf("unload play\n"); */

if (child_play_port) {
   setmem(&msg, sizeof(msg), 0);
   msg.message.mn_ReplyPort = parent_play_port;
   msg.msgtype = CMD_CLOSEDOWN;
   msg.message.mn_Node.ln_Type = NT_MESSAGE;
   PutMsg(child_play_port, &msg);
   WaitPort(parent_play_port);      /** Wait for reply **/
   rep_msg=GetMsg(parent_play_port);        /** get and ignore reply **/
   if (rep_msg != (&msg)) printf("unload: reply error!\n");
   }

if (dfltport)
   DeleteTask((int)dfltport - sizeof(struct Task));
if (play_seg)
   UnLoadSeg(play_seg);
if (parent_play_port)
   DeletePort(parent_play_port);
}


