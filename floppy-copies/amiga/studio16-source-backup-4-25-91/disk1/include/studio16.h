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


struct Disk_Samp {
   struct MinList all_node;   /** all disk samples opened **/
   int   lfp;              /** file pointer to left channel **/
   int   rfp;              /** file pointer to right channel **/
   char  *name;            /** sample label **/
   int  length;
   int  rate;
   int  type;
   unsigned long flags;
   short version;       /** used when creating temporary files **/
   int   block1_start;     /** 0 -> length-1, first sample in lblock */
   int   block2_start;     /** 0 -> length-1, first sample in lblock */
   short blockl1[BLOCK_SIZE];
   short blockl2[BLOCK_SIZE];
   short blockr1[BLOCK_SIZE];
   short blockr2[BLOCK_SIZE];
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
   int (*init_module)();         /* called when module launched */
   int (*open_window)();         /* activate module's main window */
   int (*do_event)();            /* called with every IntuiMessage*/
   int (*close_window)();        /* close module's main window */
   int (*cleanup_module)();      /* called before module is unloaded */
   char *name;                   /* module's name */
   unsigned long flags;          /* module's flags */
   };

/** Module flags **/

#define MOD_OPEN_AT_STARTUP 1
#define MOD_CALL_ALL_WINDOWS 2
#define MOD_DID_NOT_CLOSE 4

/** This is the actual Module structure linked into studioBase **/

struct Module {
   struct MinNode node;          /* links all Modules together */
   struct Window *window;
   int segment;                  /* moudules seglist */
   struct MsgPort *module_port;  /* module's command port */
   int (*init_module)();         /* called when module launched */
   int (*open_window)();         /* called to activate module */
   int (*do_event)();            /* called with every IntuiMessage*/
   int (*close_window)();        /* close module's window */
   int (*cleanup_module)();      /* called before module is unloaded */
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

#endif

