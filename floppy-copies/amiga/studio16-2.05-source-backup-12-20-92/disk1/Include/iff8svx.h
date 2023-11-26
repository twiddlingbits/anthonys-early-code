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
#define AIFF MakeID('A','I','F','F')
#define COMM MakeID('C','O','M','M')
#define SSND MakeID('S','S','N','D')

typedef LONG Fixed;

#define Unity 0x10000L;

#define sCmpNone	0
#define sCmpFibDelta 1

struct  Voice8Header {
	ULONG	oneShotHiSamples,
			repeatHiSamples,
			samplesPerHiCycle;
	UWORD	samplesPerSec;
	UBYTE	ctOctave,
			sCompression;
	Fixed	Volume;
	};


#define INTUITION_REV 35
#define MAX_SAMPLES 50
#define REV_VID 1
#define REG_VID 0
#define SOUNDPREC 127
