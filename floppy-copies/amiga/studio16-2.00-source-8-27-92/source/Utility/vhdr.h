#define ID_8SVX MakeID('8','S','V','X')
#define ID_VHDR MakeID('V','H','D','R')

typedef LONG Fixed;

#define Unity 0x10000L;

#define sCmpNone	0
#define sCmpFibDelta 1

typedef struct {
	ULONG	oneShotHiSamples,
			repeatHiSamples,
			samplesPerHiCycle;
	UWORD	samplesPerSec;
	UBYTE	ctOctave,
			sCompression;
	Fixed	Volume;
	} Voice8Header;

