#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include <libraries/dos.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"

extern struct Window *edit_win;
extern long zoom_scale;		/* samples per pixel */
extern long max_zoom;			/* largest zoom that will fit in this window */
extern long new_zoom;
extern struct StudioBase *StudioBase;
extern long edit_flags;
extern struct EditInstance EI[EI_MAX];
extern short longest_ei;

int AppendDataClips();
void swapint();
void RedrawGraphBuffer();


/************************************************************/

DoNonDestKeep(ei)
struct EditInstance *ei;
{
struct SampleDataClip temp[DS_NUM_CLIPS];
int error;

setmem(temp, sizeof(temp), 0);
error=AppendDataClips(&ei->sfp->tag.dclips[0], ei->start_mark,ei->end_mark, temp);
if (error==NO_ERROR) {
	if (SumDataClipLength(temp) < TOO_SMALL)
		return(STDERR_SAMPLE_TOO_SMALL);
	FillUndoBuffer(ei);
	movmem(temp, &ei->sfp->tag.dclips[0], sizeof(temp));

	ei->marked_samp=FALSE;	/** not marked any more! **/
	RedrawGraphBuffer(ei);
	SampleWriteTag(ei->sfp);
	}
return(error);
}

/************************************************************/

DoNonDestNoClips(ei)
struct EditInstance *ei;
{
short i;

FillUndoBuffer(ei);
ei->sfp->tag.dclips[0].start=0;
ei->sfp->tag.dclips[0].end=ei->sfp->tag.org_length-1;

for (i=1; i < DS_NUM_CLIPS; i++) {
	ei->sfp->tag.dclips[i].start=0;
	ei->sfp->tag.dclips[i].end=0;
	}

ei->marked_samp=FALSE;	/** not marked any more! **/
RedrawGraphBuffer(ei);
SampleWriteTag(ei->sfp);
return(NO_ERROR);
}

/************************************************************/

DoNonDestCut(ei)
struct EditInstance *ei;
{
DoNonDestCopy(ei);
return(DoNonDestDelete(ei));
}

/************************************************************/

DoNonDestCopy(ei)
struct EditInstance *ei;
{
setmem(ei->copybuffer, sizeof(ei->copybuffer), 0);
return(AppendDataClips(ei->sfp->tag.dclips, ei->start_mark,ei->end_mark, ei->copybuffer));
}

/************************************************************/

FillUndoBuffer(ei)
struct EditInstance *ei;
{
movmem(&ei->sfp->tag.dclips[0], ei->undobuffer, sizeof(ei->undobuffer));
ei->undo_start_mark=ei->start_mark;
ei->undo_end_mark=ei->end_mark;
ei->undo_marked_samp=ei->marked_samp;
return(NO_ERROR);
}

/************************************************************/

RestoreUndoBuffer(ei)
struct EditInstance *ei;
{
struct SampleDataClip temp[DS_NUM_CLIPS];

movmem(&ei->sfp->tag.dclips[0], temp, sizeof(temp));
movmem(ei->undobuffer, &ei->sfp->tag.dclips[0], sizeof(ei->undobuffer));
movmem(temp, ei->undobuffer, sizeof(ei->undobuffer));

swapint(&ei->undo_start_mark,&ei->start_mark);
swapint(&ei->undo_end_mark,&ei->end_mark);
swapint(&ei->undo_marked_samp,&ei->marked_samp);
SampleWriteTag(ei->sfp);

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

DoNonDestUndo(ei)
struct EditInstance *ei;
{
RestoreUndoBuffer(ei);
RedrawGraphBuffer(ei);
SampleWriteTag(ei->sfp);
return(NO_ERROR);
}

/************************************************************/

DoNonDestPaste(ei,type)
struct EditInstance *ei;
int type;
{
struct SampleDataClip temp[DS_NUM_CLIPS];
int error;
long insert_point;

FillUndoBuffer(ei);
setmem(temp, sizeof(temp), 0);

if (type==PT_INSERT_END)
	insert_point=ei->end_mark+1;
else
	insert_point=ei->start_mark;

if (insert_point) 
	AppendDataClips(&ei->sfp->tag.dclips[0], 0, insert_point-1, temp);
error=AppendDataClips(ei->copybuffer,0, SumDataClipLength(ei->copybuffer)-1, temp);

if (type==PT_OVERWRITE) {
	error=AppendDataClips(&ei->sfp->tag.dclips[0], ei->end_mark+1, ei->sfp->tag.length-1, temp);
	ei->end_mark=ei->start_mark+SumDataClipLength(ei->copybuffer)-1;
	}
else	{ /** must be PT_INSERT_START **/
	if (insert_point<ei->sfp->tag.length)
		error=AppendDataClips(&ei->sfp->tag.dclips[0], insert_point, ei->sfp->tag.length-1, temp);
	}

if (error==NO_ERROR) {
	movmem(temp, &ei->sfp->tag.dclips[0], sizeof(temp));
	RedrawGraphBuffer(ei);
	SampleWriteTag(ei->sfp);
	}
return(error);
}

/************************************************************/
/** Non Destructive delete												*/
/************************************************************/

DoNonDestDelete(ei)
struct EditInstance *ei;
{
struct SampleDataClip temp[DS_NUM_CLIPS];
int error;
setmem(temp, sizeof(temp), 0);
AppendDataClips(&ei->sfp->tag.dclips[0], 0, ei->start_mark-1, temp);
error=AppendDataClips(&ei->sfp->tag.dclips[0], ei->end_mark+1, ei->sfp->tag.length-1, temp);
if (error==NO_ERROR) {
	if (SumDataClipLength(temp) < TOO_SMALL)
		return(STDERR_SAMPLE_TOO_SMALL);
	FillUndoBuffer(ei);
	movmem(temp, &ei->sfp->tag.dclips[0], sizeof(temp));

	ei->marked_samp=FALSE;	/** not marked any more! **/
	RedrawGraphBuffer(ei);
	SampleWriteTag(ei->sfp);
	}
return(error);
}

/************************************************************/

int AppendDataClips(srcdclip, editstart, editend, destdclip)

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
	return(NO_ERROR);

/** Find 1st free destination dclip entry **/

if (destdclip[DS_NUM_CLIPS-1].end!=0)
	return(STDERR_NO_MORE_CLIPS);

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
	if (dest==DS_NUM_CLIPS) {
		destdclip[firstdest].start=start;
		destdclip[dest-1].end=end;
		return(STDERR_NO_MORE_CLIPS);
		}
	destdclip[dest].start = srcdclip[src].start;
	destdclip[dest].end = srcdclip[src].end;
	dest++;
	}

/** adjust destination dclips to match passed editstart/end **/

destdclip[firstdest].start=start;
destdclip[dest-1].end=end;
return(NO_ERROR);
}

/************************************************************/
