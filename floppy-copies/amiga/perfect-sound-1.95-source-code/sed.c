/********************************/
/**   Perfect Sound (C) 1986   **/
/**    Anthony J. Wood         **/
/**                            **/
/** FILE 2 of 4                **/
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
#include <intuition/intuition.h>
#include <stdio.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <fcntl.h>
#include "graphics/text.h"
#include "df0:sed1.h"

#define BUFSIZE 2*1024        /* playback buffers */

struct GfxBase *GfxBase;
struct IntuitionBase *IntuitionBase;
char *bufL[2],*bufR[2];
WORD diskchange=TRUE;

main()
{

extern struct Menu Titles[];
UBYTE cmap[16*3];
int len,memory,samplen,record_delay;
int i,ss,sl,temp;
register int k,j;
int lastp=0;
int cur=0;
ULONG position=0;
struct Samp samp;
char *AllocMem(),*p1,*p2,*x;
static char line[80],buf[80];
static char path[80]="";
UBYTE *pa=0xBFE001;   /* left mouse button is at this address */
SHORT mousex,mousey;
USHORT ops,ope,opp;
ULONG class;
USHORT code,menunum,itemnum,subnum;
SHORT oct[MAX_SAMPLES];
SHORT autograph;
struct TextAttr f;
struct TextFont *font;
struct Gadget *gad;
struct Window *Window;
struct IntuiMessage *message;
struct Gadget *gadget_s,*gadget_e,*gadget_p;
struct PropInfo *prop_s,*prop_e,*prop_p;
struct IOAudio IOB;
struct Image image;
struct Screen *screen=0;
static struct NewWindow NewWindow = {
   0,
   0,
   640,
   200,
   0,
   1,
   MOUSEMOVE|MENUPICK|MOUSEBUTTONS|CLOSEWINDOW|GADGETUP|RAWKEY|DISKINSERTED,
   ACTIVATE|WINDOWCLOSE|WINDOWDEPTH|NOCAREREFRESH,
   0,0,
   "Perfect Sound   Copyright 1986 SunRize Industries  Ver 1.95 ",
   0,0,
   0,0,0,0,
   WBENCHSCREEN
   };

static USHORT arrowsData[] =
{

0x0000,0x0000,0x0018,0x6000,0x0078,0x7800,
0x01F8,0x7E00,0x07F8,0x7F80,0x1FF8,0x7FE0,
0x07F8,0x7F80,0x01F8,0x7E00,0x0078,0x7800,
0x0018,0x6000,0x0000,0x0000,0x776F,0x726B,
0x696E,0x672E,0x2E2E,0x6E6C,0x792E,0x0A0A,
0x2062,0x6520,0x6672,0x6565,0x6C79,0x2064,
0x6973,0x7472,0x6962,0x7574,0x6564,0x2066,
0x6F72,0x206E,0x6F6E,0x2D70,0x726F,0x6669,
0x740A,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0000,0x0000,0x7C0C,0xF03B,0x3C0C,
0x3BF0,0x3C00,0x0000,0x663B,0x633C,0x0018,
};
static struct Image arrows =
{
0,0,          /* LeftEdge, TopEdge */
30,10,1,      /* Width, Height, Depth */
0,             /* put arrowmem here */
0x1,0,        /* PlanePick, PlaneOnOff */
NULL,        /* Pointer to next Image */
};


/* Open up the Intuition interface -- a window */

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",INTUITION_REV);
if (IntuitionBase==NULL) {
   printf("Can't open window libraries\n\n");
   exit(10);
   }

/* open graphics library */

GfxBase=(struct GfxBase *)OpenLibrary("graphics.library",0);
if (GfxBase==NULL) {
   printf("Can't open graphics library\n\n");
   exit(0);
   }


/*** load the title screen if it exists ***/

if (load_ilbm("ps.title",&image,cmap)!=-1) {
   screen=openscr(image.Width,image.Depth);
   if (screen==0) {
      printf("Can't open screen!\n");
      exit(10);
      }

   for (i=0;i<16*3;i=i+3)
      SetRGB4(&screen->ViewPort,i/3,cmap[i]>>4,cmap[i+1]>>4,cmap[i+2]>>4);
   DrawImage(&screen->RastPort,&image,0,0);
   MoveScreen(screen,0,-199);
   }

if ((arrows.ImageData=AllocMem(sizeof(arrowsData),MEMF_CHIP))==0) {
   printf("Can't allocate gadget memory!\n");
   exit(0);
   }
movmem(&arrowsData[0],arrows.ImageData,sizeof(arrowsData));

/**** allocate "double buffers" for audio device ****/

for (i=0;i<2;i++) {
   if ((bufL[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      printf("Can't allocate buffer memory!\n");
      exit(0);
      }

   if ((bufR[i]=AllocMem(BUFSIZE,MEMF_CHIP))==0) {
      printf("Can't allocate buffer memory!\n");
      exit(0);
      }
   }


/* inilize sample array to zero */

for(i=0;i<MAX_SAMPLES;i++) {
   samp.lmem[i]=0;
   samp.rmem[i]=0;
   samp.length[i]=0;
   samp.rate[i]=0;
   samp.name[i]=0;
   samp.sm[i]=0;
   samp.em[i]=0;
   samp.ctoctave[i]=0;
   samp.oneshot[i]=0;
   samp.repeat[i]=0;
   samp.cycles[i]=0;
   }

ops=-1;
ope=-1;
opp=-1;


/* initilize menu and gadget structures */

gadget_s=(struct Gadget *)makegadget();
gadget_e=(struct Gadget *)gadget_s->NextGadget;
gadget_p=(struct Gadget *)gadget_e->NextGadget;
prop_s=(struct PropInfo *)gadget_s->SpecialInfo;
prop_e=(struct PropInfo *)gadget_e->SpecialInfo;
prop_p=(struct PropInfo *)gadget_p->SpecialInfo;

NewWindow.FirstGadget=gadget_s;


if ((Window=(struct Window *)OpenWindow(&NewWindow))==NULL) {
   printf("Can't Open Window\n\n");                   
   exit(10);
   }


/* Always use TOPAZ_80 */

f.ta_Name="topaz.font";
f.ta_YSize=8;
f.ta_Style=0;
f.ta_Flags=0;
font=(struct TextFont *)OpenFont(&f);
if (font==0) {
   printf("Can't Open font!\n\n");
   exit(0);
   }
SetFont(Window->RPort,font);

/* paint screen black */

SetAPen(Window->RPort,BLACK);
SetBPen(Window->RPort,BLACK);
RectFill(Window->RPort,2,10,637,198);

/* add a white line seperating status from samples */
drawline(Window,2,GPP-4,637,GPP-4,WHITE);
drawline(Window,2,GPP-5,637,GPP-5,WHITE);


/* tell intuition about menus */

SetMenuStrip(Window,Titles);

/* draw arrows on left of gadget */

DrawImage (Window->RPort,&arrows,606,GPP);
DrawImage (Window->RPort,&arrows,606,GSP);
DrawImage (Window->RPort,&arrows,606,GEP);

/* allocate audio chanels 1 and 2 */

allocate_channel (Window,&IOB);

/** load some sample sounds if they are on the disk **/
if (screen!=0) {
   if (load_it(Window,&cur,&samp,"sounds/demo1","bang")!=-1)
   samp.em[cur]=samp.length[cur];
   if (load_it(Window,&cur,&samp,"sounds/demo2","Welcome")!=-1)
      samp.em[cur]=samp.length[cur];
   if (load_it(Window,&cur,&samp,"sounds/demo3","Button")!=-1)
      samp.em[cur]=samp.length[cur];

/**** play the loaded sounds ******/
   if (cur==2) {
      prop_e->HorizPot=0xFFFF;ope=0xFFFF;
      prop_p->HorizPot=0;opp=0;
      prop_s->HorizPot=0;ops=0;
      play(Window,&IOB,0,samp.length[0],&samp,0);
      for (k=124;k<=300;k=k+50) {
         samp.rate[1]=k;
         play(Window,&IOB,1,samp.length[1],&samp,1);
         }
      samp.rate[1]=358;
      play(Window,&IOB,1,samp.length[1],&samp,1);
      for (k=0;k<50000;k++) ;
      play(Window,&IOB,2,samp.length[2],&samp,2);
      }
   while ((*pa&64)==64) ;
   CloseScreen(screen);
   FreeMem(image.ImageData,image.Width/8*image.Height*image.Depth);
   }

RefreshGadgets(gadget_s,Window,NULL);
msg(Window,"Welcome to Perfect Sound.");

memory=frags();    /* find largest amount of free memeory */
record_delay=5;     /* period of 362 is default */
autograph=FALSE;



/* print gadget labels */

putxy(Window,4,GSP+2,"START",WHITE,BLACK);
putxy(Window,4,GEP+2,"END",WHITE,BLACK);
putxy(Window,4,GPP+2,"POS'N",WHITE,BLACK);



goto update;     /* print screen at bottom */

ignore:

SetMenuStrip(Window,Titles);   /* turn on menus */

if((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   goto ignore;
   }
class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
gad=(struct Gadget *)message->IAddress;
ReplyMsg(message);
ClearMenuStrip(Window);

if (class==MOUSEMOVE) {       /* moving proportional gadget */
   do {
      if (prop_s->HorizPot!=ops) {
         ops=prop_s->HorizPot;
         samp.sm[cur]=(prop_s->HorizPot>>7)*samp.length[cur];
         samp.sm[cur]/=0x01FF;
         lastp=samp.sm[cur];
         }
      if (prop_e->HorizPot!=ope) {
         ope=prop_e->HorizPot;
         samp.em[cur]=(prop_e->HorizPot>>7)*samp.length[cur];
         samp.em[cur]/=0x01FF;
         lastp=samp.em[cur];
         }
      if (prop_p->HorizPot!=opp) {
         opp=prop_p->HorizPot;
         position=(prop_p->HorizPot>>7)*samp.length[cur];
         position/=0x01FF;
         lastp=position;
         }
      message=(struct IntuiMessage *)GetMsg(Window->UserPort);
      if (message==0)  break;
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      gad=(struct Gadget *)message->IAddress;
      ReplyMsg(message);
   } while (class==MOUSEMOVE);
   if (autograph==TRUE) goto graphit;
   }

if (class==CLOSEWINDOW) {
   CloseFont(font);
   ClearMenuStrip(Window);
   deallocate_channel (&IOB);
   CloseWindow(Window);
   CloseLibrary(IntuitionBase);
   for(i=0;i<MAX_SAMPLES;i++)
      if (samp.lmem[i]!=0) {
         FreeMem(samp.lmem[i],samp.length[i]);
         FreeMem(samp.name[i],strlen(samp.name[i])+1);
         if (samp.type[i]==STEREO)
            FreeMem(samp.rmem[i],samp.length[i]);
         }

   FreeMem(arrows.ImageData,sizeof(arrowsData));
   for (i=0;i<2;i++) {
      FreeMem(bufL[i],BUFSIZE);
      FreeMem(bufR[i],BUFSIZE);
      }

   exit(0);
   }

if (class==MOUSEBUTTONS) {        /* selecting a new cur */
   if (code!=SELECTDOWN) goto ignore;

   if (mousey<GEP+11 && mousey>GEP) {
      alter(&samp.em[cur],samp.length[cur],mousex);
      lastp=samp.em[cur];
      if (autograph==TRUE) goto graphit;
      goto update;
      }
 
   if (mousey<GEP && mousey>GSP) {
      alter(&samp.sm[cur],samp.length[cur],mousex);
      lastp=samp.sm[cur];
      if (autograph==TRUE) goto graphit;
      goto update;
      }
      
   if (mousey<GPP+11 && mousey>GPP) {
      alter(&position,samp.length[cur],mousex);
      lastp=position;
      if (autograph==TRUE) goto graphit;
      goto update;
      }
   mousey=(mousey-11)/8;
   if (mousex>320) mousey+=MAX_SAMPLES/2;
   if (mousey<MAX_SAMPLES && samp.lmem[mousey]!=0) {
      if (cur==mousey) {
         play(Window,&IOB,0,samp.length[cur],&samp,cur);
         goto ignore;
         }
      PutName(Window,samp.name[cur],cur,REG_VID);
      cur=mousey;
      PutName(Window,samp.name[cur],cur,REV_VID);

      ModifyProp(gadget_s,Window,0,AUTOKNOB|FREEHORIZ,
      samp.sm[cur]*0xFFFF/samp.length[cur],0,0,0xFFFF);

      ModifyProp(gadget_e,Window,0,AUTOKNOB|FREEHORIZ,
      samp.em[cur]*0xFFFF/samp.length[cur],0,0,0xFFFF);

      ModifyProp(gadget_p,Window,0,AUTOKNOB|FREEHORIZ,0,0,0,0xFFFF);

      lastp=0;
      position=0;
      if (samp.ctoctave[cur]>1 || samp.repeat[cur]!=0) {
      sprintf(line,"OneShotHiSamples %d  RepeatHiSamples %d No. Octaves %d",
         samp.oneshot[cur],samp.repeat[cur],samp.ctoctave[cur]);
      msg(Window,line);
      }
      }
      goto update;
   }
if (class==DISKINSERTED) {
   diskchange=TRUE;
   goto ignore;
   }

if (class==RAWKEY) {
   if (code>89 || code<80)  goto ignore;
   if (samp.ctoctave[cur]==0) {
      msg(Window,"This sample is not an instrument.");
      goto ignore;
      }
   if ((i=code-80)>=samp.ctoctave[cur]) {
      msg(Window,"Octave not defined.");
      goto ignore;
      }
   k=(powr(2,i)-1)*(samp.oneshot[cur]+samp.repeat[cur]);
   len=powr(2,i)*samp.oneshot[cur];
   if (even(k)!=k) {
      msg(Window,"Warning: Octave address not on a word boundry!");
      k=even(k);
      }
   if (even(len)!=len) {
      msg(Window,"Warning: oneshot length not even!");
      len=even(len);
      }
   IOB.ioa_Request.io_Unit=1;
   tochan (&IOB,samp.lmem[cur]+k,len,samp.rate[cur],1);
   k=k+len;
   j=samp.repeat[cur];
   if (even(j)!=j) {
      msg(Window,"Warning: repeat length not even!");
      j=even(j);
      }
   if (j!=0)
      tochan (&IOB, samp.lmem[cur]+k,powr(2,i)*j,samp.rate[cur],0);
   while (class!=RAWKEY || code<127) {
      if((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0)
         Wait(1<<Window->UserPort->mp_SigBit);
      else {
         class=message->Class;
         code=message->Code;
         ReplyMsg(message);
         }
      }
   IOB.ioa_Request.io_Unit=3;
   IOB.ioa_Request.io_Command=CMD_RESET;
   DoIO (&IOB);
   goto ignore;
   }

if (class==MENUPICK && code==MENUNULL) {
    if (mousey<GPP-5) goto ignore;       /* play only if near gadget */
    play(Window,&IOB,0,lastp,&samp,cur);
    goto ignore;
    }


menunum=MENUNUM(code);
itemnum=ITEMNUM(code);
subnum=SUBNUM(code);

if (menunum==2 && itemnum==2) {            /* monitor the digitizer */
   msg(Window,"Press the left mouse button to exit.");
   switch (subnum) {

   case 0: listen(STEREO);
           break;
   case 1: listen(RIGHT);
           break;
   case 2: listen(LEFT);
           }
   msg(Window,"");
   goto ignore;
   }

if (menunum==2 && itemnum==3)  {  /* RECORD */
   if ((i=getslot(Window,&samp))==-1) goto ignore;
   p1=(char *)1;p2=(char *)1;
   Forbid();
   memory=frags();
   if (memory<8000) {
      Permit();
      nomem(Window);
      goto ignore;
      }
   if (subnum==0) {      /* stereo */
      memory=memory/2;
      p1=AllocMem(memory,0);
      if (p1!=0) p2=AllocMem(memory,0);
      if (p2==0) FreeMem(p1,memory);
      }
   else  
      p1=AllocMem(memory,0);
   Permit();
   if (p1==0 || p2==0) {
      nomem(Window);
      goto ignore;
      }
   switch (subnum) {
      case 0: k=STEREO;
              samp.lmem[i]=p1;
              samp.rmem[i]=p2;
              break;
      case 1: k=RIGHT;
              samp.rmem[i]=p1;
              samp.lmem[i]=p1; /* use only to find free sample elsewhere */
              break;
      case 2: k=LEFT;
              samp.lmem[i]=p1;
              samp.rmem[i]=p1;    /* only for compatibility with prev.*/
              break;
      default: msg(Window,"BUG... code 01");
              goto ignore;
      }
   samp.type[i]=k;
   msg(Window,"Monitoring... Press left mouse button to start recording.");
   listen(k);   /* listen before we start recording */

   samplen=digitze (Window,p1,p2,record_delay,memory,k);
   if (samplen==0) {
      msg(Window,"BUG... code 02");
      FreeMem(samp.lmem[i],memory);
      if (k==STEREO) FreeMem(samp.rmem[i],memory);
      goto ignore;
      }
   if (samplen==memory)
      msg(Window,"Memory filled.");
   else {
      FreeMem(rup(p1+samplen),rdn(memory-samplen));
      sprintf(line,"%d bytes used in this recording.",samplen);
      if (k==STEREO) {
         FreeMem(rup(p2+samplen),rdn(memory-samplen));
         sprintf(line,"%d bytes used in this recording.",samplen*2);
         }
      msg(Window,line);
      }
   samp.length[i]=samplen;
   samp.rate[i]=137+45*record_delay;
   if (k==STEREO) samp.rate[i]=samp.rate[i]*5/6;
   line[0]=0;
   getstr(Window,line,"Enter a label for this sample:",400);
   if (line[0]==0) strcpy(line,"A Nameless Sound");
   samp.name[i]=AllocMem(strlen(line)+1,0);
   if (samp.name[i]==0) {
      nomem(Window);
      FreeMem(samp.lmem[i],samplen);
      samp.lmem[i]=0;
      if (k==STEREO) FreeMem(samp.rmem[i],samplen);
      goto ignore;
      }
   strcpy(samp.name[i],line);    /* move filename to premenant place */
   if (samp.lmem[cur]!=0)
     PutName(Window,samp.name[cur],cur,REG_VID);
   PutName(Window,samp.name[i],i,REV_VID);
   cur=i;
   msg(Window,"");     /* clear input line */
   samp.sm[cur]=0;
   samp.em[cur]=samp.length[cur];
   position=0;
   lastp=0;
   prop_e->HorizPot=0xFFFF;
   prop_p->HorizPot=0;
   prop_s->HorizPot=0;
   RefreshGadgets(gadget_s,Window,NULL);
   }

if (menunum==2 && itemnum==0) {    /* change default recording speed */
   i=getint(Window,137+45*record_delay,124,999,
            "Enter a new recording period:",440);
   record_delay=(i-137)/45;
   sprintf(line,"%d is as close as I can get.",137+45*record_delay);
   msg(Window,line);
   }

  /* show numeric values of input */
if (menunum==2 && itemnum==1) {
   level(Window);
   goto ignore;
   }

if (menunum==3 && itemnum==0) {                   /* load a sample */
   if (load_samp(Window,&cur,&samp,path)==-1)
      goto ignore;
   samp.sm[cur]=0;
   samp.em[cur]=samp.length[cur];
   position=0;
   lastp=0;
   prop_e->HorizPot=0xFFFF;ope=0xFFFF;
   prop_p->HorizPot=0;opp=0;
   prop_s->HorizPot=0;ops=0;
   RefreshGadgets(gadget_s,Window,NULL);
   }

if (menunum==3 && itemnum==1) {      /* save AS ... */
   strcpy(line,samp.name[cur]);
   if (getfile(Window,line,path,"SAVE","Save a Sample")==-1) goto ignore;
   strcpy(buf,path);
   add(buf,line);   /* appends a file or directory to a path */
   switch (subnum) {
   case 0: save_IFF(Window,cur,&samp,buf,FALSE);
           break;
   case 1: save_samp(Window,cur,&samp,buf);
           break;
   case 2: save_IFF(Window,cur,&samp,buf,TRUE);
   }
   diskchange=TRUE;
   goto ignore;
   }

if (menunum==3 && itemnum==2) {      /* save ... */
   strcpy(buf,path);
   add(buf,samp.name[cur]);
   switch (subnum) {
   case 0: save_IFF(Window,cur,&samp,buf,FALSE);
           break;
   case 1: save_samp(Window,cur,&samp,buf);
           break;
   case 2: save_IFF(Window,cur,&samp,buf,TRUE);
   }
   diskchange=TRUE;
   goto ignore;
   }

           /*play entire sample */

if (class==GADGETUP && gad->TopEdge==GPW) {
   play(Window,&IOB,0,samp.length[cur],&samp,cur);
   goto ignore;
   }

            /* play range  */

if (class==GADGETUP && gad->TopEdge==GPR) {
   play(Window,&IOB,samp.sm[cur],samp.em[cur]-samp.sm[cur],&samp,cur);
   goto ignore;
   } 


if (menunum==0 && itemnum==0) {     /* delete a range */
   if (samp.em[cur]<=samp.sm[cur]) {
      msg(Window,"Markers not set.");
      goto ignore;
      }
   if (samp.em[cur]>=samp.length[cur] && samp.sm[cur]==0) goto whole;
   msg(Window,"Working...");

   i=samp.sm[cur];  /* start deleting including this byte */
   j=samp.em[cur];

   k=rup(samp.length[cur]-(j-i));

   if (j<samp.length[cur])
      movmem(samp.lmem[cur]+j,samp.lmem[cur]+i,samp.length[cur]-j);

    if (k<rup(samp.length[cur]))
       FreeMem(samp.lmem[cur]+k,rup(j-i));

   if (samp.type[cur]==STEREO) {
      if (j<samp.length[cur]) 
         movmem(samp.rmem[cur]+j,samp.rmem[cur]+i,samp.length[cur]-j);
      if (k<rup(samp.length[cur]))
         FreeMem(samp.rmem[cur]+k,rup(j-i));
      }


  samp.length[cur]=samp.length[cur]-(j-i);
  msg(Window,"");
  samp.sm[cur]=0;
  samp.em[cur]=samp.length[cur];
  prop_s->HorizPot=0;
  prop_e->HorizPot=0xFFFF;
  prop_p->HorizPot=0;
  lastp=0;
  position=0;
  RefreshGadgets(gadget_s,Window,NULL);
  }

if (menunum==0 && itemnum==1) {    /* delete entire sample */

whole:         /* jump here from delete range if entire sample */
   if (samp.lmem[cur]==0) goto ignore;
   FreeMem(samp.lmem[cur],samp.length[cur]);
   if (samp.type[cur]==STEREO)
      FreeMem(samp.rmem[cur],samp.length[cur]);
   FreeMem(samp.name[cur],strlen(samp.name[cur])+1);
   samp.lmem[cur]=0;
   samp.rmem[cur]=0;
   samp.length[cur]=0;
   samp.name[cur]=0;
   samp.repeat[cur]=0;
   samp.oneshot[cur]=0;
   samp.cycles[cur]=0;
   samp.ctoctave[cur]=0;
   samp.sm[cur]=0;
   samp.em[cur]=0;
   i=0;
   while(samp.lmem[i]==0 && i<MAX_SAMPLES) i++;
   PutName(Window,"",cur,REG_VID);
   if (i!=MAX_SAMPLES) {
      cur=i;
      PutName(Window,samp.name[cur],cur,REV_VID);
      prop_s->HorizPot=samp.sm[cur]*(0xFFFF/samp.length[cur]);
      prop_e->HorizPot=samp.em[cur]*(0xFFFF/samp.length[cur]);
      prop_p->HorizPot=0;
      RefreshGadgets(gadget_s,Window,NULL);
      lastp=0;
      position=0;
      }
   else
      cur=0;
   }

if (menunum==0 && itemnum==2) {      /* insert here */
   msg(Window,"Please click Slot that contains the marked range.");
   i=pickslot(Window,&samp);
   move(Window,&samp,samp.sm[i],samp.em[i],position,i,cur);
   msg(Window,"Range inserted.");
   }

if (menunum==1 && itemnum==0) {        /* flip cur sample */
   if (samp.lmem[cur]==0) goto ignore;
   msg(Window,"Flipping...");

   flip(samp.lmem[cur],samp.length[cur]);
   if (samp.type[cur]==STEREO) {
      flip(samp.rmem[cur],samp.length[cur]);
   }
   msg(Window,"");
   goto ignore;
   }

if (menunum==1 && itemnum==1) {  /* graph sample */
   graphit:
   graph(Window,&samp,cur);
   sprintf(line,"First sample graphed: %d  Last sample graphed: %d",
           *(samp.lmem[cur]+samp.sm[cur]),*(samp.lmem[cur]+samp.em[cur]-1));
   msg(Window,line);
   goto update;
   }

if (menunum==1 && itemnum==2) {   /* create a new instrument */
   if (samp.lmem[cur]==0) goto ignore;

/* create list of octaves in assending order */
   k=0;
   for (i=0;i<MAX_SAMPLES;i++)
      if (samp.lmem[i]!=0)
         oct[k++]=i;

/* sort */

   for (i=0;i<k-1;i++)
      for (j=i+1;j<k;j++)
         if (samp.length[oct[i]]>samp.length[oct[j]]) {
            temp=oct[i];
            oct[i]=oct[j];
            oct[j]=temp;
            }

   if ((i=getslot(Window,&samp))==-1) goto ignore;
   len=(powr(2,k)-1)*samp.length[oct[0]];
   p1=AllocMem(len,MEMF_CHIP);
   if (p1==0) {
      msg(Window,"Not Enough Chip Memory.");
      goto ignore;
      }
   samp.length[i]=len;
   samp.type[i]=RIGHT;
   samp.rate[i]=samp.rate[oct[0]];
   samp.lmem[i]=p1;
   samp.rmem[i]=p1;
   samp.ctoctave[i]=k;
   samp.em[i]=samp.length[i];
   samp.oneshot[i]=samp.sm[oct[0]];
   samp.repeat[i]=samp.length[oct[0]]-samp.sm[oct[0]];

   /* move all octaves into new instrument */

   len=samp.length[oct[0]];
   temp=0;
   for (j=0;j<k;j++) {
      if (samp.length[oct[j]]!=len)
         msg(Window,"Warning, an octave length is not the correct size.");
      movmem(samp.lmem[oct[j]],samp.lmem[i]+temp,len);
      temp=temp+len;
      len=len*2;
      }


   /* get a name for this sample */

   line[0]=0;
   getstr(Window,line,"Enter a name for this instrument:",400);
   if (line[0]==0) strcpy(line,"Lazy typer");
   samp.name[i]=AllocMem(strlen(line)+1,0);
   if (samp.name[i]==0) {
      nomem(Window);
      FreeMem(samp.lmem[i],samp.length[i]);
      samp.lmem[i]=0;
      samp.ctoctave[i]=0;
      samp.oneshot[i]=0;
      goto ignore;
      }
   strcpy(samp.name[i],line);

  samp.cycles[i]=getint(Window,8,0,256,"Enter SamplesPerHiCycle",256);

 /* sampperhicycle should be power of 2 for most programs */
   j=FALSE;
   for (k=0;k<16;k++)
      if (powr(2,k)==samp.cycles[i]) j=TRUE;
   if (j==FALSE)
       msg(Window,"Warning: SamplesPerHiCycle may need to be a power of 2.");
   else
      msg(Window,"Instrument created.");
  /* highlight new instrument */

   PutName(Window,samp.name[cur],cur,REG_VID);
   PutName(Window,samp.name[i],i,REV_VID);
   cur=i;
   }

/* cut period in half by eliminating samples */

if (menunum==1 && itemnum==3) {
   if (samp.lmem[cur]==0) goto ignore;
   if ((i=getslot(Window,&samp))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a name for the new sample:",400);
   if (line[0]==0) strcpy(line,"*2");
   samp.name[i]=AllocMem(strlen(line)+1,0);
   if (samp.name[i]==0) {
      nomem(Window);
      goto ignore;
      }
   msg(Window,"Working...");
   strcpy(samp.name[i],line);    /* move filename to permenant place */
   samp.length[i]=samp.length[cur]/2;
   samp.lmem[i]=AllocMem(samp.length[i],0);
   if (samp.lmem[i]==0) {
      nomem(Window);
      samp.name[i]=0;
      samp.length[i]=0;
      goto ignore;
      }
   samp.type[i]=samp.type[cur];
   samp.rate[i]=samp.rate[cur];
   samp.em[i]=samp.length[i];
   samp.rmem[i]=samp.lmem[i];
   j=0;
   for (k=0;k<samp.length[cur];k=k+2)
      *(samp.lmem[i]+j++)=*(samp.lmem[cur]+k);

   PutName(Window,samp.name[cur],cur,REG_VID);
   PutName(Window,samp.name[i],i,REV_VID);
   cur=i;
   msg(Window,"");
   }
   

/* double period by interpolating */

if (menunum==1 && itemnum==4) {
   if (samp.lmem[cur]==0) goto ignore;
   if ((i=getslot(Window,&samp))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a name for the new sample:",400);
   if (line[0]==0) strcpy(line,"/2");
   samp.name[i]=AllocMem(strlen(line)+1,0);
   if (samp.name[i]==0) {
      nomem(Window);
      goto ignore;
      }
   msg(Window,"Working...");
   strcpy(samp.name[i],line);    /* move filename to permenant place */
   samp.length[i]=samp.length[cur]*2;
   samp.lmem[i]=AllocMem(samp.length[i],0);
   if (samp.lmem[i]==0) {
      nomem(Window);
      samp.name[i]=0;
      samp.length[i]=0;
      goto ignore;
      }
   samp.type[i]=samp.type[cur];
   samp.rate[i]=samp.rate[cur];
   samp.em[i]=samp.length[i];
   samp.rmem[i]=samp.lmem[i];
   j=0;
   for (k=0;k<samp.length[cur];k++) {
      *(samp.lmem[i]+j++)=*(samp.lmem[cur]+k);
      *(samp.lmem[i]+j++)=(*(samp.lmem[cur]+k)+*(samp.lmem[cur]+k+1))/2;
      }
   *(samp.lmem[i]+j-1)=*(samp.lmem[cur]+k-1);    /* last interp. is wrong */

   PutName(Window,samp.name[cur],cur,REG_VID);
   PutName(Window,samp.name[i],i,REV_VID);
   cur=i;
   msg(Window,"");
   }

if (menunum==1 && itemnum==5) {
   if (subnum==0) autograph=TRUE;
   if (subnum==1) autograph=FALSE;
   }

if (menunum==0 && itemnum==3) {      /* enter new playback rate */
   if (samp.lmem[cur]==0) goto ignore;
   samp.rate[cur]=getint(Window,samp.rate[cur],124,700,
                  "Enter a new playback period:",440);
   }

/* copy range to a new slot */

if (menunum==0 && itemnum==4) {
   if (samp.lmem[cur]==0) goto ignore;
   len=samp.em[cur]-samp.sm[cur];
   if (len<=1 || len>8000000) {
      msg(Window,"Markers Not Set");
      goto ignore;
      }
   if ((i=getslot(Window,&samp))==-1) goto ignore;
   line[0]=0;
   getstr(Window,line,"Enter a label for the new sample",400);
   if (line[0]==0) strcpy(line,"A Nameless Sound");
   samp.name[i]=AllocMem(strlen(line)+1,0);
   if (samp.name[i]==0) {
      nomem(Window);
      goto ignore;
      }
   strcpy(samp.name[i],line);
   if ((samp.lmem[i]=AllocMem(len,0))==0) {
      nomem(Window);
      FreeMem(samp.name[i],strlen(samp.name[i])+1);
      goto ignore;
      }

   samp.rmem[i]=samp.lmem[i];

   if (samp.type[cur]==STEREO)
      if ((samp.rmem[i]=AllocMem(len,0))==0) {
         FreeMem(samp.lmem[i],len);
         FreeMem(samp.name[i],strlen(samp.name[i])+1);
         nomem(Window);
         samp.lmem[i]=0;
         samp.name[i]=0;
         goto ignore;
         }

   msg(Window,"Working...");

   movmem(samp.lmem[cur]+samp.sm[cur],samp.lmem[i],len);
   if (samp.type[cur]==STEREO)
      movmem(samp.rmem[cur]+samp.sm[cur],samp.rmem[i],len);
   msg(Window,"");
   samp.length[i]=len;
   samp.rate[i]=samp.rate[cur];
   samp.type[i]=samp.type[cur];
   PutName(Window,samp.name[cur],cur,REG_VID);
   cur=i;
   PutName(Window,samp.name[cur],cur,REV_VID);
   samp.em[cur]=samp.length[cur];
   samp.sm[cur]=0;
   prop_e->HorizPot=0xFFFF;ope=0xFFFF;
   prop_p->HorizPot=0;opp=0;
   prop_s->HorizPot=0;ops=0;
   RefreshGadgets(gadget_s,Window,NULL);
   }

/* append a slot to another slot */

if (menunum==0 && itemnum==5) {
   if (samp.lmem[cur]==0) goto ignore;
   msg(Window,"Click slot to append");
   i=pickslot(Window,&samp);
   msg(Window,"Click slot to append this to...");
   k=pickslot(Window,&samp);
   len=samp.length[i]+samp.length[k];
   move(Window,&samp,0,samp.length[i],samp.length[k],i,k);
   }
   
if (menunum==4 && itemnum==0) {    /* about */
   aboutps(Window);
   goto ignore;
   }

/* Break up a stereo into two new slots */

if (menunum==0 && itemnum==7) {
   if (samp.lmem[cur]==0) goto ignore;
   if (samp.type[cur]!=STEREO) {
      msg(Window,"Sample must be stereo.");
      goto ignore;
      }
   if ((i=getslot(Window,&samp))==-1) goto ignore;
   samp.name[i]=AllocMem(strlen(samp.name[cur])+2,0);
   if (samp.name[i]==0) {
      nomem(Window);
      goto ignore;
      }
   *samp.name[i]='L';
   strcpy(samp.name[i]+1,samp.name[cur]);
   samp.lmem[i]=samp.lmem[cur];
   samp.rmem[i]=samp.lmem[cur];
   samp.type[i]=LEFT;
   samp.length[i]=samp.length[cur];
   samp.rate[i]=samp.rate[cur];
   samp.sm[i]=0;
   samp.em[i]=samp.length[i];
   x=AllocMem(strlen(samp.name[cur])+2,0);
   if (x==0) {
      nomem(Window);
      FreeMem(samp.name[i],strlen(samp.name[i]+1));
      samp.lmem[i]=0;
      samp.rmem[i]=0;
      samp.length[i]=0;
      goto ignore;
      }
   *x='R';
   strcpy(x+1,samp.name[cur]);
   FreeMem(samp.name[cur],strlen(samp.name[cur])+1);
   samp.name[cur]=x;
   samp.lmem[cur]=samp.rmem[cur];
   samp.type[cur]=RIGHT;
   PutName(Window,samp.name[cur],cur,REV_VID);
   PutName(Window,samp.name[i],i,REG_VID);
   msg(Window,"Created seperate left and right channels.");
   }
   
/* create stereo from two mono samples */

if (menunum==0 && itemnum==6) {
   if (samp.lmem[cur]==0) goto ignore;
   msg(Window,"Click slot for left channel.");
   i=pickslot(Window,&samp);
   if (samp.type[i]==STEREO) {
      msg(Window,"Sample must be mono.");
      goto ignore;
      }
   msg(Window,"Click slot for right channel.");
   k=pickslot(Window,&samp);
   if (samp.type[k]==STEREO) {
      msg(Window,"Sample must be mono.");
      goto ignore;
      }
   if (k==i) {
      msg(Window,"Samples must be in different slots.");
      goto ignore;
      }
   if (samp.length[i]<samp.length[k]) {ss=i;sl=k;}
   else {ss=k;sl=i;}
   if (samp.length[i]!=samp.length[k])
   FreeMem(samp.lmem[sl]+samp.length[ss],samp.length[sl]-samp.length[ss]);
   FreeMem(samp.name[sl],strlen(samp.name[sl])+1);
   samp.lmem[ss]=samp.lmem[i];
   samp.rmem[ss]=samp.rmem[k];
   samp.type[ss]=STEREO;
   PutName(Window,"",sl,REG_VID);
   cur=ss;
   samp.lmem[sl]=0;
   samp.rmem[sl]=0;
   samp.length[sl]=0;
   samp.name[sl]=0;
   PutName(Window,samp.name[cur],cur,REV_VID);
   msg(Window,"Current sample is now stereo.");
   }


/** print Status information at the bottom of the screen **/

update:
if (samp.lmem[cur]!=0 && samp.em[cur]>samp.length[cur])
     samp.em[cur]=samp.length[cur];


memory=frags();
sprintf(line,"Sample length: %7.7d  playback rate: %4.4d",
        samp.length[cur],samp.rate[cur]);
putxy(Window,23,164,line,WHITE,BLACK);

sprintf(line,"Current position: %7.7d Start/End mark: %7.7d %7.7d",
        position,samp.sm[cur],samp.em[cur]);
putxy(Window,23,172,line,WHITE,BLACK);

sprintf(line,"Memory for recording: %7.7d record rate: %4.4d",
        memory,137+record_delay*45);
putxy(Window,23,180,line,WHITE,BLACK);
goto ignore;
}


/***********************************/
/* Play a digitized sample         */
/***********************************/

play(Window,IOB,start,len,samp,it)
struct Samp *samp;
struct IOAudio *IOB;
struct Window *Window;
int len,start,it;

{
struct IOAudio *IOBlks[4];
struct MsgPort *port;
int k,i,firstloop;
int unit_num=0;
int off,thislen;
if (len==0) return(0);
if (len<0) {
   msg(Window,"Invalid Range... Check Your Markers.");
   return(0);
   }

if (samp->type[it]==LEFT || samp->type[it]==STEREO) unit_num|=1;
if (samp->type[it]==RIGHT || samp->type[it]==STEREO) unit_num|=2;

/** allocate memory for Audio IOB's **/

for (k=0;k<4;k++) {
   if ((IOBlks[k]=AllocMem(sizeof(struct IOAudio),0))==0) {
      nomem(Window);
      while(k-->0)
         FreeMem(IOBlks[k],sizeof(struct IOAudio));
      return(0);
      }
   if ((port=(struct MsgPort *)CreatePort("Rize-samps", 0)) == 0) {
      msg (Window,"can't open port");
      return(0);
      }
   movmem(IOB,IOBlks[k],sizeof(struct IOAudio));
   IOBlks[k]->ioa_Request.io_Message.mn_ReplyPort = port;
   }

IOB->ioa_Request.io_Unit=3;
IOB->ioa_Request.io_Command=CMD_RESET;
DoIO (IOB);


k=0;      /* indicates which of the two buffers to use */
off=0;    /* our position in the sample relative to start */
thislen=BUFSIZE;   /* length of current chuck of sample we are playing */


IOB->ioa_Request.io_Command=CMD_STOP;
DoIO(IOB);

firstloop=TRUE;


do {

if (off+BUFSIZE>len) thislen=len-off;
if (thislen==0) thislen=2;

if ((unit_num&2)==2) {
   movmem(samp->rmem[it]+off+start,bufR[k],BUFSIZE);
   IOBlks[2+k]->ioa_Request.io_Unit=2;
   tochan (IOBlks[2+k],bufR[k],thislen,samp->rate[it],1);
   }

if ((unit_num&1)==1) {
   movmem(samp->lmem[it]+off+start,bufL[k],BUFSIZE); /* set 1st buffer up */
   IOBlks[k]->ioa_Request.io_Unit=1;
   tochan (IOBlks[k],bufL[k],thislen,samp->rate[it],1);
   }

if (k==0) k=1; else k=0;

off=off+BUFSIZE;

if (firstloop==TRUE) {
   IOB->ioa_Request.io_Unit=unit_num;
   IOB->ioa_Request.io_Command=CMD_START;
   DoIO(IOB);
   firstloop=FALSE;
   }

else {
   if ((unit_num&1)==1) WaitIO (IOBlks[k]);
   if ((unit_num&2)==2) WaitIO (IOBlks[2+k]);
   }

} while (thislen==BUFSIZE);

if (k==0) k=1;    /* wait for last request to finish */
else k=0;
if ((unit_num&1)==1) WaitIO (IOBlks[k]);
if ((unit_num&2)==2) WaitIO (IOBlks[2+k]);

IOB->ioa_Request.io_Unit=3;
IOB->ioa_Request.io_Command=CMD_RESET;
DoIO (IOB);

/* free iob's memory */

   for (i=0;i<4;i++) {
      port = IOBlks[i]->ioa_Request.io_Message.mn_ReplyPort;
      if (port != 0)
         DeletePort (port);
      FreeMem(IOBlks[i],sizeof(struct IOAudio));   /* finished with this IOB */
      }
}
