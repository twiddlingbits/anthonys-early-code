/********************************/
/**   Perfect Sound (C) 1986   **/
/**    Anthony J. Wood         **/
/**                            **/
/** FILE 3 of 4                **/
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

#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"
#include "intuition/intuition.h"
#include "devices/audio.h"
#include "fcntl.h"
#include "df0:sed1.h"


extern struct ExecBase *SysBase;     /* for memory list searches */

/******************************/
/** put @ x,y pix pos        **/
/******************************/

putxy(window,x,y,string,fg,bg)

struct Window *window;
int x,y,fg,bg;
char *string;

{

struct IntuiText text;

text.FrontPen=fg;
text.BackPen=bg;
text.DrawMode=JAM2;
text.LeftEdge=x;
text.TopEdge=y;
text.ITextFont=NULL;
text.IText=string;
text.NextText=NULL;


PrintIText (window->RPort,&text,0,0);

}


drawline(Window,fx,fy,tx,ty,pen)

struct Window *Window;
int fx,fy,tx,ty;
int pen;
{
Move(Window->RPort,fx,fy);
SetAPen(Window->RPort,pen);
Draw(Window->RPort,tx,ty);
}

/*******************/
/* find a new slot */
/*******************/

getslot(Window,samp)

struct Window *Window;
struct Samp *samp;

{
int i=0;

while(samp->lmem[i]!=0 && i<MAX_SAMPLES) i++;
if (i==MAX_SAMPLES) {
   msg(Window,"No slots available... Delete a sample.");
   return(-1);
   }
return(i);
}

/***********************/
/** plot sample       **/
/***********************/

graph(Window,samp,cur)

struct Window *Window;
struct Samp *samp;
int cur;

{

int stp;
int y;
register int val,i,x;

if (samp->lmem[cur]==0) return (0);
if (samp->sm[cur]>=samp->em[cur]) return(0);

/* 1st clear screen memory in this area */
SetAPen(Window->RPort,BLACK);
SetBPen(Window->RPort,BLACK);
RectFill(Window->RPort,2,GBOTY-255/SCALE,637,GBOTY);
drawline(Window,2,GBOTY-255/SCALE-1,637,GBOTY-255/SCALE-1,WHITE);
drawline(Window,2,GBOTY-255/SCALE-2,637,GBOTY-255/SCALE-2,WHITE);

stp=(samp->em[cur]-samp->sm[cur])/530;
x=50;    /* line up with prop gadgets */
y=GBOTY-((int)(*samp->lmem[cur])+(int)128)/SCALE;
Move(Window->RPort,x,y);
SetAPen(Window->RPort,WHITE);
if (stp!=0)
   for (i=0;i<580;i++) {
      val=128+*(samp->lmem[cur]+i*stp+samp->sm[cur]);
      val=GBOTY-val/SCALE;
      Draw(Window->RPort,++x,val);
      }

else {
   val=samp->sm[cur];
   Move(Window->RPort,x,GBOTY-(*(samp->lmem[cur]+val++)+128)/SCALE);
   stp=530/(samp->em[cur]-samp->sm[cur]);
   if (stp==0) stp=1;
   for (i=stp;i<580;i=i+stp)
      Draw(Window->RPort,x+i,GBOTY-(*(samp->lmem[cur]+val++)+128)/SCALE);
   }  


}


/******************/
/** power (**)   **/
/******************/

powr(x,y)
register int x,y;
{
register int i,z;
if (y==0) return (1);
z=1;
for (i=0;i<y;i++)
   z=z*x;
return (z);
}


/**************************************/
/**   show input levels             **/
/*************************************/

level(Window)
struct Window *Window;
{
static int k,right,left,rmin,rmax,lmin,lmax,i;
UBYTE *data,*direction,*pa;
UBYTE *data2,*direction2;
char line[80];

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

pa=(UBYTE *)0xBFE001;
*direction=(UBYTE)0;           /* all bits are read */
*data=(UBYTE)0;
*direction2=(UBYTE)2+4;      /* pa1 & pa2 write */
*data2=(UBYTE)2;            /* pa2 -- select right channel */

while (1==1) {
   lmax=0;
   rmax=0;
   lmin=999;
   rmin=999;
   for (k=0;k<30;k++) {
      if ((*pa&64)!=64) {
         msg(Window,"");
         return(0);
         }
      *data2=(UBYTE)0;           /* select both channels */
      i=*data;                   /* start conversion    */
      *data2=(UBYTE)RIGHT;       /* select right chennel */
      for(i=0;i<5000;i++) ;      /* wait till finished */
      right=*data;               /* get right channel value */
      *data2=(UBYTE)LEFT;        /* select left channel */
      left=*data;                /* get left channel value */
      if (left<lmin) lmin=left;
      if (left>lmax) lmax=left;
      if (right<rmin) rmin=right;
      if (right>rmax) rmax=right;
      sprintf(line,"L: %3.3d %3.3d %3.3d  R: %3.3d %3.3d %3.3d   D: %4.4d",
              lmin,lmax,left,rmin,rmax,right,right-left);
      msg(Window,line);
      }
   }
}

/**********************************/
/** find largest memory fragment  */
/**********************************/

frags()

{
int    newmax=0;
struct MemChunk *chunk;
struct MemHeader *mem;
struct ExecBase *eb = SysBase;


Forbid();
for (mem = (struct MemHeader *) eb -> MemList.lh_Head;
mem -> mh_Node.ln_Succ; mem = mem -> mh_Node.ln_Succ) {
   for (chunk = mem -> mh_First; chunk; chunk = chunk -> mc_Next) {
   if (chunk->mc_Bytes>newmax) newmax=chunk->mc_Bytes;
   }
}
Permit();
if (newmax<15000) newmax=15000;
return (newmax-15000);    /* leave some memory to prevent crashes */
}


/******************************/
/*** set up gadget structures */
/******************************/

makegadget()

{
static struct Gadget gadget_s,gadget_e,gadget_p;
static struct Gadget gw,gr;
static struct PropInfo prop_s,prop_e,prop_p;
static struct Image image_s,image_e,image_p;
static struct IntuiText t[2];
static struct Border b;
int i;

static SHORT bd[]={0,0,
             90,0,
             90,12,
             0,12,
             0,0};


/* define a propertional gadget to use for curers */

gadget_s.NextGadget=NULL;
gadget_s.LeftEdge=50;
gadget_s.TopEdge=GSP;
gadget_s.Width=-90;
gadget_s.Height=10;
gadget_s.Flags=GADGHNONE|GRELWIDTH;
gadget_s.Activation=FOLLOWMOUSE|GADGIMMEDIATE;
gadget_s.GadgetType=PROPGADGET;
gadget_s.GadgetRender=(APTR)&image_s;
gadget_s.SelectRender=NULL;
gadget_s.GadgetText=NULL;
gadget_s.MutualExclude=NULL;
gadget_s.SpecialInfo=(APTR)&prop_s;
prop_s.Flags=FREEHORIZ|AUTOKNOB;
prop_s.HorizPot=0;
prop_s.HorizBody=0;
prop_s.VertBody=MAXBODY;

movmem(&gadget_s,&gadget_e,sizeof(struct Gadget));
movmem(&prop_s,&prop_e,sizeof(struct PropInfo));

movmem(&gadget_s,&gadget_p,sizeof(struct Gadget));
movmem(&prop_s,&prop_p,sizeof(struct PropInfo));

gadget_e.TopEdge=GEP;
gadget_e.GadgetRender=(APTR)&image_e;
gadget_e.SpecialInfo=(APTR)&prop_e;
gadget_s.NextGadget=&gadget_e;
gadget_e.NextGadget=&gadget_p;
gadget_p.NextGadget=&gw;
gadget_p.TopEdge=GPP;
gadget_p.GadgetRender=(APTR)&image_p;
gadget_p.SpecialInfo=(APTR)&prop_p;

gw.NextGadget=&gr;
gw.LeftEdge=EDGE_GW;
gw.TopEdge=GPW;
gw.Width=88;
gw.Height=11;
gw.Flags=GADGHCOMP;
gw.Activation=RELVERIFY;
gw.GadgetType=BOOLGADGET;
gw.GadgetRender=&b;
gw.SelectRender=NULL;
gw.GadgetText=&t[0];
gw.MutualExclude=NULL;
gw.SpecialInfo=NULL;

movmem(&gw,&gr,sizeof(struct Gadget));
gr.LeftEdge=EDGE_GR;
gr.NextGadget=NULL;
gr.TopEdge=GPR;
gr.GadgetText=&t[1];

b.LeftEdge=-2;
b.TopEdge=-2;
b.FrontPen=3;
b.DrawMode=JAM1;
b.XY=&bd[0];
b.Count=5;
b.NextBorder=0;

for (i=0;i<2;i++) {
   t[i].FrontPen=3;
   t[i].BackPen=2;
   t[i].DrawMode=JAM2;
   t[i].LeftEdge=0;
   t[i].TopEdge=1;
   t[i].ITextFont=NULL;
   t[i].IText="PLAY SAMPLE";
   t[i].NextText=0;
   }
t[1].IText="PLAY RANGE";


return ((int)&gadget_s);
}

/**************************/
/** flip a range of bytes */
/**************************/

flip (addrs,len)

char *addrs;
int len;

{

register char *s,*e;
register char temp;
register int k;

s=addrs;
e=addrs+len-1;
k=len/2;

while (k-->0) {
   temp=*s;
   *s=*e;
   *e=temp;
   s++;e--;
   }
return(0);
}
   
/********************************************/
/** insert a range from one samp to another */
/********************************************/

move(Window,samp,start,end,position,from,to)

struct Window *Window;
struct Samp *samp;
int position,from,to,start,end;

{

char *y,*x;
int len;

   if (start>=end) {
      msg(Window,"Markers not set.");
      return(0);
      }

   if ((samp->type[to]==STEREO || samp->type[from]==STEREO)
        && (samp->type[to]!=samp->type[from])) {
      msg(Window,"Both samples are not stereo.");
      return(0);
      }

   msg(Window,"Working...");

   len=samp->length[to]+end-start;

   if ((x=AllocMem(len,0))==0) {
      nomem(Window);
      return(0);
      }

   if (samp->type[to]==STEREO)
      if ((y=AllocMem(len,0))==0) {
         nomem(Window);
         FreeMem(x,len);
         return(0);
         }

   movmem(samp->lmem[to],x,position);
   movmem(samp->lmem[from]+start,x+position,end-start);
   movmem(samp->lmem[to]+position,x+position+end-start,
          samp->length[to]-position);
   FreeMem(samp->lmem[to],samp->length[to]);
   samp->lmem[to]=x;

   if (samp->type[to]==STEREO) {
      movmem(samp->rmem[to],y,position);
      movmem(samp->rmem[from]+start,y+position,end-start);
      movmem(samp->rmem[to]+position,y+position+end-start,
          samp->length[to]-position);
      FreeMem(samp->rmem[to],samp->length[to]);
      samp->rmem[to]=y;
      }
    else samp->rmem[to]=x;

   samp->length[to]=len;
   msg(Window,"");
   }


/***************************/
/** Open a screen         **/
/***************************/

openscr(width,depth)
SHORT width,depth;
{
struct NewScreen cust;
struct Screen *screen;
static struct TextAttr thefont= {

   "topaz.font",
   TOPAZ_SIXTY,
   FS_NORMAL,
   FPF_ROMFONT,
   };

cust.LeftEdge=0;
cust.TopEdge=199;
cust.Width=width;
cust.Height=200;
cust.Depth=depth;
cust.DetailPen=2;
cust.BlockPen=3;
cust.ViewModes=0;
if (width>320) cust.ViewModes=HIRES;
cust.Type=CUSTOMSCREEN;
cust.Font=&thefont;
cust.DefaultTitle=" this is te";
cust.Gadgets=0;
cust.CustomBitMap=0;

screen=OpenScreen(&cust);
return((int)screen);
}



/********************************/
/* Load a ilbm File from disk   */
/********************************/

load_ilbm (fname,iff,cmap)

struct Image *iff;
char *fname;
UBYTE *cmap;

{
ULONG x;
UBYTE planes;
int plen,rowlen,len;
int templen;
register int i,z;
int offset,in,k;
BYTE *comp;
static int p_tab[]={1,3,7,15,31,63};

iff->LeftEdge=0;
iff->TopEdge=0;
iff->NextImage=0;
iff->PlaneOnOff=0;

if ((in=(int)open(fname,O_RDONLY))==-1)
   return(-1);

read(in,&k,4);

if (k!=FORM) return (-1);

read(in,&x,4);    /* skip length */
read(in,&x,4);    /* get form type */
if (x!=ILBM) {
   close(in);
   return(-1);
   }

nextchunk:

read(in,&x,4);     /* get chunk type */
while (x!=BODY && x!=CMAP && x!=BMHD) {    /* skip if not what we want */
   skip(in);
   read(in,&x,4);
   if (x==-1 || x==0) {
      close(in);
      return(-1);
      }
   }

if (x==CMAP) {
   read(in,&len,4);
   read(in,cmap,len);
   if ((len&1)==1) lseek(in,1L,1);  /* skip padding bytes */
   goto nextchunk;
   }
   
if (x==BMHD) {
   lseek(in,4L,1);   /* skip length */
   read(in,&iff->Width,2);
   read(in,&iff->Height,2);
   lseek(in,4L,1);
   read(in,&planes,1);
   iff->Depth=(SHORT)planes;
   iff->PlanePick=p_tab[planes];
   lseek(in,11L,1);
   goto nextchunk;
   }

if (x==BODY) {
   read(in,&templen,4);
   comp=AllocMem(templen,0);
   if (comp==0) {
      printf("ERROR! not enough memory to load iff file!\n");
      exit(0);
      }
   read(in,comp,templen);

   rowlen=iff->Width/8;
   if (rowlen*8!=iff->Width) rowlen++;
   plen=rowlen*iff->Height;
   iff->ImageData=AllocMem(plen*iff->Depth,MEMF_CHIP);   /* change this? */
   if (iff->ImageData==0) {
      printf("ERROR! Out of memory\n");
      exit(10);
      }
   offset=0;
   for (i=0;i<iff->Height;i++)
      for(k=0;k<iff->Depth;k++) {
         z=uncomp(&offset,comp+offset,(BYTE *)iff->ImageData+rowlen*i+k*plen,rowlen);
         if (z!=0) {
            printf("ERROR! compression fault.\n");
            exit(20);
            }
         }

   FreeMem(comp,templen);
   close (in);
   return(0);
   }
printf("ERROR: How did i get here?\n\n");
}

/*********************************************************************/
/*       Uncompress one row                                          */
/* Control Bytes:                                                    */
/*    [0..127]   : followed by n+1 bytes of data.                      */
/*    [-1..-127] : followed by byte to be repeated (-n)+1 times.       */
/*    -128       : NOOP.                                               */
/*********************************************************************/

uncomp(offset,src,dest,width)

register char *src,*dest;
int *offset,width;
{
register char n,val;


while (width>0) {
   if ((n=*src)>0) {
   src++;
   (*offset)++;
   
      while (n-->=0) {
         *dest=*src;
         dest++; src++; (*offset)++; width--;
         }
      }
   else {
      src++;
      (*offset)++;
      val=*src;
      src++;
      (*offset)++;
      while (n++<=0) {
         *dest=val; width--;
         dest++;
         }
      }     /* end else */
   }        /* end 1st while */

return (width);
}
/***********************************/
/***        record a sample      ***/
/***********************************/

digitze (Window,lp,rp,rate,maxnum,chan)
char *rp,*lp;
int rate,maxnum,chan;
struct Window *Window;
{
register char *reg1,*reg2;
register ULONG k;
static int i;
static UBYTE *data,*direction,*pa,*direction2,*data2;

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
pa=(UBYTE *)0xBFE001;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

reg1=lp;
reg2=rp;
*direction=0;           /* all bits are read */
*data=0;
*direction2=6;  /* pa1 and pa2 write */
*data2=chan;

msg(Window,"RECORDING... Press the left mouse button to stop.");
if ((*pa&64)==0) while((*pa&64)==0) ;
for (k=0;k<20;k++) ;    /* skip key bbbounce */
Disable();
k=0;
if (chan!=STEREO)
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);   /* delay loop */
      *reg1++=(*data)-128;
      }
else {
   rate=rate/2;
   while (k++<maxnum && (*pa&64)==64) {
      for(i=0;i<rate;i++);    /* delay loop */
      *data2=RIGHT;
      *reg2++=(*data)-128;
      *data2=LEFT;
      *reg1++=(*data)-128;
      }
   }

Enable();
return((int)(k-1));
}

/*********************************************/
/***        listen to digitizer            ***/
/*********************************************/

listen (chan)
int chan;
{
static UBYTE *base,*pa,*data,*direction,*data2,*direction2;
static unsigned short sam,i;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short *aud1lch,*aud1len,*aud1vol,*aud1per,*aud1dat;
unsigned short *intreq,*intreqr;
unsigned int *aud0lc,*aud1lc;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

*direction=0;           /* all bits are read */
*data=0;
*direction2=2+4;      /* pa1 & pa2 write */
Disable();

base=(UBYTE *)0xDEF000;
/* channel 0 chip addresses */
dmaconw=(unsigned short *)(base+0x096);
aud0lch=(unsigned short *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
aud0lc=(int *)aud0lch;
/* channel 1 chip addresses */
aud1lch=(unsigned short *)(base+0x0B0);
aud1len=(unsigned short *)(base+0x0B4);
aud1vol=(unsigned short *)(base+0x0B8);
aud1per=(unsigned short *)(base+0x0B6);
aud1dat=(unsigned short *)(base+0x0BA);
aud1lc=(int *)aud1lch;
intreq=(unsigned short *)(base+0x09C);
intreqr=(unsigned short *)(base+0x1E);
pa=(UBYTE *)0xBFE001;

*aud0vol=64;
*aud0per=1;
*aud1vol=64;
*aud1per=1;
i=0;

if (chan==STEREO)
   while ((*pa&64)==64) {
      *data2=LEFT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;    /* not in manual, but required to resart ch 0 */

      *data2=RIGHT;
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;    /* needed to restart ch 1 */
      }
*data2=chan;
if (chan==RIGHT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud1dat=i;
      *intreq=256;
      }

if (chan==LEFT)
   while ((*pa&64)==64) {
      sam=(*data-128);
      sam=sam&0xFF;
      i=sam|(sam<<8);
      *aud0dat=i;
      *intreq=128;
      }
while ((*pa&64)!=64) ;     /* wait for left mouse button release */
Enable();
}

/********************************/
/*** round up to nearest 8 byte**/
/********************************/

rup(p)

char *p;
{
int i;
i=(int)p;
if ((i&0xFFFFFFF8)==i) return(i);
return((i&0xFFFFFFF8)+8);
}

rdn(i)
int i;
{
return(i&0xFFFFFFF8);
}

/************/
/** alter  **/
/************/

alter(val,maxval,x)

int maxval,x;
int *val;
{
if (x>600 && x<621 && *val>0) *val=*val-1;
if (x>621 && x<640 && *val<maxval) *val=*val+1;
}

/***********/
/** even  **/
/***********/

even(i)
int i;
{
return(0xFFFFFFFE&i);
}

/**************************/
/**  get a clicked slot  **/
/**************************/

pickslot(Window,samp)
struct Window *Window;
struct Samp *samp;
{
struct IntuiMessage *message;
int class,code,mousex,mousey;

zap:
if((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   goto zap;
   }
class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
ReplyMsg(message);

if (class==MOUSEBUTTONS) {        /* selecting a new cur */
   if (code!=SELECTDOWN) goto zap;
   mousey=(mousey-11)/8;
   if (mousex>320) mousey+=MAX_SAMPLES/2;
   if (mousey>MAX_SAMPLES || samp->lmem[mousey]==0) {
      msg(Window,"Invalid Selection, try again.");
      goto zap;
      }
   }
return(mousey);
}

/**********************************/
/**  write msg to cmd line       **/
/**********************************/
msg (Window,message)
char *message;
struct Window *Window;
{
int i;

putxy(Window,23,190,message,RED,BLACK);
for (i=23+strlen(message)*8;i<630;i=i+8)
   putxy(Window,i,190," ",BLACK,BLACK);
}

/************************************/
/**  Print   "out of memory" msg    */
/************************************/

nomem(Window)
struct Window *Window;
{
msg(Window,"OUT OF MEMORY");
}

/******************************************/
/**  allocate an audio channel          ***/
/******************************************/

allocate_channel (Window,IOB)
struct Window *Window;
struct IOAudio *IOB;
{
struct MsgPort *port;
int i;
char *x;
static UBYTE allocation_map[] = {3};

x=(char *)IOB;
for (i=0;i<sizeof(struct IOAudio);i++)
   x[i]=(char)0;

if (OpenDevice(AUDIONAME, 0, IOB, 0) != 0)
   msg (Window,"can't open audio device");

/* now allocate a channel */

IOB->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port=(struct MsgPort *)CreatePort("samples", 0)) == 0)
   msg (Window,"can't open port");
IOB->ioa_Request.io_Message.mn_ReplyPort = port;
IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
IOB->ioa_Data = allocation_map;
IOB->ioa_Length = sizeof (allocation_map);

SendIO (IOB);
if (WaitIO (IOB))
   msg (Window,"error in allocation");

}

/****************************************/
/** deallocate an audio channel        **/
/****************************************/

deallocate_channel (IOB)
struct IOAudio *IOB;
{
struct Device *device;
struct MsgPort *port;

if (IOB != 0) {
   device = IOB->ioa_Request.io_Device;
   if (device != 0) {
      IOB->ioa_Request.io_Command = ADCMD_FREE;
      DoIO (IOB);
      CloseDevice (IOB);
   }
   port = IOB->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
}
}

/*************************************************/
/**  Play a sample on an alocated channel       **/
/*************************************************/

tochan (IOB, data, length, rate,cyc)
int rate,length,cyc;
char *data;
struct IOAudio *IOB;
{

IOB->ioa_Request.io_Command = CMD_WRITE;

IOB->ioa_Request.io_Flags = ADIOF_PERVOL;

IOB->ioa_Data = data;
IOB->ioa_Length = (ULONG)length;

IOB->ioa_Period = rate;
IOB->ioa_Volume = 64;

IOB->ioa_Cycles = cyc;

BeginIO (IOB);
}

/*****************************************************/
/* PutName -- put a sample name in the correct place */
/*****************************************************/

PutName(Window,name,location,vid)

struct Window *Window;
char *name;
int location;
int vid;
{
int x;
if (location>=MAX_SAMPLES/2) {
   x=320;
   location=location-MAX_SAMPLES/2;
   }
   else x=3;

if (name[0]==0) {
   for (vid=x;vid<x+305;vid=vid+8)
      putxy(Window,vid,11+location*8," ",BLACK,BLACK);
   return(0);
   }

if (vid==REV_VID)
   putxy(Window,x,11+location*8,name,BLACK,WHITE);
else
   putxy(Window,x,11+location*8,name,WHITE,BLACK);
}

