#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"


extern struct Samp samp[MAX_SAMPLES];
extern int cur, start_mark, end_mark;
extern int marked_samp;
extern struct Screen *screen;

/***********************/
/** plot sample       **/
/***********************/

void graph(flags)

int flags;
{
int x,y;
int index,len;
char *ptr;
int offset;

SetAPen(&screen->RastPort,BLACK);
SetDrMd(&screen->RastPort,JAM1);
RectFill(&screen->RastPort,GRAPH_X,GRAPH_Y,GRAPH_X+GRAPH_W-1,GRAPH_Y+GRAPH_H-1);

if (flags&GRAPH_RIGHT) {
   ptr=samp[cur].rmem;
   }
else {
   ptr=samp[cur].lmem;
   }

if (ptr==0) return (0);

if (flags&GRAPH_ZOOM) {
   if (marked_samp!=cur) return (0);
   offset=start_mark;
   len=end_mark-start_mark+1;
   }
else {
   offset=0;
   len=samp[cur].length;
   }

y = GRAPH_Y+(GRAPH_H/2);
SetAPen(&screen->RastPort,RED);

for (x=GRAPH_X; x < GRAPH_X+GRAPH_W; x++) {
   index = ((x-GRAPH_X)*len)/GRAPH_W;
   Move (&screen->RastPort, x, y);
   Draw (&screen->RastPort, x, y+(*(ptr+index+offset)*(GRAPH_H/2))/128);
   }
}

void lgraph()

{
int x,y;
int index,len;
int num_samples;
int i,endy;

num_samples = LGRAPH_W/6;

SetAPen(&screen->RastPort,BLACK);
SetDrMd(&screen->RastPort,JAM1);
RectFill(&screen->RastPort,LGRAPH_X,LGRAPH_Y,LGRAPH_X+LGRAPH_W-1,LGRAPH_Y+LGRAPH_H-1);

if (samp[cur].lmem==0 || marked_samp!=cur) return (0);

len=samp[cur].length;
y = LGRAPH_Y+(LGRAPH_H/2);
SetAPen(&screen->RastPort, WHITE);

index = start_mark - num_samples/2;
for (x=LGRAPH_X; x < LGRAPH_X+num_samples*6; x=x+6) {
   if (index >=0 && index <len) {
      endy = y+(*(samp[cur].lmem+index)*(LGRAPH_H/2))/128;
      if (index==start_mark) {
         Move (&screen->RastPort, x+2, LGRAPH_Y);
         Draw (&screen->RastPort, x+2, LGRAPH_Y+LGRAPH_H);
         }
      for (i=0; i < 5; i++) {
         Move (&screen->RastPort, x+i, y);
         Draw (&screen->RastPort, x+i, endy);
         }
      }
   index ++;
   }
}



void rgraph()

{
int x,y;
int index,len;
int num_samples;
int i,endy;

num_samples = RGRAPH_W/6;

SetAPen(&screen->RastPort,BLACK);
SetDrMd(&screen->RastPort,JAM1);
RectFill(&screen->RastPort,RGRAPH_X,RGRAPH_Y,RGRAPH_X+RGRAPH_W-1,RGRAPH_Y+RGRAPH_H-1);

if (samp[cur].lmem==0 || marked_samp!=cur) return (0);

len=samp[cur].length;
y = RGRAPH_Y+(RGRAPH_H/2);
SetAPen(&screen->RastPort, WHITE);

index = end_mark - num_samples/2;
for (x=RGRAPH_X; x < RGRAPH_X+num_samples*6; x=x+6) {
   if (index >=0 && index <len) {
      endy = y+(*(samp[cur].lmem+index)*(RGRAPH_H/2))/128;
      if (index==end_mark) {
         Move (&screen->RastPort, x+2, RGRAPH_Y);
         Draw (&screen->RastPort, x+2, RGRAPH_Y+RGRAPH_H);
         }
      for (i=0; i < 5; i++) {
         Move (&screen->RastPort, x+i, y);
         Draw (&screen->RastPort, x+i, endy);
         }
      }
   index ++;
   }
}

void erase_zoom_graphs()
{
SetAPen(&screen->RastPort,BLACK);
SetDrMd(&screen->RastPort,JAM1);
RectFill(&screen->RastPort,LGRAPH_X,LGRAPH_Y,LGRAPH_X+LGRAPH_W-1,LGRAPH_Y+LGRAPH_H-1);
RectFill(&screen->RastPort,RGRAPH_X,RGRAPH_Y,RGRAPH_X+RGRAPH_W-1,RGRAPH_Y+RGRAPH_H-1);
}

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
