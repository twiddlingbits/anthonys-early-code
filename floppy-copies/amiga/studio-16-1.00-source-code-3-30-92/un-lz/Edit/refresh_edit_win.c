#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"
#include "/include/soundedit.h"

extern struct NewGadgetHeader dadd;
extern struct NewGadgetHeader dpaste;
extern struct NewGadgetHeader dcopy;
extern struct NewGadgetHeader compress;
extern struct NewGadgetHeader keep;
extern struct NewGadgetHeader delete;
extern struct NewGadgetHeader paste;
extern struct NewGadgetHeader copy;
extern struct NewGadgetHeader cut;
extern struct NewGadgetHeader nondest;
extern struct NewGadgetHeader begin;
extern struct NewGadgetHeader flip;
extern struct NewGadgetHeader scale;
extern struct NewGadgetHeader freehand;
extern struct NewGadgetHeader silence;
extern struct NewGadgetHeader undo;
extern struct NewGadgetHeader rate;
extern struct NewGadgetHeader range;
extern struct NewGadgetHeader showrange;
extern struct NewGadgetHeader stop;
extern struct NewGadgetHeader playwave;
extern struct NewGadgetHeader playrange;
extern struct NewGadgetHeader zoombutn;
extern struct NewGadgetHeader zoom;
extern struct NewGadgetHeader hscroll;

extern struct EditInstance editparms;

void EditSetGadsBox();
void EditGadsResize();

void refresh_edit_win(edit_win)
struct Window *edit_win;
{
extern struct EditInstance *ei;
extern struct NewSlider hscroll;
extern struct NewSlider zoom;
short w,h;
static char wintitle[80];

w=edit_win->Width;
h=edit_win->Height;

BuildDivTab(ei);	/** used by graph for quick division **/

LockGadLibRender();
SetAPen(edit_win->RPort, BROWN);	/** USE STUDIO PENS !!!! **/
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, 2,12, w-3, h-10);   /* draw down to resize gad  */
RectFill(edit_win->RPort, 2,h-9, w-17, h-2); /* fill  left of resize gad */
draw_indent(edit_win->RPort, GRAPH_X-2, GRAPH_Y1-1, ei->graph_width+GRAPH_X+1, ei->graph_height+GRAPH_Y1,DARK_BROWN,WHITE);

EditSetGadsBox();
EditGadsResize(edit_win);
graph(ei);
UnLockGadLibRender();
refresh_gadget_chain(edit_win);

ei->old_x1 = sampletomousenoclip(matchscale(ei->start_mark));
ei->old_x2 = sampletomousenoclip(matchscale(ei->end_mark));

if (ei->cur!=NULL)
   sprintf(wintitle,"%s",ei->cur->name);
else
   sprintf(wintitle,"No Samples Open");


WindTitleChange(ei->edit_win,wintitle);
}

/************************************************************/

void EditGadsResize(edit_win)
struct Window *edit_win;
{
struct NewGadgetHeader *ng;
short w,h;

w=edit_win->Width;
h=edit_win->Height;

if ((ei->zoom_scale==ei->max_zoom) && (w==edit_win->MaxWidth))		/** Too eliminate any rounding confusion **/
	((struct slider *)(hscroll.gad_ptr))->show_size = ((struct slider *)(hscroll.gad_ptr))->list_size;
else
	((struct slider *)(hscroll.gad_ptr))->show_size = ei->graph_width*ei->zoom_scale;

new_gadget_size(hscroll.gad_ptr,
	GRAPH_X-2,											/* x1 */
	ei->edit_win->Height - GRAPH_Y2 + 2,		/* y1 */
	ei->edit_win->Width - GRAPH_X + 1,			/* x2 */
	ei->edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H);	/* y2 */

/** resizing gadget could alter the graph's starting position **/

	ei->graph_start = matchscale(((struct slider *)hscroll.gad_ptr)->top_element);
	FixOldX(ei,ei->graph_start);

new_gadget_size(zoom.gad_ptr,
	GRAPH_X-2,																/* x1 */
	ei->edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H + 2,		/* y1 */
	GRAPH_X + 80,															/* x2 */
	ei->edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H*2 + 2);	/* y2 */


for (ng=&zoom; ng; ng=ng->next) {
	new_gadget_size(ng->gad_ptr, ng->box_x1, ng->box_y1, ng->box_x2, ng->box_y2);
	}
}

/************************************************************/

void EditSetGadsBox()
{
short row1_y1,row1_y2;
short row2_y1,row2_y2;
short row3_y1,row3_y2;
short row4_y1,row4_y2;

hscroll.box_y1=editparms.edit_win->Height - GRAPH_Y2 + 2;
hscroll.box_y2=hscroll.box_y1+HSCROLL_H;
hscroll.box_x2=editparms.edit_win->Width - GRAPH_X + 1;

row1_y1=editparms.edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H + 3;
row1_y2=row1_y1+10;

row2_y1=row1_y2+3;
row2_y2=row2_y1+10;

row3_y1=row2_y2+3;
row3_y2=row3_y1+10;

row4_y1=row3_y2+3;
row4_y2=row4_y1+10;

/** Row 4 **/

dadd.box_y1=row4_y1;
dadd.box_y2=row4_y2;
dpaste.box_y1=row4_y1;
dpaste.box_y2=row4_y2;
dcopy.box_y1=row4_y1;
dcopy.box_y2=row4_y2;
compress.box_y1=row4_y1;
compress.box_y2=row4_y2;

keep.box_y1=row4_y1;
keep.box_y2=row4_y2;
delete.box_y1=row4_y1;
delete.box_y2=row4_y2;
paste.box_y1=row4_y1;
paste.box_y2=row4_y2;
copy.box_y1=row4_y1;
copy.box_y2=row4_y2;
cut.box_y1=row4_y1;
cut.box_y2=row4_y2;
nondest.box_y1=row4_y1;
nondest.box_y2=row4_y2;

/** Row 3 **/

begin.box_y1=row3_y1;
begin.box_y2=row3_y2;
flip.box_y1=row3_y1;
flip.box_y2=row3_y2;
scale.box_y1=row3_y1;
scale.box_y2=row3_y2;
freehand.box_y1=row3_y1;
freehand.box_y2=row3_y2;
silence.box_y1=row3_y1;
silence.box_y2=row3_y2;

/** ROW 2 **/

undo.box_y1=row2_y1;
undo.box_y2=row2_y2;
rate.box_y1=row2_y1;
rate.box_y2=row2_y2;
range.box_y1=row2_y1;
range.box_y2=row2_y2;
showrange.box_y1=row2_y1;
showrange.box_y2=row2_y2;

/*** ROW 1 **/

stop.box_y1=row1_y1;
stop.box_y2=row1_y2;
playwave.box_y1=row1_y1;
playwave.box_y2=row1_y2;
playrange.box_y1=row1_y1;
playrange.box_y2=row1_y2;
zoombutn.box_y1=row1_y1;
zoombutn.box_y2=row1_y2;
zoom.box_y1=row1_y1;
zoom.box_y2=row1_y2;
}