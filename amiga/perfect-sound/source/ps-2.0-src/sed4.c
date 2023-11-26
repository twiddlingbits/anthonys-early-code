/********************************/
/**   Perfect Sound (C) 1986   **/
/**    Anthony J. Wood         **/
/**                            **/
/** FILE 4 of 4                **/
/**                            **/
/** This Source code may be    **/
/** copied for backup only.    **/
/** This Source code is for    **/
/** reference only, you may    **/
/** NOT modify and compile     **/
/** this source except for     **/
/** PERSONAL use. Permission   **/
/** is given to distribute     **/
/** copies of the object code  **/
/** generated from compiling   **/
/** Perfect Sound ONLY if the  **/
/** source code is not changed **/
/** in any way. Permision is   **/
/** granted to use subroutines **/
/** in this file within your   **/
/** own programs if you notify **/
/** SunRize Industries of your **/
/** intent to do so and        **/
/** SunRize Industries agrees  **/
/** to your request.           **/
/**                            **/
/**  SunRize Industries        **/
/**  PO Box 1453               **/
/**  College Station, TX 77841 **/
/**  (409) 846-1311            **/
/**                            **/
/********************************/

#include <exec/types.h>
#include <stdio.h>
#include <exec/exec.h>
#include <fcntl.h>
#include <intuition/intuition.h>
#include "libraries/dos.h"
#include "df0:sed1.h"


#define SHOW 8      /* no. of samples to display in load requester */
#define IHI 50      /* height of getint requester */
#define SHI 30

WORD delta[16]={-34,-21,-13,-8,-5,-3,-2,-1,0,1,2,3,5,8,13,21};

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
j=(UWORD)(1.0/((float)samp->rate[current]*.279365*1e-6));

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

/*******************************/
/** compressed write          **/
/*******************************/

cwrite(fp,data,len)
int fp,len;
BYTE *data;
{
WORD prev=0;
register int pererr=0;
register int i;
register int k,j;
int bpos;
char val;
BYTE buf[512];

if (write(fp,&prev,1)==-1) return(-1);   /* padding */
prev=*data;
if (write(fp,data,1)==-1) return(-1);    /* 1st value */
i=len/2;
bpos=0;

for (k=0;k<i;k++) {
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;
   for (j=1;j<16;j++)
      if ((prev+delta[j]) > (WORD)*data) break;
   j--;
   if (prev+delta[j] < -128) j++;
   val=(val<<4)|j;
   prev=prev+delta[j];
   if (prev!=*data) pererr++;
   data++;

   buf[bpos++]=val;
   if (bpos==512) {
      bpos=0;
      if (write(fp,buf,512)==-1) return(-1);
      }
   }
   if (bpos!=0)
      if (write(fp,buf,bpos)==-1) return(-1);
return(pererr*100/len);
}

/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (Window,current,samp,path)

int *current;
struct Samp  *samp;
struct Window *Window;
char *path;

{
char name[80],fname[80],line[80];
name[0]=0;

if (getfile(Window,name,path,"LOAD","Load a Sample")==-1) return(-1);

strcpy(fname,path);
add(fname,name);       /* fname is now path+filename */

sprintf(line,"Loading: %s",fname);
msg(Window,line);
return(load_it(Window,current,samp,fname,name));
}


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
   samp->rate[i]=350;           /* default length */
   samp->type[i]=LEFT;          /* use left (ch 0) as default */
   pos=lseek(in,0L,2);
   samp->length[i]=(int)pos;
   msg(Window,"No playback rate specified -- 350 used");
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
      samp->rate[i]=(int)(1.0/(float)z*3579546.5);
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

/****************************/
/* compresed data read    ***/
/****************************/
cread(fp,data,len)
int fp;
register int len;
register char *data;
{
BYTE x,d;
register LONG i,j;
BYTE buf[512];

read(fp,&d,1);   /* skip padding */
read(fp,&x,1);   /* get inital value */

len=len-2;
if (read(fp,buf,min(512,len))==-1) return(-1);
j=0;
for (i=0;i<len;i++) {
   *data=x+delta[15&(buf[j]>>4)];
   x=*data++;
   *data=x+delta[buf[j++]&15];
   x=*data++;
   if (j==512) {
      j=0;
      if (read(fp,buf,min(512,(len-i-1)))==-1) return(-1);
      }
   }
return(0);
}



/**********************/
/**    skip a form    */
/**********************/

skip(in)
int in;

{
ULONG x;
read(in,&x,4);  /* get lenth */
lseek(in,x,1);  /* seek past it */
if ((x&1)==1)
   lseek(in,1L,1);  /* skip padding if odd */
}


/**************************/
/**  about perfect sound **/
/**************************/

aboutps(Window)


struct Window *Window;
{
int fp;

fp=Open("RAW:56/12/528/188/About Perfect Sound",MODE_NEWFILE);
if (fp==0) {
   msg(Window,"ERROR -- can't open AmigaDOS RAW:");
   return(0);
   }

pr(fp,"");
pr(fp,"This program, \"The Perfect Sound Sound Editor,\" is");
pr(fp,"copyrighted 1986 by Anthony J. Wood. You are free");
pr(fp,"to distribute copies of this program for non ");
pr(fp,"commercial use, and if  you enjoy this program,");
pr(fp,"feel free to send lots of money to the address below.");
pr(fp,"Seriously, $20 will buy you a manual, update info, and");
pr(fp,"our appreciation.");
pr(fp,"");
pr(fp,"This sound editor was written to support the");
pr(fp,"\"Perfect Sound\" Audio digitizer. You can");
pr(fp,"purchase The Perfect Sound stereo digitizer, ");
pr(fp,"Sound Editor with source code, Manual, and a");
pr(fp,"Library of recorded sounds (gunshots, splashes,");
pr(fp,"laughing, etc.) for $79.95 from the address below.");
pr(fp,"Order by phone or letter. There is no P&H, however,");
pr(fp,"Texas residents should include 5% sales tax.");
pr(fp,"Dealer inquiries are welcome.");
waitkey(fp);
pr(fp,"\n\n\n\n\n\n");
pr(fp,"                    SunRize Industries");
pr(fp,"                      PO Box 1453");
pr(fp,"                College Station, TX 77841");
pr(fp,"                    (409) 846-1311");
waitkey(fp);
Close(fp);
}


/****************************/
/** print a line to dos fp  */
/****************************/

pr(fp,string)

int fp;
char *string;
{
UBYTE newline=10;

Write(fp,string,strlen(string));
Write(fp,&newline,1);
}

/******************************/
/** wait for a key press     **/
/*****************************/

waitkey(fp)
int fp;
{
char xx;
pr(fp,"");
pr(fp,"Press any key to continue...");
Read(fp,&xx,1);
pr(fp,"\014");
}

/**********************************/
/**  request a file name         **/
/**********************************/

getfile(Window,fname,drawer,ok,title)

struct Window *Window;
char *fname,*drawer;
char *ok,*title;


{
extern WORD diskchange;
static char blank[]="                            ";
char line[80];
int i;
int class,code,x,y;
SHORT ver;
static int top,numfiles,vpot;
struct Gadget *address;
static char undo[80];
static char fn[90][36];
static int ft[80];
static struct Border b[4];
static struct IntuiText t[5];
static struct IntuiText nt[SHOW];
static struct Gadget g[5];
static struct Gadget name[SHOW];
static struct StringInfo s[2];
static struct PropInfo prop;
static struct Requester loadreq;
static struct IntuiMessage *message;
static struct Image image;

static SHORT bd0[]={0,0,
                    274,0,
                    274,149,
                    0,149,
                    0,0,
                    0,1,
                    273,1,
                    273,148,
                    1,148,
                    1,0,
                    0,0,
                    0,12,
                    274,12};
static SHORT bd1[]={0,100,275,100};
static SHORT bd2[]={30,130,86,130,86,142,30,142,30,130};
static SHORT bd3[]={180,130,
            236,130,
            236,142,
            180,142,
            180,130};

int lock,success;
struct FileInfoBlock *info;

if (drawer[0]==0) strcpy(drawer,"df0:");

if ((info=(struct FileInfoBlock *)
          AllocMem(sizeof(struct FileInfoBlock),0))==0) {
   return(-1);
   }

for (i=0;i<4;i++) {
   b[i].DrawMode=JAM1;
   b[i].LeftEdge=0;
   b[i].TopEdge=0;
   b[i].FrontPen=3;
   b[i].BackPen=0;
   b[i].NextBorder=&b[i+1];
}
b[3].NextBorder=NULL;
b[0].Count=13;
b[0].XY=&bd0[0];
b[1].Count=2;
b[1].XY=&bd1[0];
b[2].Count=5;
b[2].XY=&bd2[0];
b[3].Count=5;
b[3].XY=&bd3[0];

/** initilize text structures **/

for (i=0;i<5;i++) {
   t[i].DrawMode=JAM1;
   t[i].FrontPen=1;
   t[i].BackPen=0;
   t[i].ITextFont=NULL;
   t[i].NextText=&t[i+1];
}

t[4].NextText=NULL;

t[0].LeftEdge=8;
t[0].TopEdge=105;
t[0].IText="Drawer:";

t[1].LeftEdge=23;
t[1].TopEdge=119;
t[1].IText="File:";

t[2].LeftEdge=42;
t[2].TopEdge=133;
t[2].IText=ok;

t[3].LeftEdge=184;
t[3].TopEdge=133;
t[3].IText="CANCEL";

t[4].LeftEdge=70;
t[4].TopEdge=3;
t[4].IText=title;

/* add gadgets */
for (i=0;i<SHOW;i++) {
   name[i].GadgetType=REQGADGET|BOOLGADGET;
   name[i].Flags=GADGHCOMP;
   name[i].NextGadget=&name[i+1];
   name[i].Activation=RELVERIFY;
   name[i].GadgetRender=NULL;
   name[i].SelectRender=NULL;
   name[i].GadgetText=&nt[i];
   name[i].LeftEdge=10;
   name[i].TopEdge=i*10+15;
   name[i].Width=230;
   name[i].Height=11;

   nt[i].DrawMode=JAM2;
   nt[i].FrontPen=1;
   nt[i].BackPen=2;
   nt[i].ITextFont=NULL;
   nt[i].NextText=NULL;
   nt[i].TopEdge=1;
   nt[i].LeftEdge=0;
   nt[i].IText="";
   }
name[SHOW-1].NextGadget=&g[0];

for (i=0;i<5;i++) {
   g[i].GadgetType=REQGADGET;
   g[i].Flags=GADGHNONE;
   g[i].NextGadget=&g[i+1];
   g[i].Activation=RELVERIFY;
   g[i].GadgetRender=NULL;
   g[i].SelectRender=NULL;
   g[i].GadgetText=NULL;
}

g[4].NextGadget=NULL;

/* load */
g[0].LeftEdge=30;
g[0].TopEdge=130;
g[0].Width=56;
g[0].Height=12;
g[0].GadgetType|=BOOLGADGET;
g[0].Activation|=ENDGADGET;
/* cancel */
g[1].LeftEdge=180;
g[1].TopEdge=130;
g[1].Width=56;
g[1].Height=12;
g[1].GadgetType|=BOOLGADGET;
g[1].Activation|=ENDGADGET;
/* Drawer */
g[2].LeftEdge=70;
g[2].TopEdge=105;
g[2].Width=180;
g[2].Height=15;
g[2].GadgetType|=STRGADGET;
g[2].SpecialInfo=(APTR)&s[0];
g[2].Flags=GADGHCOMP;
/* file */
g[3].LeftEdge=70;
g[3].TopEdge=119;
g[3].Width=180;
g[3].Height=15;
g[3].GadgetType|=STRGADGET;
g[3].SpecialInfo=(APTR)&s[1];
g[3].Flags=GADGHCOMP;
/* autoknob */
g[4].LeftEdge=250;
g[4].TopEdge=13;
g[4].Width=24;
g[4].Height=87;
g[4].GadgetType|=PROPGADGET;
g[4].SpecialInfo=(APTR)&prop;
g[4].GadgetRender=&image;

/* prop structure */
prop.Flags=AUTOKNOB|FREEVERT;
prop.HorizPot=0;
prop.VertPot=0;
prop.HorizBody=0xFFFF;
prop.VertBody=0xFFFF;


/* string gadget info */

s[0].Buffer=drawer;
s[1].Buffer=fname;

for (i=0;i<2;i++) {
   s[i].UndoBuffer=undo;
   s[i].BufferPos=0;
   s[i].DispPos=0;
   s[i].MaxChars=80;
}


InitRequester(&loadreq);

loadreq.LeftEdge=15;
loadreq.TopEdge=11;
loadreq.Width=275;
loadreq.Height=150;
loadreq.ReqBorder=&b[0];
loadreq.ReqText=&t[0];
loadreq.ReqGadget=&name[0];
loadreq.BackFill=2;   /* black */

Request(&loadreq,Window);
if (diskchange==FALSE) goto quick;

/**  load current dir **/

newdir:
lock=Lock(drawer,ACCESS_READ);
if (lock==0) {
   numfiles=0;
   top=0;
   goto fresh;
   }

if ((success=Examine(lock,info))==0) {
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(-1);
   }

ExNext(lock,info);  /* skip volume name */

i=0;
do {
   strncpy(fn[i],info->fib_FileName,30);
   ft[i]=info->fib_DirEntryType;   /* + is dir, - is file */
   if (ft[i]>0) strcpy(fn[i]+strlen(fn[i])," (dir)");
   i++;
   } while (i<90 && ExNext(lock,info)!=0);
UnLock(lock);
numfiles=i;
diskchange=FALSE;
top=0;  /* 1st file shown in list */
vpot=0;  /* prop position */

quick:

ver=(WORD)((float)0xFFFF*(float)SHOW/(float)numfiles);
ModifyProp(&g[4],Window,&loadreq,AUTOKNOB|FREEVERT,0,vpot,0xFFFF,ver);
fresh:

for (i=0;i<SHOW;i++)
   nt[i].IText=blank;

RefreshGadgets(&name[0],Window,&loadreq);

for (i=top;i<top+SHOW;i++) {
   if (i<numfiles) nt[i-top].IText=fn[i];
   }

RefreshGadgets(&name[0],Window,&loadreq);

/* wait for a msg */
loop:
Wait(1<<Window->UserPort->mp_SigBit);

while (message=GetMsg(Window->UserPort)) {
class=message->Class;
code=message->Code;
address=message->IAddress;
x=message->MouseX;
y=message->MouseY;
ReplyMsg(message);

if (class!=GADGETUP) {
   goto skip;
   }

if (address==&g[0]) {    /* Load file */
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(0);
   }

if (address==&g[1]) {   /* cancel */
   FreeMem(info,sizeof(struct FileInfoBlock));
   return(-1);
   }

if (address==&g[2]) {    /* drawer */
   goto newdir;
   }

if (address==&g[4]) {  /* prop gadget */
   top=prop.VertPot*numfiles/0xFFFF;
   vpot=prop.VertPot;
   goto fresh;
   }

for (i=0;i<SHOW;i++)  /* picking an entry? */
   if (&name[i]==address) {
      i=i+top;
      if (i>=numfiles) goto skip;
      if (ft[i]>0) {      /* directory */
         strncpy(line,fn[i],strlen(fn[i])-6);
         add(drawer,line);
         goto newdir;
         }
      else {
         strcpy(fname,fn[i]);
         RefreshGadgets(&name[0],Window,&loadreq);
         }
      }

skip: ;
}
goto loop;
}


/************************/
/** extend filename     */
/************************/

add(to,from)
char *to,*from;
{

int i;

if (*to==0) {
   strcpy(to,from);
   return(0);
   }
if (to[strlen(to)-1]==':') {
   strcpy(to+strlen(to),from);
   return(0);
   }
to[(i=strlen(to))]='/';
strcpy(to+i+1,from);
}

/*****************************/
/** request an integer value */
/*****************************/

getint(Window,ival,imin,imax,title,width)

struct Window *Window;
int ival,width,imin,imax;
char *title;

{
struct Border b;
struct IntuiText t[2];
struct Gadget g[3];
struct StringInfo s;
struct PropInfo prop;
struct Requester loadreq;
struct IntuiMessage *message;
struct Image image;
int class,code,x,y;
struct Gadget *address;
char undo[80],ascii[80];
int i,hor;
static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,IHI-1,
                   0,IHI-1,
                   0,0,
                   0,11,
                   99,11};

bd[2]=width-1;
bd[4]=bd[2];
bd[12]=bd[2];

/* initilize string (int) gadget */
stci_d(ascii,ival,80);


   b.DrawMode=JAM1;
   b.LeftEdge=0;
   b.TopEdge=0;
   b.FrontPen=3;
   b.BackPen=0;
   b.NextBorder=NULL;
   b.NextBorder=NULL;
   b.Count=7;
   b.XY=&bd[0];

/** initilize text structures **/

for (i=0;i<2;i++) {
   t[i].DrawMode=JAM1;
   t[i].FrontPen=1;
   t[i].BackPen=0;
   t[i].ITextFont=NULL;
   t[i].NextText=&t[i+1];
}

t[1].NextText=NULL;

t[0].LeftEdge=(width-8*strlen(title))/2;
t[0].TopEdge=2;
t[0].IText=title;

t[1].LeftEdge=width-60;
t[1].TopEdge=IHI-15;
t[1].IText=" OK! ";
t[1].DrawMode=JAM2;

/* add gadgets */

for (i=0;i<3;i++) {
   g[i].GadgetType=REQGADGET;
   g[i].Flags=GADGHCOMP;
   g[i].NextGadget=&g[i+1];
   g[i].Activation=RELVERIFY;
   g[i].GadgetRender=NULL;
   g[i].SelectRender=NULL;
   g[i].GadgetText=NULL;
}

g[2].NextGadget=NULL;

/* ok! */
g[0].LeftEdge=width-60;
g[0].TopEdge=IHI-15;
g[0].Width=40;
g[0].Height=8;
g[0].GadgetType|=BOOLGADGET;
g[0].Activation|=ENDGADGET;

/* int string (enter value) */
g[2].LeftEdge=30;
g[2].TopEdge=IHI-15;
g[2].Width=42;
g[2].Height=11;
g[2].GadgetType|=STRGADGET;
g[2].Activation|=LONGINT|STRINGCENTER;
g[2].SpecialInfo=(APTR)&s;

/* autoknob */
g[1].LeftEdge=10;
g[1].TopEdge=20;
g[1].Width=width-20;
g[1].Height=9;
g[1].GadgetType|=PROPGADGET;
g[1].SpecialInfo=(APTR)&prop;
g[1].GadgetRender=&image;
g[1].Activation=FOLLOWMOUSE|GADGIMMEDIATE;
g[1].Flags=GADGHNONE;

/* prop structure */
prop.Flags=AUTOKNOB|FREEHORIZ;
prop.HorizPot=(SHORT)((LONG)(ival-imin)*(LONG)0xFFFF/(LONG)(imax-imin));
prop.VertPot=0;
prop.HorizBody=1;
prop.VertBody=0xFFFF;


/* string gadget info */

s.Buffer=ascii;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=80;
s.LongInt=ival;


InitRequester(&loadreq);

loadreq.LeftEdge=100;
loadreq.TopEdge=50;
loadreq.Width=width;
loadreq.Height=IHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t[0];
loadreq.ReqGadget=&g[0];
loadreq.BackFill=2;   /* black */

Request(&loadreq,Window);

loop:
Wait(1<<Window->UserPort->mp_SigBit);

while (message=GetMsg(Window->UserPort)) {
   class=message->Class;
   code=message->Code;
   address=(struct Gadget *)message->IAddress;
   x=message->MouseX;
   y=message->MouseY;
   ReplyMsg(message);
}

if (class==GADGETUP && address==&g[0]) {  /* ok! */
   return((int)s.LongInt);
   }

if (class==MOUSEMOVE) {
   ival=prop.HorizPot*(imax-imin)/0xFFFF+imin;
   stci_d(ascii,ival,80);
   s.LongInt=ival;
   RefreshGadgets(&g[2],Window,&loadreq);
   }

if (class==GADGETUP && address==&g[2]) {    /* new integer */
   ival=s.LongInt;
   if (ival>imax) ival=imax;
   if (ival<imin) ival=imin;
   hor=(int)((LONG)(ival-imin)*(LONG)0xFFFF/(LONG)(imax-imin));
   ModifyProp(&g[1],Window,&loadreq,AUTOKNOB|FREEHORIZ,hor,0,1,0xFFFF);
   }

goto loop;   
   
}
getstr(Window,string,title,width)

struct Window *Window;
int width;
char *title,*string;

{
struct Border b;
struct IntuiText t;
struct Gadget g;
struct StringInfo s;
struct Requester loadreq;
struct IntuiMessage *message;
char undo[80];
static SHORT bd[]={0,0,          /* 99 are changed below */
                   99,0,
                   99,SHI-1,
                   0,SHI-1,
                   0,0,
                   0,11,
                   99,11};

bd[2]=width-1;
bd[4]=bd[2];
bd[12]=bd[2];


b.DrawMode=JAM1;
b.LeftEdge=0;
b.TopEdge=0;
b.FrontPen=3;
b.BackPen=0;
b.NextBorder=NULL;
b.NextBorder=NULL;
b.Count=7;
b.XY=&bd[0];

/** initilize text structures **/

t.DrawMode=JAM1;
t.FrontPen=1;
t.BackPen=0;
t.ITextFont=NULL;
t.NextText=NULL;
t.LeftEdge=(width-8*strlen(title))/2;
t.TopEdge=2;
t.IText=title;


/* add gadgets */

g.GadgetType=REQGADGET|STRGADGET;
g.Flags=GADGHCOMP;
g.Activation=ENDGADGET|RELVERIFY;
g.GadgetRender=NULL;
g.SelectRender=NULL;
g.GadgetText=NULL;
g.NextGadget=NULL;
g.LeftEdge=20;
g.TopEdge=SHI-15;
g.Width=width-40;
g.Height=11;
g.SpecialInfo=(APTR)&s;

s.Buffer=string;
s.UndoBuffer=undo;
s.BufferPos=0;
s.DispPos=0;
s.MaxChars=80;

InitRequester(&loadreq);

loadreq.LeftEdge=(640-width)/2;
loadreq.TopEdge=50;
loadreq.Width=width;
loadreq.Height=SHI;
loadreq.ReqBorder=&b;
loadreq.ReqText=&t;
loadreq.ReqGadget=&g;
loadreq.BackFill=2;   /* black */

Request(&loadreq,Window);

again:
Wait(1<<Window->UserPort->mp_SigBit);
while(message=GetMsg(Window->UserPort)) {
   if (message->Class==GADGETUP) {
      ReplyMsg(message);
      return(0);
      }
   ReplyMsg(message);
   }
goto again;

}
