/**********************************/
/**********************************/
struct CueModState
	{
	struct StandardModState state;
	int PlayFlags;
	int MaxNumOfSndsPreloaded;
	char path[STUDIO16_MAX_PATH_SIZE];
	int Reserved[20];
	};
/**********************************/
#define CEF_WAITING	(1<<0)
#define CEF_PLAYING	(1<<1)
/**********************************/
struct CueEntry {
	char line[101];
	struct Region regn;
	char Rem[40];
	int end;
	char path[STUDIO16_MAX_PATH_SIZE];
	int flags;
	ULONG UnloadSigMask;
	struct Task *task;
	};
/**********************************/
#define  PP_TRIGGER_NOW		(1<<0)
#define  PRELOAD_TIME 0x00050000
