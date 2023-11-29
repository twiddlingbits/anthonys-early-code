#include "exec/types.h"
#include "intuition/intuition.h"

#define FAST 1
#define SLOW 2

struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
short invert = FALSE;
short lace   = TRUE;

void main()
{
void grab_frame(), show_frame(), cycle_frame();
static unsigned char buffer[320*200];
static unsigned char buffer2[320*200];
static unsigned char buffer3[320*200];
static unsigned char buffer4[320*200];
int f,x;
unsigned long pix;
unsigned short temp;
char fn[60];

while (TRUE) {
   printf("1. Grab Image\n2. Invert\n3. Average Grab\n4. Debug Grab\n");
   printf("5. Save Last Frame\n6. Load Frame\n7. New Grab\n");
   printf("8. Cycle (bit 7)\n9. Grab Sequence\n10. Toggle Interlaced\n");
   printf("11. clock\n12. Quit\n\n");

   scanf("%d",&x);

   switch (x) {

      case 1:
         grab_frame(buffer);
         show_frame(buffer);
         break;

      case 2:
         printf("\nInverted Color Map.\n");
         if (invert)
            invert = FALSE;
         else
            invert = TRUE;
         break;

      case 3:
         grab_frame(buffer);

         printf("Enter 1 for next frame... ");
         scanf("%d",&x);
         grab_frame(buffer2);

         printf("Enter 1 for next frame... ");
         scanf("%d",&x);
         grab_frame(buffer3);

         printf("Enter 1 for next frame... ");
         scanf("%d",&x);
         grab_frame(buffer4);

         for (pix=0; pix < 64000; pix++)  {
            temp = buffer[pix]+buffer2[pix]+buffer3[pix]+buffer4[pix];
            buffer[pix] = temp>>2;
            }

         show_frame(buffer);
         break;

      case 4:
         grab_frame(buffer);
         for (pix = 0; pix < 64000; pix++)
            if (buffer[pix]==0)
               buffer[pix]=2;
         show_frame(buffer);
         break;

      case 5:
         printf("Enter Filename to Save: ");
         scanf("%s",fn);
         f = creat(fn,0);
         if (f==0) {
            printf("can't open file.\n");
            break;
            }
         for (pix=0; pix < 64000; pix++)  {
            temp = buffer[pix];
            buffer[pix] = temp<<2;
            }
         write(f, buffer, 64000);
         close(f);
         printf("done... (buffer destroyed)\n");
         break;

      case 6:
         printf("Enter Filename to Load: ");
         scanf("%s",fn);
         f = open(fn,0);
         if (f==0) {
            printf("can't open file.\n");
            break;
            }
         read(f, buffer, 64000);
         close(f);
         for (pix=0; pix < 64000; pix++)  {
            temp = buffer[pix];
            buffer[pix] = temp>>2;
            }
         show_frame(buffer);
         break;

      case 7:
         new_grab(buffer);
         show_frame(buffer);
         break;

      case 8:
         cycle_frame();
         break;

      case 9:
         printf("grabbing....\n");
         new_grab(buffer);
         new_grab(buffer2);
         new_grab(buffer3);
         new_grab(buffer4);
         printf("grabbed\n");

         show_frame(buffer);
         show_frame(buffer2);
         show_frame(buffer3);
         show_frame(buffer4);
         break;

      case 10:
         if (lace) {
            printf("interlace turned off.\n\n");
            lace = FALSE;
            }
         else {
            printf("interlace turned on.\n\n");
            lace = TRUE;
            }
         break;

      case 11:
         Disable();
         while (TRUE)
            new_grab(buffer);

      case 12:
         exit(0);

      }
   }
}


void show_frame(buf)

unsigned char *buf;

{
struct Screen *screen;
struct RastPort *rp;
struct ViewPort *vp;
unsigned short x,y,i;
unsigned long pix,size;

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   printf("Can't open Intuition!\n");
   exit(0);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!\n");
   CloseLibrary(IntuitionBase);
   exit(0);
   }

size = 320*200;
x = 0;
y = 0;

screen = (struct Screen *)openscreen(320,200,4);
if (screen == 0) {
   printf("Can't open custom screen\n");
   exit(0);
   }

vp = &screen->ViewPort;
rp = &screen->RastPort;

if (invert) {
   for (i = 0; i < 16; i++)
      SetRGB4(vp, i, 15-i, 15-i, 15-i);
   }

else {
   for (i = 0; i < 16; i++)
      SetRGB4(vp, i, i, i, i);
   }

show(buf,rp->BitMap->Planes[0], rp->BitMap->Planes[1],
       rp->BitMap->Planes[2], rp->BitMap->Planes[3]);
goto xx;

SetDrMd(rp, JAM1);

for (pix = 0; pix < size; pix ++) {
   SetAPen(rp, buf[pix] >> 2);
   WritePixel(rp, x, y);
   x++;
   if (x == 320) {
      x = 0;
      y++;
      }
   }
xx:
printf("Enter 1 to continue... ");
scanf("%d",&pix);

CloseScreen(screen);
CloseLibrary(GfxBase);
CloseLibrary(IntuitionBase);
}

void cycle_frame()

{
struct Screen *screen;
struct RastPort *rp;
struct ViewPort *vp;
unsigned short i;

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",0);
if (IntuitionBase==NULL) {
   printf("Can't open Intuition!\n");
   exit(0);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!\n");
   CloseLibrary(IntuitionBase);
   exit(0);
   }

if (lace)
   screen = (struct Screen *)openscreen(320,400,4);
else
   screen = (struct Screen *)openscreen(320,200,4);

if (screen == 0) {
   printf("Can't open custom screen\n");
   exit(0);
   }

vp = &screen->ViewPort;
rp = &screen->RastPort;

if (invert) {
   for (i = 0; i < 16; i++)
      SetRGB4(vp, i, 15-i, 15-i, 15-i);
   }

else {
   for (i = 0; i < 16; i++)
      SetRGB4(vp, i, i, i, i);
   }

while (1) {
   if (lace)
      lace_cycle(rp->BitMap->Planes[0], rp->BitMap->Planes[1],
             rp->BitMap->Planes[2], rp->BitMap->Planes[3]);
   else
      cycle(rp->BitMap->Planes[0], rp->BitMap->Planes[1],
             rp->BitMap->Planes[2], rp->BitMap->Planes[3]);
   }

CloseScreen(screen);
CloseLibrary(GfxBase);
CloseLibrary(IntuitionBase);
}
