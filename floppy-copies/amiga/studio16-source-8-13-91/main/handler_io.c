#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "intuition/intuition.h"
#include "/include/psound.h"

/** local static variables **/

#define NUM_MSGS 30

static struct play_msg msg_array[NUM_MSGS];

struct play_msg *get_free_msg();
void free_send_msg();

extern struct MsgPort *parent_play_port;

/*############################################################
NAME  - handle_io_msg

FUNCTION
   This function is called every time a message is received 
   from the audio handler task.  This function does several things:
   1. If the message is a CMD, it must be a reply to a send_msg.
      In this case the appropriate msg_array is freed for re-use.
   2. if it is a MSG, then the appropriate functions is called.
INPUTS
   NONE
RESULTS
   NONE
##############################################################*/

void handle_io_msg()
{
struct play_msg *msg;

while (msg=(struct play_msg *)GetMsg(parent_play_port)) {
   if (msg->msgtype < 100) {     /** all CMD_ < 100 **/
/*   printf("handle_io: REPLY'd msg=%x type=%d_\n",msg,msg->msgtype); */
      free_send_msg(msg);
      }
   else {
      switch (msg->msgtype) {
      case MSG_READY_FOR_DATA:
         msg_ready_for_data(msg);         /** This guy Reply(msg)'s */
         break;
      case MSG_HERES_DATA:
         msg_heres_data(msg);         /** This guy Reply(msg)'s */
         break;
      default:
         telluser("INTERNAL ERROR!","unknown msg from audio handler");
      }
      }
   }
}

/*############################################################
NAME  - send_msg

FUNCTION
   This function sends the request command to an audio handler.
   The command is sent as a message via PutMsg() and a reply is
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
extern struct MsgPort *child_play_port;

if (msg=get_free_msg()) {

   msg->msgtype = cmd; 
   msg->arg1 = arg1;
   msg->arg2 = arg2;
   msg->arg3 = arg3;
   msg->arg4 = arg4;
   msg->arg5 = arg5;
   msg->flags |= flags;

/* printf("sending cmd %d...",cmd); */

   PutMsg(child_play_port, msg);
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
   msg_array[i].message.mn_ReplyPort = parent_play_port;
   }
}

/*************************************************************/

struct play_msg *get_free_msg()
{
int i;
Forbid();      /** so this code is re-entrant! **/
for (i=0; i < NUM_MSGS; i++) {
   if (msg_array[i].flags&FREE_MSG) {
      msg_array[i].flags=0;
      Permit();
      return(&msg_array[i]);
      }
   }
Permit();
telluser("INTERNAL ERROR!!!","no free send_msg!");
return(NULL);
}

void free_send_msg(m)
struct play_msg *m;
{
m->flags=FREE_MSG;
}

