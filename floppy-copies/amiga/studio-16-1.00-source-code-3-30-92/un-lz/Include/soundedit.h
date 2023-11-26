#ifndef SOUNDEDIT_H
#define SOUNDEDIT_H TRUE

#ifndef GRAPHICS_RASTPORT_H
#include <graphics/rastport.h>
#endif

#ifndef STUDIO16_H
#include <studio16.h>
#endif

struct EditInstance {
	struct Window *edit_win;
	int mark_drawn;               /** TRUE if part of graph outlined **/
	int old_x1;
	int old_x2;
	int start_mark;
	int end_mark;                 /** define points within a sample marked */
	int insert_mark;              /** point within sample to insert at ***/
	struct Disk_Samp *marked_samp;  /** sample that start, end mark apply to */
	struct Disk_Samp *insert_samp;  /** sample where insertion is to happen **/
	struct Disk_Samp *cur;        /** currently selected sample **/
	int inserting;
	int graph_width;
	int graph_height;    /* edit window graph */
	long zoom_scale;		/* samples per pixel */
	int graph_start;		/* 1st sample to graph */
	long max_zoom;		/* largest zoom that will fit in this window */
	unsigned long flags;
	unsigned char *org_graph_buffer;	/** Generated from disk file **/
	unsigned char *graph_buffer;		/** Generated from org_graph_buffer based on user edits **/
	int org_gb_size;		/* length of org_graph_bufer in bytes */
	int gb_size;			/* length of graph_bufer in bytes */
	int gb_scale;			/* samples/pix in both graph_buffers */
	struct RastPort rp;	/* For double buffering */
	struct BitMap bm;
	long	plane_size;
	long	line_pos;
	struct SampleFilePointer *sp;	/** Used by freehand **/
	struct ActiveSample *as;	/** for drawing line_pos **/
	};

/** flags **/

#define EI_REFRESH_HSCROLL 	(1<<0)
#define EI_EMPTY_GRAPH_BUFFER (1<<1)
#define EI_TOTAL_REFRESH		(1<<2)
#define EI_CENTER_RANGE			(1<<3)
#define EI_SET_MAX_WIDTH		(1<<4)
#define EI_FREEHAND				(1<<5)
#define EI_DRAWING				(1<<6)

/** Edit state **/

#define ES_DESTRUCTIVE_EDITS	(1<<0)
#define ES_IN_USE					(1<<1)
#define ES_SMPTE_UNITS			(1<<2)

struct EditState {
	struct StandardModState state;
	unsigned long es_flags;
	};


#define IHI 50      /* height of getint requester */
#define SHI 30

/** Define location and size of area to put sample status information at **/

#define GRAPH_X 15
#define GRAPH_Y1 16		/** TOP border **/
#define GRAPH_Y2 66		/** BOTTOM border, includes buttons! **/

#define GRAPH_ZOOM 1
#define GRAPH_LEFT 2
#define GRAPH_RIGHT 4

#define HSCROLL_H 9

#define EDIT_ROW1 1
#define EDIT_ROW2 2

#define DEF_GBSIZE 200000

#define INRANGE(v,r1,r2) ((v)>=(r1) && (v)<=(r2))
#define GMAP(v) ((v)>0 ? div_tab[min(127,abs((v)>>8))] : -div_tab[min(127,abs((v)>>8))])


/*** returned by AskUserPasteType ****/

#define PT_CANCEL 0
#define PT_INSERT 1
#define PT_OVERWRITE 2

#endif

