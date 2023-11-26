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

/** Disk_Samp flags **/

#define DIRTY1 1
#define SAMPLE_OPEN 2
#define KEEP_NAME 4

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


#define MAX_READ_LOCKS 16

struct Disk_Samp {
   struct MinList all_node;   /** all disk samples opened **/
   char  *name;            /** sample label **/
   int  length;
   int  rate;
   int  type;
   unsigned long flags;
	int  filter3db;
   short version;       /** used when creating temporary files **/
	int   wfp;
	int   rfp[MAX_READ_LOCKS];
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

struct region {
   struct MinNode all_node;   /* every region is in this list */
   struct MinNode local_node; /* regions in a particular samp */
   struct Disk_Samp *samp;    /* sample this region is part of */
   struct effect *defaults;   /* standard effect for this regn*/
   unsigned long start;       /* starting sample, inclusive */
   unsigned long end;         /* ending sample, inclusive **/
   unsigned long flags;
   };


/*** studio 16's own internal preferences **/

struct defaults {
   char samp_io_path[80];           /** Save As... * Open path **/
   char temp_files_path[80];        /** directory to put temp files in **/
   short screen_width;
   short screen_viewmodes;
	struct GadgetColors colors;
   };

/** base structure **/

struct studioBase {
   struct Library   LibNode;
   struct MinList   modules;
   struct MinList   regions;
   struct MinList   samps;          /** all Disk_Samps **/
   struct MinList   cue_events;
   struct Screen    *screen;
   struct Window    *back_window;
   struct defaults  defaults;
	struct StudioEvent *event_list;
	unsigned char *bigblock[2][4];
	char **eventidarray;
	short max_registered_ids;
	struct SignalSemaphore lock;
	struct MinList *notifyarray;
	struct List handlers;
   };

/* each function in studio 16 is implemented as a seperate 'module'
   and is assigned one of the following structures.  Currently,
   these moudles are all part of the same task.  This may change
   so that they are seperate tasks.  If this happens, do_module
   will probably become unnecessary as each process can get messages
   from intuition. */

/**!!!! make my knob & list gadets chained together, then put a
   pointer to the first gadget in the window's data pointer!!! */


/** NewModule is filled and passed to add_module().  The strucutre can
   then then be discarded, just like inutition's NewWindow struct. **/


struct NewModule {
   char *name;                   /* module's name */
   unsigned long flags;          /* module's flags */
   };

/** Module flags **/

#define MOD_OPEN_AT_STARTUP 1
#define MOD_CALL_ALL_WINDOWS 2
#define MOD_DID_NOT_CLOSE 4
#define MOD_IDCMP_PRIVATE 8

/** This is the actual Module structure linked into studioBase **/

struct Module {
   struct MinNode node;          /* links all Modules together */
   struct MsgPort *notifyme;		/* module's command port */
   char *name;                   /* module's name */
   unsigned long flags;          /* module's flags */
   APTR module_data;             /* for module's use */
   };

/** DEFINES for changing pointer ***/

#define PTR_RESTORE 0
#define PTR_NORMAL 1
#define PTR_BUSY 2
#define PTR_GRABIT 3
#define PTR_CROP 4
#define PTR_CROSS 5
#define PTR_PASTE 6

struct NotifyLink {
	struct MinNode node;
	struct Module *notifyme;
	};

/*** Broadcast ****/

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


/** Audio Playback & Record releatd **/

struct NewAudioHandler {
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long flags;
	short	play_channels;
	short record_channels;
	short num_bits;
	short priority;
	};

struct AudioHandler {
	struct Node node;
	char *name;
	struct ConfigDev *hardware;
	struct Module *handler_mod;
	unsigned long ChansInUse;
	unsigned long flags;
	short	play_channels;
	short record_channels;
	short num_bits;
	};

/** Used to keep track of an allocated channel **/

struct ChanKey {
	struct AudioHandler *handler;
	int chan_num;
	unsigned long chan_mask;
	};

/*** Used to define an "audio state" **/

struct SampleInfo {
	int rate;				/** sampling rate **/
	int three_db;			/** Anti-aliasing filter value **/
	short volume;			/** prefered volume level **/
	short channel;			/** prefreed channel (left, right) **/
	short loop;				/** loop sound this many times **/
	};

/** Used by DiskIO and the handler when a sample is playing **/

/** PlayingSample Flags  **/

#define LAST_BLOCK (1<<0)

struct PlayingSample {
	struct Disk_Samp *playing_samp;	/** Sample to play **/
	struct Module *owner;				/** Module sending SE_PRELOAD_SAMPLE **/
	unsigned long load_count;			/** Total samples loaded into a big_block **/
	unsigned long load_start;			/** first sample to play **/
	unsigned long load_len;				/** Total samples to play **/	
	unsigned long loop_count;			/** Times to loop sample **/
	unsigned long bb_len;				/** Total samples in one big_block **/
	unsigned short bb_diskio_index;	/** last loaded big_block **/
	unsigned short bb_handler_index; /** big_block handler is processing **/
	short *big_block[2];					/** pointers to big_block's **/
	int fp;									/** read lock on playing sample **/
	unsigned long flags;					/** LAST_BLOCK? **/
	unsigned long owner_data;			/** for use by Module sending SE_PRELOAD_SAMPLE **/
	struct ChanKey *key;					/** ChanKey to play this sample on **/
	};

struct TimeCode {
   unsigned char hours;
   unsigned char minutes;
   unsigned char seconds;
   unsigned char frames;
   };


/** Used in the hilow message **/

struct HLchan {
	char high;
	char low;
	};

struct	HLinfo		/** arg 1 **/
	{
	struct HLchan input;
	struct HLchan output;
	struct HLchan chan[4];
	} HLinfo;

#endif

