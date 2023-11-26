#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE
#define FLASE 0
#endif


/** memory allocation macros ***/

#define RUP(x) ( ((x)+7)&0xFFFFFFF8 )
#define RDN(x) ( (x)&0xFFFFFFF8 )


#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define FORM MakeID('F','O','R','M')
#define BODY MakeID('B','O','D','Y')
#define SVX  MakeID('8','S','V','X')
#define BMHD MakeID('B','M','H','D')
#define CMAP MakeID('C','M','A','P')
#define ILBM MakeID('I','L','B','M')
#define VHDR MakeID('V','H','D','R')
#define CHAN MakeID('C','H','A','N')

#define INTUITION_REV 33
#define MAX_SAMPLES 6
#define REV_VID 1
#define REG_VID 0
#define SOUNDPREC 127
#define RIGHT 4
#define LEFT 2
#define STEREO 6

#define BLUE 0
#define WHITE 1
#define BLACK 0
#define RED 5

#define IHI 50      /* height of getint requester */
#define SHI 30

#define BUFSIZE 512

/** Define location and size of area to put sample status information at **/

#define STAT_X 350
#define STAT_Y 125
#define STAT_W 260
#define STAT_H 53

#define GRAPH_X 92
#define GRAPH_Y 16
#define GRAPH_W 455
#define GRAPH_H 56

#define LGRAPH_X 92
#define LGRAPH_Y 87
#define LGRAPH_W 211
#define LGRAPH_H 23

#define RGRAPH_X 335
#define RGRAPH_Y 87
#define RGRAPH_W 211
#define RGRAPH_H 23

#define ENDUP_X 450
#define ENDUP_Y 85
#define ENDUP_W 100
#define ENDUP_H 25

#define ENDDN_X 335
#define ENDDN_Y 85
#define ENDDN_W 100
#define ENDDN_H 25

#define STARTDN_X 92
#define STARTDN_Y 85
#define STARTDN_W 100
#define STARTDN_H 25

#define STARTUP_X 200
#define STARTUP_Y 85
#define STARTUP_W 100
#define STARTUP_H 25


/* define widht, height, and location of buttons */

#define BUT_W 40
#define BUT_H 14

#define PLAY_RNG_X 13
#define PLAY_RNG_Y 15
#define PLAY_LP_X 13
#define PLAY_LP_Y 32
#define STOP_X 13
#define STOP_Y 49
#define SET_PLAYBACK_X 15
#define SET_PLAYBACK_Y 79
#define FIND_LOOP_X 15
#define FIND_LOOP_Y 96
#define DEL_RNG_X 586
#define DEL_RNG_Y 15
#define PASTE_X 586
#define PASTE_Y 32
#define INSERT_X 586
#define INSERT_Y 49
#define NEW_SLOT_X 586
#define NEW_SLOT_Y 79
#define APPEND_X 586
#define APPEND_Y 96

#define REC_X 40
#define REC_Y 104
#define REC_X2 210
#define REC_Y2 136
#define REC_ZERO 120
#define REC_SCALE 8

#define PS_3 1
#define PS_2 0

/** loop point search modes **/

#define INC_SM 0
#define INC_EM 1
#define DEC_SM 2
#define DEC_EM 3

/** flags passed to the graph() subroutine **/

#define GRAPH_ZOOM 1
#define GRAPH_LEFT 2
#define GRAPH_RIGHT 4

/** flags **/

#define CHIP 1
#define LOOP 2
#define LOOPING 4

struct Samp {
   char *name;
   char *lmem;
   char *rmem;
   int  length;
   int  rate;
   int  type;
   int  oneshot;
   int  repeat;
   int  cycles;
   char ctoctave;
   unsigned long flags;
   };


