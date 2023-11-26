#include "exec/types.h"
#include "intuition/intuition.h"
#include "/include/psound.h"


extern struct Disk_Samp *cur;
extern int start_mark, end_mark;
extern struct Disk_Samp *marked_samp;
extern int graph_width;
extern int graph_height;
extern struct Window *edit_win;

/***********************/
/** plot sample       **/
/***********************/

graph(flags)

long flags;

{
short get_right_sample1();
int x,y;
int index,len;
/* short (*ptr)(); */
int offset;
short get_left_sample1();

#if 0
if (flags&GRAPH_RIGHT) {
   ptr = get_right_sample1;
   }

if (flags&GRAPH_LEFT) {
   ptr = get_left_sample1;
   }
#endif

SetAPen(edit_win->RPort,BROWN);  /** was BLACK **/
SetDrMd(edit_win->RPort,JAM1);
RectFill(edit_win->RPort,GRAPH_X,GRAPH_Y,GRAPH_X+graph_width-1,GRAPH_Y+graph_height-1);

if (cur==NULL) return (0);

if (flags&GRAPH_ZOOM) {
   if (marked_samp!=cur) return (0);
   offset=start_mark;
   len=end_mark-start_mark+1;
   }
else {
   offset=0;
   len=cur->length;
   }

y = GRAPH_Y+(graph_height/2);
SetAPen(edit_win->RPort,WHITE);  /** was RED **/

for (x=GRAPH_X; x < GRAPH_X+graph_width; x++) {
   index = ((x-GRAPH_X)*len)/graph_width;
   Move (edit_win->RPort, x, y);
   if (flags&GRAPH_RIGHT)
      Draw (edit_win->RPort, x, y+(get_right_sample1(cur,index+offset)*(graph_height/2))/32768);
   else
      Draw (edit_win->RPort, x, y+(get_left_sample1(cur,index+offset)*(graph_height/2))/32768);
   }
}

#if 0
void lgraph()

{
int x,y;
int index,len;
int num_samples;
int i,endy;

num_samples = LGRAPH_W/6;

SetAPen(edit_win->RPort,BLACK);
SetDrMd(edit_win->RPort,JAM1);
RectFill(edit_win->RPort,LGRAPH_X,LGRAPH_Y,LGRAPH_X+LGRAPH_W-1,LGRAPH_Y+LGRAPH_H-1);

if (cur==NULL) || marked_samp!=cur) return (0);

len=cur->length;
y = LGRAPH_Y+(LGRAPH_H/2);
SetAPen(edit_win->RPort, WHITE);

index = start_mark - num_samples/2;
for (x=LGRAPH_X; x < LGRAPH_X+num_samples*6; x=x+6) {
   if (index >=0 && index <len) {
      endy = y+(get_left_sample1(cur,index)*(LGRAPH_H/2))/32768;
      if (index==start_mark) {
         Move (edit_win->RPort, x+2, LGRAPH_Y);
         Draw (edit_win->RPort, x+2, LGRAPH_Y+LGRAPH_H);
         }
      for (i=0; i < 5; i++) {
         Move (edit_win->RPort, x+i, y);
         Draw (edit_win->RPort, x+i, endy);
         }
      }
   index ++;
   }
}

 /***!!!!!! DO THESE GUES HANDLE R/L KEY CORRECTLY??? ***/
void rgraph()

{
int x,y;
int index,len;
int num_samples;
int i,endy;

num_samples = RGRAPH_W/6;

SetAPen(edit_win->RPort,BLACK);
SetDrMd(edit_win->RPort,JAM1);
RectFill(edit_win->RPort,RGRAPH_X,RGRAPH_Y,RGRAPH_X+RGRAPH_W-1,RGRAPH_Y+RGRAPH_H-1);

if (cur->flags==NULL) || marked_samp!=cur) return (0);

len=cur->length;
y = RGRAPH_Y+(RGRAPH_H/2);
SetAPen(edit_win->RPort, WHITE);

index = end_mark - num_samples/2;
for (x=RGRAPH_X; x < RGRAPH_X+num_samples*6; x=x+6) {
   if (index >=0 && index <len) {
      endy = y+(get_left_sample1(cur,index)*(RGRAPH_H/2))/32768;
      if (index==end_mark) {
         Move (edit_win->RPort, x+2, RGRAPH_Y);
         Draw (edit_win->RPort, x+2, RGRAPH_Y+RGRAPH_H);
         }
      for (i=0; i < 5; i++) {
         Move (edit_win->RPort, x+i, y);
         Draw (edit_win->RPort, x+i, endy);
         }
      }
   index ++;
   }
}

void erase_zoom_graphs()
{
SetAPen(edit_win->RPort,BLACK);
SetDrMd(edit_win->RPort,JAM1);
RectFill(edit_win->RPort,LGRAPH_X,LGRAPH_Y,LGRAPH_X+LGRAPH_W-1,LGRAPH_Y+LGRAPH_H-1);
RectFill(edit_win->RPort,RGRAPH_X,RGRAPH_Y,RGRAPH_X+RGRAPH_W-1,RGRAPH_Y+RGRAPH_H-1);
}

#endif

/** Called by monitor for dynamic graphing **/

void rec_plot(pt)
char pt;
{
extern struct RastPort *rec_rp;

ScrollRaster(rec_rp,-1,0,REC_X,REC_Y,REC_X2,REC_Y2);
Move(rec_rp,REC_X,REC_ZERO);
Draw(rec_rp,REC_X,pt/REC_SCALE+REC_ZERO);
}

void box_plot()
{
Move(rec_rp,REC_X-1,REC_Y-1);
Draw(rec_rp,REC_X-1,REC_Y2+1);
Draw(rec_rp,REC_X2+1,REC_Y2+1);
Draw(rec_rp,REC_X2+1,REC_Y-1);
Draw(rec_rp,REC_X-1,REC_Y-1);

Move(rec_rp,REC_X-2,REC_Y-2);
Draw(rec_rp,REC_X-2,REC_Y2+2);
Draw(rec_rp,REC_X2+2,REC_Y2+2);
Draw(rec_rp,REC_X2+2,REC_Y-2);
Draw(rec_rp,REC_X-2,REC_Y-2);
}
