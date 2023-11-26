#include <exec/types.h>1
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include <stdio.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"

/** graphasm(buf, index, ei->gb_scale) **/

char __asm graphasm( register __a0 short *, register __d1 long, 
					  register __d2 long);


short SampleGetOne();
int FixOldX();
void graph();
void FillBufsize();
struct SampleFilePointer *SampleOpen();

extern struct StudioBase *StudioBase;

static short internal_update = FALSE;
static int internal_gstart;
static int internal_gend;
static int internal_istart;

unsigned short div_tab[128];

void CopyDoubleBuffer();
void BuildDivTab();


/***************************************************************/
/** Takes a number from 0 -> 127 and maps it into the current **/
/** graph height. **/

void BuildDivTab(ei)
struct EditInstance *ei;
{
short i;
short gh;

gh = ei->graph_height;

for (i=0; i < 128; i++) {
	div_tab[i] = (i*gh)/256;
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
struct SampleFilePointer *sp;

int i;
/*char bug[80];*/

if (ei->org_graph_buffer==0)
	return;

len = (ei->gb_scale*ei->org_gb_size)*2;	/** ~= ei->cur->length*2 **/
bufsize = min(StudioBase->defaults.copy_buf_size, len);
bufsize = (bufsize/(ei->gb_scale*2))*(ei->gb_scale*2);	/** make multiple of gb_scale-words **/
buf = (short *)AllocMem(bufsize, 0);

if (buf==0) {
/*dprintf("graph.c:CANT ALLOC buffer in FillGraph!\n");*/
	FreeGraphBuffer(ei);
	return;
	}

SetDrMd(ei->edit_win->RPort,JAM1);

sp = SampleOpen(ei->cur, SP_MODE_OLDFILE);
if (sp==0) {
/*dprintf("graph.c:CANT OPEN SAMPLE IN FILLGRAPH!\n");*/
	FreeMem(buf, bufsize);
	FreeGraphBuffer(ei);
	return;
	}

start = 0;

/** must seek to the beginning, SampleOpen() doesn't match SRead! **/
SSeek(sp->fp, 0L, OFFSET_BEGINNING);

do {
	tomove = min(bufsize, len);
	if ((i=SRead(sp->fp, buf, tomove))!=tomove) {
/*sprintf(bug,"FillGraph Read error: got %d out of %d\n",i,tomove);
dprintf(bug);*/
		break; /*(READ_ERROR);*/
		}
	FillBufsize(ei, buf, tomove>>1,start);
	start = start + (tomove>>1);
	len = len - tomove;
	} while (len);

SampleClose(sp);
FreeMem(buf, bufsize);
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
	y = GRAPH_Y1+(ei->graph_height/2);
	SetAPen(ei->edit_win->RPort,WHITE);  /** was RED **/

	x = GRAPH_X;

	draw_count_val = ei->org_gb_size/ei->graph_width;
	draw_count = draw_count_val;
	}


for (index=0; index < tomove; index += ei->gb_scale) {

#if 0
	maxval=0;
	for (i=0; i < ei->gb_scale; i++) {
		if (abs(buf[index+i]) > maxval)
			maxval=abs(buf[index+i]);
		}
	maxval=maxval/256;
	if (maxval > 127) maxval=127;
#endif

	maxval=graphasm(buf, index, ei->gb_scale);
	ei->org_graph_buffer[gbi++]=maxval;
	draw_count--;
	if (draw_count==0) {
		draw_count = draw_count_val;
		offset=div_tab[ei->org_graph_buffer[gbi-1]];
	   Move (ei->edit_win->RPort, x, y-offset);
  	   Draw (ei->edit_win->RPort, x, y+offset);
		x++;
		}
	}
}

/***************************************************************/

#define SMALLBUFSIZE 1536

void graph(ei)

struct EditInstance *ei;

{
static short quickbuffer[SMALLBUFSIZE];
short y,offset;
short x;
int index;
struct SampleFilePointer *sp;
short flags;
short gs,ge;
int i1,i2;
short x1,x2;
int smrnd, emrnd;
short range;
int indexstart;

flags = 0;

SetDrMd(&ei->rp,JAM1);

if (!internal_update) {
	SetAPen(&ei->rp,0); 
	RectFill(&ei->rp,0,0,ei->graph_width-1,ei->graph_height-1);
	}

y = ei->graph_height/2;
SetAPen(&ei->rp,1);

if (internal_update) {
	gs = internal_gstart;
	ge = internal_gend;
	index=internal_istart;
	}
else {
	gs = 0;
	ge = ei->graph_width;
	index=ei->graph_start;
	}

SetDrMd(&ei->rp,JAM1);

if (ei->graph_buffer && ei->zoom_scale >= ei->gb_scale && ei->zoom_scale!=1) {
	
	/** Going to redraw graph from graph_buffer **/

	for (x=gs; x < ge; x++) {
		offset=div_tab[ei->graph_buffer[index/ei->gb_scale]];
	   Move (&ei->rp, x, y-offset);
  	   Draw (&ei->rp, x, y+offset);
		index += ei->zoom_scale;	/**!!!! combine gb_scale & use fixed point math!! **/
   	}
	}

	/** Going to redraw graph direct from disk **/

else {
	new_pointer(PTR_BUSY);
	sp = SampleOpen(ei->cur, SP_MODE_OLDFILE);
	if (sp==0) {
	/*dprintf("Cant open file in graph!\n");*/
		new_pointer(PTR_NORMAL);
		return;
		}

	if (gs==0) {/** Expose left? **/
		Move(&ei->rp, gs, y+GMAP(SampleGetOne(sp,index)));
		SampleSeek(sp, index, SS_OFFSET_BEGINNING);
		}
	else {				/** No, expose right **/
		Move(&ei->rp, gs-1, y+GMAP(SampleGetOne(sp,index-ei->zoom_scale)));
		}

	/** Try and read a block of data **/

	range=(ge-gs)*ei->zoom_scale;
	if (range <= SMALLBUFSIZE) {
		indexstart=index;
		SampleRead(sp, quickbuffer, range);
	
		for (x=gs; x < ge; x++) {
			Draw(&ei->rp, x, y+GMAP(quickbuffer[index-indexstart]));
			index += ei->zoom_scale;
		   }
		}

	/** Do a seek and read one byte **/		

	else {
		for (x=gs; x < ge; x++) {
			Draw(&ei->rp, x, y+GMAP(SampleGetOne(sp,index)));
			index += ei->zoom_scale;
		   }
		}

	if (gs==0)	{/** expose left? **/
		Draw(&ei->rp, x, y+GMAP(SampleGetOne(sp,buffertosample(ge))));
		}
	SampleClose(sp);
	new_pointer(PTR_NORMAL);
	}

/** convert pixel position to a sample number **/

	i1=buffertosample(gs);
	i2=buffertosample(ge-1);

/** get start & end sample numbers rounded so they map into the **/
/** current display resolution without fractions **/

	smrnd = matchscale(ei->start_mark);
	emrnd = matchscale(ei->end_mark);

/*printf("gs %d i1 %d ge %d i2 %d sm %d em %d\n",gs,i1,ge,i2, ei->start_mark,ei->end_mark);*/

	if (!(smrnd >= i2 || emrnd <= i1) /** any part touch graphed area? **/
		&& ei->marked_samp) {

		/** draw marked range cliped to graph edge **/

		if (INRANGE(smrnd,i1,i2))
			x1=sampletobuffer(smrnd);
		else
			x1=gs;
		if (INRANGE(emrnd,i1,i2))
			x2=sampletobuffer(emrnd);
		else
			x2=ge-1;
		SetDrMd(&ei->rp,COMPLEMENT);
	   RectFill(&ei->rp, x1, 0, x2, ei->graph_height-1);
		}

CopyDoubleBuffer(ei);
}

/***************************************************************/

void move_graph(ei,snatch_gs)
struct EditInstance *ei;
int snatch_gs;
{
int delta;	/* pixel's to scroll graph */

snatch_gs=matchscale(snatch_gs);
delta=FixOldX(ei,snatch_gs);

if (delta==0) {
	ei->graph_start = snatch_gs;	/** probably redundant **/
	return;
	}

if (delta >= ei->graph_width) {
	ei->graph_start = snatch_gs;
	graph(ei);
	return;
	}

SetBPen(&ei->rp,BROWN);
if (snatch_gs > ei->graph_start) {    /** scroll left **/
	ScrollRaster(&ei->rp, delta,0,0,0,ei->graph_width-1,ei->graph_height-1);
	internal_gstart=ei->graph_width-delta;
	internal_gend=ei->graph_width;
	internal_istart=snatch_gs+internal_gstart*ei->zoom_scale;
	}
else {
	ScrollRaster(&ei->rp,-delta,0,0,0,ei->graph_width-1,ei->graph_height-1);
	internal_gstart=0;
	internal_gend=delta;
	internal_istart=snatch_gs;
	}

internal_update=TRUE;
/*printf("ei->graph_start %d snatch_gs %d\n",ei->graph_start,snatch_gs);*/

ei->graph_start = snatch_gs;
graph(ei);
internal_update=FALSE;
}

/***************************************************************/

int FixOldX(ei,snatch_gs)
struct EditInstance *ei;
int snatch_gs;
{
int delta;	/* pixel's to scroll graph */

delta = (ei->graph_start-snatch_gs)/ei->zoom_scale;
ei->old_x1 += delta;
ei->old_x2 += delta;
if (delta < 0)
	delta = -delta;
return(delta);
}

/***************************************************************/

void CopyDoubleBuffer(ei)
struct EditInstance *ei;
{
LockGadLibRender();
ClipBlit(&ei->rp,0,0,									/** source **/
			ei->edit_win->RPort,GRAPH_X,GRAPH_Y1,	/** destination **/
			ei->graph_width,								/** xsize **/
			ei->graph_height,								/** ysize **/
			0x0c0);											/** copy minterm **/

ei->line_pos=0xFFFFFFFF;		/** meanss no line xored on graph **/
UnLockGadLibRender();
}

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