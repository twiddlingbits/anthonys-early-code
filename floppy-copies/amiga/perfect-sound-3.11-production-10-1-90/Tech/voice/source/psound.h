/** COPYRIGHT 1990 ANTHONY WOOD **/

#ifndef TRUE 
#define TRUE 1
#endif

#ifndef FALSE
#define FLASE 0
#endif


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
#define MAX_SAMPLES 8
#define RIGHT 4
#define LEFT 2
#define STEREO 6

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


