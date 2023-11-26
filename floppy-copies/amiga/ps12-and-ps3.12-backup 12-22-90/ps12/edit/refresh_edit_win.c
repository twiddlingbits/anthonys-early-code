#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/psound.h"

void refresh_edit_win()
{
extern struct Disk_Samp samp[MAX_SAMPLES];
extern struct list open_list;
extern struct Window *edit_win;
extern int cur, mark_drawn, marked_samp;
extern short graph_mode;
short w,h;
static char wintitle[80];

cur = open_list.current;

w=edit_win->Width;
h=edit_win->Height;

SetAPen(edit_win->RPort, BROWN);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, 2,11, w-3, h-10);   /* draw down to resize gad  */
RectFill(edit_win->RPort, 2,h-9, w-17, h-2); /* fill  left of resize gad */
draw_indent(edit_win->RPort, GRAPH_X-2, GRAPH_Y-1, w-GRAPH_X+2, h-GRAPH_Y);
graph(graph_mode);
mark_drawn = FALSE;
if (cur==marked_samp)
   draw_marked(edit_win);

if (samp[cur].flags&SAMPLE_OPEN)
   sprintf(wintitle,"Editing: %s",samp[cur].name);
else
   sprintf(wintitle,"No Samples Open");
SetWindowTitles(edit_win,wintitle,-1);
}

