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

#define MAX_READ_LOCKS 16

/*** Define common paramaters **/

struct SampleInfo {
	int rate;				/** sampling rate **/
	int filter3db;			/** Anti-aliasing filter value **/
	unsigned short volume;			/** prefered volume level **/
	int starttimecode;
	float smpte_sampling_rate;
	long reserved[3];
	};

struct SampleDataClip {
	long start;			/** inclusive **/
	long end;			/** inclusive **/
	};

#define DS_NUM_CLIPS 128

/** Disk_Samp types **/

#define RIGHT 4
#define LEFT 2
#define STEREO 6

/** Disk_Samp flags **/

#define DIRTY1 			(1<<0)
#define SAMPLE_OPEN 		(1<<1)
#define KEEP_NAME 		(1<<2)
#define DS_NEW_SAMPLE 	(1<<3)	/** set by CreateSampleStruct() */

struct Disk_Samp {
   struct MinNode all_node;   /** all disk samples opened **/
   char  *name;      	      /** sample label **/
   int  length;
   int  type;
   unsigned long flags;
   short version;     		  /** used when creating temporary files **/
	int   wfp;
	int   rfp[MAX_READ_LOCKS];
	struct SampleInfo parms;
	struct SampleDataClip dclips[DS_NUM_CLIPS];
	int org_length;				/** length of samp with no data clips **/
   };


/** This structure is stored at the beginning of working files on disk **/

#define TAG_KWIK (('K'<<24)|('W'<<16)|('I'<<8)|('K'))

struct SampleFileTag {
	unsigned long id;			/** set to TAG_KWIK **/
	struct SampleInfo parms;
	int org_length;			/** length of samp with no data clips **/
	int length;					/** length of samp with using data clips **/
	struct SampleDataClip dclips[DS_NUM_CLIPS];
	};

/** The following is just a possible effects structure.  All
effects should have two modes: real time and non-realime.  ie,
realtime is download to dsp and non-realtime is applied to a
region. OR non-realtime is downloaded to dsp, but dsp loops data back **/

struct effect {
   struct MinNode all_node;
   struct MinNode local_node;
   char *name;                /** name of this effect **/
   APTR code;                 /** code to download to dsp **/
   short cpu;                 /** % cpu used by this effect **/
   unsigned long flags;
   };


/** a region defines a range in a sample **/

struct Region {
   struct MinNode all_node;   /* every region is in this list */
   struct MinNode local_node; /* regions in a particular samp */
   struct Disk_Samp *samp;    /* sample this region is part of */
   unsigned long start;       /* starting sample, inclusive */
   unsigned long end;         /* ending sample, inclusive **/
	struct SampleInfo parms;	/** filter, rate, volume,etc. **/
	struct Module *owner;		/** for compatibility, REMOVE!! */
   unsigned long user_data;
   unsigned long flags;
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

struct defaults {
	char samp_io_path[80];           /** Save As... * Open path **/
	char temp_files_path[80];        /** directory to put temp files in **/
	char instance_config_file[80];
	short screen_width;
	short screen_viewmodes;
	struct GadgetColors colors;
	int copy_buf_size;
	int active_buf_size;			/* Value in Samples */
	short undo_levels;
	unsigned short palette[16];
	unsigned long flags;
	unsigned long smpte_mode;
	char SmpteSourceName[30];
	int SmpteSourceUnit;
	unsigned short SmpteTimeOut;
	unsigned short MeterSpeed;
	int record_safety_size;			/* Value in Samples */
	unsigned short Pad[124];
	};


/** StudioBase flags **/

#define SB_SMPTE_ON		(1<<0)	/** Smpte time code running? **/
#define SB_APP_SCREEN	(1<<1)	/** Using an application's **/
											/** screen, not one opened by **/
											/** IntitStudio16UserEnv **/
struct StudioBase {
	struct Library   LibNode;
	int CurrentSmpteTime;
	int SmpteTimeOut;
	struct SmpteSource *CurrentSmpteSource;
	struct List SmpteSinks;
	struct List SmpteSources;
	struct MinList	modules;
	struct MinList   regions;
	struct MinList   samps;	       /** all Disk_Samps **/
	struct MinList   cue_events;
	struct Screen    *screen;
	struct Window    *back_window;
	struct defaults  defaults;  /* NOTE: Only change the structure beyond this point */
	struct StudioEvent *event_list;
	char **eventidarray;
	short max_registered_ids;
	struct SharedSemaphore lockstudio;
	struct MinList *notifyarray;
	struct List handlers;
	struct List file_filters;
	struct List module_states;
	unsigned long flags;
	};


/** backwards compatibility **/

#define StudioBase StudioBase

/**!!!! make my knob & list gadets chained together, then put a
   pointer to the first gadget in the window's data pointer!!! */

struct NewModule {
   char *name;                   /* module's name */
   unsigned long state_flags;    /* OR these flags into state IF STATE iS CREATED BY ADDMOD! */
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
   struct MinNode node;          /* links all Modules together */
   struct MsgPort *notifyme;		/* module's event port */
   char *name;                   /* module's name */
   unsigned long flags;          /* module's flags */
   APTR module_data;             /* for module's use */
	struct StandardModState *state;
   };

struct NotifyLink {
	struct MinNode node;
	struct Module *notifyme;
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
	struct MinNode node;
   };


/** Audio Playback & Record releated **/

struct NewAudioHandler {
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long flags;
	short	play_channels;
	short record_channels;
	short monitor_channels;
	short output_channels;
	short num_bits;
	short priority;
	short class_id;
	};

#define AHC_PLAY 0			/** Audio Handler Channel **/
#define AHC_RECORD 1
#define AHC_OUTPUT 2
#define AHC_MONITOR 3
#define AHC_MAX	4

struct AudioHandler {
	struct Node node;
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long chans_in_use[AHC_MAX];		/** see CIU_ **/
	short	num_channels[AHC_MAX];
	unsigned long flags;
	short num_bits;
	short class_id;
	};

/** flags passed to AllocChan() & stored in ChanKey.flags **/

#define CK_CHAN_PLAY		(1<<0)
#define CK_CHAN_RECORD	(1<<1)
#define CK_CHAN_OUTPUT	(1<<2)
#define CK_CHAN_MONITOR (1<<3)

#define CK_ID_ONLY		(1<<4)

#define CK_LEFT			(1<<5)			/** NOT USED YET **/
#define CK_RIGHT			(1<<6)			/** NOT USED YET **/
#define CK_TAP_OUTPUT	(1<<7)
#define CK_TAP_INPUT		(1<<8)

/** example flag combos:
	CK_CHAN_PLAY					Alloc a Play channel
	CK_CHAN_PLAY|CK_LEFT			Alloc a play channel on the left chan
	CK_CHAN_RECORD					Alloc any record channel
	CK_CHAN_OUTPUT					Alloc Audio out as a record chan
	CK_CHAN_MONITOR|CK_ID_ONLY	Don't allocate resources
	

**/

/** Used to keep track of an allocated channel **/

struct ChanKey {
	struct AudioHandler *handler;
	int chan_num;
	unsigned long chan_mask;
	unsigned long flags;
	};


/** Used by DiskIO and the handler when a sample is playing **/

/** ASOpen() flags and ActiveSample Flags  **/

#define AS_ERROR_DISPLAYED	(1<<1)	/** set after user is notified of as->error **/

/** flags passable to AS_OPEN ***/

#define AS_PLAY				(1<<2)
#define AS_RECORD				(1<<3)
#define AS_AUTO_DATA			(1<<4)	/** Disk IO handles ASRead(), ASWrite() **/
#define AS_AUTO_CLOSE		(1<<5)	/** Disk IO handles ASClose() **/
#define AS_AUTO_FREECHAN	(1<<7)	/** Disk IO handles FreeChan() **/

/*** Flags not passed to ASOpen() ***/
#define AS_IS_ACTIVE			(1<<6)	/** set by diskio when channel_done received **/
#define AS_IGNORE_DEFAULT_VOLUME	(1<<8)



/*** ASSeek() Flags ***/

#define ASS_OFFSET_CURRENT (1<<0)
#define ASS_OFFSET_BEGINNING (1<<1)

#if 0		/*** !!!!!! OLD ActiveSample ******/
struct ActiveSample {
	struct Disk_Samp *active_samp;	/** Sample to play **/
	struct Module *owner;				/** Module sending SE_PRELOAD_SAMPLE **/
	unsigned long load_count;			/** Total samples loaded into a big_block **/
	unsigned long load_start;			/** first sample to play **/
	unsigned long load_len;				/** Total samples to play **/	
	unsigned long loop_count;			/** Times to loop sample **/
	unsigned long bb_len;				/** Total samples in one big_block **/
	unsigned short bb_diskio_index;	/** last loaded big_block **/
	unsigned short bb_handler_index; /** big_block handler is processing **/
	short *big_block[4];					/** pointers to big_block's **/
	struct SampleFilePointer *sp;		/** read lock on playing sample **/
	unsigned long flags;				
	unsigned long owner_data;			/** for use by Module sending SE_PRELOAD_SAMPLE **/
	struct ChanKey *key;					/** ChanKey to play this sample on **/
	unsigned long each_bb_len[4];		/** Real length of each bb **/
	struct SampleInfo parms;			/** filter, rate, volume,etc. **/
	short deleteme_a;						/** used by EditedSeek & EditedRead **/
	int error;								/** STD_ERROR_CODE **/
	long skip_amt;							/** ASRead() skip this amt **/
	};

#endif

#define BBF_EOF 1

struct BigBlock {
	struct Node node;
	short *data;					/** start of buffered data **/
	unsigned long samp_pos;		/** sample index that corresponds to data[0] **/
	unsigned long alloclen;		/** was bb_len **/
	short *allocdata;				/** AllocDmaMem() return ptr **/
	unsigned long islen;			/** was each_bb_len[4] **/
	unsigned long xferlen;		/** # of THIS BLOCK'S samples xferd to card **/
	unsigned long flags;
	};


struct ActiveSample {
	struct Region region;				/** copy of region to play **/
	struct List	FullBlocks;				/** no data here **/
	struct List	EmptyBlocks;
	struct BigBlock *HandlerBlock;	/** Handler using this block **/
	struct SampleFilePointer *sp;		/** playing/recording sample **/
	unsigned long flags;					/** AS Flags **/
	struct ChanKey *key;					/** ChanKey to play this sample on **/
	int error;								/** STD_ERROR_CODE **/
	long skip_amt;							/** next ASRead() skip this amt **/
	unsigned long position;				/** Aprx. sample index recording or playing **/
	unsigned short block_align;		/** used to keep write buffers optim. aligned **/
	struct SignalSemaphore rwlock;	/** Only one ASRead()/ASWrite() at a time **/
	unsigned long next_write_pos;		/** File index where next BigBlock will bw written **/
	};

/** Used in the hilow message **/

struct HLchan {
	char high;
	char low;
	};

struct HLinfo		/** arg 1 **/
	{
	struct HLchan output;
	struct HLchan input;
	struct HLchan chan[4];
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
	char *instance_name;			/** name of this state **/
	char *module_name;			/** module type this state belongs to*/
	short state_size;				/** total size of mod state **/
	short	win_le;
	short win_te;
	short win_w;
	short win_h;
	unsigned long flags;
	UBYTE	*pref_name;
	struct Window	*(*pref_display_func)();
	void	(*pref_hide_func)();
	char *handler_name;
	short handler_class_id;
	struct AudioHandler *hand;
	struct Module *attached_module;
	int lock;
	struct Window *window;	/** this state's default window or NULL **/
	};

#define FF_SAVE_FILTER	(1<<0)
#define FF_LOAD_FILTER	(1<<1)

struct NewFileFilter {
	char *name;
	unsigned long flags;
	int (*func)();
	short num_bits;
	short priority;
	};
	
struct FileFilter {
   struct Node node;       	   /* links all filters together */
   char *name;                   
   unsigned long flags;          
	int (*func)();
	short num_bits;
   };

struct Handler12
	{
	struct StandardModState state;
	int	number_of_filter_settings;
	int	(*FreqValFunc)();
	ULONG	sampling_rate[32];
	USHORT	filter[32];
	USHORT	input_gain[32];
	USHORT	vol_play[32];
	USHORT	vol_out[32];
	USHORT	vol_in[32];
	USHORT	Extra[32];
	};

#define PREF_WIND_WIDTH		200
#define PREF_WIND_HEIGHT	66

/** flags used by CreateSampleStruct() **/

#define CSS_KEEP_WORKING_FILE (1<<0)

/*** default stack size modules can use when linking with cback.o **/

#define CBACK_STACK 7000

/** Used by all SampleXxxx functions ***/

/** Flags set by user when making function calls **/

#define SP_MODE_NEWFILE (1<<0)
#define SP_MODE_OLDFILE (1<<1)
#define SP_MODE_READWRITE (1<<2)

struct SampleFilePointer {
	long fp;
	struct Disk_Samp *disksamp;
	long fileindex;
	short dclipindex;
	unsigned long flags;
	struct SignalSemaphore locksfp;
	};

/** SampleSeek() values **/

#define SS_OFFSET_BEGINNING 1
#define SS_OFFSET_END		 2
#define SS_OFFSET_CURRENT	 3

/** InitStudio16UserEnv() flags ***/

#define	ISUE_DO_ASSIGNS					(1<<0)
#define	ISUE_LOAD_STUDIO16INSTANCE		(1<<1)
#define	ISUE_LOAD_STUDIO16BASE			(1<<2)
#define	ISUE_ATTACH_WORKING_FILES		(1<<3)
#define	ISUE_OPEN_DEF_SCREEN				(1<<4)
#define	ISUE_USE_MY_SCREEN				(1<<5)
#define	ISUE_FORCE							(1<<6)

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

#endif

