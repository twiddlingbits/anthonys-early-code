#include "exec/types.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "psound.h"

/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,fname,name)

struct Window *Window;
char *fname,*name;

{
extern int cur;
extern struct Samp samp[MAX_SAMPLES];
int in,i,len;
ULONG k,x;
UWORD e1,e2,z;
long pos;
static char line[80];
UBYTE comp=0;
int memtype=0;

if ((i=getslot(Window))==-1) return(-1);
if ((in=(int)open(fname,O_RDONLY))==-1) {
   show_msg(Window,173);
   return(-1);
   }

read(in,&k,4);
if (k!=FORM) {         /* not in standard form */
   samp[i].rate=10000;           /* default length */
   samp[i].type=LEFT;          /* use left (ch 0) as default */
   pos=lseek(in,0L,2);
   samp[i].length=(int)even(pos);
   show_msg(Window,174);
   lseek(in,0L,0);    /* rewind */
   }

else {
   read(in,&x,4);    /* skip length */
   read(in,&x,4);    /* get form type */
   if (x!=SVX) {
      show_msg(Window,175);
      close(in);
      return(-1);
      }

   samp[i].type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   read(in,&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      skip(in);
      read(in,&x,4);
      if (x==-1 || x==0) {
         show_msg(Window,176);
         close(in);
         return(-1);
         }
      }

   if (x==CHAN) {
      read(in,&x,4);
      if (x!=4) {
         show_msg(Window,177);
         close(in);
         return(-1);
         }
      read(in,&samp[i].type,4);
      if (samp[i].type!=STEREO && samp[i].type!=RIGHT
          && samp[i].type!=LEFT) samp[i].type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      read(in,&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         show_msg(Window,178);
         close (in);
         return(-1);
         }
      read(in,&samp[i].oneshot,4); /* should be length of sample */
      read(in,&samp[i].repeat,4);
      read(in,&samp[i].cycles,4);
      read(in,&z,2);   /* get samples per second */
      read(in,&samp[i].ctoctave,1);
      if (samp[i].repeat==0 && samp[i].ctoctave==1)
      sprintf(line,get_msg_ptr(199),z);
      else {
         memtype=MEMF_CHIP;   /* load instruments into chip memory */
         sprintf(line,get_msg_ptr(200), samp[i].ctoctave);
         }
      msg(Window,line);
      samp[i].rate=z;
      read(in,&comp,1);
      lseek(in,4L,1);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   read(in,&samp[i].length,4);
   if (samp[i].type==STEREO)
      samp[i].length=samp[i].length/2;
   }

/* read in data--either BODY or dump */

len=samp[i].length;
if (comp==1) len=(len-2)*2;
samp[i].lmem=AllocMem(fix_len(len),memtype);
if (samp[i].lmem==0) {
   show_msg(Window,179);
   close(in);
   return(-1);
   }
samp[i].rmem=samp[i].lmem;
if (samp[i].type==STEREO) {
   samp[i].rmem=AllocMem(fix_len(len),0);
   if (samp[i].rmem==0) {
      show_msg(Window,179);
      FreeMem(samp[i].lmem,fix_len(len));
      close(in);
      return(-1);
      }
   }

if (samp[i].type==STEREO || samp[i].type==LEFT) {
   if (comp==0)  
      e1=read(in,samp[i].lmem,samp[i].length);     /* read in sample */
   if (comp==1)
      e1=cread(in,samp[i].lmem,samp[i].length);     /* read in sample */
   }

if (samp[i].type==STEREO || samp[i].type==RIGHT) {
   if (comp==0)
     e2=read(in,samp[i].rmem,samp[i].length);     /* read in sample */
   if (comp==1)
     e2=cread(in,samp[i].rmem,samp[i].length);     /* read in sample */
   }

close(in);

if (e1==-1 || e2==-1) {
   show_msg(Window,181);
   return(-1);
   }


/* get sample name */

samp[i].length=fix_len(len);
samp[i].name=AllocMem(strlen(name)+1,0);
if (samp[i].name==0) {
   nomem(Window);
   return(-1);
   }
strcpy(samp[i].name,name);        /* put filename in sample structure */

PutName(Window,samp[cur].name,cur,REG_VID);
cur=i;
PutName(Window,samp[i].name,i,REV_VID);
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

