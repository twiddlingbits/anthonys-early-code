/********************************/
/**   Perfect Sound (C) 1986   **/
/**    Anthony J. Wood         **/
/**                            **/
/** FILE 1 of 4                **/
/**                            **/
/** This Source code may be    **/
/** copied for backup only.    **/
/** This Source code is for    **/
/** reference only, you may    **/
/** NOT modify and compile     **/
/** this source except for     **/
/** PERSONAL use. Permission   **/
/** is given to distribute     **/
/** copies of the object code  **/
/** generated from compiling   **/
/** Perfect Sound ONLY if the  **/
/** source code is not changed **/
/** in any way. Permision is   **/
/** granted to use subroutines **/
/** in this file within your   **/
/** own programs if you notify **/
/** SunRize Industries of your **/
/** intent to do so and        **/
/** SunRize Industries agrees  **/
/** to your request.           **/
/**                            **/
/**  SunRize Industries        **/
/**  PO Box 1453               **/
/**  College Station, TX 77841 **/
/**  (409) 846-1311            **/
/**                            **/
/********************************/

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

#define SCALE 7
#define GBOTY GPP-6      /* bottom of graph */

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
