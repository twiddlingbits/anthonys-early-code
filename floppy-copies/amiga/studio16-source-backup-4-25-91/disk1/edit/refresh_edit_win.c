#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/psound.h"

void refresh_edit_win(edit_win)
struct Window *edit_win;
{
extern struct list *open_list;
extern struct EditInstance *ei;
short w,h;
static char wintitle[80];

w=edit_win->Width;
h=edit_win->Height;

SetAPen(edit_win->RPort, BROWN);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, 2,11, w-3, h-10);   /* draw down to resize gad  */
RectFill(edit_win->RPort, 2,h-9, w-17, h-2); /* fill  left of resize gad */
draw_indent(edit_win->RPort, GRAPH_X-2, GRAPH_Y-1, w-GRAPH_X+2, h-GRAPH_Y,DARK_BROWN,WHITE);
graph(ei, GRAPH_LEFT);
ei->mark_drawn = FALSE;
if (ei->cur==ei->marked_samp)
   draw_marked(edit_win);

if (ei->cur!=NULL)
   sprintf(wintitle,"Editing: %s",ei->cur->name);
else
   sprintf(wintitle,"No Samples Open");
SetWindowTitles(edit_win,wintitle,-1);
}

