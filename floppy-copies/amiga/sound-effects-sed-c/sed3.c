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
static SHORT bd[4];
static struct Border b={
   0,0,
   0,0,JAM1,
   2,&bd[0],NULL};
bd[0]=fx;
bd[1]=fy;
bd[2]=tx;
bd[3]=ty;
b.FrontPen=pen;
DrawBorder (Window->RPort,&b,0,0);
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
int y,k;
register int val,sum,i,x;

if (samp->lmem[cur]==0) return (0);

/* 1st clear screen memory in this area */
SetAPen(Window->RPort,BLACK);
SetBPen(Window->RPort,BLACK);
RectFill(Window->RPort,2,GBOTY-255/SCALE,637,GBOTY);
drawline(Window,2,GBOTY-255/SCALE-1,637,GBOTY-255/SCALE-1,WHITE);
drawline(Window,2,GBOTY-255/SCALE-2,637,GBOTY-255/SCALE-2,WHITE);

stp=(samp->em[cur]-samp->sm[cur])/580;
x=50;    /* line up with prop gadgets */
y=GBOTY-((int)(*samp->lmem[cur])+(int)128)/SCALE;
Move(Window->RPort,x,y);
SetAPen(Window->RPort,WHITE);
if (stp!=0)
   for (i=0;i<580;i++) {
      sum=0;
      for (k=0;k<stp;k++)
         sum=sum+*(samp->lmem[cur]+k+i*stp+samp->sm[cur]);
      val=sum/stp;
      val=val+128;
      val=GBOTY-val/SCALE;
      Draw(Window->RPort,++x,val);
      }

else {
   val=samp->sm[cur];
   stp=580/(samp->em[cur]-samp->sm[cur]);
   if (stp==0) stp=1;
   for (i=0;i<580;i=i+stp)
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
gadget_s.Width=-60;
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

char *s,*e;
char temp;
register int k;

s=addrs;
e=addrs+len-1;
k=len/2;
printf("s,e,k,len %d %d %d %d\n",s,e,k,len);

while (k-->0) {
   temp=*s;
   *s=*e;
   *e=temp;
   s++;e--;
   }
printf("%d k\n",k);
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
      msg(Window,"No Action Taken");
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
         return(0);
         }

   movmem(samp->lmem[to],x,position);
   movmem(samp->lmem[from]+start,x+position,end-start);
   movmem(samp->lmem[to]+position,x+position+end-start,
          samp->length[to]-position);
   FreeMem(samp->lmem[to],samp->length[to]);
   samp->lmem[to]=x;
   samp->rmem[to]=x;

   if (samp->type[to]==STEREO) {
      movmem(samp->rmem[to],y,position);
      movmem(samp->rmem[from]+start,y+position,end-start);
      movmem(samp->rmem[to]+position,y+position+end-start,
          samp->length[to]-position);
      FreeMem(samp->rmem[to],samp->length[to]);
      samp->rmem[to]=y;
      }

   samp->length[to]=len;
   msg(Window,"");
   }
