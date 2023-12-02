
#define AOK 0
#define OUTOFMEM 1
#define DISKERROR 2
#define CANTOPENFILE 3
#define WRONGFILETYPE 4
#define BADIFF 5

#define RAW_DATA_SIZE 256000

#define BLACK_WHITE_ONLY 1

#define BUSY 0
#define ZERO 1
#define NORMAL 2


#define ABS(x) (((x)>=0) ? (x) : (-(x)))
#define MAX(x,y) (((x) >= (y)) ? (x) : (y))
#define MIN(x,y) (((x) <= (y)) ? (x) : (y))
#define L(x) (MAX(MIN(x,255),0))

/** flags **/

#define IMG_BW 1
#define IMG_LOG 2
#define IMG_NOLACE 4

struct config {
   int flags;
   int v_dpi;
   int scan_time;
   int init_len;
   int v_feed_len;
   int h_feed_len;
   int h_return_len;
   int rev_feed_len;
   int actual_height;
   int return_delay;
   int scan_height;
   int scan_offset;
   int scan_delay;
   long raw_data_size;
   short sw;
   short sh;
   unsigned char init[20];
   unsigned char v_feed[20];
   unsigned char h_feed[73];
   unsigned char h_return[73];
   unsigned char rev_feed[20];
   unsigned char *rawdata;
   unsigned char translate[256];
   };

