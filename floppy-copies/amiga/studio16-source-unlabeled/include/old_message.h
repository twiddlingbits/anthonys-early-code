#ifndef MESSAGE_H
#define MESSAGE_H TRUE

/** play handler commands (always < 100)**/

#define CMD_BEGIN_PLAYING 	1
#define CMD_QUE_PLAY 		2
#define CMD_DATA 		3
#define CMD_STOP_PLAYING 	4
#define CMD_INFO 		5
#define CMD_TEST 		6
#define CMD_CLOSEDOWN 		7

#define CMD_SET_INPUT_GAIN	8
#define CMD_SET_CUTOFF_FREQ	9
#define CMD_SET_SAMPLING_RATE	10

#define CMD_MONITOR_ON	11
#define CMD_MONITOR_OFF	12
#define CMD_DELAY_ON	13

#define CMD_SET_VAR0	14
#define CMD_SET_VAR1	15
#define CMD_SET_VAR2	16
#define CMD_SET_VAR3	17
#define CMD_SMPTE_ON	18
#define CMD_SMPTE_OFF	19

#define CMD_RESET_DSP	20
#define CMD_DelayBuff	21

#define CMD_SENDCMD		28
#define CMD_BEGIN_RECORDING 22
#define CMD_STOP_RECORDING  23
#define CMD_DELAY_OFF	24
#define CMD_HILO_OFF	25
#define CMD_HILO_ON	26
#define CMD_SET_CHANNEL_VOLUME 27

/** messages (sent form handler to studio16) are 100 and greater **/

#define MSG_READY_FOR_DATA 100
#define MSG_I_AM_ALIVE 101
#define MSG_CAN_NOT_FIND_BORED 102
#define MSG_CAN_NOT_OPEN_EXPANSIONBASE 103
#define MSG_HERES_DATA 104
#define MSG_HILO_INFO 105
#define MSG_PRINTME 106
#define MSG_SMPTE_SF 107
#define MSG_SMPTE_HM 108
#define MSG_CHAN_DONE 109

/** MSG & CMD flags for play_msg struct **/

#define LAST_BLOCK (1<<0)
#define FREE_MSG (1<<1)
#define LAST_MSG (1<<2)

#define BIG_BLOCK_SIZE 65536
/* #define BIG_BLOCK_SIZE 256 */

struct play_msg {
   struct Message message;
   unsigned long msgtype;
   unsigned long flags;
   unsigned long  return_error;
   unsigned long arg1;
   unsigned long arg2;
   unsigned long arg3;
   unsigned long arg4;
   unsigned long arg5;
   };

/**** studio 16 events ******/

#define MAX_EVENT_TYPE 100

#define SE_EDIT_SAMPLE 0
#define SE_LOAD_SAMPLE 1
#define SE_SAVE_SAMPLE 2
#define SE_SAVE_AS_SAMPLE 3
#define SE_SAMPLE_CREATED 4	/* arg1: *Disk_Samp */
#define SE_SAMPLE_DELETED 5	/* arg1: int (sample index deleted) */
#define SE_MODULE_ADDED 6		/* arg1: *module */
#define SE_MODULE_DELETED 7	/* arg1: int (module index deleted) */
#define SE_MODULE_SELECTED 8	/* arg1: *module */
#define SE_KILL_MODULE 9		/* no args (!! change so arg1: *Module) */
#define SE_PLAY_SAMPLE 10		/* arg1: *Disk_Samp, arg2: int (start), arg3: int (len) */
#define SE_HILO_INFO 11
#define SE_HILO_OFF	12
#define SE_HILO_ON	13
#define SE_SET_CHANNEL_VOLUME 14
#define SE_STOP_CHAN 15			/* arg1: unit_num */

/** Events used when recording or playing to comunicate between
	the DiskIO module and the card handler **/

/** RESERVED 20-30 for _IO_ messages **/

#define SE_IO_BEGIN_PLAYING 20/*arg1: unit_num, arg2: samp->rate, arg3:vol, arg4:len */
#define SE_IO_STOP_PLAYING	21	/* arg1: unit_num */
#define SE_IO_PLAY_DATA 22			/* arg1: short * (data), int (len),int (unit_num). flags: LAST_BLOCK */
#define SE_IO_READY_FOR_DATA 23	/* arg1: unit_num */
#define SE_IO_RECORD_DATA 24
#define SE_IO_CHAN_DONE 25			/* arg1: unit_num */

/** RESERVED 50 - 99 for Todd stuff **/
/** RESERVED 0 - 49 for AW stuff **/

#endif

