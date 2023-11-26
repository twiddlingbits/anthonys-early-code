/* COPYRIGHT 1992 SunRize Industries */

#ifndef STUDIO16_H
#define STUDIO16_H TRUE

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif

#ifndef EXEC_LIBRARIES_H
#include <exec/libraries.h>
#endif

#ifndef EXEC_PORTS_H
#include <exec/ports.h>
#endif

#ifndef EXEC_SEMAPHORES_H
#include <exec/semaphores.h>
#endif

#ifndef GADLIB_H
#include "/include/gadlib.h"
#endif

#ifndef INTUITION_INTUITION_H
#include <intuition/intuition.h>
#endif

#ifndef DOS_H
#include <dos.h>
#endif

#ifndef INTERFACE_H
#include "/include/interface.h"
#endif

#ifndef SMPTE_H
#include "/include/smpte.h"
#endif

/** Version number **/

#define VERSION_STRING "2.05"
#define STUDIO16_CONFIG_ID 205

/** main screen's 3 bitplane colors **/

#define LIGHT_BROWN 0
#define WHITE 1
#define BROWN 2
#define BLACK 3
#define RED 4
#define DARK_BLUE 5
#define DARK_BROWN 6
#define LIGHT_BLUE 7

#define OPEN_OFFSETY 15

#define BLOCK_SIZE 256  /** 256 words is 512 bytes **/

/** low_level_io error return codes **/

#define NO_ERROR 0
#define CANT_OPEN_FILE 1
#define WRITE_ERROR 2
#define READ_ERROR 3
#define DISK_ERROR 4
#define OUT_OF_MEMORY 5
#define NOT_8SVX 6
#define BAD_IFF 7
#define DUPLICATE_NAME 8
#define STDERR_CANT_OPEN_SCREEN 9
#define STDERR_CANT_OPEN_WINDOW 10
#define STDERR_CANT_GET_WRITE_LOCK 11
#define STDERR_UNKNOWN_FILE_FORMAT 12
#define STDERR_CANT_GET_READ_LOCK 13
#define STDERR_CANT_DO_RENAME_TO_TEMP 14
#define STDERR_CANT_DO_RENAME_FROM_TEMP 15
#define STDERR_SINGLE_RESIDENT 16
#define STDERR_NO_TAG 17
#define STDERR_CANT_CREATE_SAMPLE 18
#define STDERR_CANT_OPEN_SAMPLE 19
#define STDERR_CANT_FIND_COPYBUFFER 20
#define STDERR_DATA_NOT_AVAILABLE_YET 21
#define STDERR_DATA_EOF 22
#define STDERR_SAMPLE_TOO_SMALL 23
#define STDERR_COPYBUFFER_IN_USE 24
#define STDERR_NO_MORE_CLIPS	25
#define STDERR_NO_STUDIO16_ASSIGN 26
#define STDERR_BAD_STUDIO16_ASSIGN 27
#define STDERR_CANT_DELETE_FILE 28
#define STDERR_CANT_FIND_REGION 29
#define STDERR_CANT_ADD_REGION 30
#define STDERR_CANT_FIND_SAMPLE 31
#define STDERR_INVALID_CONFIG_FORMAT 32
#define STDERR_COPYBUFFER_EMPTY 33

/** Maximum numnbe of characters in complete paths supported by studio 16 **/

#define STUDIO16_MAX_PATH_SIZE	120

/*** Define common paramaters **/

struct SampleInfo {
	int rate;						/** sampling rate **/
	int filter3db;					/** Anti-aliasing filter value **/
	unsigned short volume;		/** prefered volume level **/
	int starttimecode;
	float smpte_sampling_rate;
	int pan;
	long flags;
	long reserved[1];
	};

#define SI_TAP_OUTPUT		(1<<7)
#define SI_TAP_INPUT		(1<<8)

struct SampleDataClip {
	long start;			/** inclusive **/
	long end;			/** inclusive **/
	};

#define DS_NUM_CLIPS 128

#define REGION_DELIMITER_STRING ">"
#define REGION_DELIMITER_CHAR '>'
#define REGION_DELIMITER >

/** Regions have two parts: a portion saved to disk as part of **/
/** the SampleFileTag, and a second part filled by FindRegion() **/

struct DiskRegion {				/* DiskRegion is saved to disk */
	char region_name[40];
	unsigned long start;		 	/* starting sample, inclusive */
	unsigned long end;			/* ending sample, inclusive **/
	struct SampleInfo parms;	/** filter, rate, volume,etc. **/
	unsigned long flags;
	};

/** This is a RAM resident structure & contains additional **/
/** info over the DiskRegion struct **/

struct Region {

/**** Start: An EXACT copy of struct DiskRegion ******/

	char region_name[40];
	unsigned long start;		 	/* starting sample, inclusive */
	unsigned long end;			/* ending sample, inclusive **/
	struct SampleInfo parms;	/** filter, rate, volume,etc. **/
	unsigned long flags;

/**** End: An EXACT copy of struct DiskRegion ******/

	unsigned long user_data;
	char samp_name[STUDIO16_MAX_PATH_SIZE];	/** complete path **/
	void (*effect)();		/** if non-null, called by ASRead() **/
	long loop;	/** -1==loop forever, 0=no loops (play once) **/
	};

/** Shared Semaphores **/

struct SharedLock {
	struct MinNode node;
	struct Task *task;
	unsigned long sigbit;
	short count;
	};

struct SharedSemaphore {
	struct MinList pending_shared;
	struct MinList pending_exclusive;
	struct MinList granted_shared;
	struct SharedLock *granted_exclusive;
	};	


/*** studio 16's own internal preferences **/

#define SBD_WALLPAPER_ON (1<<0)
#define WIND_POPTOFRONT		(1<<1)
#define WIND_SHORTTITLEBAR	(1<<2)
#define SBD_USE_EXTENDED_MEMORY (1<<3)

struct defaults {
	unsigned long smpte_mode;
	char SmpteSourceName[30];
	int SmpteSourceUnit;
	unsigned short SmpteTimeOut;
/********/
	char samp_io_path[80];			/** Save As... * Open path **/
	char instance_config_file[80];
	char file_filter_name[80];
	char backdrop_pattern_name[80];
	char samp_paths[STUDIO16_MAX_PATH_SIZE*4]; /** NULL delimited, first entry is primary & write path **/
/********/
	unsigned short palette[16];
	short screen_width;
	short screen_viewmodes;
	struct GadgetColors colors;
/********/
	int copy_buf_size;
	int active_buf_size;			/* Value in Samples */
/********/
	unsigned long flags;
	unsigned short MeterSpeed;
	int record_safety_size;			/* Value in Samples */
	short undo_levels;
	int XXX[124];
	};

/** StudioBase flags **/

#define SB_SMPTE_ON		(1<<0)	/** Smpte time code running? **/
#define SB_APP_SCREEN	(1<<1)	/** Using an application's **/
											/** screen, not one opened by **/
											/** IntitStudio16UserEnv **/
struct StudioBase {
	struct Library	LibNode;
	int CurrentSmpteTime;
	int SmpteTimeOut;
	struct SmpteSource *CurrentSmpteSource;
	struct List SmpteSinks;
	struct List SmpteSources;
	struct defaults  defaults;  /* NOTE: Only change the structure beyond this point */

	struct MinList	modules;
	struct Screen	 *screen;
	struct Window	 *back_window;
	struct StudioEvent *event_list;
	char **eventidarray;
	short max_registered_ids;
	struct SharedSemaphore lockstudio;
	struct MinList *notifyarray;
	struct List handlers;
	struct List file_filters;
	struct List module_states;
	unsigned long flags;
	struct Task *quit_task;
	int    quit_sigbit;
	int    XXX[10];
	};


/** backwards compatibility **/

#define StudioBase StudioBase

/**!!!! make my knob & list gadets chained together, then put a
	pointer to the first gadget in the window's data pointer!!! */

struct NewModule {
	char *name;						 /* module's name */
	unsigned long state_flags;	 /* OR these flags into state IF STATE iS CREATED BY ADDMOD! */
	unsigned short state_size;		/* size of module's state struct */ 
	unsigned long mod_flags;		/* flags for this module */
	};

/** Module flags **/

/**  DELETE IF CCALL WORKS OK 
#define MOD_OPEN_AT_STARTUP 1
#define MOD_CALL_ALL_WINDOWS 2
#define MOD_DID_NOT_CLOSE 4
#define MOD_IDCMP_PRIVATE 8
***/

#define MOD_SINGLE_RESIDENT (1<<0)	/** Only one module allowed loaded **/
#define MOD_JUST_ADDED		 (1<<1)	/** Set by AddModule() **/

/** This is the actual Module structure linked into StudioBase **/

struct Module {
	struct MinNode node;			/* links all Modules together */
	struct MsgPort *notifyme;	/* module's event port */
	char *name;						/* module's name */
	unsigned long flags;			/* module's flags */
	APTR module_data;				/* for module's use */
	struct StandardModState *state;
	};

struct NotifyLink {
	struct MinNode node;
	struct MsgPort *notifyme;
	};

/*** Broadcast ****/

#define MAX_EVENT_TYPE 100

struct StudioEvent {
	struct Message message;
	short type;
	unsigned long flags;
	unsigned long arg1;
	unsigned long arg2;
	unsigned long arg3;
	unsigned long arg4;
	unsigned long arg5;
	struct MemLock *ml;
	struct MinNode node;
	};


/** Audio Channels **/

struct NewChannel {
	char	name[40];
	int	flags;
	int	num;		/* Phaze out this var */
	};

/** flags passed to AllocChan() & stored in ChanKey.flags **/

#define CK_CHAN_PLAY		(1<<0)
#define CK_CHAN_RECORD		(1<<1)
#define CK_CHAN_OUTPUT		(1<<2)
#define CK_CHAN_MONITOR		(1<<3)
#define CK_CHAN_INPUT		(1<<3)

#define CK_ID_ONLY		(1<<4)

#define CK_LEFT			(1<<5)			/** NOT USED YET **/
#define CK_RIGHT		(1<<6)			/** NOT USED YET **/
#define CK_TAP_OUTPUT		(1<<7)
#define CK_TAP_INPUT		(1<<8)

#define CHAN_PLAY	(1<<0)
#define CHAN_RECORD	(1<<1)
#define CHAN_OUTPUT	(1<<2)
#define CHAN_INPUT	(1<<3)
#define CK_CHAN_PAN	(1<<9)
#define CK_CHAN_IN_USE	(1<<10)
#define CK_CHAN_HILO	(1<<11)
#define CK_CHAN_STEREO	(1<<12)
#define CK_CHAN_VOLUME	(1<<13)
#define CK_CHAN_LEFT		(1<<14)
#define CK_CHAN_RIGHT		(1<<15)

/** example flag combos:
	CK_CHAN_PLAY			Alloc a Play channel
	CK_CHAN_PLAY|CK_LEFT		Alloc a play channel on the left chan
	CK_CHAN_RECORD			Alloc any record channel
	CK_CHAN_OUTPUT			Alloc Audio out as a record chan
	CK_CHAN_MONITOR|CK_ID_ONLY	Don't allocate resources
	

**/

/** Used to keep track of an allocated channel **/

struct ChanKey {
	struct AudioHandler *handler;
	unsigned long flags;
	char	name[36];
	int	use_count;
	byte	hilo_left;
	byte	hilo_right;
	struct ActiveSample *as;
	struct SampleInfo parms;
	APTR user_data;
	APTR handler_data;
	struct SampleInfo *state_parms;
	int	pad[47];
	int chan_num;		/* Phaze out this var */
	unsigned long chan_mask;		/* Phaze out this var */
	};

/** Audio Playback & Record releated **/

struct NewAudioHandler {
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long flags;
	short play_channels;
	short record_channels;
	short monitor_channels;
	short output_channels;
	short num_bits;
	short priority;
	short class_id;
	int	number_of_filter_settings;
	int	(*FilterValueFunc)();
	int	(*FindBestFilter)();
	int	number_of_gain_settings;
	int	(*GainValueFunc)();
	int	(*FindBestGain)();
	int	number_of_rate_settings;
	int	(*RateValueFunc)();
	int	(*FindBestRate)();

	int	(*SetVolume)();
	int	(*SetFilter)();
	int	(*SetSamplingRate)();
	int	(*SetInputGain)();
	int	(*PreTrigger)();
	int	(*Trigger)();
	int	(*TriggerList)();
	int	(*Stop)();
	int	(*QueryChanPos)();
	BOOL	(*AllocChannel)();
	void	(*FreeChannel)();

	int	number_of_channels;
	struct NewChannel newchannel[32];
	};

#define AH_STEREO	(1<<1)
#define AH_FILTER_ADJ	(1<<2)
#define AH_GAIN_ADJ	(1<<3)
#define AH_RATE_ADJ	(1<<4)

struct AudioHandler {
	struct Node node;
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long flags;
	short	num_bits;
	short	class_id;
	int	number_of_filter_settings;
	int	(*FilterValueFunc)();
	int	(*FindBestFilter)();
	int	number_of_gain_settings;
	int	(*GainValueFunc)();
	int	(*FindBestGain)();
	int	number_of_rate_settings;
	int	(*RateValueFunc)();
	int	(*FindBestRate)();
	int	number_of_channels;

	int	(*SetVolume)();
	int	(*SetFilter)();
	int	(*SetSamplingRate)();
	int	(*SetInputGain)();
	int	(*PreTrigger)();
	int	(*Trigger)();
	int	(*TriggerList)();
	int	(*Stop)();
	int	(*QueryChanPos)();
	BOOL	(*AllocChannel)();
	void	(*FreeChannel)();

	int	current_input_gain;
	int	current_filter;
	int	current_sampling_rate;
	int	Expansion[47];

	struct ChanKey channel[32];
	};

/** Used by DiskIO and the handler when a sample is playing **/

/** ASOpen() flags and ActiveSample Flags  **/

#define AS_ERROR_DISPLAYED	(1<<1)	/** set after user is notified of as->error **/

/** flags passable to AS_OPEN ***/

#define AS_PLAY			(1<<2)
#define AS_RECORD		(1<<3)
#define AS_AUTO_DATA		(1<<4)	/** Disk IO handles ASRead(), ASWrite() **/
#define AS_AUTO_CLOSE		(1<<5)	/** Disk IO handles ASClose() **/
#define AS_AUTO_FREECHAN	(1<<7)	/** Disk IO handles FreeChan() **/

/*** Flags not passed to ASOpen() ***/
#define AS_IS_ACTIVE			(1<<6)	/** set by diskio when channel_done received **/
#define AS_IGNORE_DEFAULT_VOLUME	(1<<8)



/*** ASSeek() Flags ***/

#define ASS_OFFSET_CURRENT (1<<0)
#define ASS_OFFSET_BEGINNING (1<<1)

#define BBF_EOF 1

struct BigBlock {
	struct Node node;
	short *data;						/** start of buffered data **/
	unsigned long samp_pos;			/** sample index that corresponds to data[0] **/
	unsigned long alloclen;			/** was bb_len **/
	short *allocdata;					/** AllocDmaMem() return ptr **/
	unsigned long islen;				/** was each_bb_len[4] **/
	unsigned long xferlen;			/** # of THIS BLOCK'S samples xferd to card **/
	unsigned long flags;
	};


struct ActiveSample {
	struct Region region;			/** copy of region to play **/
	struct List	FullBlocks;			/** no data here **/
	struct List	EmptyBlocks;
	struct BigBlock *HandlerBlock;		/** Handler using this block **/
	struct SampleFilePointer *sp;			/** playing/recording sample **/
	unsigned long flags;						/** AS Flags **/
	struct ChanKey *key;						/** ChanKey to play this sample on **/
	int error;									/** STDERR_ERROR_CODE **/
	long skip_amt;								/** next ASRead() skip this amt **/
	unsigned long samp_pos;					/** currently active block's start index **/
	unsigned long xferlen;					/** currently active block's # samp's xfered to handler **/
	unsigned short block_align;			/** used to keep write buffers optim. aligned **/
	struct SignalSemaphore rwlock;		/** Only one ASRead()/ASWrite() at a time **/
	unsigned long next_write_pos;			/** File index where next BigBlock will bw written **/
	};

/** Used in the hilow message **/

struct HLchan {
	char Left;
	char Right;
	};

struct HLinfo		/** IS THIS STRUCT USED ANYWERE? **/
	{
	USHORT input;
	USHORT chan[4];
	USHORT output;
	};

/** Realtime module status **/

/* flags */

#define MS_WINDOW_OPEN			(1<<0)
#define MS_CHOOSE_HANDLER		(1<<1)
#define MS_CHOOSE_WINDOW		(1<<2)
#define MS_CUSTOM_PREFS			(1<<3)
#define MS_KEEP_RESIDENT		(1<<4)


/** The following flag is supposed to tell a module after **/
/** a call to "AddModule" if the returned state is initilized **/
/** eg. How many meters are open? (custom module state) **/
/** Its all fucked up.  It's not set by the **/
/** individule modules after they initilize the state, so it **/
/** doesn't work correctly.  It is **/
/** currently set in a couple of places where it is likly that */
/** the state has been initilized (in FindModState & instance.c) **/
/** Ideally, it should only be set by LoadModState & by the module **/
/** itself. **/

#define MS_STATE_INITILIZED	(1<<5)
#define MS_NEED_MODULE			(1<<6)	/* This state has priority when a new module is loaded */
#define MS_UTILITY_MODULE		(1<<7)	/* AudioHandler, DiskIO, etc. */
#define MS_STATE_SAVED			(1<<8)	/* Needed in studio/studio.c/SaveModState() */

struct StandardModState {
	struct Node node;
	char *instance_name;		/** name of this state **/
	char *module_name;		/** module type this state belongs to*/
	short state_size;			/** total size of mod state **/
	short	win_le;
	short win_te;
	short win_w;
	short win_h;
	unsigned long flags;
	int unused[3];
	char *handler_name;
	short handler_class_id;
	struct AudioHandler *hand;
	struct Module *attached_module;
	int lock;
	struct Window *window;	/** this state's default window or NULL **/
	int user_data[8];	/** These varable can be used for anything **/
	};

/** Stuff this FileFilter supports **/

#define FF_READ		(1<<0)
#define FF_WRITE		(1<<1)
#define FF_REGIONS	(1<<2)
#define FF_DCLIPS		(1<<3)
#define FF_PAN			(1<<4)

struct NewFileFilter {
	char *name;
	unsigned long flags;
	short num_bits;
	short priority;
	long	(*FFOpen)();
	long	(*FFRead)();
	long	(*FFWrite)();
	long	(*FFSeek)();
	long	(*FFClose)();
	long	(*FFReadTag)();
	long	(*FFWriteTag)();
	long	(*FFQueryAlign)();
	long	(*FFQueryOptions)();
	};
	
struct FileFilter {
	struct Node node;		/* links all filters together */
	char *name;
	unsigned long flags;
	short num_bits;
	long	(*FFOpen)();
	long	(*FFRead)();
	long	(*FFWrite)();
	long	(*FFSeek)();
	long	(*FFClose)();
	long	(*FFReadTag)();
	long	(*FFWriteTag)();
	long	(*FFQueryAlign)();
	long	(*FFQueryOptions)();
	};

/*struct Handler12
	{
	struct StandardModState state;
	int	number_of_filter_settings;
	int	(*FreqValFunc)();
	int	sampling_rate[32];
	int	filter[32];
	int	input_gain[32];
	USHORT	vol_play[32];
	USHORT	vol_out[32];
	USHORT	vol_in[32];
	USHORT	Extra[100];
	};
*/
#define PREF_WIND_WIDTH		200
#define PREF_WIND_HEIGHT	66

/** flags used by CreateSampleStruct() **/

#define CSS_KEEP_WORKING_FILE (1<<0)

/*** default stack size modules can use when linking with cback.o **/

#define CBACK_STACK 7000

/** Used by all SampleXxxx functions ***/

/** Tag structure is used to communicate Sample paramaters to/from **/
/** file filters. **/

#define SFT_NUM_REGIONS 32

struct SampleFileTag {
	struct SampleInfo parms;
	int org_length;			/** length of samp with no data clips **/
	int length;					/** length of samp with using data clips **/
	struct SampleDataClip dclips[DS_NUM_CLIPS];
	struct DiskRegion dregions[SFT_NUM_REGIONS];
	};

/** Flags set by user when making function calls **/

#define SP_MODE_NEWFILE (1<<0)
#define SP_MODE_OLDFILE (1<<1)
#define SP_MODE_READWRITE (1<<2)
#define SP_NEW_SAMPLE 	(1<<3)	/** set when new sample is created **/

struct SampleFilePointer {
	long ffp;
	long fileindex;
	short dclipindex;
	unsigned long flags;
	struct SignalSemaphore locksfp;
	struct FileFilter *ff;
	struct SampleFileTag tag;
	char samp_name[STUDIO16_MAX_PATH_SIZE];	/** complete path **/
	};

/** SampleSeek() values **/

#define SS_OFFSET_BEGINNING 1
#define SS_OFFSET_END		 2
#define SS_OFFSET_CURRENT	 3

/** InitStudio16UserEnv() flags ***/

#define	ISUE_DO_ASSIGNS					(1<<0)
#define	ISUE_LOAD_STUDIO16INSTANCE		(1<<1)
#define	ISUE_LOAD_STUDIO16BASE			(1<<2)
#define	ISUE_OPEN_DEF_SCREEN				(1<<3)
#define	ISUE_USE_MY_SCREEN				(1<<4)
#define	ISUE_FORCE							(1<<5)
#define	ISUE_LOAD_DRIVERS					(1<<6)

/** ASCountBlocks flags ***/

#define ASCB_EMPTY_BLOCKS	(1<<0)
#define ASCB_FULL_BLOCKS	(1<<1)
#define ASCB_INUSE_BLOCKS	(1<<2)

#define ZERO_DB 3200

/** Used by SlowFreeMem.c ***/

struct FreeNotice {
	struct Message message;
	char *mem;
	unsigned long size;
	};

/*** Used by UnlockedFreeMem()  ******/

struct MemLock {
	struct MinNode node;
	APTR mem;
	int count;
	};

struct ChunkToFree {
	struct MinNode node;
	APTR mem;	
	long size;
	};

/*** SE_CMD_PAUSE paramater ***/
#define FLUSH_PLAY_BUFFERS 1

/*** ASTriggerList paramater ***/
#define TRIGGER_LIST_MANUAL_SYNC 1

/*** Studio 16 Alerts ****/

#define S16A_BADBASE					0x35000000	
#define S16A_MAX_EVENT_TYPE		0x35000001	
#define S16A_LOCK_FREE_MEM1		0x35000002
#define S16A_LOCK_FREE_MEM2		0x35000003
#define S16A_LOCK_FREE_MEM3		0x35000004
#define S16A_SEM_WAIT_ERR			0x35000005
#define S16A_SEM_CORRUPT			0x35000006
#define S16A_STACK_WARNING			0x35000007

#endif

