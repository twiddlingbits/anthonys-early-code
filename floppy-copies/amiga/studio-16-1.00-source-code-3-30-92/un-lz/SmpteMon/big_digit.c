#include "exec/types.h"
#include "intuition/intuition.h"
#include "intuition/intuitionbase.h"
#include "/include/psound.h"
#include "stdio.h"

#define BIT0 (1<<0)
#define BIT1 (1<<1)
#define BIT2 (1<<2)
#define BIT3 (1<<3)
#define BIT4 (1<<4)
#define BIT5 (1<<5)
#define BIT6 (1<<6)
#define BIT7 (1<<7)
#define BIT8 (1<<8)

extern struct StudioBase *StudioBase;

static short x[9];
static short y[9];
static short w[9];
static short h[9];

/* Bit assignment
      1
      _
   0 | |  2       + 7
      -   3       + 8
   5 | |  4
      -
      6
                  */

void big_digit(rp, val,oval, xoff, yoff)
struct RastPort *rp;
int val,oval;
int xoff, yoff;
{
int bit;
unsigned short mask,mask2;

unsigned short mask_vals[] =
	{BIT0|BIT1|BIT2|BIT4|BIT6|BIT5,
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
mask2= mask_vals[oval];
for (bit=0; bit < 9; bit++)
	{
	if (mask ^ mask2)
		{
		if (mask & (1<<bit)) SetAPen(rp,StudioBase->defaults.colors.gadget_window.topleft_3d);
			else SetAPen(rp,StudioBase->defaults.colors.gadget_window.hitbox_unselected);

			RectFill (rp,xoff+x[bit],yoff+y[bit],xoff+w[bit]-1,yoff+h[bit]-1);
		}
	}
}


/**************************************************************/
/**************************************************************/

void new_big_digit_size(height,width)
int height,width;
{
int SWid=max(2,width>>3);
int SHei=max(1,height>>4);
int LWid= width-(SWid<<1);
int LHeU=(height-(SHei*3))>>1;
int LHeL=height-LHeU-(SHei*3);
int a=0,b=SWid,c=SWid+LWid,d=(SWid*2)+LWid,e=0,f=SHei,g=SHei+LHeU,
	H=(SHei*2)+LHeU,i=(SHei*2)+LHeU+LHeL,j=(SHei*3)+LHeU+LHeL,
	k=((d+1)>>1)-((b+1)>>1) ,l=((d+1)>>1)+((b+1)>>1),
	m=((H+1)>>1)-((f+1)>>1), n=((H+1)>>1)+((f+1)>>1),
	o=((j-g+1)>>1)-((f+1)>>1)+g, p=((j-g+1)>>1)+((f+1)>>1)+g;

 x[0]=a;x[1]=b;x[2]=c;x[3]=b;x[4]=c;x[5]=a;x[6]=b;x[7]=k;x[8]=k;
 y[0]=f;y[1]=e;y[2]=f;y[3]=g;y[4]=H;y[5]=H;y[6]=i;y[7]=m;y[8]=o;
 w[0]=b;w[1]=c;w[2]=d;w[3]=c;w[4]=d;w[5]=b;w[6]=c;w[7]=l;w[8]=l;
 h[0]=g;h[1]=f;h[2]=g;h[3]=H;h[4]=i;h[5]=i;h[6]=j;h[7]=n;h[8]=p;
}
/**************************************************************/
/**************************************************************/

