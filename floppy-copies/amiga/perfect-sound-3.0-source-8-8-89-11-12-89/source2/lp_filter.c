#include <math.h>
#include "psound.h"


void lp_filter(samp,fc)

struct Samp *samp;
int fc;

{
int k,n;
double pi;
double theta_c;
float sum;
float h[21];

pi = 3.14159265359;


theta_c = 2.0*pi*(float)fc/(float)samp->rate;
for (n=-10; n<=10; n++) {
   if (n)
      h[n+10] = (float)sin((double)(theta_c*n))/(n*pi);
   }
h[10] = theta_c/pi;

for (n=0; n < samp->length; n++) {
   sum = 0.0;
   for (k=-10; k<=10; k++) {
      if (n-k >=0)
         sum = sum + h[k+10]*(float)samp->lmem[n-k];
      }
   k = (int)sum;
   if (k < -128) k = -128;
   else if (k > 127) k = 127;
   samp->lmem[n] = (char)k;
   }
}

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

