#include "exec/types.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "/include/psound.h"

extern int start_mark;
extern int end_mark;
extern int marked_samp;

/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,fname,name)

struct Window *Window;
char *fname,*name;

{
extern int cur;
extern struct Disk_Samp samp[MAX_SAMPLES];
char *AllocMem();
int in,i,len,j;
ULONG k,x;
UWORD e1,e2,z;
long pos;
static char line[80];
UBYTE comp=0;
int memtype=0;
UBYTE *lmem, *rmem;
int use_16;

use_16 = FALSE;

if ((i=getslot(Window))==-1) return(-1);
if ((in=(int)open(fname,O_RDONLY))==-1) {
   show_msg(Window,173);
   return(-1);
   }

read(in,(char *)&k,4);
if (k!=FORM) {         /* not in standard form */
   if (k==DUMP || k==DP16) {       /* dump with extra info */
      read(in,(char *)&samp[i].length,4);
      read(in,(char *)&samp[i].rate,4);
      read(in,(char *)&start_mark,4);
      read(in,(char *)&end_mark,4);
      samp[i].type=LEFT;
      marked_samp=i;
      show_msg(Window,250);   /** File format is dump v3.0 **/
      if (k==DP16)
         use_16 = TRUE;
      }
   else {                           /* straight dump */
      samp[i].rate=10000;           /* default length */
      samp[i].type=LEFT;          /* use left (ch 0) as default */
      pos=lseek(in,0L,2);
      samp[i].length=(int)even(pos);
      show_msg(Window,174);
      lseek(in,0L,0);    /* rewind */
      }
   }

else {
   read(in,(char *)&x,4);    /* skip length */
   read(in,(char *)&x,4);    /* get form type */
   if (x!=SVX) {
      show_msg(Window,175);
      close(in);
      return(-1);
      }

   samp[i].type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   read(in,(char *)&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      skip(in);
      read(in,(char *)&x,4);
      if (x==-1 || x==0) {
         show_msg(Window,176);
         close(in);
         return(-1);
         }
      }

   if (x==CHAN) {
      read(in,(char *)&x,4);
      if (x!=4) {
         show_msg(Window,177);
         close(in);
         return(-1);
         }
      read(in,(char *)&samp[i].type,4);
      if (samp[i].type!=STEREO && samp[i].type!=RIGHT
          && samp[i].type!=LEFT) samp[i].type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      read(in,(char *)&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         show_msg(Window,178);
         close (in);
         return(-1);
         }
      read(in,(char *)&samp[i].oneshot,4); /* should be length of sample */
      read(in,(char *)&samp[i].repeat,4);
      read(in,(char *)&samp[i].cycles,4);
      read(in,(char *)&z,2);   /* get samples per second */
      read(in,(char *)&samp[i].ctoctave,1);
      if (samp[i].repeat==0 && samp[i].ctoctave==1)
      sprintf(line,get_msg_ptr(199),z);
      else {
         memtype=MEMF_CHIP;   /* load instruments into chip memory */
         sprintf(line,get_msg_ptr(200), samp[i].ctoctave);
         }
      msg(Window,line);
      samp[i].rate=z;
      read(in,(char *)&comp,1);
      lseek(in,4L,1);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   read(in,(char *)&samp[i].length,4);
   if (samp[i].type==STEREO)
      samp[i].length=samp[i].length/2;
   }

/* read in data--either BODY or dump */

len=samp[i].length;
if (comp==1) len=(len-2)*2;
if (use_16) len = len*2;
lmem=AllocMem(fix_len(len),memtype);
if (lmem==0) {
   show_msg(Window,179);
   close(in);
   return(-1);
   }
rmem=lmem;
if (samp[i].type==STEREO) {
   rmem=AllocMem(fix_len(len),0);
   if (rmem==0) {
      show_msg(Window,179);
      FreeMem(lmem,fix_len(len));
      close(in);
      return(-1);
      }
   }

if (samp[i].type==STEREO || samp[i].type==LEFT) {
   if (comp==0)  
      e1=read(in,lmem,len);     /* read in sample */
   if (comp==1)
      e1=cread(in,lmem,samp[i].length);     /* read in sample */
   }

if (samp[i].type==STEREO || samp[i].type==RIGHT) {
   if (comp==0)
     e2=read(in,rmem,len);     /* read in sample */
   if (comp==1)
     e2=cread(in,rmem,samp[i].length);     /* read in sample */
   }

close(in);

if (e1==-1 || e2==-1) {
   show_msg(Window,181);
   return(-1);
   }


/* get sample name */

if (use_16)
   samp[i].length=fix_len(len/2);
else
   samp[i].length=fix_len(len);

samp[i].name=AllocMem(strlen(name)+1,0);
if (samp[i].name==0) {
   nomem(Window);
   return(-1);
   }
strcpy(samp[i].name,name);        /* put filename in sample structure */

create_sample(&samp[i]);

if (use_16)
   for (j=0; j < samp[i].length; j++) {
      put_samplel1(((short *)lmem)[j]);
      }
else
   for (j=0; j < samp[i].length; j++) {
      put_samplel1(lmem[j]<<8);
      }

FreeMem(lmem,len);

if (samp[i].type==STEREO) {
   for (j=0; j < samp[i].length; j++) {
      put_sampler1(rmem[j]<<8);
      }
   FreeMem(rmem,len);
   }

cur=i;
if (comp==1) show_msg(Window,182);
return(0);
}

/** force a length to be even by increasing the length one if needed **/

fix_len(i)
int i;
{
int even;

even=0xFFFFFFFE&i;
if (i==even)
   return(i);
else
   return(i+1);
}

