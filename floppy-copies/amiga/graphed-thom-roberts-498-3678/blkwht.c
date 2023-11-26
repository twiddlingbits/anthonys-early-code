#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"
#include "graphed.h"


blkwht(window,gmode)
struct Window *window;
int gmode;
{

int x;
USHORT *dataarea;

static USHORT blkwhtgadgetData[] =
{

0x07FF,0xFFFF,0xFFFF,0xFFFF,
0x8000,0x04FC,0x38A0,0x0F39,0x1FF7,0x8000,
0x0466,0x1860,0x0F39,0x9FE7,0x8000,0x0466,
0x1866,0x0F39,0x93C1,0x8000,0x047C,0x186C,
0x0E29,0x89E7,0x8000,0x0466,0x1878,0x0F01,
0x99E7,0x8000,0x0466,0x186C,0x0F10,0x99E5,
0x8000,0x04FC,0x3CE6,0x0F39,0x19F3,0x8000,
0x07FF,0xFFFF,0xFFFF,0xFFFF,0x8000,0x0000,
0x0000,0x0000,

};
static struct Image blkwhtgadget =
{
0,0,          /* LeftEdge, TopEdge */
69,9,1,      /* Width, Height, Depth */
&blkwhtgadgetData[0],
1,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};

dataarea = AllocMem(RASSIZE(73,9),MEMF_CHIP);
movmem( &blkwhtgadgetData[0], dataarea, RASSIZE(73,9)  );
blkwhtgadget.ImageData = dataarea;
x = 249;
if (gmode == 1 || gmode == 3)
   x = 540;

DrawImage(window->RPort,&blkwhtgadget,x,10);

FreeMem(dataarea,RASSIZE(73,9));

}
