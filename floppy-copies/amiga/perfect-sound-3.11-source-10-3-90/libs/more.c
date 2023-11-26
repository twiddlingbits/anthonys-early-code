#include "exec/types.h"

#define  PS_2   0
#define  PS_3   1
#define  RIGHT  4
#define  LEFT   2
#define  STEREO 6

static void (*user_plot)()=0;
static int vid_clock = 3579545;     /** NTSC **/

int ps_monitor(flags)

unsigned long flags;

{
if (flags&RIGHT)
   new_monright();
else if (flags&LEFT)
   new_monleft();
return(0);
}

int ps_record(lp, rp, maxsize, speed, flags)

char *rp,*lp;
int maxsize, speed;
unsigned long flags;

{
int ticks;
int len;

ticks = ss_to_ticks(speed);
len = 0;

if ((flags&STEREO)==STEREO) {
   ticks = ticks/2;
   len = new_stereo(lp, rp, maxsize, ticks);
   }

else if (flags&RIGHT)
   len = new_right(lp, maxsize, ticks);

else if (flags&LEFT)
   len = new_left(lp, maxsize, ticks);

return(len);
}


void dec_gain()
{
UBYTE *data,*data2,*direction;

data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */

*data2 &= (~4);
*data &= (~128);
*data |= 128;
}

void inc_gain()
{
UBYTE *data,*data2,*direction;

data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */

*data2 |= 4;
*data &= (~128);
*data |= 128;
}

void set_auto_gain()
{
short k;
short x;

for (k=0; k < 16; k++) {
   if ((x=get_max()) > 220)
      dec_gain();
   else
      inc_gain();
   }
}


get_max()
{
UBYTE *data2,*data,*direction,*direction2;
short k,x;
short max,val;

data=(UBYTE *)0xBFE101;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

*direction=64+128;
*direction2 = 4;

*data |= 64;           /** pulse RD high **/
*data &= (~64);        /** pulse RD low **/

max = 0;
for (k=0; k < 200; k++) {
   for (x=0; x<20; x++) ;        /** pause roughly 100 usecs **/
   val = (*data)&0x3f;           /** get upper 6 bits **/
   val = val << 2;               /** move bits to correct positions **/
   if (val > max)
      max = val;
   *data2 |= 4;
   *data |= 64;                 /** start next sample **/
   *data &= (~64);

   for (x=0; x<20; x++) ;        /** pause roughly 100 usecs **/
   val = (*data)&0x3f;           /** get upper 6 bits **/
   val = val << 2;               /** move bits to correct positions **/
   if (val > max)
      max = val;
   *data2 &= (~4);
   *data |= 64;                 /** start next sample **/
   *data &= (~64);
   }

return (max);
}

int query_digi_type()
{
UBYTE *data,*direction;

data=(UBYTE *)0xBFE101;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */

*direction=0;        /** all read **/
if (*data==0xff && *data==0xff)
   return(PS_3);
else
   return(PS_2);
}

void rec_plot(val)
int val;
{
if (user_plot)
   (*user_plot)(val);

}

void set_plot_point(ptr)
void (*ptr)();
{
user_plot = ptr;
}

void set_vid_clock(clk)
long clk;
{
vid_clock = clk;
}

round_ss(ss)
int ss;
{
return (ticks_to_ss(ss_to_ticks(ss)));
}

ticks_to_ss(ticks)
int ticks;
{
return((vid_clock/10)/ticks);
}

ss_to_ticks(ss)
int ss;
{
return((vid_clock/10)/ss);
}

int get_sample(chan)
int chan;
{
UBYTE *data2,*data,*direction,*direction2;
short x,val;

data=(UBYTE *)0xBFE101;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

*direction=64+128;
*direction2 = 4;

if (chan==RIGHT)
   *data2 |= 4;
else
   *data2 &= (~4);

*data |= 64;           /** pulse RD high **/
*data &= (~64);        /** pulse RD low **/

for (x=0; x<10; x++) ;        /** pause  **/
val = (*data)&0x3f;           /** get upper 6 bits **/
val = val << 2;               /** move bits to correct positions **/
x = (*data2)&3;
val = val | x;
*data |= 64;
return(val);
}

