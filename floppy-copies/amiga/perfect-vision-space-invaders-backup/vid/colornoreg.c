#include <intuition/intuition.h>
#include "fcntl.h"

#define ABS(x) (((x)>=0) ? (x) : (-(x)))

#define NBUFS  16

static struct NewScreen ns = {
   0, 0, 320, 400,      /* size */
   6,                   /* depth */
   1, 2,                /* pens */
   HAM|LACE,            /* mode */
   CUSTOMSCREEN, 0, 0, 0, 0
};

static struct NewWindow nw = {
   0, 0, 320, 400,
   1, 2,
   0,
   BORDERLESS|BACKDROP,
   0,0,0,0,0,
   0,0,0,0,
   CUSTOMSCREEN
};

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;

struct Window *w;
struct Screen *s;

int rf, gf, bf;

main()
{

USHORT colormap[16];
static int colorfreq[16][16][16];
int i, j, k, line, stat;
int red, green, blue;
int rd, gd, bd;
char fname[80], rname[80], gname[80], bname[80];
static UBYTE rbuf[NBUFS][512], gbuf[NBUFS][512], bbuf[NBUFS][512];
int bufn;
/* struct IntuiMessage *message; */

rf=-1;
gf=-1;
bf=-1;

IntuitionBase = OpenLibrary("intuition.library", 0);
GfxBase = OpenLibrary("graphics.library", 0);

s = (struct Screen *)OpenScreen(&ns);
if (!s)
   error("can't open screen");

nw.Screen = s;
w = (struct Window *)OpenWindow(&nw);
if (!w)
   error("can't open window");

for (i=0; i<16; i++) {
   colormap[i] = (i<<8) + (i<<4) + i;
   printf("map[%x] = %x\n", i, colormap[i]);
}

s->ViewPort.ColorMap = GetColorMap(16);

LoadRGB4(&s->ViewPort, colormap, 16);

printf("Name:");
scanf("%s\n", fname);

strcpy(rname, fname);
strcat(rname, ".red");
strcpy(gname, fname);
strcat(gname, ".grn");
strcpy(bname, fname);
strcat(bname, ".blu");

rf = open(rname, O_RDONLY);
if (rf == -1)
   error("Can't open red");
gf = open(gname, O_RDONLY);
if (gf == -1)
   error("Can't open green");
bf = open(bname, O_RDONLY);
if (bf == -1)
   error("Can't open blue");

for (i=0; i<16; i++)
   for (j=0; j<16; j++)
      for (k=0; k<16; k++)
         colorfreq[i][j][k] = 0;

bufn = NBUFS;

for (line=0; line<400; line++) {
   printf("line %d\n", line);

   if (++bufn >= NBUFS) {
      stat = read(rf, rbuf, 512*NBUFS);
      if (stat == -1)
         error("error reading red");
      stat = read(gf, gbuf, 512*NBUFS);
      if (stat == -1)
         error("error reading green");
      stat = read(bf, bbuf, 512*NBUFS);
      if (stat == -1)
         error("error reading blue");
      bufn = 0;
   }
   red = 0;
   green = 0;
   blue = 0;

   for (i=0; i<320; i++) {

      colorfreq[rbuf[bufn][i]>>4][gbuf[bufn][i]>>4][bbuf[bufn][i]>>4]++;

      rd = ABS(red-(int)(rbuf[bufn][i]>>4));
      gd = ABS(green-(int)(gbuf[bufn][i]>>4));
      bd = ABS(blue-(int)(bbuf[bufn][i]>>4));

#if 0
      printf("red  =%x, pix=%x, rd=%x\n", red  , rbuf[bufn][i]>>4, rd);
      printf("green=%x, pix=%x, gd=%x\n", green, gbuf[bufn][i]>>4, gd);
      printf("blue =%x, pix=%x, bd=%x\n", blue , bbuf[bufn][i]>>4, bd);
#endif

      if (rd >= gd && rd >= bd) {
         red = rbuf[bufn][i]>>4;
         SetAPen(w->RPort, 0x20 | red);
      }
      else if (gd >= rd && gd >= bd) {
         green = gbuf[bufn][i]>>4;
         SetAPen(w->RPort, 0x30 | green);
      }
      else {
         blue = bbuf[bufn][i]>>4;
         SetAPen(w->RPort, 0x10 | blue);
       }
      WritePixel(w->RPort, i, line);
   }
}

for (i=0; i<16; i++)
   for (j=0; j<16; j++)
      for (k=0; k<16; k++)
         printf("r=%x, g=%x, b=%x, freq=%d\n", i, j, k, colorfreq[i][j][k]);


#if 0
wait (1<<w->UserPort->mp_SigBit);
message = (struct IntuiMessage *)GetMsg(w->UserPort);
ReplyMsg(message);
#endif

printf("entering scanf\n");
scanf("%s\n", fname);
printf("done with scanf\n");

printf("...done\n");
error("Done");
}


error(s)
char *s;
{

printf("%s\n", s);
if (rf != -1)
   close(rf);
if (gf != -1)
   close(gf);
if (bf != -1)
   close(bf);
printf("closing window\n");
if (w)
   CloseWindow(w);
printf("closing screen\n");
if (s)
   CloseScreen(s);
printf("closing gfx lib\n");
if (GfxBase)
   CloseLibrary(GfxBase);
printf("closing intuition lib\n");
if (IntuitionBase)
   CloseLibrary(IntuitionBase);

printf("exiting\n");
exit(0);
}

