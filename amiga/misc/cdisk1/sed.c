/* Sound Editor */

#include <exec/types.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <exec/exec.h>
#include <devices/audio.h>
#include <fcntl.h>

#define INTUITION_REV 29
#define MODE_OLDFILE 1005
#define MODE_NEWFILE 1006
#define MAX_SAMPLES 20
#define REV_VID 1
#define REG_VID 0
#define SOUNDPREC 20
#define NUM_ITEMS 16
#define RIGHT 2
#define LEFT 4

struct Sample {
   int size;
   UBYTE *waveform;
};

struct Channel {
   struct IOAudio *IOB;
   struct Sample samp;
};

struct Samp {
   char *name[MAX_SAMPLES];
   char *addrs[MAX_SAMPLES];
   int  length[MAX_SAMPLES];
   int  rate[MAX_SAMPLES];
   };

struct IntuitionBase *IntuitionBase;

char clrscn[]={0x1b,'H',0x1b,'J'};
char clrline[]={0x0d,0x9b,0x4b};
char lline[]={0x9b,'2','3',0x3b,'0',0x48};
char home[]={0x1b,'H'};
char info[]={0x9b,'2','1',0x3b,'0',0x48};
char cursor_off[]={0x9b,0x30,0x20,0x70};
char cursor_on[]={0x9b,0x20,0x70};
char up2[]={0x9b,'2',0x41};
char down1={10};


main()
{

int len,memory,samplen,fp;
int i,k,current,position,mark,start_mark,end_mark;
struct Samp samp;
char *AllocMem(),*x,*point;
char line[80];
SHORT mousex,mousey;
ULONG class;
USHORT code,menunum,itemnum;
struct NewWindow NewWindow;
struct Window *Window;
struct IOStdReq *conWriteMsg;
struct IOStdReq *conReadMsg;
struct MsgPort *conWritePort,*conReadPort;
struct MenuItem item[NUM_ITEMS];
struct IntuiText text[NUM_ITEMS];
struct IntuiMessage *message;
struct Gadget gadget;
struct PropInfo prop;
struct Image image;

/********************/
/* set up the menus */
/********************/

struct Menu menu[4];

for(i=0;i<4;i++) {
   menu[i].NextMenu=&menu[i+1];
   menu[i].LeftEdge=5+i*95;
   menu[i].TopEdge=0;
   menu[i].Width=80;
   menu[i].Height=10;
   menu[i].Flags=MENUENABLED;
   }
menu[3].NextMenu=NULL;
menu[0].MenuName="Edit";
menu[0].FirstItem=&item[10];
menu[1].MenuName="Playback";
menu[1].FirstItem=&item[8];
menu[2].MenuName="Digitize";
menu[2].FirstItem=&item[3];
menu[3].MenuName="File";
menu[3].FirstItem=&item[0];

/* set up menu items */
k=0;
for (i=0;i<NUM_ITEMS;i++) {
item[i].NextItem=&item[i+1];
item[i].LeftEdge=0;
item[i].TopEdge=11*k++;
item[i].Width=160;
item[i].Height=11;
item[i].Flags=ITEMTEXT|ITEMENABLED|HIGHCOMP;
item[i].MutualExclude=0;
item[i].ItemFill=(APTR)&text[i];
item[i].SelectFill=0;
item[i].Command=0;
item[i].SubItem=0;
item[i].NextSelect=0;
text[i].FrontPen=0;
text[i].BackPen=1;
text[i].DrawMode=JAM2;
text[i].LeftEdge=0;
text[i].TopEdge=1;
text[i].ITextFont=0;
text[i].NextText=0;
if (i==2 || i==7 || i==9) k=0;       /* make each new item start at 0 */
}

item[2].NextItem=0;
item[7].NextItem=0;
item[9].NextItem=0;
item[15].NextItem=0;
k=0;
text[k++].IText=(UBYTE *)"Load a Sample";
text[k++].IText=(UBYTE *)"Save a Sample";
text[k++].IText=(UBYTE *)"Load a CLI";
text[k++].IText=(UBYTE *)"Monitor Right Channel";
text[k++].IText=(UBYTE *)"Monitor Left Channel";
text[k++].IText=(UBYTE *)"Record in Stereo";
text[k++].IText=(UBYTE *)"Record Right Channel";
text[k++].IText=(UBYTE *)"Record Left Chennel";
text[k++].IText=(UBYTE *)"Play current sample";
text[k++].IText=(UBYTE *)"Play marked range";
text[k++].IText=(UBYTE *)"Delete marked range";
text[k++].IText=(UBYTE *)"Delete Current Sample";
text[k++].IText=(UBYTE *)"Insert marked range";
text[k++].IText=(UBYTE *)"Flip current sample";
text[k++].IText=(UBYTE *)"Set 1st marker";
text[k++].IText=(UBYTE *)"Set 2nd marker";

/* define a propertional gadget to use for markers */

gadget.NextGadget=NULL;
gadget.LeftEdge=10;
gadget.TopEdge=155;
gadget.Width=-20;
gadget.Height=10;
gadget.Flags=GADGHNONE|GRELWIDTH;
gadget.Activation=FOLLOWMOUSE;
gadget.GadgetType=PROPGADGET;
gadget.GadgetRender=(APTR)&image;
gadget.SelectRender=NULL;
gadget.GadgetText=NULL;
gadget.MutualExclude=NULL;
gadget.SpecialInfo=(APTR)&prop;
prop.Flags=FREEHORIZ|AUTOKNOB;
prop.HorizPot=0;
prop.HorizBody=0;
prop.VertBody=MAXBODY;


/* Open up the Intuition interface -- a window */

IntuitionBase=(struct IntuitionBase *)
   OpenLibrary("intuition.library",INTUITION_REV);
if (IntuitionBase==NULL) {
   printf("Can't open window libraries\n\n");
   exit(10);
   }
NewWindow.LeftEdge=0;
NewWindow.TopEdge=0;
NewWindow.Width=640;
NewWindow.Height=200;
NewWindow.DetailPen=0;
NewWindow.BlockPen=1;
NewWindow.Title="The Sound Editor    Copyright 1986 SunRize Industries";
NewWindow.Flags=SMART_REFRESH|ACTIVATE|WINDOWDRAG|WINDOWCLOSE|
                WINDOWDEPTH|WINDOWSIZING|NOCAREREFRESH;
NewWindow.IDCMPFlags=MOUSEMOVE|MENUPICK|MOUSEBUTTONS|CLOSEWINDOW;
NewWindow.FirstGadget=&gadget;
NewWindow.Type=WBENCHSCREEN;
NewWindow.CheckMark=NULL;
NewWindow.Screen=NULL;
NewWindow.BitMap=NULL;
NewWindow.MinWidth=50;
NewWindow.MaxWidth=640;
NewWindow.MinHeight=200;
NewWindow.MaxHeight=200;

if ((Window=(struct Window *)OpenWindow(&NewWindow))==NULL) {
   printf("Can't Open Window\n\n");                   
   exit(10);
   }


/* tell intuition about menus */

SetMenuStrip(Window,&menu[0]);

/* create msg ports for writing to console */

conWritePort=(struct MsgPort *)CreatePort("mycon.write",0);
if (conWritePort==0) {
   printf("Can't create a port\n");
   exit(10);
   }
conWriteMsg=(struct IOStdReq *)CreateStdIO(conWritePort);
if (conWriteMsg==0) {
   printf("Can't create stdIO.\n");
   exit(10);
   }

/* Create ports and IO's for reading from the console */

if ((conReadPort=(struct MsgPort *)CreatePort("mycon.read",0))==0) {
   printf("Can't open port!\n");
   exit(10);
   }

if ((conReadMsg=(struct IOStdReq *)CreateStdIO(conReadPort))==0) {
   printf("Can't create stdIO!");
   exit(10);
   }
/* open the console */
OpenConsole(conWriteMsg,conReadMsg,Window);


/* Print options */

ConWrite(conWriteMsg,clrscn,4);
ConWrite(conWriteMsg,cursor_off,4);
msg(conWriteMsg,"No Samples Loaded");

memory=1024*100;
current=-1;
mark=0;
start_mark=0;
end_mark=0;

/* initilize samples to not used */

for (i=0;i<MAX_SAMPLES;i++)
   samp.addrs[i]=0;

/* main loop, wait for a command */

refresh:

prop.HorizPot=0;
position=0;
RefreshGadgets(&gadget,Window,NULL);

ignore:
if((message=(struct IntuiMessage *)GetMsg(Window->UserPort))==0) {
   Wait(1<<Window->UserPort->mp_SigBit);
   goto ignore;
   }
class=message->Class;
code=message->Code;
mousex=message->MouseX;
mousey=message->MouseY;
ReplyMsg(message);

if (class==MOUSEMOVE)       /* moving proportional gadget */
   do {
      position=(prop.HorizPot>>4)*(samp.length[current]-1);
      position=position/0x0FFF;
      message=(struct IntuiMessage *)GetMsg(Window->UserPort);
      if (message==0) break;
      class=message->Class;
      code=message->Code;
      mousex=message->MouseX;
      mousey=message->MouseY;
      ReplyMsg(message);
   } while (class==MOUSEMOVE);

if (class==CLOSEWINDOW) {
   ClearMenuStrip(Window);
   CloseDevice(conWriteMsg);
   DeleteStdIO(conWriteMsg);
   DeleteStdIO(conReadMsg);
   DeletePort(conWritePort);
   DeletePort(conReadPort);
   CloseWindow(Window);
   CloseLibrary(IntuitionBase);
   for(i=0;i<MAX_SAMPLES;i++)
      if (samp.addrs[i]!=0) FreeMem(samp.addrs[i],samp.length[i]);
   exit(0);
   }

if (class==MOUSEBUTTONS) {        /* selecting a new current */
   if (code!=SELECTDOWN) goto ignore;
   mousey=(mousey-11)/8;
   if (mousey<MAX_SAMPLES && samp.addrs[mousey]!=0) {
      if (current==mousey) {
         play(conWriteMsg,samp.addrs[mousey],
         samp.addrs[mousey]+samp.length[mousey],samp.rate[current]);
         goto ignore;
         }
      PutName(conWriteMsg,samp.name[current],current,REG_VID);
      current=mousey;
      PutName(conWriteMsg,samp.name[current],current,REV_VID);
      position=0;
      prop.HorizPot=0;
      RefreshGadgets(&gadget,Window,NULL);
      }
      goto update;
   }

if (class==MENUPICK && code==MENUNULL) {
    play(conWriteMsg,samp.addrs[current],samp.addrs[current]+position,
        samp.rate[current]);
    goto ignore;
    }
menunum=MENUNUM(code);
itemnum=ITEMNUM(code);

if (menunum==2 && itemnum==0) {            /* monitor the digitizer */
   listen(conWriteMsg);
   goto ignore;
   }
if (menunum==2 && itemnum==1)  {
   i=0;
   while(samp.length[i++]==0 && i<MAX_SAMPLES) ;
   if (i==MAX_SAMPLES) {
      msg(conWriteMsg,"No slots available... Delete a sample");
      goto ignore;
      }
   point=AllocMem(memory,MEMF_CHIP);
   if (point==0) {
      msg(conWriteMsg,"Internal error, reboot to ensure system integrity");
      goto ignore;
      }
   samplen=digitze (conWriteMsg,point,5,memory);
   if (samplen==memory)
      msg(conWriteMsg,"Memory filled.... Press a key.");
   else {
      msg(conWriteMsg,"Recording Ended Normaly.... Press a key");
      FreeMem(point+samplen,memory-samplen);
      sprintf(line,"used: %d",samplen);
      msg(conWriteMsg,line);
      }
   samp.length[i]=samplen;
   samp.addrs[i]=point;
   ConRead(conReadMsg,&samplen,1);
   msg(conWriteMsg,"Enter a label for this sample: ");
   GetWord(conReadMsg,conWriteMsg,samp.name[i]);
   PutName(conWriteMsg,samp.name[current],current,REG_VID);
   PutName(conWriteMsg,samp.name[i],i,REV_VID);
   current=i;
   }

if (menunum==3 && itemnum==0) {                   /*load a sample */
   load_samp(conWriteMsg,conReadMsg,&current,&samp);
   if (current==-1) goto ignore;
   position=0;
   prop.HorizPot=0;
   RefreshGadgets(&gadget,Window,NULL);
   }

if (menunum==3 && itemnum==1) {                  /* save current sample */
   save_samp(conWriteMsg,conReadMsg,current,&samp);
   goto ignore;
   }

if (menunum==3 && itemnum==2) {
   fp=Open("CON:0/100/640/100/A Sonic SubProcess",MODE_NEWFILE);
   if (fp==0) {
      msg(conWriteMsg,"ERROR -- can't open AmigaDOS CON:");
      goto ignore;
      }
   Write(fp,"Type ENDCLI to close window.\n",30);
   Execute("",fp,NULL);     /* spawn a NEWCLI */
   Close(fp);
   goto ignore;
   }

if (menunum==1 && itemnum==0)            /*  play entire sample */
   play(conWriteMsg,samp.addrs[current],
        samp.addrs[current]+samp.length[current],samp.rate[mark]);

if (menunum==1 && itemnum==1) {            /* play range  */
   sprintf(line,"%x %x %x %x",samp.addrs[mark],samp.addrs[current],
            samp.addrs[mark]+start_mark,samp.addrs[mark]+end_mark);
   msg(conWriteMsg,line);
   play(conWriteMsg,samp.addrs[mark]+start_mark,
        samp.addrs[mark]+end_mark,samp.rate[mark]);
   }

if (menunum==0 && (itemnum==3 || itemnum==4)) {       /* mark a range value */
   mark=current;
   if (itemnum==3) {
      start_mark=position;
      msg(conWriteMsg,"Start of range marked.");
      }
    else {
       end_mark=position;
       msg(conWriteMsg,"End of range marked.");
       }
    }

if (menunum==0 && itemnum==0) {     /* delete a range */
  if (end_mark<=start_mark) {
     msg(conWriteMsg,"No Action Taken");
     goto ignore;
     }
  msg(conWriteMsg,"Working...");
  if ((x=AllocMem(samp.length[mark]-end_mark+start_mark-1,MEMF_CHIP))==0) {
     nomem(conWriteMsg);
     goto ignore;
     }
  for(i=0;i<start_mark;i++)
     *(x+i)=*(samp.addrs[mark]+i);
  for(k=0;k<samp.length[mark]-end_mark-1;k++)
     *(x+i+k)=*(samp.addrs[mark]+end_mark+k);
  FreeMem(samp.addrs[mark],samp.length[mark]);
  samp.addrs[mark]=x;
  samp.length[mark]=samp.length[mark]-end_mark+start_mark-1;
  msg(conWriteMsg,"");
  }

if (menunum==0 && itemnum==1) {      /* insert here */
   if (start_mark>=end_mark) {
      msg(conWriteMsg,"No Action Taken");
      goto ignore;
      }
   msg(conWriteMsg,"Working...");
   len=samp.length[current]+end_mark-start_mark+1;
   if((x=AllocMem(len,0))==0) {
      nomem(conWriteMsg);
      goto ignore;
      }
    i=0;
    for (k=0;k<position;k++)         /* copy up to position to new buffer */
       *(x+i++)=*(samp.addrs[current]+k);
    for (k=start_mark;k<end_mark+1;k++)       /* copy range to new buffer */
       *(x+i++)=*(samp.addrs[mark]+k);
    for (k=position;k<samp.length[current];k++) /*copy rest of dest. samp */
       *(x+i++)=*(samp.addrs[current]+k);
    FreeMem(samp.addrs[current],samp.length[current]);
    samp.addrs[current]=x;
    samp.length[current]=len;
    msg(conWriteMsg,"");
    }
if (menunum==0 && itemnum==2) {        /* flip current sample */
   msg(conWriteMsg,"Working...");
   if((x=AllocMem(samp.length[current],0))==0) {
      nomem(conWriteMsg);
      goto ignore;
      }
   k=0;
   for(i=samp.length[current]-1;i>=0;i--)
      *(x+i)=*(samp.addrs[current]+k++);
   FreeMem(samp.addrs[current],samp.length[current]);
   samp.addrs[current]=x;
   msg(conWriteMsg,"");
   }

/** print Status information at the bottom of the screen **/

update:
if (current==-1) goto ignore;
if (end_mark>samp.length[mark]-1) end_mark=samp.length[mark]-1;
ConWrite(conWriteMsg,info,6);
sprintf(line,"Sample length:    %6.6d  Memory request size: %6.6d  Playback rate: %4.4d\n",
        samp.length[current],memory,samp.rate[current]);
ConWrite(conWriteMsg,line,strlen(line));
sprintf(line,"Current position: %6.6d  Start/End mark: %6.6d %6.6d",
        position,start_mark,end_mark);
ConWrite(conWriteMsg,line,strlen(line));
goto ignore;
}

/********************************/
/*  save current sample to disk */
/*******************************/

save_samp (conWriteMsg,conReadMsg,current,samp)

struct Samp *samp;
struct IOStdReq *conWriteMsg,*conReadMsg;
int current;
{
int fp,i;
if (samp->addrs[current]==0) return(0);
msg(conWriteMsg,"Writting...");
if ((fp=open(samp->name[current],O_WRONLY|O_CREAT|O_TRUNC))==-1) {
   msg(conWriteMsg,"ERROR opening file for write");
   return(0);
   }
i=0x00FF01FE;       /* SED file identification long word */
write(fp,&i,4);
write(fp,&samp->length[current],4);
write(fp,&samp->rate[current],4);
if (write(fp,samp->addrs[current],samp->length[current])==-1)
   msg(conWriteMsg,"ERROR writting to disk file!");
else
   msg(conWriteMsg,"Finished Writting... No Errors");
close(fp);
}


/********************************/
/*  Load a new sample from disk */
/********************************/

load_samp (conWriteMsg,conReadMsg,current,samp)

struct IOStdReq *conReadMsg,*conWriteMsg;
int *current;
struct Samp  *samp;
{

int in,i,k;
long pos;
char fname[50],*p;

i=0;
while (samp->addrs[i]!=0) i++;    /* get an open sample space */
if (i>MAX_SAMPLES) {
   msg(conWriteMsg,"No Slots left.");
   return(0);
   }
msg(conWriteMsg,"Enter Filename of the sample to load: ");
ConWrite(conWriteMsg,cursor_on,3);
GetWord(conReadMsg,conWriteMsg,fname);
ConWrite(conWriteMsg,cursor_off,4);
ConWrite(conWriteMsg,clrline,3);       /* erase line */
if ((in=(int)open(fname,O_RDONLY))==-1) {
   msg(conWriteMsg,"Can't open that file.");
   return(0);
   }
read(in,&k,4);
if (k!=0x00FF01FE) {         /* not in standard form */
   samp->rate[i]=350;           /* default length */
   pos=lseek(in,0L,2);
   samp->length[i]=(int)pos;
   msg(conWriteMsg,"No playback rate specified -- 350 used");
   lseek(in,0L,0);    /* rewind */
   }
else {
   read(in,&samp->length[i],4);
   read(in,&samp->rate[i],4);
   }

samp->addrs[i]=AllocMem(samp->length[i],0);
if (samp->addrs[i]==0) {
   nomem(conWriteMsg);
   return(0);
   }
read(in,samp->addrs[i],samp->length[i]);     /* read in sample */
close(in);
samp->name[i]=AllocMem(50,0);
if (samp->name[i]==0) {
   nomem(conWriteMsg);
   return(0);
   }
if ((p=(char *)strchr(fname,':'))==NULL)
    p=fname;  /* strip any device */
else
   p++;       /* get past colon */
strcpy(samp->name[i],p);        /* put filename in sample structure */
if (*current != -1)
   PutName(conWriteMsg,samp->name[*current],*current,REG_VID);
*current=i;
PutName(conWriteMsg,samp->name[i],i,REV_VID);
return(0);
}

/**********************************/
/**  write msg to cmd line       **/
/**********************************/
msg (conWriteMsg,message)
struct IOStdReq *conWriteMsg;
char *message;
{
ConWrite(conWriteMsg,lline,6);
ConWrite(conWriteMsg,clrline,3);
ConWrite(conWriteMsg,message,strlen(message));
}

/************************************/
/**  Print   "out of memory" msg    */
/************************************/

nomem(conWriteMsg)
struct IOStdReq *conWriteMsg;
{
msg(conWriteMsg,"OUT OF MEMORY");
}

/**********************************/
/**  input a word from raw:      **/
/**********************************/

GetWord(conReadMsg,conWriteMsg,word)
struct IOStdReq *conReadMsg,*conWriteMsg;
char *word;
{
char letter;
int i;
i=0;
do {
   ConRead(conReadMsg,&letter,1);
   if (letter==0x08)
      if (i!=0) {
         ConWrite(conWriteMsg,&letter,1);
         i--;
         }
      else ;
   else {
      ConWrite(conWriteMsg,&letter,1);
      *(word+i++)=letter;
      }
   } while (letter!=0x0D);
*(word+(--i))=0;
}

/***********************************/
/* Play a digitized sample         */
/***********************************/

play(fp,start,end,rate)
struct IOStdReq *fp;
char *start,*end;
int rate;
{
struct Channel ch;
if (end==start) return(0);
if (end<start) {
   msg(fp,"Invalid Range... Check Your Markers.");
   return(0);
   }
ch.samp.size=(int)(end-start);
ch.samp.waveform=start;
allocate_channel (fp,&ch);
tochan (&ch, rate);
deallocate_channel (&ch);
}

/******************************************/
/**  allocate an audio channel          ***/
/******************************************/

allocate_channel (fp,chan)
struct IOStdReq *fp;
struct Channel *chan;
{
struct MsgPort *port;
struct MsgPort ;

static UBYTE allocation_map[] = { 1, 8, 2, 4 };


chan->IOB = (struct IOAudio *)AllocMem (sizeof(struct IOAudio),
                                         MEMF_PUBLIC | MEMF_CLEAR);
if (chan->IOB == NULL)
   msg (fp,"can't allocate mem for IOB");

if (OpenDevice(AUDIONAME, 0, chan->IOB, 0) != 0)
   msg (fp,"can't open audio device");

/* now allocate a channel */

chan->IOB->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port =(struct MsgPort *)CreatePort("samples", 0)) == 0)
   msg (fp,"can't open port");
chan->IOB->ioa_Request.io_Message.mn_ReplyPort = port;
chan->IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
chan->IOB->ioa_Data = allocation_map;
chan->IOB->ioa_Length = sizeof (allocation_map);

SendIO (chan->IOB);
if (WaitIO (chan->IOB))
   msg (fp,"error in allocation");

}

/****************************************/
/** deallocate an audio channel        **/
/****************************************/

deallocate_channel (chan)
struct Channel *chan;
{
struct Device *device;
struct MsgPort *port;

if (chan->IOB != 0) {
   device = chan->IOB->ioa_Request.io_Device;
   if (device != 0) {
      chan->IOB->ioa_Request.io_Command = ADCMD_FREE;
      DoIO (chan->IOB);
      CloseDevice (chan->IOB);
   }
   port = chan->IOB->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   FreeMem (chan->IOB, sizeof (struct IOAudio));
}
}

/*************************************************/
/**  Play a sample on an alocated channel       **/
/*************************************************/

tochan (chan, rate)
struct Channel *chan;
int rate;
{

chan->IOB->ioa_Request.io_Command = CMD_WRITE;

chan->IOB->ioa_Request.io_Flags = ADIOF_PERVOL;

chan->IOB->ioa_Data = chan->samp.waveform;
chan->IOB->ioa_Length = chan->samp.size;

chan->IOB->ioa_Period = rate;
chan->IOB->ioa_Volume = 64;

chan->IOB->ioa_Cycles = 1;

BeginIO (chan->IOB);
WaitIO  (chan->IOB);

}

/*****************************************************/
/* PutName -- put a sample name in the correct place */
/*****************************************************/

PutName(conWriteMsg,name,location,vid)

FILE *conWriteMsg;
char *name;
int location;
int vid;
{

static char start={0x9b};
static char fin[]={0x3b,'0',0x48};
static char inv[]={0x9b,'7',0x6d};
static char plain[]={0x9b,'0',0x6d};
static char *number;
if (vid==REV_VID) {
   ConWrite(conWriteMsg,inv,3);
   }
ConWrite(conWriteMsg,&start,1);
number="   ";
sprintf(number,"%d",location+1);     /* change this so not limited to 9 */
ConWrite(conWriteMsg,number,1);
ConWrite(conWriteMsg,fin,3);
ConWrite(conWriteMsg,name,strlen(name));
ConWrite(conWriteMsg,plain,3);
}

/***********************************/
/***        record a sample      ***/
/***********************************/

/*   location - pointer to where data should go,
     rate     - a delay factor between 1-10 (5 = period of 350)
     max      - maximum number of bytes to record               */

digitze (fp,location,rate,maxnum)
char *location;
int rate,maxnum;
FILE *fp;
{

static int i,k;
static UBYTE *data,*direction,*pa;

direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
pa=(UBYTE *)0xBFE001;
*direction=0;           /* all bits are read */
*data=0;

msg(fp,"RECORDING... Press the left mouse button to stop.");
Disable();
k=0;

/* while (k++<maxnum && (*pa&64)==64) { */
   while(k++<maxnum) {
   for(i=0;i<rate;i++);   /* delay loop */
   *location++=(*data)-128;
   }
Enable();
return(k);
}

/*********************************************/
/***        listen to digitizer            ***/
/*********************************************/

listen (ConWriteMsg)
struct IOStdReq *ConWriteMsg;
{
static UBYTE *base,*pa,*data,*direction,*data2,*direction2;
static unsigned short sam,i;
unsigned short *aud0lch,*aud0len,*aud0vol,*aud0per,*aud0dat,*dmaconw;
unsigned short *intreq,*intreqr;
unsigned int *aud0lc;
direction=(UBYTE *)0xBFE301;     /* set bits here to a 1 for output */
data=(UBYTE *)0xBFE101;
data2=(UBYTE *)0xBFD000;
direction2=(UBYTE *)0xBFD200;

msg(ConWriteMsg,"Enter Channel value: ");


*direction=0;           /* all bits are read */
*data=0;
*direction2=2+4;      /* pa1 & pa2 write */
*data2=(UBYTE)getint(ConWriteMsg,ConWriteMsg);   /* pa2 -- select right channel */
msg(ConWriteMsg,"Press the left mouse button to exit monitor mode.");
Disable();

base=(UBYTE *)0xDEF000;
dmaconw=(unsigned short *)(base+0x096);
aud0lch=(unsigned short *)(base+0x0A0);
aud0len=(unsigned short *)(base+0x0A4);
aud0vol=(unsigned short *)(base+0x0A8);
aud0per=(unsigned short *)(base+0x0A6);
aud0dat=(unsigned short *)(base+0x0AA);
aud0lc=(int *)aud0lch;
intreq=(unsigned short *)(base+0x09C);
intreqr=(unsigned short *)(base+0x1E);
pa=(UBYTE *)0xBFE001;

*aud0vol=64;
*aud0per=1;
i=0;
while ((*pa&64)==64) {
   sam=(*data-128);
   sam=sam&0xFF;
   i=sam|(sam<<8);
   *aud0dat=i;
   *intreq=128;
   }
*dmaconw=1;
                  /* turn off sound dma */
Enable();
}

/**************************************/
/**    input an integer from RAW:    **/
/**************************************/

getint(ConReadMsg,ConWriteMsg)
struct IOStdReq ConReadMsg,ConWriteMsg;

{
int i;
char line[80];

GetWord(ConReadMsg,ConWriteMsg,line);
sscanf(line,"%d",&i);
return(i);
}


/*******************************************/
/**         Open the console device      ***/
/*******************************************/

OpenConsole(writerequest,readrequest,Window)

struct IOStdReq *writerequest;
struct IOStdReq *readrequest;
struct Window *Window;
{
int i;

writerequest->io_Data=(APTR)Window;
writerequest->io_Length=sizeof(*Window);
if ((i=OpenDevice("console.device",0,writerequest,0))!=NULL) {
   printf("Can't open console device");
   exit(10);
   }
readrequest->io_Device=writerequest->io_Device;
readrequest->io_Unit=writerequest->io_Unit;
}

/*************************************/
/***    write to console            **/
/*************************************/

ConWrite(request,string,length)
struct IOStdReq *request;
char *string;
int length;
{
request->io_Command=CMD_WRITE;
request->io_Data=(APTR)string;
request->io_Length=length;
DoIO(request);
}

/***************************************/
/***        Read from console       ****/
/***************************************/

ConRead(request,string,length)
struct IOStdReq *request;
char *string;
int length;
{
request->io_Command=CMD_READ;
request->io_Data=(APTR)string;
request->io_Length=length;
DoIO(request);
}

