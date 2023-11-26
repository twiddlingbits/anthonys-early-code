#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE
#define FLASE 0
#endif


/** gadget id's **/

#define START 0
#define END 1
#define POSN 2
#define PLAYRANGE 3
#define GRAPH 4
#define STOP 5

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
#define MAX_SAMPLES 14
#define REV_VID 1
#define REG_VID 0
#define SOUNDPREC 127
#define RIGHT 4
#define LEFT 2
#define STEREO 6
#define GEP 147
#define GSP GEP-13
#define GPP GSP-13
#define GPR 163
#define GPW 179
#define EDGE_GR 525
#define EDGE_GW 525
#define BLUE 0
#define WHITE 1
#define BLACK 0
#define RED 3

#define SCALE 7
#define GBOTY 75      /* bottom of graph */

#define SHOW 8      /* no. of samples to display in load requester */
#define IHI 50      /* height of getint requester */
#define SHI 30

#define BUFSIZE 2*1024

/** Define location and size of area to put sample status information at **/

#define STAT_X 415
#define STAT_Y 127
#define STAT_W 200
#define STAT_H 53

#define GRAPH_X 92
#define GRAPH_Y 16
#define GRAPH_W 455
#define GRAPH_H 55

#define LGRAPH_X 92
#define LGRAPH_Y 87
#define LGRAPH_W 211
#define LGRAPH_H 23

#define RGRAPH_X 335
#define RGRAPH_Y 87
#define RGRAPH_W 211
#define RGRAPH_H 23

/* define widht, height, and location of buttons */

#define BUT_W 40
#define BUT_H 14

#define PLAY_X 13
#define PLAY_Y 15
#define PLAY_RNG_X 13
#define PLAY_RNG_Y 24
#define STOP_X 13
#define STOP_Y 49
#define SET_PLAYBACK_X 15
#define SET_PLAYBACK_Y 79
#define LOOP_X 15
#define LOOP_Y 96
#define RECORD_X 297
#define RECORD_Y 124
#define DELETE_X 586
#define DELETE_Y 15
#define DEL_RNG_X 586
#define DEL_RNG_Y 32
#define INSERT_X 586
#define INSERT_Y 49
#define PASTE_X 586
#define PASTE_Y 79
#define MIX_X 586
#define MIX_Y 96

struct Samp {
   char *name[MAX_SAMPLES];
   char *lmem[MAX_SAMPLES];
   char *rmem[MAX_SAMPLES];
   int  length[MAX_SAMPLES];
   int  rate[MAX_SAMPLES];
   int  type[MAX_SAMPLES];
   ULONG sm[MAX_SAMPLES];
   ULONG em[MAX_SAMPLES];
   int  oneshot[MAX_SAMPLES];
   int  repeat[MAX_SAMPLES];
   int  cycles[MAX_SAMPLES];
   char ctoctave[MAX_SAMPLES];
   };
