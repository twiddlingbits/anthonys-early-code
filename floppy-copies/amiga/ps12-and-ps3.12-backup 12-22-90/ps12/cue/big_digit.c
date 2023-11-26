#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "/include/psound.h"
#include "stdio.h"

#if 0
struct IntuitionBase near *IntuitionBase;
struct GfxBase near *GfxBase;

main()
{
void big_digit();
struct RastPort *rp;
int i;

IntuitionBase=(struct IntuitionBase *)OpenLibrary("intuition.library",33);
if (IntuitionBase==0) {
   printf("Use Workbench 1.2 or greater.");
   exit(10);
   }

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library!");
   exit(10);
   }

rp = IntuitionBase->ActiveWindow->RPort;

for (i=0; i < 10; i++) {
   big_digit(rp, i, 20,20,50,50);
   Delay(30);
   }

CloseLibrary(IntuitionBase);
CloseLibrary(GfxBase);
}
#endif

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)
#define BIT8 (1<<8)

/* Bit assignment
      1
      _
   0 | |  2       + 7
      -   3       + 8
   5 | |  4
      -
      6
                  */

void big_digit(rp, val, xoff, yoff,height,width)
struct RastPort *rp;
int val;
int xoff, yoff;
int height,width;

/*
#define V0 00
#define V1 04
#define V2 48
#define V3 52
#define V4 44
#define V5 96
*/

#define V0 00
#define V1 8
#define V2 46
#define V3 54
#define V4 38
#define V5 92

{
int bit;
unsigned short mask;
short x[]={V0,V1,V2,V1,V2,V0,V1,23,23};
short y[]={V1,V0,V1,V2,V3,V3,V5,26,58};
short h[]={V4,V1,V4,V1,V4,V4,V1,16,16};
short w[]={V1,V4,V1,V4,V1,V1,V4, 8, 8};

unsigned short mask_vals[] = {
   BIT0|BIT1|BIT2|BIT4|BIT6|BIT5,
   BIT2|BIT4,
   BIT1|BIT2|BIT3|BIT5|BIT6,
   BIT1|BIT2|BIT3|BIT4|BIT6,
   BIT0|BIT3|BIT2|BIT4,
   BIT1|BIT0|BIT3|BIT4|BIT6,
   BIT0|BIT5|BIT6|BIT4|BIT3,
   BIT1|BIT2|BIT4,
   BIT0|BIT1|BIT2|BIT3|BIT4|BIT5|BIT6,
   BIT0|BIT1|BIT2|BIT3|BIT4,
   BIT7|BIT8};


mask = mask_vals[val];

for (bit=0; bit < 9; bit++) {
   if (mask & (1<<bit)) {
      SetAPen(rp, WHITE);
      RectFill (rp, xoff+(x[bit]*width)/54, yoff+(y[bit]*height)/100,
                xoff+(x[bit]*width)/54+max((w[bit]*width)/54-1,0),
                yoff+(y[bit]*height)/100+max((h[bit]*height)/100-1,0));
      }
   else {
      SetAPen(rp, BROWN);
      RectFill (rp, xoff+(x[bit]*width)/54, yoff+(y[bit]*height)/100,
                xoff+(x[bit]*width)/54+max((w[bit]*width)/54-1,0),
                yoff+(y[bit]*height)/100+max((h[bit]*height)/100-1,0));
      }
   }
}

