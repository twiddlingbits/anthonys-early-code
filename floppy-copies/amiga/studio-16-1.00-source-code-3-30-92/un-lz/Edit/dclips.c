#include <exec/types.h>
#include <exec/exec.h>
#include <stdio.h>
#include "/include/soundedit.h"
#include "/include/studio16.h"

void MapEditedToData2();


/************************************************************/
/** Given a corrdinate in the "edited" sample, return      **/
/** the equivilant position in the actual data on disk 	  **/
/************************************************************/

int MapEditedToData(samp,editval)
struct Disk_Samp *samp;
int editval;
{
short dclipindex;
int fileindex;

MapEditedToData2(&samp->dclips[0],editval,&fileindex,&dclipindex);
return(fileindex);
}

/************************************************************/

void MapEditedToData2(dclips,editval,fileindex,dclipindex)
struct SampleDataClip *dclips;
int editval;
int *fileindex;
short *dclipindex;
{
short i;
int edit_pos;
int clip_len;

i=0;
edit_pos=0;
while (dclips[i].end && i < DS_NUM_CLIPS) {
	clip_len=DataClipLength(&dclips[i]);
	if (editval >= edit_pos && editval < (edit_pos+clip_len)) {
		*fileindex=dclips[i].start+editval-edit_pos;
		*dclipindex=i;
		return;
		}
	edit_pos += clip_len;
	i++;
	}
*fileindex=-1;
*dclipindex=i;
}

/************************************************************/

DataClipLength(dclip)
struct SampleDataClip *dclip;
{
return(dclip->end-dclip->start+1);
}

/************************************************************/

int SumDataClipLength(dclips)
struct SampleDataClip *dclips;
{
int i;
int clip_len;

i=0;
clip_len=0;

while (dclips[i].end && i < DS_NUM_CLIPS) {
	clip_len +=DataClipLength(&dclips[i]);
	i++;
	}

return(clip_len);
}

/************************************************************/
/** Generate a graph buffer based on dclips & the original **/
/** graph buffer **/
/************************************************************/

void GenEditGraphBuffer(ei)
struct EditInstance *ei;
{
extern int new_zoom;
struct SampleDataClip *dclips;
int i;
int gbdest,gbsrc,gblen;
int edit_pos;
int clip_len;

dclips=&ei->cur->dclips[0];

/** first calc new sample length based on edits **/

ei->cur->length=SumDataClipLength(dclips);

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
