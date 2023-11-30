#include "exec/types.h"
#include "intuition/intuition.h"

drawbunker(w,x,y)
struct Window *w;
{

static USHORT bunkerData[] =
{
0x007F,0xFFFF,0xFF80,0x0000,0x01FF,0xFFFF,
0xFFE0,0x0000,0x07FF,0xFFFF,0xFFF8,0x0000,
0x3FFF,0xFFFF,0xFFFF,0x0000,0xFFFF,0xFFFF,
0xFFFF,0xC000,0xFFFF,0xFFFF,0xFFFF,0xC000,
0xFFFF,0xFFFF,0xFFFF,0xC000,0xFFFF,0xFFFF,
0xFFFF,0xC000,0xFFFF,0xFFFF,0xFFFF,0xC000,
0xFFFF,0xFFFF,0xFFFF,0xC000,0xFFFF,0xFFFF,
0xFFFF,0xC000,0xFFFF,0xFFFF,0xFFFF,0xC000,
0xFFFF,0xFFFF,0xFFFF,0xC000,0xFFFF,0xFFFF,
0xFFFF,0xC000,0xFFFF,0xFFFF,0xFFFF,0xC000,
0xFFFF,0xFFFF,0xFFFF,0xC000,0xFFFF,0xFFFF,
0xFFFF,0xC000,0xFFFF,0xE001,0xFFFF,0xC000,
0xFFFF,0x8000,0x7FFF,0xC000,0xFFFE,0x0000,
0x1FFF,0xC000,0xFFF8,0x0000,0x07FF,0xC000,
0xFFE0,0x0000,0x01FF,0xC000,0xFFC0,0x0000,
0x00FF,0xC000,0x202D,0xFFD4,0x776F,0x726B,
0x696E,0x672E,0x2E2E,0x6E6C,0x792E,0x0A0A,
0x2062,0x6520,0x6672,0x6565,0x6C79,0x2064,
0x6973,0x7472,0x6962,0x7574,0x6564,0x2066,
0x6F72,0x206E,0x6F6E,0x2D70,0x726F,0x6669,
0x740A,0x676E,0x206D,0xFFFC,0x0CA8,0x0000,
0x0003,0x0004,0x6C60,0x206D,0xFFFC,0x2028,
0x0004,0xE580,0x41EC,0x8D42,0x2F30,0x0800,
};

static struct Image bunker =
{
0,0,          /* LeftEdge, TopEdge */
50,23,1,      /* Width, Height, Depth */
&bunkerData[0],
1,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};

DrawImage(w->RPort,&bunker,x,y);
}
