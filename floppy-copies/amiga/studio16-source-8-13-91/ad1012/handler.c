#include "exec/types.h"
#include "exec/ports.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "/include/psound.h"

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

arg1:   channel number
arg2:   sampling rate
arg3:   volume
arg4:   sample length

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


NOTES
=====

Current verions of play handlers assume that CMD's will never be sent out of
sequence.  For example, if PS12 send a CMD_DATA followed by a
CMD_STOP_PLAYING, this code will lock up.  The PS12 will be waiting for a
reply to its CMD_STOP_PLAYING and the play handler will be waiting for a
reply to its MSG_READY_FOR_DATA!

To avoid this problem, PS12 will send CMD_STOP_PLAYING instead of a
CMD_DATA.

*/


ULONG DOSBase;
ULONG SysBase;


struct MsgPort *child_play_port;
struct MsgPort *parent_play_port;

void free_ioblocks();
void setup();
struct play_msg *get_free_msg();
void init_send_msg();
void free_send_msg();
void send_msg();

#define NUM_MSGS 10
static struct play_msg msg_array[NUM_MSGS];

void main()
{
struct play_msg *GetMsg();
struct play_msg *msg;
int unit_num,rate,vol,len;
int sigbits;
int bufindex;
int thislen;
int j;
int more;
short empty_buffers;


setup(); 
   
empty_buffers=0;     /** get rid of compiler errors **/
more=FALSE;


send_msg(MSG_I_AM_ALIVE, child_play_port, 0, 0,0,0,0);

/** Now wait for a command from the Main (parent) perfect sound **/

while (TRUE) {
   sigbits=Wait( /*(1<<int_sigbit)|*/(1<<child_play_port->mp_SigBit));

/*** first handle any buffers that are done playing ***/

   if (sigbits&(1<<iobport->mp_SigBit)) {
   /***** HERE IF CARD SIGNALED THAT IT WANTS MORE DATA ****/
      }
   
/*** now handle any messages recieved from studio16 ***/

   while (msg=(struct play_msg *)GetMsg(child_play_port)) {
      if (msg->msgtype >= 100) {     /** all CMD_ < 100 **/
         free_send_msg(msg);
         }
      else {
         switch (msg->msgtype) {
         case CMD_TEST:
            msg->return_error = msg->arg1;
            ReplyMsg(msg);
            break;

         case CMD_CLOSEDOWN:
            DeletePort(child_play_port);
            free_ioblocks();
            msg->return_error = 0;
            ReplyMsg(msg);
            Wait(0);
            break;

         case CMD_BEGIN_PLAYING:
            msg->return_error = 0;
            unit_num = msg->arg1;
            rate = msg->arg2;
            vol  = msg->arg3;
            len  = msg->arg4;
            ReplyMsg(msg);

            break;
               
         case CMD_STOP_PLAYING:
            ReplyMsg(msg);
            break;

         case CMD_SET_FREQVOL1:
            /*** PUT SENDW()'s here ***/
            break;

         default: 
            ReplyMsg(msg);
            }
         }
      }
   }
}



/**********************************************************/

void setup()
{
void deallocate_channel();
void free_ioblocks();
struct play_msg *GetMsg();

SysBase = *((ULONG *)4);

/**** allocate "double buffers" for audio device ****/

bufptr[0]=buf0;      /** change if NUMBUFFERS != 4 **/
bufptr[1]=buf1;
bufptr[2]=buf2;
bufptr[3]=buf3;

/* allocate audio channels 1 and 2 */

if (allocate_channel (&IOB)==FALSE) {
   Wait(0);
   }

if (alloc_ioblocks(&IOB)) {
   deallocate_channel(&IOB);
   Wait(0);
   }

parent_play_port = (struct MsgPort *)FindPort("ParentPlayPort");
if (parent_play_port==0) {             /** This should never happen **/
   free_ioblocks();
   deallocate_channel(&IOB);
   Wait(0);
   }

child_play_port=(struct MsgPort *)CreatePort(0,0);       /** create private port **/
if (child_play_port==0) {
   free_ioblocks();
   deallocate_channel(&IOB);
   Wait(0);
   }

init_send_msg();
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

/** PROBLEM:

Since the get_sample() buffers are in the samp structure, only one task
at a time can access the sample.  This prevents different regions of the
same sample from playing similtaneously.

Possible solution: have a segment/region structure that conatins the
buffer information and global pointers. have the samp structure contain
a linked list of segment/region's associated with the sample.

Alternatly, make the segment/region list totaly seperated from the sample
structure.

**/

#if 0    /***!!!!!!! OLD STUFF FOR REFERENCE  ****/

            do {
               while (more && empty_buffers) {
                  send_msg(MSG_READY_FOR_DATA, 0, 0, 0,0,0,0);
                  WaitPort(child_reply_port);            /** wait for reply **/
                              msg=GetMsg(child_reply_port);          /** Discard reply **/

                 WaitPort(child_play_port);             /** wait for data **/
                  msg=GetMsg(child_play_port);
                  if (msg->msgtype!=CMD_DATA) {   /** CMD_STOP_PLAYING activates this */
                     ReplyMsg(msg);
                     more = FALSE;
                     break;
                     }
                  for (j=0; j<BLOCK_SIZE; j++) {
                     bufptr[bufindex][j] = ((short *)(msg->arg1))[j]>>8;
                     }
                  if (msg->flags & LAST_BLOCK) more=FALSE;
                  thislen = msg->arg2;
                  ReplyMsg(msg);       /** ReplyMsg after copying its data **/
                  IOBlks[bufindex]->ioa_Request.io_Unit=(struct Unit *)unit_num;
                  tochan (IOBlks[bufindex],bufptr[bufindex],thislen,toperiod(rate),1,vol);
                  bufindex++;
                  if (bufindex==NUMBUFFERS) bufindex = 0;
                  empty_buffers--;
                  }
            
               if (more) {
                  Wait(1<<iobport->mp_SigBit);  /** wait for a playing block to finish **/
                  while (msg=GetMsg(iobport)) { /** get AudioDevice CMD_WRITE replys **/
                     empty_buffers++;           /** every done block frees a buffer **/
                     }
                  }
               else {      /** wait for all playing buffers to finish **/
                  while (empty_buffers < NUMBUFFERS) {
                     WaitPort(iobport);
                     GetMsg(iobport);
                     empty_buffers++;
                     }
                  }
            
               } while (more);
            
            IOB.ioa_Request.io_Unit=(struct Unit *)unit_num;
            IOB.ioa_Request.io_Command=CMD_RESET;
            DoIO (&IOB);
            break;
#endif
