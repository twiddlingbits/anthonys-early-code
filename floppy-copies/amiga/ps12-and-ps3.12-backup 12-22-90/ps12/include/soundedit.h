#ifndef SOUNDEDIT_H
#define SOUNDEDIT_H TRUE

#define DEMO FALSE

/** After changing DEMO, re-compile main.c **/

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
#define DUMP MakeID('D','U','M','P')
#define DP16 MakeID('D','P','1','6')

#define INTUITION_REV 33
#define MAX_SAMPLES 50
#define REV_VID 1
#define REG_VID 0
#define SOUNDPREC 127
#define RIGHT 4
#define LEFT 2
#define STEREO 6

#define IHI 50      /* height of getint requester */
#define SHI 30

/** Define location and size of area to put sample status information at **/

#define STAT_X 350
#define STAT_Y 125
#define STAT_W 260
#define STAT_H 53

#define GRAPH_X 15
#define GRAPH_Y 16
#define GRAPH_W 350
#define GRAPH_H 56

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

#define LOOP 2
#define LOOPING 4

#endif

