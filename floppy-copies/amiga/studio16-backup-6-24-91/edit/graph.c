#include <exec/types.h>1
#include <intuition/intuition.h>
#include <libraries/dos.h>
#include "/include/studio16.h"
#include "/include/soundedit.h"



short GetOneSample();

/***********************/
/** plot sample       **/
/***********************/

graph(ei)

struct EditInstance *ei;

{
short y;
int x;
int index,len;
int offset;
int fp;
short div_const;
short flags;

flags = 0;

SetAPen(ei->edit_win->RPort,BROWN);  /** was BLACK **/
SetDrMd(ei->edit_win->RPort,JAM1);
RectFill(ei->edit_win->RPort,GRAPH_X,GRAPH_Y,GRAPH_X+ei->graph_width-1,GRAPH_Y+ei->graph_height-1);

if (ei->cur==NULL) return (0);

if (flags&GRAPH_ZOOM) {
   if (ei->marked_samp!=ei->cur) return (0);
   offset=ei->start_mark;
   len=ei->end_mark-ei->start_mark+1;
   }
else {
   offset=0;
   len=ei->cur->length;
   }

fp = ReadLockSamp(ei->cur);

y = GRAPH_Y+(ei->graph_height/2);
SetAPen(ei->edit_win->RPort,WHITE);  /** was RED **/


div_const = 32768/(ei->graph_height/2);

for (x=GRAPH_X; x < GRAPH_X+ei->graph_width; x++) {
   index = ((x-GRAPH_X)*len)/ei->graph_width;
   Move (ei->edit_win->RPort, x, y);
   if (flags&GRAPH_RIGHT)
      Draw (ei->edit_win->RPort, x, y+GetOneSample(fp,index+offset)/div_const);
   else
      Draw (ei ->edit_win->RPort, x, y+GetOneSample(fp,index+offset)/div_const);
/*      Draw (ei->edit_win->RPort, x, y+(graph_get_samp(fp,index+offset)*(ei->graph_height/2))/32768);*/
   }
ReadUnLockSamp(ei->cur, fp);
}

