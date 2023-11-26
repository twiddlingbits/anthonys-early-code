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

#define CMD_SENDCMD		21
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

/** MSG & CMD flags for play_msg struct **/

#define LAST_BLOCK (1<<0)
#define FREE_MSG (1<<1)

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

#endif

