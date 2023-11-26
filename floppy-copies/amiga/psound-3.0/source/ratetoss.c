#include "exec/types.h"
#include "stdio.h"
#include "psound.h"

/** rate to Samples per Second

   Takes a rate (as used in digitize.c) and returns samples per second */


ratetoss(rate,chan)
int rate;
int chan;
{                          /** 11+130 **/
if (chan == STEREO)
   return(7159090/((rate)*10+104));

return(7159093/((rate)*10+104));
}


sstorate(ss,chan)
int ss;
int chan;
{
if (chan==STEREO)
   return( ((7159090/ss-104)/10));

return(((7159093/ss-104)/10));
}

