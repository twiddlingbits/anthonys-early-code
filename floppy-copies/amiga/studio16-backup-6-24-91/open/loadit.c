#include <exec/types.h>
#include <exec/exec.h>
#include <libraries/dos.h>
#include <fcntl.h>
#include "/include/psound.h"

extern struct list *open_list;

void QuickSplit();

/*****************************************/
/** perform load once got file and path **/
/*****************************************/

load_it(Window,fname,name,ptrptr)

struct Window *Window;
char *fname,*name;
struct Disk_Samp **ptrptr;

{
char *AllocMem();
int in,i,len,oldlen;
ULONG k,x;
UWORD e1,z;
long pos;
static char line[80];
UBYTE comp=0;
int memtype=0;
UBYTE *lmem, *rmem;
int use_16;
struct Disk_Samp *s;
int fp;
char *tmem;

int start_mark, end_mark;		/**!!!!!! DO SOMETHING WITH THIS ***/

use_16 = FALSE;
*ptrptr=0;
if ((in=(int)Open(fname,MODE_OLDFILE))==-1) {
   return(CANT_OPEN_FILE);
   }

i=CreateSample(name, 0, &s);
if (i!=NO_ERROR) {
   return(i);
   }
/*printf("loadit s=%x '%s'\n",s,s->name);*/
Read(in,&k,4);
if (k!=FORM) {         /* not in standard form */
   if (k==DUMP || k==DP16) {       /* dump with extra info */
      Read(in,(char *)&len,4);
      Read(in,(char *)&s->rate,4);
      Read(in,(char *)&start_mark,4);
      Read(in,(char *)&end_mark,4);
      s->type=LEFT;
/*      marked_samp=i; */
		telluser("Loading file dump 3.0 format",0,0);
      if (k==DP16)
         use_16 = TRUE;
      }
   else {                           /* straight dump */
      s->rate=10000;           /* default length */
      s->type=LEFT;          /* use left (ch 0) as default */
      pos=Seek(in,0L,OFFSET_CURRENT);
      len=(int)even(pos);
		telluser("Loading RAW data",0,0);
      Seek(in,0L,OFFSET_BEGINNING);    /* rewind */
      }
   }

else {
   Read(in,(char *)&x,4);    /* skip length */
   Read(in,(char *)&x,4);    /* get form type */
   if (x!=SVX) {
      Close(in);
      return(NOT_8SVX);
      }

   s->type=RIGHT;   /* default if IFF but not Perfect Sound IFF */

   getchunk:

   Read(in,(char *)&x,4);     /* get chunk type */
   while (x!=BODY && x!=CHAN && x!=VHDR) {    /* skip if not what we want */
      Skip(in);
      Read(in,(char *)&x,4);
      if (x==-1 || x==0) {
         Close(in);
         return(BAD_IFF);
         }
      }

   if (x==CHAN) {
      Read(in,(char *)&x,4);
      if (x!=4) {
         Close(in);
         return(BAD_IFF);
         }
      Read(in,(char *)&s->type,4);
      if (s->type!=STEREO && s->type!=RIGHT
          && s->type!=LEFT) s->type=RIGHT;
      goto getchunk;
      }

   if (x==VHDR) {
      Read(in,(char *)&x,4);  /* should be 20, length of VHDR */
      if (x!=20) {
         Close (in);
         return(BAD_IFF);
         }
      Read(in,(char *)&x,4);   /* oneshot:should be length of sample */
      Read(in,(char *)&x,4);   /* repeat */
      Read(in,(char *)&x,4);   /* cycles */
      Read(in,(char *)&z,2);   /* get samples per second */
      Read(in,(char *)&x,1);
      sprintf(line,"IFF at %d samples per second",z);
      telluser(line,0,0);
      s->rate=z;
      Read(in,(char *)&comp,1);
      Seek(in,4L,OFFSET_CURRENT);    /* skip volume */
      goto getchunk;
      }


    /* this must be a body (only thing left) */

   Read(in,(char *)&len,4);
   if (s->type==STEREO)
      len=len/2;
   }

/* read in data--either BODY or dump */

if (comp==1) {oldlen=len;len=(len-2)*2;}
if (use_16) {
	len = len*2;
	}
else {
	lmem=AllocMem(fix_len(len),memtype);
	if (lmem==0) {
	   Close(in);
	   return(OUT_OF_MEMORY);
	   }
	rmem=lmem;
   if (comp==0)  
      e1=Read(in,lmem,len);     /* read in sample */
   if (comp==1)
      e1=CRead(in,lmem,oldlen);     /* read in sample */

	Close(in);
	if (e1==-1) {
  		 return(READ_ERROR);
	   }
	}

/* get sample name */

len=fix_len(len);

if (use_16) {
	fp = WriteLockSamp(s);	
	if (fp!=-1) {
		QuickCopy(fp, in, len);
		SetSampLength(s,fp);
		WriteUnLockSamp(s);
		}
	Close(in);
	}
else {
	tmem = AllocMem(len*2, 0);
	if (tmem) {
		QuickSplit(tmem,lmem,len);
		fp = WriteLockSamp(s);	
		if (fp!=-1) {
			Write(fp, tmem, len*2);
			FreeMem(tmem, len*2);
			SetSampLength(s,fp);
			WriteUnLockSamp(s);
			}
		}
		FreeMem(lmem,len);
   }

if (comp==1) telluser("compressed IFF file",0,0);
*ptrptr=s;
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

void QuickSplit(tmem,lmem,len)
short tmem[];
char lmem[];
int len;
{
int i;

for (i=0; i < len; i++)
	tmem[i] = lmem[i]<<8;
	tmem[i] = tmem[i]&0xFF00;
}
