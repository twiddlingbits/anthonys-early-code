#include "exec/types.h"
#include "exec/exec.h"
#include "fcntl.h"
#include "/include/psound.h"

extern int start_mark;
extern int end_mark;
extern int marked_samp;
extern struct list *open_list;

/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,fname,name)

struct Window *Window;
char *fname,*name;

{
extern struct Disk_Samp *cur;
char *AllocMem();
int in,i,len,oldlen,j;
ULONG k,x;
UWORD e1,e2,z;
long pos;
static char line[80];
UBYTE comp=0;
int memtype=0;
UBYTE *lmem, *rmem;
int use_16;
struct Disk_Samp *s;

use_16 = FALSE;

if ((in=(int)open(fname,O_RDONLY))==-1) {
   return(CANT_OPEN_FILE);
   }

i=create_sample(name, 0, &s);
if (i!=NO_ERROR) {
   return(i);
   }
printf("loadit s=%x '%s'\n",s,s->name);
read(in,(char *)&k,4);
if (k!=FORM) {         /* not in standard form */
   if (k==DUMP || k==DP16) {       /* dump with extra info */
      read(in,(char *)&len,4);
      read(in,(char *)&s->rate,4);
      read(in,(char *)&start_mark,4);
      read(in,(char *)&end_mark,4);
      s->type=LEFT;
      marked_samp=i;
      show_msg(Window,250);   /** File format is dump v3.0 **/
      if (k==DP16)
         use_16 = TRUE;
      }
   else {                           /* straight dump */
      s->rate=10000;           /* default length */
      s->type=LEFT;          /* use left (ch 0) as default */
      pos=lseek(in,0L,2);
      len=(int)even(pos);
      show_msg(Window,174);
      lseek(in,0L,0);    /* rewind */
      }
   }

else {
   read(in,(char *)&x,4);    /* skip length */
   read(in,(char *)&x,4);    /* get form type */
   if (x!=SVX) {
      close(in);
      return(NOT_8SVX);
      }

   s->type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   read(in,(char *)&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      skip(in);
      read(in,(char *)&x,4);
      if (x==-1 || x==0) {
         close(in);
         return(BAD_IFF);
         }
      }

   if (x==CHAN) {
      read(in,(char *)&x,4);
      if (x!=4) {
         close(in);
         return(BAD_IFF);
         }
      read(in,(char *)&s->type,4);
      if (s->type!=STEREO && s->type!=RIGHT
          && s->type!=LEFT) s->type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      read(in,(char *)&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         close (in);
         return(BAD_IFF);
         }
      read(in,(char *)&x,4);   /* oneshot:should be length of sample */
      read(in,(char *)&x,4);   /* repeat */
      read(in,(char *)&x,4);   /* cycles */
      read(in,(char *)&z,2);   /* get samples per second */
      read(in,(char *)&x,1);
      sprintf(line,get_msg_ptr(199),z);
      msg(Window,line);
      s->rate=z;
      read(in,(char *)&comp,1);
      lseek(in,4L,1);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   read(in,(char *)&len,4);
   if (s->type==STEREO)
      len=len/2;
   }

/* read in data--either BODY or dump */

if (comp==1) {oldlen=len;len=(len-2)*2;}
if (use_16) len = len*2;
lmem=AllocMem(fix_len(len),memtype);
if (lmem==0) {
   close(in);
   return(OUT_OF_MEMORY);
   }
rmem=lmem;
if (s->type==STEREO) {
   rmem=AllocMem(fix_len(len),0);
   if (rmem==0) {
      FreeMem(lmem,fix_len(len));
      close(in);
      return(OUT_OF_MEMORY);
      }
   }

if (s->type==STEREO || s->type==LEFT) {
   if (comp==0)  
      e1=read(in,lmem,len);     /* read in sample */
   if (comp==1)
      e1=cread(in,lmem,oldlen);     /* read in sample */
   }

if (s->type==STEREO || s->type==RIGHT) {
   if (comp==0)
     e2=read(in,rmem,len);     /* read in sample */
   if (comp==1)
     e2=cread(in,rmem,oldlen);     /* read in sample */
   }

close(in);

if (e1==-1 || e2==-1) {
   return(READ_ERROR);
   }


/* get sample name */

len=fix_len(len);

if (use_16) {
   for (j=0; j < len/2; j++) {
      i=put_samplel1(((short *)lmem)[j]);
      if (i!=NO_ERROR) {
   printf("loadit: error on put j%d\n",j);
         return(i);
         }
      }
   }
else {
   for (j=0; j < len; j++) {
      i=put_samplel1(lmem[j]<<8);
      if (i!=NO_ERROR) {
   printf("loadit: error on put j%d\n",j);
         return(i);
         }
      }
   }

FreeMem(lmem,len);

if (s->type==STEREO) {
   for (j=0; j < len; j++) {
      i=put_sampler1(rmem[j]<<8);
      if (i!=NO_ERROR) {
   printf("loadit: error on put\n");
         return(i);
         }
      }
   FreeMem(rmem,len);
   }

add_list_entry(open_list, s->name, s);
cur=s;
printf("loadit2 s=%x '%s'\n",s,s->name);
if (comp==1) show_msg(Window,182);
return(NO_ERROR);
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

