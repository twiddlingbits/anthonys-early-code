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

main()
{

extern struct Menu Titles[];
int len,memory,samplen,record_delay;
int k,cur,mark,ss,sl,temp;
register int i,j;
int lastp=0;
ULONG position=0;
struct Samp samp;
char *AllocMem(),*p1,*p2,*x;
static char line[80],buf[80];
static char path[80]="";
SHORT mousex,mousey;
USHORT ops,ope,opp;
ULONG class;
USHORT code,menunum,itemnum,subnum;
SHORT oct[MAX_SAMPLES];
struct TextAttr f;
struct TextFont *font;
struct Gadget *gad;
struct NewWindow NewWindow;
struct Window *Window;
struct IntuiMessage *message;
struct Gadget *gadget_s,*gadget_e,*gadget_p;
struct PropInfo *prop_s,*prop_e,*prop_p;
struct IOAudio IOB;

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


printf("Start with %d bytes\n\n",AvailMem(MEMF_CHIP));
/* inilize sample array to zero */

for(i=0;i<MAX_SAMPLES;i++) {
   samp.lmem[i]=0;
   samp.rmem[i]=0;
   samp.length[i]=0;
   samp.name[i]=0;
   samp.rate[i]=0;
   samp.sm[i]=0;
   samp.em[i]=0;
   samp.ctoctave[i]=0;
   samp.oneshot[i]=0;
   samp.repeat[i]=0;
   samp.cycles[i]=0;
   }

ops=0;
ope=0;
opp=0;


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

/* initilize menu and gadget structures */

gadget_s=(struct Gadget *)makegadget();
gadget_e=(struct Gadget *)gadget_s->NextGadget;
gadget_p=(struct Gadget *)gadget_e->NextGadget;
prop_s=(struct PropInfo *)gadget_s->SpecialInfo;
prop_e=(struct PropInfo *)gadget_e->SpecialInfo;
prop_p=(struct PropInfo *)gadget_p->SpecialInfo;


/* IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",INTUITION_REV);
if (IntuitionBase==NULL) {
   printf("Can't open window libraries\n\n");
   exit(10);
   } */

NewWindow.LeftEdge=0;
NewWindow.TopEdge=0;
NewWindow.Width=640;
NewWindow.Height=200;
NewWindow.DetailPen=0;
NewWindow.BlockPen=1;
NewWindow.Title="Perfect Sound   Copyright 1986 SunRize Industries  Ver 1.3 ";
NewWindow.Flags=ACTIVATE|WINDOWCLOSE|
                WINDOWDEPTH|NOCAREREFRESH;
NewWindow.IDCMPFlags=MOUSEMOVE|MENUPICK|MOUSEBUTTONS|CLOSEWINDOW|
                     GADGETUP|RAWKEY;
NewWindow.FirstGadget=gadget_s;
NewWindow.Type=WBENCHSCREEN;
NewWindow.CheckMark=NULL;
NewWindow.Screen=NULL;
NewWindow.BitMap=NULL;

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

RefreshGadgets(gadget_s,Window,NULL);



/* tell intuition about menus */

SetMenuStrip(Window,Titles);


/* allocate audio chanels 1 and 2 */

allocate_channel (Window,&IOB);

/* Print options */

msg(Window,"No Samples Loaded");

memory=frags();    /* find largest amount of free memeory */
cur=0;
mark=0;
record_delay=5;     /* period of 362 is default */


/* initilize samples to not used */

for (i=0;i<MAX_SAMPLES;i++)
   samp.lmem[i]=0;

/* print gadget labels */

putxy(Window,4,GSP+2,"START",WHITE,BLACK);
putxy(Window,4,GEP+2,"END",WHITE,BLACK);
putxy(Window,4,GPP+2,"POS'N",WHITE,BLACK);

/* main loop, wait for a command */


prop_s->HorizPot=0;
prop_e->HorizPot=0;
RefreshGadgets(gadget_s,Window,NULL);
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

if (class==MOUSEMOVE)       /* moving proportional gadget */
   do {
      if (prop_s->HorizPot!=ops) {
         ops=prop_s->HorizPot;
         samp.sm[cur]=(prop_s->HorizPot>>4)*samp.length[cur];
         samp.sm[cur]/=0x0FFF;
         lastp=samp.sm[cur];
         }
      if (prop_e->HorizPot!=ope) {
         ope=prop_e->HorizPot;
         samp.em[cur]=(prop_e->HorizPot>>4)*samp.length[cur];
         samp.em[cur]/=0x0FFF;
         lastp=samp.em[cur];
         }
      if (prop_p->HorizPot!=opp) {
         opp=prop_p->HorizPot;
         position=(prop_p->HorizPot>>4)*samp.length[cur];
         position/=0x0FFF;
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

for (i=0;i<2;i++) {
   FreeMem(bufL[i],BUFSIZE);
   FreeMem(bufR[i],BUFSIZE);
   }

   printf("End with %d bytes\n\n",AvailMem(MEMF_CHIP));
   exit(0);
   }

if (class==MOUSEBUTTONS) {        /* selecting a new cur */
   if (code!=SELECTDOWN) goto ignore;
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
      if (samp.oneshot[cur]!=0) {
      sprintf(line,"OneShotHiSamples %d  RepeatHiSamples %d No. Octaves %d",
         samp.oneshot[cur],samp.repeat[cur],samp.ctoctave[cur]);
      msg(Window,line);
      }
      }
      goto update;
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
   play(Window,&IOB,k,len,&samp,cur);  /* play non-repeat */
   k=k+samp.oneshot[cur];
   while (class!=RAWKEY || code<127) {
      if ((message=(struct IntuiMessage *)GetMsg(Window->UserPort))!=0) {
         class=message->Class;
         code=message->Code;
         ReplyMsg(message);
         }
      play(Window,&IOB,k,powr(2,i)*samp.repeat[cur],&samp,cur);  /* play non-repeat */
      }
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
   case 0: i=save_IFF(Window,cur,&samp,buf);
           break;
   case 1: i=save_samp(Window,cur,&samp,buf);
   }
   }

if (menunum==3 && itemnum==2) {      /* save ... */
   strcpy(buf,path);
   add(buf,samp.name[cur]);
   switch (subnum) {
   case 0: save_IFF(Window,cur,&samp,buf);
           break;
   case 1: save_samp(Window,cur,&samp,buf);
   }
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

   i=rdn(samp.sm[cur]);  /* start deleting including this byte */
   j=rup(samp.em[cur]);

   k=samp.length[cur]-(j-i);
   k=rup(k);

   if (j<samp.length[cur]) 
      movmem(samp.lmem[cur]+j,samp.lmem[cur]+i,samp.length[cur]-j);
    FreeMem(samp.lmem[cur]+k,j-i);

   if (samp.type[cur]==STEREO) {
      if (j<samp.length[cur]) 
         movmem(samp.rmem[cur]+j,samp.rmem[cur]+i,samp.length[cur]-j);
      FreeMem(samp.rmem[cur]+k,j-i);
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

if (menunum==1 && itemnum==1)   /* graph sample */
   graph(Window,&samp,cur);

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
   p1=AllocMem(len,0);
   if (p1==0) {
      nomem(Window);
      goto ignore;
      }
   samp.length[i]=len;
   samp.type[i]=RIGHT;
   samp.rate[i]=samp.rate[oct[0]];
   samp.lmem[i]=p1;
   samp.rmem[i]=p1;
   samp.ctoctave[i]=k;
   samp.em[i]=samp.length[i];
   samp.oneshot[i]=samp.em[oct[0]];
   samp.repeat[i]=samp.length[oct[0]]-samp.em[oct[0]];

   /* move all octaves into new instrument */

   len=samp.length[oct[0]];
printf("len is %d\n",len);
   temp=0;
   for (j=0;j<k;j++) {
      if (samp.length[oct[j]]!=len)
         msg(Window,"Warning, an octave length is not the correct size.");
      movmem(samp.lmem[oct[j]],samp.lmem[i]+temp,len);
      printf("moved %d from samp #%d\n",len,oct[j]);
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
      FreeMem(samp.lmem[i],samplen);
      samp.lmem[i]=0;
      samp.ctoctave[i]=0;
      samp.oneshot[i]=0;
      goto ignore;
      }
   strcpy(samp.name[i],line);

  samp.cycles[i]=getint(Window,2,0,20,"Enter SamplesPerHiCycle",250);

  /* highlight new instrument */

   PutName(Window,samp.name[cur],cur,REG_VID);
   PutName(Window,samp.name[i],i,REV_VID);
   cur=i;
   msg(Window,"Instrument created.");
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
   tochan (IOBlks[2+k],bufR[k],thislen,samp->rate[it]);
   }

if ((unit_num&1)==1) {
   movmem(samp->lmem[it]+off+start,bufL[k],BUFSIZE); /* set 1st buffer up */
   IOBlks[k]->ioa_Request.io_Unit=1;
   tochan (IOBlks[k],bufL[k],thislen,samp->rate[it]);
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
   if (unit_num&1==1) WaitIO (IOBlks[k]);
   if (unit_num&2==2) WaitIO (IOBlks[2+k]);
   }

} while (thislen==BUFSIZE);

if (k==0) k=1;    /* wait for last request to finish */
else k=0;
if (unit_num&1==1) WaitIO (IOBlks[k]);
if (unit_num&2==2) WaitIO (IOBlks[2+k]);

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

tochan (IOB, data, length, rate)
int rate,length;
char *data;
struct IOAudio *IOB;
{

IOB->ioa_Request.io_Command = CMD_WRITE;

IOB->ioa_Request.io_Flags = ADIOF_PERVOL;

IOB->ioa_Data = data;
IOB->ioa_Length = (ULONG)length;

IOB->ioa_Period = rate;
IOB->ioa_Volume = 64;

IOB->ioa_Cycles = 1;

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
