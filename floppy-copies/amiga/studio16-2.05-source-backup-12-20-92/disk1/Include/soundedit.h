#ifndef SOUNDEDIT_H
#define SOUNDEDIT_H TRUE

#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#ifndef STUDIO16_H
#include <studio16.h>
#endif

struct EditInstance {

/** Describe sample that this struct represents **/

	struct SampleFilePointer *sfp;				/** open all the time **/
	char samp_name[STUDIO16_MAX_PATH_SIZE];	/** complete path **/
	unsigned long flags;

/*** Describe the currently marked range (inclusive) **/
	int start_mark;
	int end_mark;  
	int marked_samp;		/** TRUE if range defined **/

/** Describe where in the window to render the graph ***/

	int graph_width;
	int graph_height;
	int graph_y;			/* y2 = y+height **/
	unsigned short div_tab[128];

/** Describe which part of sample is graphed  **/

	int graph_start;		/* 1st sample to graph */

/** Describe the 'graph buffer' used for fast graph rendering **/

	unsigned char *org_graph_buffer;	/** Generated from disk file **/
	unsigned char *graph_buffer;		/** Generated from org_graph_buffer based on user edits **/
	int org_gb_size;		/* length of org_graph_bufer in bytes */
	int gb_size;			/* length of graph_bufer in bytes */
	int gb_scale;			/* samples/pix in both graph_buffers */

/** Describe double buffer for cleaner display updates **/

	struct RastPort rp;	/* For double buffering */
	struct BitMap bm;
	long	plane_size;
	long	draw_width;

/** Describe line position **/
	long	line_pos;
	struct ActiveSample *as;			/** for drawing line_pos **/

/** Non destructive edits undo & copy buffers **/

	struct SampleDataClip copybuffer[DS_NUM_CLIPS];
	struct SampleDataClip undobuffer[DS_NUM_CLIPS];
	int undo_start_mark;
	int undo_end_mark;
	int undo_marked_samp;

/** Region list for this sample (valid only if region_win open) **/

	struct list *rl;
	};

/** flags - Edit "edit_flags" **/

#define EDIT_REFRESH_HSCROLL 		(1<<0)
#define EDIT_EMPTY_GRAPH_BUFFER 	(1<<1)
#define EDIT_TOTAL_REFRESH			(1<<2)
#define EDIT_CENTER_RANGE			(1<<3)
#define EDIT_SET_MAX_WIDTH			(1<<4)
#define EDIT_FREEHAND				(1<<5)
#define EDIT_DRAWING					(1<<6)
#define EDIT_RANGE_LOOPING			(1<<7)

/** flags - EditInstance.flags **/

#define EI_IN_USE					(1<<0)

/** flags - Edit state **/

#define ES_DESTRUCTIVE_EDITS	(1<<0)
#define ES_IN_USE					(1<<1)
#define ES_SMPTE_UNITS			(1<<2)
#define ES_OKAY_REQ				(1<<3)
#define ES_SHOW_REGIONS			(1<<4)
#define ES_FAST_GRAPH			(1<<5)

struct EditState {
	struct StandardModState state;
	unsigned long es_flags;
	char copy_path[STUDIO16_MAX_PATH_SIZE];
	short grid;		/** matches submenu index **/
	};

/** Define location and size of area to put sample status information at **/

#define GRAPH_X 15
#define GRAPH_Y1 26		/** TOP border for graph **/
#define GRAPH_Y2 31		/** BOTTOM border, includes buttons! **/

#define GRAPH_ZOOM 1
#define GRAPH_LEFT 2
#define GRAPH_RIGHT 4

#define HSCROLL_H 9

#define EDIT_ROW1 1
#define EDIT_ROW2 2

#define DEF_GBSIZE 200000

#define INRANGE(v,r1,r2) ((v)>=(r1) && (v)<=(r2))
#define GMAP(ei,v) ((v)>0 ? (ei)->div_tab[min(127,abs((v)>>8))] : -(ei)->div_tab[min(127,abs((v)>>8))])
#define DRAWWIDTH (edit_win->Width - GRAPH_X - GRAPH_X)
#define SMALLBUFSIZE 32768 /** 1536 < 1.02, 8192 < 2.0 **/
#define TOO_SMALL 1281

/*** Max number of simultaneous editing sounds ***/

#define EI_MAX 8

/*** returned by AskUserPasteType ****/

#define PT_INSERT_START 1
#define PT_INSERT_END 2
#define PT_OVERWRITE 3

/*** ProcessEdit flags ***/

#define PE_DEST				(1<<0)
#define PE_NONDEST			(1<<1)
#define PE_NO_RANGE_CHECK	(1<<2)
#define PE_PARM2				(1<<3)


#endif

