
#define BLACK 0
#define WHITE 1
#define LIGHT_BROWN 2
#define BROWN 3
#define DARK_BROWN 4
#define RED 5
#define DARK_BLUE 6
#define LIGHT_BLUE 7

#define OPEN_OFFSETY 15
#define MAX_CUES 100
#define MAX_LIST_SIZE 100

#define KNOB_3D 1

/** handle_list() return codes **/

#define NO_CODE       0    /** 0 returned if nothing interesting happened **/
#define DOUBLE_SELECT 1    /** user clicked on selected item in list **/
#define NEW_SELECT    2    /** user clicked on new list entry **/

struct knob {
   short knob_height;
   short knob_y;
   short box_x1,box_y1;
   short box_x2,box_y2;
   struct Window *kw;
   short list_size;
   short show_size;
   short top_element;
   unsigned short flags;
   unsigned char APen;
   unsigned char BPen;
   struct list *list;
   };

struct list {
   short current;
   short box_x;
   short box_y;
   struct Window *lw;
   short list_size;
   short show_size;
   short top_element;
   short max_show_chars;
   struct knob *knob;
   unsigned char APen;
   unsigned char BPen;
   char *list_text[MAX_LIST_SIZE];
   };

struct cue_event {
   struct cue_event *next;
   struct cue_event *prev;
   struct Disk_Samp *samp;       /* put pointer to region structure here!!!*/
   short frames;                 /* put time code struct here!!!!!!*/
   short seconds;
   short minutes;
   short hours;
   int play_offset;
   int play_length;
   int fade_in_length;           /* what about cross fade types???*/
   int fade_out_length;
   int loop_count;
   unsigned long flags;
   char event_text[80];
   char description[40];
   };

struct time_code {
   short hours;
   short minutes;
   short seconds;
   short frames;
   };

#define CMD_BEGIN_PLAYING 1
#define CMD_QUE_PLAY 2
#define CMD_DATA 3
#define CMD_STOP_PLAYING 4
#define CMD_INFO 5
#define CMD_TEST 6
#define CMD_CLOSEDOWN 7

#define MSG_READY_FOR_DATA 0
#define MSG_I_AM_ALIVE 1

struct play_msg {
   struct Message message;
   unsigned long msgtype;
   unsigned long arg1;
   unsigned long arg2;
   unsigned long arg3;
   unsigned long arg4;
   unsigned long arg5;
   int return_error;
   };

