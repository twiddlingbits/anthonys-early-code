#include <math.h>
#include "psound.h"

/** use -fim for mixed mode, ieee libraries **/

void lp_filter(ptr, len, rate, fc)

char *ptr;
int rate;
int len;
int fc;

{
int k,n;
double pi;
double theta_c;
float sum;
float h[21];

/* printf("filter %x %d\n",ptr,len); */

pi = 3.14159265359;

theta_c = 2.0*pi*(float)fc/(float)rate;
for (n=-10; n<=10; n++) {
   if (n)
      h[n+10] = (float)sin((double)(theta_c*n))/(n*pi);
   }
h[10] = theta_c/pi;

for (n=20; n < len; n++) {
   sum = 0.0;
   for (k=-10; k<=10; k++) {
      sum = sum + h[k+10]*(float)ptr[n-(k+10)];
      }
   k = (int)sum;
   if (k < -128) k = -128;
   else if (k > 127) k = 127;
   ptr[n-20] = (char)k;
   }
for (n=len-20; n < len; n++)
   ptr[n]=0;
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
