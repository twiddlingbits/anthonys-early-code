#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <libraries/dos.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"

#define TOO_SMALL 1281

extern struct EditInstance *ei;
void AppendDataClips();
void swapint();
void RedrawGraphBuffer();
void FixIckyGadBug();
void CalcNewScrollBars();
void RethinkZoom();
void SetNewZoom();

struct SampleDataClip copybuffer[DS_NUM_CLIPS];
struct SampleDataClip undobuffer[DS_NUM_CLIPS];
int undo_start_mark;
int undo_end_mark;
struct Disk_Samp *undo_marked_samp;
int undo_mark_drawn;

extern int new_zoom;
extern struct NewButton zoombutn;
extern struct NewSlider zoom;
extern char zoombutn_text[20];
extern struct StudioBase *StudioBase;

/************************************************************/

DoNonDestKeep()
{
struct SampleDataClip temp[DS_NUM_CLIPS];

setmem(temp, sizeof(temp), 0);
AppendDataClips(&ei->cur->dclips[0], ei->start_mark,ei->end_mark, temp);
if (SumDataClipLength(temp) < TOO_SMALL)
	return(STDERR_SAMPLE_TOO_SMALL);
FillUndoBuffer();
movmem(temp, &ei->cur->dclips[0], sizeof(temp));

ei->marked_samp=NULL;	/** not marked any more! **/
ei->mark_drawn=FALSE;
RedrawGraphBuffer(ei->edit_win);
return(NO_ERROR);
}

/************************************************************/

DoNonDestNoClips()
{
short i;

FillUndoBuffer();
ei->cur->dclips[0].start=0;
ei->cur->dclips[0].end=ei->cur->org_length-1;

for (i=1; i < DS_NUM_CLIPS; i++) {
	ei->cur->dclips[i].start=0;
	ei->cur->dclips[i].end=0;
	}

ei->marked_samp=NULL;	/** not marked any more! **/
ei->mark_drawn=FALSE;
RedrawGraphBuffer(ei->edit_win);
return(NO_ERROR);
}

/************************************************************/

DoNonDestCut()
{
DoNonDestCopy();
return(DoNonDestDelete());
}

/************************************************************/

DoNonDestCopy()
{
setmem(copybuffer, sizeof(copybuffer), 0);
AppendDataClips(ei->cur->dclips, ei->start_mark,ei->end_mark, copybuffer);
return(NO_ERROR);
}

/************************************************************/

FillUndoBuffer()
{
movmem(&ei->cur->dclips[0], undobuffer, sizeof(undobuffer));
undo_start_mark=ei->start_mark;
undo_end_mark=ei->end_mark;
undo_marked_samp=ei->marked_samp;
undo_mark_drawn=ei->mark_drawn;
return(NO_ERROR);
}

/************************************************************/

RestoreUndoBuffer()
{
struct SampleDataClip temp[DS_NUM_CLIPS];
/*int dest,sum;*/

movmem(&ei->cur->dclips[0], temp, sizeof(temp));
movmem(undobuffer, &ei->cur->dclips[0], sizeof(undobuffer));
movmem(temp, undobuffer, sizeof(undobuffer));

swapint(&undo_start_mark,&ei->start_mark);
swapint(&undo_end_mark,&ei->end_mark);
swapint(&undo_marked_samp,&ei->marked_samp);
swapint(&undo_mark_drawn,&ei->mark_drawn);

return(NO_ERROR);
}

void swapint(a,b)
int *a,*b;
{
int t;
t=*a;
*a=*b;
*b=t;
}

/************************************************************/

DoNonDestUndo()
{
RestoreUndoBuffer();
RedrawGraphBuffer(ei->edit_win);
return(NO_ERROR);
}

/************************************************************/

DoNonDestPaste()
{
struct SampleDataClip temp[DS_NUM_CLIPS];
int type;

type=AskUserPasteType();
if (type==PT_CANCEL)
	return(NO_ERROR);

FillUndoBuffer();
setmem(temp, sizeof(temp), 0);
AppendDataClips(&ei->cur->dclips[0], 0, ei->start_mark-1, temp);
AppendDataClips(copybuffer,0, DataClipLength(copybuffer)-1, temp);

if (type==PT_OVERWRITE) {
	AppendDataClips(&ei->cur->dclips[0], ei->end_mark+1, ei->cur->length-1, temp);
	ei->end_mark=ei->start_mark+DataClipLength(copybuffer)-1;
	}
else	{ /** must be PT_INSERT **/
	AppendDataClips(&ei->cur->dclips[0], ei->start_mark, ei->cur->length-1, temp);
	}

movmem(temp, &ei->cur->dclips[0], sizeof(temp));
/*ei->marked_samp=NULL;*/	/** not marked any more! **/
/*ei->mark_drawn=FALSE;*/
RedrawGraphBuffer(ei->edit_win);
return(NO_ERROR);
}

/************************************************************/
/** Non Destructive delete												*/
/************************************************************/

DoNonDestDelete()
{
struct SampleDataClip temp[DS_NUM_CLIPS];

setmem(temp, sizeof(temp), 0);
AppendDataClips(&ei->cur->dclips[0], 0, ei->start_mark-1, temp);
AppendDataClips(&ei->cur->dclips[0], ei->end_mark+1, ei->cur->length-1, temp);
if (SumDataClipLength(temp) < TOO_SMALL)
	return(STDERR_SAMPLE_TOO_SMALL);
FillUndoBuffer();
movmem(temp, &ei->cur->dclips[0], sizeof(temp));

ei->marked_samp=NULL;	/** not marked any more! **/
ei->mark_drawn=FALSE;
RedrawGraphBuffer(ei->edit_win);
return(NO_ERROR);
}

/************************************************************/

void AppendDataClips(srcdclip, editstart, editend, destdclip)

struct SampleDataClip *srcdclip;
struct SampleDataClip *destdclip;
int editstart;
int editend;
{
short src, dest,firstdest;
int start,end;
short startdclip, enddclip;

/** Check for zero byte moves **/

if (editend < editstart)
	return;

/** Find 1st free destination dclip entry **/

for (dest=0; dest < DS_NUM_CLIPS; dest++) {
	if (destdclip[dest].end==0)
		break;
	}

firstdest = dest;

/** Find source dclip range **/

MapEditedToData2(srcdclip,editstart,&start,&startdclip);
MapEditedToData2(srcdclip,editend,&end,&enddclip);

/** append source dclips to destination dclip array **/

for (src=startdclip; src <= enddclip; src++) {
	destdclip[dest].start = srcdclip[src].start;
	destdclip[dest].end = srcdclip[src].end;
	dest++;
	}

/** adjust destination dclips to match passed editstart/end **/

destdclip[firstdest].start=start;
destdclip[dest-1].end=end;
}

/************************************************************/


void RedrawGraphBuffer(edit_win)
struct Window *edit_win;
{
extern struct EditInstance *ei;
extern struct NewSlider hscroll;
extern struct NewButton zoombutn;

GenEditGraphBuffer(ei);
RethinkZoom(ei);
refresh_gadget(zoombutn.gad_ptr);
refresh_gadget(hscroll.gad_ptr);

LockGadLibRender();
graph(ei);
UnLockGadLibRender();

ei->old_x1 = sampletomousenoclip(matchscale(ei->start_mark));
ei->old_x2 = sampletomousenoclip(matchscale(ei->end_mark));
update_status(ei);
}

/************************************************************/

void RethinkZoom(ei)
struct EditInstance *ei;
{
int maxwidth;

ei->max_zoom = ei->cur->length/(StudioBase->screen->Width-GRAPH_X-GRAPH_X)+1;

if (ei->zoom_scale > ei->max_zoom) {
	ei->zoom_scale = ei->max_zoom;	
	new_zoom = ei->zoom_scale;
	}

maxwidth = ei->cur->length/ei->max_zoom+GRAPH_X+GRAPH_X;
if (ei->edit_win->Width > maxwidth) {
	SizeWindow(ei->edit_win,maxwidth-ei->edit_win->Width,0);
	ei->flags|=EI_SET_MAX_WIDTH;
	}
CalcNewScrollBars();
WindowLimits(ei->edit_win,0,0,maxwidth,0);
}

/***********************************************************/

void CalcNewScrollBars()
{
struct slider *s;

s=(struct slider *)hscroll.gad_ptr;
s->list_size = ei->cur->length;
s->show_size = ei->graph_width*ei->zoom_scale;
ei->graph_start = matchscale(min(ei->cur->length-s->show_size, ei->graph_start));
s->top_element = ei->graph_start;
FixIckyGadBug(s);
calc_slider_internals(s,&hscroll);

SetNewZoom(new_zoom);
}

/** I'm using calc_slider_internals when I should really add
	 a new gadlib function.  calc_slider_internals assumes
	 you have set a new s->box and it makes internal adjustments
	 opposite to those i'm making here ***/

void FixIckyGadBug(s)
struct slider *s;
{
if (s->flags&BORDER_3D) {
   s->box_x1 -= 2;
   s->box_x2 += 2;
   s->box_y1 -= 1;
   s->box_y2 += 1;
   }
}

void SetNewZoom(nz)
int nz;
{
struct slider *s;

new_zoom=nz;
s=(struct slider *)zoom.gad_ptr;
s->top_element = new_zoom-1;
s->list_size = ei->max_zoom;
s->show_size = 1;
FixIckyGadBug(s);
calc_slider_internals(s,&zoom);
sprintf(zoombutn_text,"ZM 1:%d",ei->zoom_scale);
((struct button *)(zoombutn.gad_ptr))->text=zoombutn_text;
}

/************************************************************/
