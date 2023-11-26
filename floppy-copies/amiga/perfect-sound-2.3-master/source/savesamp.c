#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

/********************************/
/*  save current sample to disk */
/********************************/

save_samp (Window,current,samp,fname)

struct Samp *samp;
struct Window *Window;
int current;
char *fname;
{
int fp;
char line[80];
if (samp->lmem[current]==0) return(-1);
sprintf(line,"Writing: %s",fname);
msg(Window,line);
if ((fp=open(fname,O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   msg(Window,"ERROR opening file for write");
   return(-1);
   }

if (samp->type[current]==LEFT || samp->type[current]==STEREO)
   if (write(fp,samp->lmem[current],samp->length[current])==-1) {
      msg(Window,"ERROR writing to disk file!");
      close(fp);
      return(-1);
      }
if (samp->type[current]==RIGHT || samp->type[current]==STEREO)
   if (write(fp,samp->rmem[current],samp->length[current])==-1) {
      msg(Window,"ERROR writing to disk file!");
      close(fp);
      return(-1);
      }

   msg(Window,"Finished Writing Dump... No Errors");
close(fp);
return(0);
}

