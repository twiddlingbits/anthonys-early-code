#include "exec/types.h"
#include "intuition/intuition.h"
#include "psound.h"


/***********************/
/** plot sample       **/
/***********************/

graph(Window,samp,cur)

struct Window *Window;
struct Samp *samp;
int cur;

{
ULONG x,s,stp,addx,len;
int y,val;
char *offset;

if (samp->lmem[cur]==0) return (0);
if (samp->sm[cur]>=samp->em[cur]) return(0);

/* 1st clear screen memory in this area */
SetAPen(Window->RPort,BLACK);
SetBPen(Window->RPort,BLACK);
RectFill(Window->RPort,2,GBOTY-255/SCALE,637,GBOTY);
drawline(Window,2,GBOTY-255/SCALE-1,637,GBOTY-255/SCALE-1,WHITE);
drawline(Window,2,GBOTY-255/SCALE-2,637,GBOTY-255/SCALE-2,WHITE);

len=(samp->em[cur]-samp->sm[cur])<<9;
stp=len/530;
addx=(1<<9);
if (stp < (1<<9)) {
   addx=(530<<9)/(samp->em[cur]-samp->sm[cur]);
   stp=1<<9;
   }

x=50<<9;    /* line up with prop gadgets */


y=GBOTY-((int)(*(samp->lmem[cur]+samp->sm[cur]))+128)/SCALE;
Move(Window->RPort,x>>9,y);
SetAPen(Window->RPort,WHITE);

offset=samp->lmem[cur]+samp->sm[cur];
x=x+addx;
for (s=stp; s < len ;s=s+stp) {
   val=128+*(offset+(s>>9));
   val=GBOTY-val/SCALE;
   x=x+addx;
   Draw(Window->RPort,x>>9,val);
   }
}

