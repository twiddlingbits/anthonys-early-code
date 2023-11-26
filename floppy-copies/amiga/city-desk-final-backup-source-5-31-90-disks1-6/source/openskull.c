#include "exec/types.h"
#include "exec/exec.h"
#include "intuition/intuition.h"


USHORT *sd;
extern SHORT screensize;

/****************************/
/*** Draws a skull        ***/
/****************************/



openskull(window)

struct Window *window;
{

extern USHORT *sd;

static unsigned short skullData[] =
{

0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0001,
0xFFC0,0x0000,0x0000,0x0000,0x001E,0x003C,
0x0000,0x0000,0x0000,0x0060,0x0003,0x8000,
0x0000,0x0000,0x00C0,0x0000,0x4000,0x0000,
0x00F8,0x0080,0x0000,0x4001,0xE000,0x01CC,
0x0100,0x0000,0x2003,0x3000,0x0386,0x0100,
0x0000,0x2002,0x1C00,0x0706,0x0100,0x0000,
0x200C,0x0E00,0x0C01,0x0100,0x0000,0x2030,
0x1E00,0x07F8,0xC100,0x0000,0x20C1,0xF800,
0x000E,0x310F,0x0078,0x2306,0x0000,0x0003,
0x8D1D,0xC9DC,0x2C18,0x0000,0x0000,0xE38F,
0x1C78,0x7060,0x0000,0x0000,0x3878,0x1C07,
0xE180,0x0000,0x0000,0x0F16,0x3609,0x0600,
0x0000,0x0000,0x03C9,0x0013,0x1800,0x0000,
0x0000,0x00E8,0xD562,0x6000,0x0000,0x0000,
0x0034,0x6AC5,0x8000,0x0000,0x0000,0x000E,
0x000E,0x0000,0x0000,0x0000,0x0003,0x0010,
0x0000,0x0000,0x0000,0x0003,0xFFF8,0x0000,
0x0000,0x0000,0x000E,0x1F3E,0x0000,0x0000,
0x0000,0x0038,0xF18F,0x8000,0x0000,0x0000,
0x00F3,0xC063,0xE000,0x0000,0x0000,0x03C7,
0x0018,0xF800,0x0000,0x0000,0x0F1C,0x0006,
0x3E00,0x0000,0x0000,0x3C70,0x0001,0x8F80,
0x0000,0x0000,0xF1C0,0x0000,0x63E0,0x0000,
0x0003,0xC700,0x0000,0x1878,0x0000,0x007E,
0x1C00,0x0000,0x061F,0xC000,0x00E0,0x7000,
0x0000,0x0186,0xE000,0x00F8,0xE000,0x0000,
0x00C8,0xE000,0x003C,0xE000,0x0000,0x00C1,
0x8000,0x0019,0xC000,0x0000,0x0063,0x0000,
0x000F,0x8000,0x0000,0x003E,0x0000,0x0000,
0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0xBF10,0x226D,0x0008,
0x776F,0x726B,0x696E,0x672E,0x2E2E,0x6E6C,
0x792E,0x0A0A,0x2062,0x6520,0x6672,0x6565,
0x6C79,0x2064,0x6973,0x7472,0x6962,0x7574,
0x6564,0x2066,0x6F72,0x206E,0x6F6E,0x2D70,
0x726F,0x6669,0x740A,0x6710,0x206D,0x0008,
0x2F08,0x4EBA,0xFE42,0x205F,0xD168,0x0008,
};

static struct Image skull =
{
10,156,          /* LeftEdge, TopEdge */
75,39,1,      /* Width, Height, Depth */
NULL,
1,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};

skull.TopEdge = screensize - 44;
sd=(USHORT *)AllocMem(sizeof(skullData),MEMF_CLEAR|MEMF_CHIP);
if (sd==NULL) {
   printf("Not enuf mem to open skull!\n");
   return(NULL);
   }
movmem(skullData,sd,sizeof(skullData));
skull.ImageData=sd;
DrawImage(window->RPort,&skull,0,0);
FreeMem(sd,sizeof(skullData));
}


/*****************************************/
/*** Check if an x,y,h,y overlaps skull **/
/*****************************************/



overskull(x,y,h,w)

int x,y,h,w;

{
return(overlap(x,y,h,w,10,screensize - 44,39,75));
}

