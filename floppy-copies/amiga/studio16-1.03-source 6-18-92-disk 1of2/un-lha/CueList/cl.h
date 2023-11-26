/**********************************/
/**********************************/
struct CueModState
	{
	struct StandardModState state;
	int PlayFlags;
	int MaxNumOfSndsPreloaded;
	int Reserved[20];
	};
/**********************************/
#define CEF_WAITING	(1<<0)
#define CEF_PLAYING	(1<<1)
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
	int flags;
	struct ActiveSample *as;
	ULONG UnloadSigMask;
	struct Task *task;
	};
/**********************************/
#define  PP_TRIGGER_NOW		(1<<0)
#define  PRELOAD_TIME 0x00003000
