#include "exec/types.h"
#include "fcntl.h"
#include "/include/psound.h"

/********************************/
/*  save current sample to disk */
/********************************/

save_samp (Window,samp,fname,sm,em)

struct Disk_Samp *samp;
struct Window *Window;
char *fname;
int sm,em;
{
int fp,i;
char line[80];
short data;

if (!(samp->flags&SAMPLE_OPEN)) return(-1);
sprintf(line,get_msg_ptr(202),fname);
msg(Window,line);
if ((fp=open(fname,O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   show_msg(Window,185);
   return(-1);
   }

if (sm+em!=0) {
   show_msg(Window,249);
   write(fp,"DP16",4);
   write(fp,(char *)&samp->length,4);
   write(fp,(char *)&samp->rate,4);
   write(fp,(char *)&sm,4);
   write(fp,(char *)&em,4);
   }

init_sample1(samp, 0);
for (i=0; i < samp->length; i++) {
   data = (short)get_samplel1();
   if (write(fp,(char *)&data,2)==-1) {
      show_msg(Window,186);
      close(fp);
      return(-1);
      }
   }

if (samp->type==STEREO) {
   init_sample1(samp, 0);
   for (i=0; i < samp->length; i++) {
      data = (short)get_sampler1();
      if (write(fp,(char *)&data,2)==-1) {
         show_msg(Window,186);
         close(fp);
         return(-1);
         }
      }
   }

show_msg(Window,188);
close(fp);
return(0);
}

