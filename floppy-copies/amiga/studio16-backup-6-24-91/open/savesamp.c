#include "exec/types.h"
#include <libraries/dos.h>
#include "/include/studio16.h"
#include <stdio.h>

/********************************/
/*  save current sample to disk */
/********************************/

save_samp (Window,samp,fname,sm,em)

struct Disk_Samp *samp;
struct Window *Window;
char *fname;
int sm,em;
{
short GetOneSample();
int rfp,wfp;
char line[80];

/*sprintf(line,get_msg_ptr(202),fname);
msg(Window,line);*/
if ((wfp=Open(fname,MODE_NEWFILE))==0) {
   show_msg(Window,185);
   return(-1);
   }

if (sm+em!=0) {
   Write(wfp,"DP16",4);
   Write(wfp,&samp->length,4);
   Write(wfp,&samp->rate,4);
   Write(wfp,&sm,4);
   Write(wfp,&em,4);
   }

rfp=ReadLockSamp(samp);
show_error(QuickCopy(wfp,rfp,samp->length));
Close(wfp);
ReadUnLockSamp(samp,rfp);
return(0);
}

