#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

/********************************/
/*  save current sample to disk */
/********************************/

save_samp (Window,samp,fname)

struct Samp *samp;
struct Window *Window;
char *fname;
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

