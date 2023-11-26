#include "/include/psound.h"

static int recording=FALSE;
struct Disk_Samp *recording_samp;

record_sound(name)
char *name;
{
struct play_msg *GetMsg();
struct MsgPort *CreatePort();
void send_msg();
int i;

if (recording) {
   recording=FALSE;
   send_msg(CMD_STOP_RECORDING, 1 ,0, 0, 0, 0, 0);
   }
else {
   recording = TRUE;
   i=create_sample(name, 0, &recording_samp);
   if (i!=NO_ERROR) {
      return(i);
      }
   recording_samp->rate=10000;
   recording_samp->type=RIGHT;

/** PUT IN ERROR DETECTION STUFF ***/

   send_msg(CMD_BEGIN_RECORDING, 1, recording_samp->rate, 0, 0, 0,0);
   }
}

/*******************************************************/
/*** called every time there is a MSG_HERES_DATA ***/
/*******************************************************/

void msg_heres_data(msgtorep)
struct play_msg *msgtorep;
{
extern struct list *open_list;

if (recording) {
   quick_dump(recording_samp, msgtorep->arg1);

   if (msgtorep->flags & LAST_BLOCK) {
      recording=FALSE;
      update_block1(recording_samp,0);
      add_list_entry(open_list, recording_samp->name, recording_samp);
      }
   }
ReplyMsg(msgtorep);       /** ReplyMsg after copying its data **/
}
