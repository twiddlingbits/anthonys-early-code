#include <math.h>
#include "/include/psound.h"


void lp_filter(ptr, len, rate, fc)

struct Disk_Samp *ptr;
int rate;
int len;
int fc;

{
short get_left_sample2();
int k,n;
double pi;
double theta_c;
double sum;
double h[21];

pi = 3.14159265359;

theta_c = 2.0*pi*(double)fc/(double)rate;
for (n=-10; n<=10; n++) {
   if (n)
      h[n+10] = (double)sin((double)(theta_c*n))/(n*pi);
   }
h[10] = theta_c/pi;

init_sample1(ptr, 0);

for (n=20; n < len; n++) {
   sum = 0.0;
   for (k=-10; k<=10; k++) {
      sum = sum + h[k+10]*(double)get_left_sample2(ptr,n-(k+10));
      }
   k = (int)sum;
   if (k < -32768) k = -32768;
   else if (k > 32767) k = 32767;
   /* ptr[n-20] = (char)k; */
   put_samplel1(k);
   }
for (n=len-20; n < len; n++)
   put_samplel1(0);

if (ptr->type==STEREO) {
   init_sample1(ptr, 0);

   for (n=20; n < len; n++) {
      sum = 0.0;
      for (k=-10; k<=10; k++) {
         sum = sum + h[k+10]*(double)get_right_sample2(ptr,n-(k+10));
         }
      k = (int)sum;
      if (k < -32768) k = -32768;
      else if (k > 32767) k = 32767;
     /* ptr[n-20] = (char)k; */
      put_sampler1(k);
      }
   for (n=len-20; n < len; n++)
      put_sampler1(0);
   }
}

#if 0
void gen_sin(samp,kh)

struct Samp *samp;
int kh;  /** Khert rate of sin wave to generate **/

{
int i,j;
double pi;

pi = 3.14159265359;

for (i=0; i < samp->length; i++)
   samp->lmem[i]=0;
for (i=0; i < samp->length; i=i+2500) {
   samp->lmem[i]=50;
/*
   j = (int)(127.0*sin(2.0*pi*(double)kh*(double)i/(double)samp->rate));
   samp->lmem[i] = (char)j;*/
   }
}
#endif

