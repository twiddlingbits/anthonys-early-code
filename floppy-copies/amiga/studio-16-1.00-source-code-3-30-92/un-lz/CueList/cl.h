/**********************************/
/**********************************/
struct CueEntry {
	char line[101];
	long Start;
	char StartTimeStr[12];
	long End;
	int Vol;
	char VolStr[4];
	char Sample[40];
	char Rem[40];
	int pcnt;
	struct ActiveSample *as;
	ULONG UnloadSigMask;
	struct Task *task;
	};
/**********************************/
#define  PP_TRIGGER_NOW		(1<<0)
#define  PRELOAD_TIME 0x00003000
