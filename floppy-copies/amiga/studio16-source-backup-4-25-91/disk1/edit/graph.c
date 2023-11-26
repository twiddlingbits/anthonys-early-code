#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"


/***********************/
/** plot sample       **/
/***********************/

graph(ei, flags)

struct EditInstance *ei;
long flags;

{
short get_right_sample1();
int x,y;
int index,len;
/* short (*ptr)(); */
int offset;
short get_left_sample1();

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

y = GRAPH_Y+(ei->graph_height/2);
SetAPen(ei->edit_win->RPort,WHITE);  /** was RED **/

for (x=GRAPH_X; x < GRAPH_X+ei->graph_width; x++) {
   index = ((x-GRAPH_X)*len)/ei->graph_width;
   Move (ei->edit_win->RPort, x, y);
   if (flags&GRAPH_RIGHT)
      Draw (ei->edit_win->RPort, x, y+(get_right_sample1(ei->cur,index+offset)*(ei->graph_height/2))/32768);
   else
      Draw (ei->edit_win->RPort, x, y+(get_left_sample1(ei->cur,index+offset)*(ei->graph_height/2))/32768);
   }
}

