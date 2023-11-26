
#include "stdio.h"
#include "libraries/dos.h"
#include "exec/types.h"
#include "intuition/intuition.h"
#include "graphics/gfxbase.h"
#include "graphed.h"
#include "desk.h"


savebrush(win,screen,brush)

struct Screen *screen;
struct Window *win;
struct BitMap *brush;

{

char temp[8],temp2[100];
int error,x,y;
int f,i;
static SHORT cm[] = {0xffff,0x0000};
UBYTE *buf;
extern char gooblepop1[50],gooblepop2[50];
static struct graphic g;
static struct object o;

i = 0;
while(i < brush->Depth)
   g.planes[i] = brush->Planes[i++];
g.w = brush->BytesPerRow*8;
g.h = brush->Rows;
g.usex = 0;
g.usey = 0;
g.usew = g.w;
g.useh = g.h;
g.numplanes = brush->Depth;
g.aspect=(ASPLOW*(ULONG)g.w)/((ULONG)g.h);

o.type = GRAPHIC;
o.data = (APTR) &g;

saveobject(win,&o);


}



