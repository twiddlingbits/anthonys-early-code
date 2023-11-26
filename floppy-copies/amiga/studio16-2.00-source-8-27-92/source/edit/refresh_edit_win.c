#include <exec/types.h>
#include <intuition/intuition.h>
#include "/include/studio16.h"
#include "/include/gadlib.h"
#include "/include/soundedit.h"
#include <stdio.h>

extern struct NewButton playwave;
extern struct NewSlider hscroll;

extern struct Window *edit_win;
extern long zoom_scale;		/* samples per pixel */
extern struct EditInstance EI[EI_MAX];
extern short longest_ei;

void EditSetGadsBox();
void EditGadsResize();
void ClearAllGraphs();
void RefreshEditTitle();

void refresh_edit_win(edit_win)
struct Window *edit_win;
{
extern struct StudioBase *StudioBase;
short w,h;

w=edit_win->Width;
h=edit_win->Height;

LockGadLibRender();
ClearAllGraphs();
EditSetGadsBox();
EditGadsResize(edit_win);
UnLockGadLibRender();

if (CountEI() > 0)
	DrawEditGraphBuffers();

RefreshEditTitle();
refresh_gadget_chain(edit_win);
update_status();
}

/************************************************************/

void RefreshEditTitle()
{
static char wintitle[160];
short i;
short cnt;

cnt=CountEI();

if (cnt==0) {
   sprintf(wintitle,"Editor");
	}
else if (cnt==1) {		/** show complete path name **/
	sprintf(wintitle,"%s",EI[longest_ei].samp_name);
	}
else {
	wintitle[0]=0;
	for (i=0; i < EI_MAX; i++) {
		if (EI[i].flags&EI_IN_USE) {
			strcat(wintitle,StrFileName(EI[i].samp_name));
			strcat(wintitle,",");
			}
		}
	wintitle[strlen(wintitle)-1]=NULL;
	}
WindTitleChange(edit_win,wintitle);
}

/************************************************************/

void ClearAllGraphs()
{
SetAPen(edit_win->RPort, StudioBase->defaults.colors.list.hitbox_unselected);
SetDrMd(edit_win->RPort, JAM1);
RectFill(edit_win->RPort, GRAPH_X,GRAPH_Y1, DRAWWIDTH+GRAPH_X, edit_win->Height - GRAPH_Y2);
draw_indent(edit_win->RPort, GRAPH_X-2, GRAPH_Y1-1, DRAWWIDTH+GRAPH_X+1, 
				edit_win->Height - GRAPH_Y2,StudioBase->defaults.colors.list.topleft_3d,
				StudioBase->defaults.colors.list.bottomright_3d);
}

/************************************************************/

void EditGadsResize(edit_win)
struct Window *edit_win;
{
struct NewGadgetHeader *ng;
short w,h,i;

w=edit_win->Width;
h=edit_win->Height;

if (CountEI()>0)
	((struct slider *)(hscroll.gad_ptr))->show_size = DRAWWIDTH*zoom_scale;
else
	((struct slider *)(hscroll.gad_ptr))->show_size = ((struct slider *)(hscroll.gad_ptr))->list_size;

new_gadget_size(hscroll.gad_ptr,
	GRAPH_X-2,											/* x1 */
	edit_win->Height - GRAPH_Y2 + 2,		/* y1 */
	edit_win->Width - GRAPH_X + 1,			/* x2 */
	edit_win->Height - GRAPH_Y2 + 2 + HSCROLL_H);	/* y2 */

/** resizing gadget could alter the graph's starting position **/
	for (i=0; i < EI_MAX; i++) {
		if (EI[i].flags&EI_IN_USE) {
			EI[i].graph_start = ((struct slider *)hscroll.gad_ptr)->top_element;
			EI[i].graph_start = matchscale(EI[i].graph_start);
			}
		}

for (ng=&playwave; ng; ng=ng->next) {
	new_gadget_size(ng->gad_ptr, ng->box_x1, ng->box_y1, ng->box_x2, ng->box_y2);
	}
}

/************************************************************/

void EditSetGadsBox()
{
short row1_y1,row1_y2;
short width;
short x;
struct NewButton *nb;

hscroll.box_y1=edit_win->Height - GRAPH_Y2 + 2;
hscroll.box_y2=hscroll.box_y1+HSCROLL_H;
hscroll.box_x2=edit_win->Width - GRAPH_X + 1;
hscroll.box_x1=GRAPH_X-2;

row1_y1=edit_win->Height - GRAPH_Y2 + HSCROLL_H + 7;
row1_y2=row1_y1+10;

width = (DRAWWIDTH+4)/7;
x=GRAPH_X-2;
for (nb=&playwave; nb; nb=nb->next) {
	nb->box_y1=row1_y1;
	nb->box_y2=row1_y2;
	nb->box_x1=x;
	nb->box_x2=x+width-4;
	x=x+width;
	}
}