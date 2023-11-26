#ifndef PLAY_H
#define PLAY_H TRUE

/** play handler commands **/

#define CMD_BEGIN_PLAYING 1
#define CMD_QUE_PLAY 2
#define CMD_DATA 3
#define CMD_STOP_PLAYING 4
#define CMD_INFO 5
#define CMD_TEST 6
#define CMD_CLOSEDOWN 7

#define MSG_READY_FOR_DATA 100
#define MSG_I_AM_ALIVE 101

/** MSG & CMD flags for play_msg struct **/

#define LAST_BLOCK 1

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

