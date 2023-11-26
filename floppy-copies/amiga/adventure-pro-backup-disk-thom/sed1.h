
#define MakeID(a,b,c,d) ( (a)<<24|(b)<<16|(c)<<8|(d) )
#define FORM MakeID('F','O','R','M')
#define BODY MakeID('B','O','D','Y')
#define SVX  MakeID('8','S','V','X')
#define BMHD MakeID('B','M','H','D')
#define CMAP MakeID('C','M','A','P')
#define ILBM MakeID('I','L','B','M')
#define VHDR MakeID('V','H','D','R')
#define CHAN MakeID('C','H','A','N')

#define INTUITION_REV 29
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
#define BLACK 2
#define RED 3
#define BUFSIZE 2*1024

#define SCALE 7
#define GBOTY GPP-6      /* bottom of graph */

struct Samp {
   char *name;
   char *lmem;
   char *rmem;
   int  length;
   int  rate;
   int  type;
   ULONG sm;
   ULONG em;
   int  oneshot;
   int  repeat;
   int  cycles;
   char ctoctave;
   };

