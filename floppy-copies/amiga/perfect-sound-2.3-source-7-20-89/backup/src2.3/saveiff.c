#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

save_IFF (Window,current,samp,fname,comp)

struct Samp *samp;
struct Window *Window;
int current,comp;
char *fname;
{
int fp;
ULONG i,len,evenlen;
UWORD j;
UBYTE k;
char line[80];
if (samp->lmem[current]==0) return(-1);
sprintf(line,"Writing: %s",fname);
msg(Window,line);
if ((fp=open(fname,O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   msg(Window,"ERROR opening file for write");
   return(-1);
   }

evenlen=samp->length[current]&(~1);   /*make length an even number */
if (evenlen!=samp->length[current])
   msg(Window,"Warning last byte lost--length must be even.");

len=evenlen;

if (samp->type[current]==STEREO) len=len*2;
j=(UWORD)samp->rate[current];

write(fp,"FORM",4);
if (comp==FALSE) i=4+28+76+8+len+12;
else {
   i=4+28+76+8+(len>>1)+2+12;
   if (samp->type[current]==STEREO) i=i+2;
   }
write(fp,&i,4);
write(fp,"8SVXVHDR",8);
i=20; write(fp,&i,4);

if (samp->oneshot[current]==0 || samp->oneshot[current]==1)
   write(fp,&len,4);    /* # of samples field */
else 
   write(fp,&samp->oneshot[current],4);
write(fp,&samp->repeat[current],4);
write(fp,&samp->cycles[current],4);
write(fp,&j,2);

k=1;
if (samp->oneshot[current]==0)
  write(fp,&k,1);
else
   write(fp,&samp->ctoctave[current],1);

k=0;if (comp==TRUE) k=1;
write(fp,&k,1);   /* compression */
i=0x10000;   /*max volume */
write(fp,&i,4);

write(fp,"ANNO",4);
i=68; write(fp,&i,4);
write(fp,"Recorded with PERFECT SOUND from SunRize Industries.  (409) 846-1311",
          68);

/* The following new chuck CHAN is 4 bytes long and is used to store
  channel information (RIGHT, LEFT, STEREO --see sed1.h) */

write(fp,"CHAN",4);  /* non standard channel chunck */
i=4;write(fp,&i,4);  /* length */
write(fp,&samp->type[current],4);

write(fp,"BODY",4);
len=evenlen;
if (comp==TRUE) len=(len>>1)+2;
if (samp->type[current]==STEREO) len=len*2;
write(fp,&len,4);

if (samp->type[current]==LEFT || samp->type[current]==STEREO) {
   if (comp==FALSE) i=write(fp,samp->lmem[current],evenlen);
   if (comp==TRUE)  i=cwrite(fp,samp->lmem[current],evenlen);
   if (i==-1) {
      msg(Window,"ERROR writing to file!");
      close(fp);
      return(-1);
      }
   }

if (samp->type[current]==RIGHT || samp->type[current]==STEREO) {
   if (comp==FALSE) i=write(fp,samp->rmem[current],evenlen);
   if (comp==TRUE)  i=cwrite(fp,samp->rmem[current],evenlen);
   if (i==-1) {
      msg(Window,"ERROR writing to file!");
      close(fp);
      return(-1);
      }
   }

if (comp==FALSE) msg(Window,"Finished Writing... No Errors");
else {
     sprintf(line,"File Writen with %d%% of samples altered during compression.",i);
     msg(Window,line);
     }
close(fp);
return(0);
}                              

