#include "exec/types.h"
#include "stdio.h"
#include "psound.h"

/** rate to Samples per Second

   Takes a rate (as used in digitize.c) and returns samples per second */


#define MAXMONO 10       /* maximum value for rate */
#define MAXSTEREO 7

int mono[]={23283,18191,15046,12787,11127,9859,8813,8012,7322,6750,6259};
int stereo[]={15000,12723,11084,9821,8816,8015,7290,6750};

ratetoss(rate,chan)
int rate;
int chan;
{
if (chan == STEREO) return(stereo[min((rate),MAXSTEREO)]);
return(mono[min(rate,MAXMONO)]);
}


sstorate(ss,chan)
int ss;
int chan;
{
int rate,i;

if (chan==STEREO) {
   rate=0;
   for (i=0;i<MAXSTEREO;i++)
      if (ss < stereo[i]) rate=i+1;
   }
else {
   rate=0;
   for (i=0;i<MAXMONO;i++)
      if (ss < mono[i]) rate=i+1;
   }
return(rate);
}

