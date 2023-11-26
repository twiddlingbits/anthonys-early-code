#include "exec/types.h"
#include "fcntl.h"
#include "psound.h"

/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,current,samp,fname,name)

int *current;
struct Samp  *samp;
struct Window *Window;
char *fname,*name;

{
int in,i,len;
ULONG k,x;
UWORD e1,e2,z;
long pos;
static char line[80];
UBYTE comp=0;

if ((i=getslot(Window,samp))==-1) return(-1);
if ((in=(int)open(fname,O_RDONLY))==-1) {
   msg(Window,"Can't open that file.");
   return(-1);
   }

read(in,&k,4);
if (k!=FORM) {         /* not in standard form */
   samp->rate[i]=10000;           /* default length */
   samp->type[i]=LEFT;          /* use left (ch 0) as default */
   pos=lseek(in,0L,2);
   samp->length[i]=(int)pos;
   msg(Window,"No playback rate specified -- 10,000 samples/sec used");
   lseek(in,0L,0);    /* rewind */
   }

else {
   read(in,&x,4);    /* skip length */
   read(in,&x,4);    /* get form type */
   if (x!=SVX) {
      msg(Window,"File is IFF, but not form 8SVX !!");
      close(in);
      return(-1);
      }

   samp->type[i]=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   read(in,&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      skip(in);
      read(in,&x,4);
      if (x==-1 || x==0) {
         msg(Window,"Bad IFF file--Can't load.");
         close(in);
         return(-1);
         }
      }

   if (x==CHAN) {
      read(in,&x,4);
      if (x!=4) {
         msg(Window,"Bad CHAN length.");
         close(in);
         return(-1);
         }
      read(in,&samp->type[i],4);
      if (samp->type[i]!=STEREO && samp->type[i]!=RIGHT
          && samp->type[i]!=LEFT) samp->type[i]=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      read(in,&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         msg(Window,"Format error--invalid length for VHDR");
         close (in);
         return(-1);
         }
      read(in,&samp->oneshot[i],4); /* should be length of sample */
      read(in,&samp->repeat[i],4);
      read(in,&samp->cycles[i],4);
      read(in,&z,2);   /* get samples per second */
      read(in,&samp->ctoctave[i],1);
      if (samp->repeat[i]==0 && samp->ctoctave[i]==1)
      sprintf(line,"File type is IFF -- Playback rate (samples/second) is %d",z);
      else
         sprintf(line,"File is an IFF instrument file with %d octaves.",
                 samp->ctoctave[i]);
      msg(Window,line);
      samp->rate[i]=z;
      read(in,&comp,1);
      lseek(in,4L,1);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   read(in,&samp->length[i],4);
   if (samp->type[i]==STEREO) samp->length[i]=samp->length[i]>>1;
   }

/* read in data--either BODY or dump */

len=samp->length[i];
if (comp==1) len=(len-2)*2;
samp->lmem[i]=AllocMem(len,0);
if (samp->lmem[i]==0) {
   msg(Window,"File to large for available memory.");
   close(in);
   return(-1);
    }
samp->rmem[i]=samp->lmem[i];
if (samp->type[i]==STEREO) {
   samp->rmem[i]=AllocMem(len,0);
   if (samp->rmem[i]==0) {
      msg(Window,"File to large for available memory.");
      FreeMem(samp->lmem[i],len);
      close(in);
      return(-1);
      }
   }

if (samp->type[i]==STEREO || samp->type[i]==LEFT) {
   if (comp==0)  
      e1=read(in,samp->lmem[i],samp->length[i]);     /* read in sample */
   if (comp==1)
      e1=cread(in,samp->lmem[i],samp->length[i]);     /* read in sample */
   }

if (samp->type[i]==STEREO || samp->type[i]==RIGHT) {
   if (comp==0)
     e2=read(in,samp->rmem[i],samp->length[i]);     /* read in sample */
   if (comp==1)
     e2=cread(in,samp->rmem[i],samp->length[i]);     /* read in sample */
   }

close(in);

if (e1==-1 || e2==-1) {
   msg(Window,"ERROR loading file!");
   return(-1);
   }


/* get sample name */

samp->length[i]=len;
samp->name[i]=AllocMem(strlen(name)+1,0);
if (samp->name[i]==0) {
   nomem(Window);
   return(-1);
   }
strcpy(samp->name[i],name);        /* put filename in sample structure */

PutName(Window,samp->name[*current],*current,REG_VID);
*current=i;
PutName(Window,samp->name[i],i,REV_VID);
if (comp==1) msg(Window,"This File was compressed 'Fibonacci-delta'");
return(0);
}

