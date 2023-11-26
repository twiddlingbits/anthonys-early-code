#include <intuition/intuition.h>
#include "fcntl.h"

static struct NewScreen ns = {
   0, 0, 640, 400,      /* size */
   4,                   /* depth */
   1, 2,                /* pens */
   HIRES|LACE,          /* mode */
   CUSTOMSCREEN, 0, 0, 0, 0
};

static struct NewWindow nw = {
   0, 0, 640, 400,
   1, 2,
   ACTIVEWINDOW,
   BORDERLESS|BACKDROP,
   0,0,0,0,0,
   0,0,0,0,
   CUSTOMSCREEN
};

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

main()
{

struct Window *w;
struct Screen *s;
USHORT colormap[16];
int i, line, stat;
char fname[80];
int f;
UBYTE buf[512];
struct IntuiMessage *message;

IntuitionBase = OpenLibrary("intuition.library", 0);
GfxBase = OpenLibrary("graphics.library", 0);

s = (struct Screen *)OpenScreen(&ns);
if (!s) {
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   exit(10);
}

nw.Screen = s;
w = (struct Window *)OpenWindow(&nw);
if (!w) {
   CloseScreen(s);
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   exit(10);
}

for (i=0; i<16; i++) {
   colormap[i] = (i<<8) + (i<<4) + i;
   printf("map[%d] = %d\n", i, colormap[i]);
}

for (i=0; i<100000; i++)
   ;

s->ViewPort.ColorMap = GetColorMap(16);

LoadRGB4(&s->ViewPort, colormap, 16);

printf("Name:");
scanf("%s\n", fname);

f = open(fname, O_RDONLY);
if (f == -1) {
   printf("open error\n");
   CloseWindow(w);
   CloseScreen(s);
   CloseLibrary(IntuitionBase);
   CloseLibrary(GfxBase);
   exit(10);
}

for (line=0; line<400; line++) {
   stat = read(f, buf, 512);
   printf("line %d readstat %d\n", line, stat);
   for (i=0; i<512; i++) {
      SetAPen(w->RPort, buf[i]>>4);
      WritePixel(w->RPort, i, line);
   }
}

close(f);

#if 0
wait (1<<w->UserPort->mp_SigBit);
message = (struct IntuiMessage *)GetMsg(w->UserPort);
ReplyMsg(message);
#endif

scanf("%s\n", fname);

CloseWindow(w);
CloseScreen(s);

CloseLibrary(IntuitionBase);
CloseLibrary(GfxBase);
}


