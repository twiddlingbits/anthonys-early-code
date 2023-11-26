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

int MapEditedToData(dclips,editval)
struct SampleDataClip *dclips;
int editval;
{
short dclipindex;
int fileindex;

MapEditedToData2(dclips,editval,&fileindex,&dclipindex);
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
/*kprintf("MAPFROM %04x%04x MAPTO: clip # %04x%04x clipindex %04x%04x\n",editval, *dclipindex, *fileindex); */
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
