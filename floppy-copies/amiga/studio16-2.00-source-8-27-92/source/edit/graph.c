#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <graphics/gfxmacros.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"

/***************************************************************/

/** graphasm(buf, index, ei->gb_scale) **/

char __asm graphasm( register __a0 short *, register __d1 long, 
					  register __d2 long);

/***************************************************************/

short SampleGetOne();
void BuildEIRP();
void FillBufsize();
void DoBlit();
void DrawGrid();
void DrawAllRP();
void BuildDivTab();
void RethinkZoom();
void GenEditGraphBuffer();
void DrawEditGraphBuffers();
void AllocOrgGraphBuffer();
void AllocGraphBuffer();
void FillGraphBuffer();
void InitGraphBuffer();
void FreeGraphBuffer();
void FreeOrgGraphBuffer();
void FreeRenderBuffer();
void SetEIGraphWidth();
void ExtendedBuildEIRP(struct EditInstance *ei,short gs,short ge,int index);
unsigned long CalcGridX();

/***************************************************************/

extern struct StudioBase *StudioBase;
extern struct Window *edit_win;
extern long zoom_scale;		/* samples per pixel */
extern short longest_ei;
extern struct EditInstance EI[EI_MAX];
extern long max_zoom;	/* largest zoom that will fit in this window */
extern long edit_flags;
extern long new_zoom;
extern struct NewSlider hscroll;
extern struct EditState *thisstate;

/************************************************************/

void RedrawGraphBuffer(ei)
struct EditInstance *ei;
{
GenEditGraphBuffer(ei);
RethinkZoom();
DrawEditGraphBuffers();
update_status();
}

/************************************************************/

void RethinkZoom()
{
max_zoom = EI[longest_ei].sfp->tag.length/DRAWWIDTH+1;
if (zoom_scale > max_zoom)
	new_zoom=max_zoom;
}

/************************************************************/
/** Generate a graph buffer based on dclips & the original **/
/** graph buffer **/
/************************************************************/

void GenEditGraphBuffer(ei)
struct EditInstance *ei;
{
struct SampleDataClip *dclips;
int i;
int gbdest,gbsrc,gblen;
int edit_pos;
int clip_len;

dclips=&ei->sfp->tag.dclips[0];

/** first calc new sample length based on edits **/

ei->sfp->tag.length=SumDataClipLength(dclips);

/** Now recalc graph buffer size where edits go **/

if (ei->graph_buffer) {
	FreeMem(ei->graph_buffer, ei->gb_size);
	ei->graph_buffer=NULL;
	}

AllocGraphBuffer(ei);

/** Now generate the edit graph's data **/

if (ei->graph_buffer && ei->org_graph_buffer) {

	i=0;
	edit_pos=0;

	while (dclips[i].end && i < DS_NUM_CLIPS) {
		clip_len=DataClipLength(&dclips[i]);
		gbdest=edit_pos/ei->gb_scale;
		gbsrc=dclips[i].start/ei->gb_scale;
		gblen=clip_len/ei->gb_scale;
		gblen=min(gblen,ei->org_gb_size);
		movmem(ei->org_graph_buffer+gbsrc,ei->graph_buffer+gbdest,gblen);
		edit_pos += clip_len;
		i++;
		}
	}
}

/***********************************************************/

void RegenGraph(ei, usegraphfile)
struct EditInstance *ei;
{
InitGraphBuffer(ei);			/** pick size & allocate memory **/

if (!usegraphfile) {
	DeleteGraphFile(ei);
	}

if (!AttemptReadGraphBuffer(ei)) {
	FillGraphBuffer(ei);			/** fill org_graph_buffer with data **/
	}
else {
	DeleteGraphFile(ei);		/** Remove since it may change **/
	}

#if 0
SetAPen(edit_win->RPort, StudioBase->defaults.colors.list.hitbox_unselected);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, GRAPH_X,ei->graph_y, GRAPH_X+DRAWWIDTH-1, ei->graph_y+ei->graph_height-1);

GenEditGraphBuffer(ei);		/** fill graph_buffer with data **/

SetEIGraphWidth(ei,ei->graph_start);
BuildEIRP(ei);
DrawAllRP();
#endif
RedrawGraphBuffer(ei);
}

/************************************************************/

AttemptReadGraphBuffer(ei)
struct EditInstance *ei;
{
long fp;
long len;
char buf[100];

GraphFilePath(ei->samp_name,buf,sizeof(buf));

fp = Open(buf, MODE_OLDFILE);
if (fp==0)
	return(FALSE);

new_pointer(PTR_BUSY);
len=Seek(fp, 0L, OFFSET_END);
len=Seek(fp, 0L, OFFSET_BEGINNING);
if (len!=ei->org_gb_size) {
	new_pointer(PTR_NORMAL);
	telluser("EDIT:","Regenerating graph buffer file.",0);
	Close(fp);
	return(FALSE);
	}
Read(fp,ei->org_graph_buffer, len);
Close(fp);
new_pointer(PTR_NORMAL);
return(TRUE);
}

/************************************************************/

WriteGraphBuffer(ei)
struct EditInstance *ei;
{
long fp;
char buf[100];

GraphFilePath(ei->samp_name,buf,sizeof(buf));

fp = Open(buf, MODE_NEWFILE);
if (fp==0)
	return(FALSE);

new_pointer(PTR_BUSY);
Write(fp,ei->org_graph_buffer, ei->org_gb_size);
Close(fp);
new_pointer(PTR_NORMAL);
return(TRUE);
}

/************************************************************/

DeleteGraphFile(ei)
struct EditInstance *ei;
{
char buf[100];

GraphFilePath(ei->samp_name,buf,sizeof(buf));

return(DeleteFile(buf));
}

/***********************************************************/

int GraphFilePath(name,buf,size)
char *name;
char *buf;
int size;
{
char temp[STUDIO16_MAX_PATH_SIZE];

sprintf(temp,"%s.graph",name);
if (strlen(temp) >= size) {
	return(OUT_OF_MEMORY);
	}
strcpy(buf,temp);
return(NO_ERROR);
}

/************************************************************/

void DrawEditGraphBuffers()
{
int middle;
short i;
long gs;

/** Decide which sample should be centered in updated **/
/** graph, if possible. **/

if (edit_flags&EDIT_CENTER_RANGE)
	middle = EI[longest_ei].start_mark+(EI[longest_ei].end_mark-EI[longest_ei].start_mark)/2;
else
	middle = EI[longest_ei].graph_start+(EI[longest_ei].graph_width*zoom_scale)/2;
edit_flags &= (~EDIT_CENTER_RANGE);


/** Set new zoom in case it has changed **/

zoom_scale = new_zoom;

/** Since zoom may have changed, we must recalc "graph_width" **/
/** which describes the area to render into (graph_width <= DRAWWIDTH) **/
/** The trick here is to calculate graph_width without relying **/
/** on graph_start.  This can be done for longest_ei. **/

SetEIGraphWidth(&EI[longest_ei],0);

/** Calculate graph_start (the first sample graphed) **/

gs = middle-(EI[longest_ei].graph_width*zoom_scale)/2;
gs = matchscale(gs);

/** Make sure entire graph_width will be drawn into.  Adjust **/
/** graph_start if needed to make this happen **/

if (gs < 0)
	gs=0;
if (gs + EI[longest_ei].graph_width*zoom_scale >  EI[longest_ei].sfp->tag.length) {
	gs=EI[longest_ei].sfp->tag.length-EI[longest_ei].graph_width*zoom_scale;
	gs = matchscale(gs);
	if (gs < 0)
		gs=0;
	}

/** All samples should have same graph_start as longest_ei, plus **/
/** need to recalc their graph_widths based on new zoom & graph_start **/

for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		EI[i].graph_start = gs;
		SetEIGraphWidth(&EI[i],gs);
		}

/** Adjust Horiz. scroll slider to match new graph start & zoom ***/

((struct slider *)(hscroll.gad_ptr))->list_size = EI[longest_ei].sfp->tag.length;
((struct slider *)(hscroll.gad_ptr))->show_size = EI[longest_ei].graph_width*zoom_scale;
((struct slider *)(hscroll.gad_ptr))->top_element=EI[longest_ei].graph_start;
new_gadget_size(hscroll.gad_ptr,
		GRAPH_X-2,											/* x1 */
		edit_win->Height - GRAPH_Y2 + 2,				/* y1 */
		edit_win->Width - GRAPH_X + 1,				/* x2 */
		edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H);	/* y2 */

		gs = ((struct slider *)hscroll.gad_ptr)->top_element;
		gs = matchscale(gs);

	for (i=0; i < EI_MAX; i++)
		if (EI[i].flags&EI_IN_USE) {
if (EI[i].graph_start != gs) {
/*	kprintf("INTERNAL ERROR in graph.c, gs changed !!\n");*/
	}
			BuildEIRP(&EI[i]); 
			}
	DrawAllRP();
	refresh_gadget(hscroll.gad_ptr);
}

/***************************************************************/

void BuildEIRP(ei)
struct EditInstance *ei;
{
SetAPen(&ei->rp,0); 
SetDrMd(&ei->rp,JAM1);
RectFill(&ei->rp,0,0,DRAWWIDTH-1,ei->graph_height-1);
ExtendedBuildEIRP(ei,0,ei->graph_width,ei->graph_start);
}

/***************************************************************/

void ExtendedBuildEIRP(struct EditInstance *ei,short gs,short ge,int index)
{
short *quickbuffer;
short y,offset;
short x;
short flags;
short range;
int indexstart;

flags = 0;
y = ei->graph_height/2;

if (ei->graph_width==0) {
	DrawGrid(ei);			/** Overlay the grid on top of ei->rp **/
	return;
	}

SetAPen(&ei->rp,1);
SetDrMd(&ei->rp,JAM1);

if (ei->graph_buffer && zoom_scale >= ei->gb_scale && zoom_scale!=1) {
	
	/** Going to redraw graph from graph_buffer **/
	for (x=gs; x < ge; x++) {
		offset=ei->div_tab[ei->graph_buffer[index/ei->gb_scale]];
	   Move (&ei->rp, x, y-offset);
  	   Draw (&ei->rp, x, y+offset);
		index += zoom_scale;	/**!!!! combine gb_scale & use fixed point math!! **/
   	}
	}

	/** Going to redraw graph direct from disk **/

else {
	new_pointer(PTR_BUSY);
	
	if (gs==0) {/** Expose left? **/
		Move(&ei->rp, gs, y+GMAP(ei,SampleGetOne(ei->sfp,index)));
		SampleSeek(ei->sfp, index, SS_OFFSET_BEGINNING);
		}
	else {				/** No, expose right **/
		Move(&ei->rp, gs-1, y+GMAP(ei,SampleGetOne(ei->sfp,index-zoom_scale)));
		}

	/** Try and read a block of data **/

	range=(ge-gs)*zoom_scale;
	if (range <= SMALLBUFSIZE) {
		quickbuffer=(short *)AllocMem(SMALLBUFSIZE*2,0);
		if (quickbuffer) {
			indexstart=index;
			SampleRead(ei->sfp, quickbuffer, range);
			for (x=gs; x < ge; x++) {
				Draw(&ei->rp, x, y+GMAP(ei,quickbuffer[index-indexstart]));
				index += zoom_scale;
				}
			FreeMem(quickbuffer,SMALLBUFSIZE*2);
		   }
		}

	/** Do a seek and read one byte **/		

	else {
		for (x=gs; x < ge; x++) {
			Draw(&ei->rp, x, y+GMAP(ei,SampleGetOne(ei->sfp,index)));
			index += zoom_scale;
		   }
		}

	if (gs==0 && gs < ge)	{/** expose left? **/
		if (buffertosample(ei,ge) < ei->sfp->tag.length) {/** Connect last dot? **/
			Draw(&ei->rp, x, y+GMAP(ei,SampleGetOne(ei->sfp,buffertosample(ei,ge))));
			}
		}
	new_pointer(PTR_NORMAL);
	}

DrawGrid(ei);			/** Overlay the grid on top of ei->rp **/
}

/***************************************************************/

void ScrollEIRP(ei,snatch_gs)
struct EditInstance *ei;
int snatch_gs;
{
int delta;	/* pixel's to scroll graph */
short internal_gstart;
short internal_gend;
int internal_istart;

SetEIGraphWidth(ei,snatch_gs);
delta=CalcDelta(ei,snatch_gs);

if (delta==0) {
	ei->graph_start = snatch_gs;	/** probably redundant **/
	return;
	}

if (delta >= ei->graph_width) {
	ei->graph_start = snatch_gs;
	BuildEIRP(ei);
	return;
	}

SetBPen(&ei->rp,StudioBase->defaults.colors.list.hitbox_unselected);
if (snatch_gs > ei->graph_start) {    /** scroll left **/
	ScrollRaster(&ei->rp, delta,0,0,0,DRAWWIDTH-1,ei->graph_height-1);
	internal_gstart=ei->graph_width-delta;
	internal_gend=ei->graph_width;
	internal_istart=snatch_gs+internal_gstart*zoom_scale;
	}
else {
	ScrollRaster(&ei->rp,-delta,0,0,0,DRAWWIDTH-1,ei->graph_height-1);
	internal_gstart=0;
	internal_gend=delta;
	internal_istart=snatch_gs;
	}

ei->graph_start = snatch_gs;
ExtendedBuildEIRP(ei,internal_gstart,internal_gend,internal_istart);
}

/***************************************************************/

int CalcDelta(ei,snatch_gs)
struct EditInstance *ei;
int snatch_gs;
{
int delta;	/* pixel's to scroll graph */

delta = (ei->graph_start-snatch_gs)/zoom_scale;
if (delta < 0)
	delta = -delta;
return(delta);
}

/***************************************************************/

void DrawAllRP()
{
short i;
int x1,x2,temp;

if (CountEI()==0) 	/** At least one graph to render? **/
	return;

if (EI[longest_ei].marked_samp) {
	x1 = sampletomousenoclip(&EI[longest_ei],EI[longest_ei].start_mark)-GRAPH_X;
	x2 = sampletomousenoclip(&EI[longest_ei],EI[longest_ei].end_mark)-GRAPH_X;
	if (x2 < x1) {    /** swap **/
	   temp = x1;
	   x1 = x2;
	   x2 = temp;
	   }
	}
else {
	x1=x2=-1;
	}

LockGadLibRender();
for (i=0; i < EI_MAX; i++)
	if (EI[i].flags&EI_IN_USE) {
		DoBlit(&EI[i],0,x1-1,0x0c0);
		DoBlit(&EI[i],x1,x2,0x030);
		DoBlit(&EI[i],x2+1,DRAWWIDTH-1,0x0c0);
		EI[i].line_pos=0xFFFFFFFF;		/** means no line xored on graph **/
		}

UnLockGadLibRender();
}

/***************************************************************/

void DoBlit(ei,x1,x2,minterm)
struct EditInstance *ei;
int x1,x2,minterm;
{
if (x1 >= DRAWWIDTH || x2 < 0)
	return;

if (x1<0)
	x1=0;

if (x2>=DRAWWIDTH)
	x2=DRAWWIDTH-1;

ClipBlit(&ei->rp,x1,0,									/** source **/
		edit_win->RPort,GRAPH_X+x1,ei->graph_y,	/** destination **/
		x2-x1+1,												/** xsize **/
		ei->graph_height,									/** ysize **/
		minterm);											/** copy minterm **/
}

/***************************************************************/

void DrawGrid(ei)
struct EditInstance *ei;
{
short y,x;
long si;
int gridcount;

if (!(ei->flags&EI_IN_USE))
	return;

/** draw center line **/

SetDrMd(&ei->rp, JAM2);
SetDrPt(&ei->rp,0xCCCC);
SetAPen(&ei->rp, 1);
SetBPen(&ei->rp, 0);
y = ei->graph_height/2;
Move(&ei->rp, 0, y);
Draw(&ei->rp, DRAWWIDTH-1, y);

/** draw grid only if there is fewer than an arbirtary number **/
/** of grid lines (17) **/

if (thisstate->grid) {
	gridcount=CalcGridCount();
	if (DRAWWIDTH*zoom_scale/gridcount < 17) {
		si=ei->graph_start+gridcount;
		x = sampletobuffernoclip(ei,CalcGridX(si));
		y = ei->graph_height-1;
		while (x < DRAWWIDTH) {
			Move(&ei->rp, x, 0);
			Draw(&ei->rp, x, y);
			si=si+gridcount;
			x = sampletobuffernoclip(ei,CalcGridX(si));
			}
		}
	}
SetDrPt(&ei->rp,-1);
}

/************************************************************/
/** Determine number of samples between each grid marker **/

int CalcGridCount()
{
int gridcount;
int rate;
struct EditInstance *ei;

ei=&EI[longest_ei];

if (ei->sfp->tag.parms.smpte_sampling_rate)
	rate=(int)ei->sfp->tag.parms.smpte_sampling_rate;
else
	rate=ei->sfp->tag.parms.rate;

switch(thisstate->grid) {
	case 0:
		gridcount=0;
		break;
	case 1:				/** frames **/
		gridcount=rate/StudioBase->defaults.smpte_mode;
		break;
	case 2:				/** seconds **/
		gridcount=rate;
		break;
	case 3:				/** 10 seconds **/
		gridcount=rate*10;
		break;
	case 4:				/** minutes **/
		gridcount=rate*60;
		break;
	}
return(gridcount);
}

/************************************************************/
/** Round passed sample to appropriate boundry **/

unsigned long CalcGridX(si)
{
unsigned long rate;
unsigned long x;
struct EditInstance *ei;

ei=&EI[longest_ei];

if (ei->sfp->tag.parms.smpte_sampling_rate)
	rate=(int)ei->sfp->tag.parms.smpte_sampling_rate;
else
	rate=ei->sfp->tag.parms.rate;

switch(thisstate->grid) {
	case 1:				/** frames **/
		x=((((si*StudioBase->defaults.smpte_mode)/rate)) * 
		  rate)/StudioBase->defaults.smpte_mode;
		break;
	case 2:				/** seconds **/
		x=(si/rate)*rate;
		break;
	case 3:				/** 10 seconds **/
		x=(si/(rate*10))*(rate*10);
		break;
	case 4:				/** minutes **/
		x=(si/(rate*60))*(rate*60);
		break;
	}
return(x);
}

/***************************************************************/
/** Takes a number from 0 -> 127 and maps it into the current **/
/** graph height. 														  **/
/***************************************************************/

void BuildDivTab(ei)
struct EditInstance *ei;
{
short i;
short gh;

gh=ei->graph_height;
for (i=0; i < 128; i++) {
	ei->div_tab[i] = (i*gh)/256;
	}

}

/***************************************************************/

void FillGraphBuffer(ei)
struct EditInstance *ei;
{
short *buf;
int tomove;
int bufsize;
int start;
int len;

if (ei->org_graph_buffer==0)
	return;

len = (ei->gb_scale*ei->org_gb_size)*2;	/** ~= ei->sfp->tag.length*2 **/
bufsize = min(StudioBase->defaults.copy_buf_size, len);
bufsize = (bufsize/(ei->gb_scale*2))*(ei->gb_scale*2);	/** make multiple of gb_scale-words **/
buf = (short *)AllocMem(bufsize, 0);

if (buf==0) {
	FreeGraphBuffer(ei);
	return;
	}

SetAPen(edit_win->RPort, StudioBase->defaults.colors.list.hitbox_unselected);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, GRAPH_X,ei->graph_y, GRAPH_X+DRAWWIDTH-1, ei->graph_y+ei->graph_height-1);

start = 0;

/** must seek to the beginning, SampleOpen() doesn't match SRead! **/
(*ei->sfp->ff->FFSeek)(ei->sfp->ffp,0L,OFFSET_BEGINNING);

do {
	tomove = min(bufsize, len);
	if ((*ei->sfp->ff->FFRead)(ei->sfp->ffp,buf, (tomove>>1))!=(tomove>>1)) {
		break; /*(READ_ERROR);*/
		}
	FillBufsize(ei, buf, tomove>>1,start);
	start = start + (tomove>>1);
	len = len - tomove;
	} while (len);

FreeMem(buf, bufsize);

/** Erase what graph **/
SetAPen(edit_win->RPort, StudioBase->defaults.colors.list.hitbox_unselected);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, GRAPH_X,ei->graph_y, GRAPH_X+DRAWWIDTH-1, ei->graph_y+ei->graph_height-1);

return;
}

/***************************************************************/

void FillBufsize(ei, buf, tomove, start)
struct EditInstance *ei;
short *buf;
int tomove;		/** in samples **/
int start;		/** in samples (shorts) **/
{
static short y;
static short x;
static short draw_count_val;
static short draw_count;
static int gbi;
int index;
char maxval;
short offset;

if (start==0) {
	gbi=0;
	y = ei->graph_y+(ei->graph_height/2);
	x = GRAPH_X;
	draw_count_val = ei->org_gb_size/DRAWWIDTH+1;
	draw_count = draw_count_val;
	}

for (index=0; index < tomove; index += ei->gb_scale) {

	maxval=graphasm(buf, index, ei->gb_scale);
	ei->org_graph_buffer[gbi++]=maxval;
	draw_count--;
	if (draw_count==0) {
		draw_count = draw_count_val;
		offset=ei->div_tab[ei->org_graph_buffer[gbi-1]];
		LockGadLibRender();
		SetDrMd(edit_win->RPort, COMPLEMENT);
	   Move (edit_win->RPort, x, y-offset);
  	   Draw (edit_win->RPort, x, y+offset);
		UnLockGadLibRender();
		x++;
		}
	}
}

/***********************************************************/

void InitGraphBuffer(ei)
struct EditInstance *ei;
{
FreeGraphBuffer(ei);
ei->gb_scale = ei->sfp->tag.org_length/DEF_GBSIZE+1;	/** buffer:sample ratio **/
AllocOrgGraphBuffer(ei);
AllocGraphBuffer(ei);
}

/***********************************************************/

void AllocGraphBuffer(ei)
struct EditInstance *ei;
{
ei->gb_size = ei->sfp->tag.length/ei->gb_scale;	/** PUT DEF_GBSIZE in edit state!!!! **/
ei->graph_buffer = (char *)AllocMem(ei->gb_size, MEMF_CLEAR);
if (ei->graph_buffer==0)
	telluser("EDIT:","Not enough free RAM for graph buffer.",0);
}

/***********************************************************/

void AllocOrgGraphBuffer(ei)
struct EditInstance *ei;
{
ei->org_gb_size = ei->sfp->tag.org_length/ei->gb_scale;
ei->org_graph_buffer = (char *)AllocMem(ei->org_gb_size, MEMF_CLEAR);
if (ei->org_graph_buffer==0)
	telluser("EDIT:","Not enough free RAM for org graph buffer.",0);
}

/***********************************************************/

void FreeGraphBuffer(ei)
struct EditInstance *ei;
{
if (ei->graph_buffer) {
	FreeMem(ei->graph_buffer, ei->gb_size);
	ei->graph_buffer=NULL;
	}
FreeOrgGraphBuffer(ei);
}

/***********************************************************/

void FreeOrgGraphBuffer(ei)
struct EditInstance *ei;
{
if (ei->org_graph_buffer) {
	FreeMem(ei->org_graph_buffer, ei->org_gb_size);
	ei->org_graph_buffer=NULL;
	}
}

/***********************************************************/

int InitRenderBuffer(ei)
struct EditInstance *ei;
{
int dw;

FreeRenderBuffer(ei);
dw=DRAWWIDTH;
InitBitMap(&ei->bm,1L,dw,ei->graph_height);	/** depth, width, height of bitmap **/

ei->plane_size=RASSIZE(dw,ei->graph_height);
ei->bm.Planes[0]=(PLANEPTR)PadAllocMem(ei->plane_size,MEMF_CHIP|MEMF_CLEAR);
if (ei->bm.Planes[0]==NULL)
	return(OUT_OF_MEMORY);
InitRastPort(&ei->rp);
ei->rp.BitMap = &ei->bm;
ei->rp.Mask = 1;
return(NO_ERROR);
}

/************************************************************/

void FreeRenderBuffer(ei)
struct EditInstance *ei;
{
if (ei->bm.Planes[0]) {
	PadFreeMem(ei->bm.Planes[0], ei->plane_size);
	ei->bm.Planes[0]=0;
	}
}

/********************************************************/

void SetEIGraphWidth(ei,gs)
struct EditInstance *ei;
int gs;
{																		/** Round Up **/
ei->graph_width = min(DRAWWIDTH,(ei->sfp->tag.length-gs+zoom_scale-1)/zoom_scale);
if (ei->graph_width < 0)
	ei->graph_width=0;
}

/************************************************************/

#if 0
cksum(p,len)
char *p;
int len;
{
int i,sum;

sum=0;
for (i=0; i < len; i++)
	sum=sum+p[i];

return(sum);
}
#endif
