#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

/** use - b0 **/

/********************************/
/*  save current sample to disk */
/********************************/

save_samp (Window,samp,fname,sm,em)

struct Samp *samp;
struct Window *Window;
char *fname;
int sm,em;
{
int fp;
char line[80];

if (samp->lmem==0) return(-1);
sprintf(line,get_msg_ptr(202),fname);
msg(Window,line);
if ((fp=open(fname,O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   show_msg(Window,185);
   return(-1);
   }

if (sm+em!=0) {
   show_msg(Window,249);
   write(fp,"DUMP",4);
   write(fp,&samp->length,4);
   write(fp,&samp->rate,4);
   write(fp,&sm,4);
   write(fp,&em,4);
   }

if (samp->type==LEFT || samp->type==STEREO)
   if (write(fp,samp->lmem,samp->length)==-1) {
      show_msg(Window,186);
      close(fp);
      return(-1);
      }
if (samp->type==RIGHT || samp->type==STEREO)
   if (write(fp,samp->rmem,samp->length)==-1) {
      show_msg(Window,186);
      close(fp);
      return(-1);
      }

   show_msg(Window,188);
close(fp);
return(0);
}

/*********************************************************************/

#define ASFData  12      /* LONG offset to sample data start (from "ASF1"),this value is fixed, do not alter it */
#define ASFSampleSize 16 /* LONG size of sample */
#define ASFPeriod  20    /* LONG playback period */
#define ASFVolume  24    /* WORD volume */
#define ASFLoop 26       /* WORD # of times to loop sample (or until ^C) */
#define ASFContLoop  28  /* BYTE continuous loop on/off (overrides ASFLoop if on) ;1=on, 0=off */
#define ASFPri  29       /* BYTE task priority to run at (default = 25)  */
#define ASFFilter  30    /* BYTE filter on/off   ;1=on, 0=off */

save_ASF (Window,samp,fname)

struct Samp *samp;
struct Window *Window;
char *fname;
{
int fp;
int size;
int rate;
char line[80];
extern int vid_clock;
extern unsigned char ASFhdr;
extern unsigned char ASFid;
extern unsigned char ASFend;
extern unsigned long ASFhunk_size1;
extern unsigned long ASFhunk_size2;

if (samp->lmem==0) return(-1);
sprintf(line,get_msg_ptr(202),fname);
msg(Window,line);
if ((fp=open(fname,O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   show_msg(Window,185);
   return(-1);
   }

size=samp->length&(~3);    /* long word alligned sample size */
rate = toperiod(samp->rate);

ASFhunk_size1 = size/4;
ASFhunk_size2 = size/4;

*((int *)(&ASFid + ASFSampleSize)) = size;
*((int *)(&ASFid + ASFPeriod)) = rate;
*((short *)(&ASFid + ASFVolume)) = 63;
*((short *)(&ASFid + ASFLoop)) = 1;
*(&ASFid+ASFContLoop)=0;   /* loop off */
*(&ASFid+ASFFilter)=1;     /* filter on */

if (write(fp,&ASFhdr,&ASFend-&ASFhdr)==-1) { /* write header */
   show_msg(Window,186);
   close(fp);
   return(-1);
   }

if (write(fp,samp->lmem,size)==-1) { /* write sound data */
   show_msg(Window,186);                          /* longword alligned */
   close(fp);
   return(-1);
   }

if (write(fp,&ASFend,4)==-1) {                      /* write ASFend */
   show_msg(Window,186);
   close(fp);
   return(-1);
   }

show_msg(Window,253);
close(fp);
return(0);
}

