/****************************   Sound.c   *********************************

    Sound is copyright (c) 1988 by Richard Lee Stockton, 21305 60th Ave W.,
Mountlake Terrace, Washington 98043, 206/776-1253(voice), but may be freely
distributed as long as no profit is made from its distribution or sale
without my written permission. I call this concept 'FreeWare', you can
call it whatcha want.

    I also include the source code, (Manx 3.4/3.6), in the hope that it
will be of benefit to someone in the Amiga programming community. Feel free
to alter it at will, (but at your own risk!). I _would_ appreciate
receiving a copy of whatever it may become and it is always nice to receive
credit, (perhaps just a few lines of glowing tribute.^)

               Long Live Leo and All the Little Schwabies!

                    To Manufacture with Manx 3.4/3.6:

                            cc Sound.c
                            ln +cd Sound.o -lc

     You'll get a warning that you've re-defined IconBase. Ignore it.

**************************************************************************/

#include <exec/memory.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <workbench/icon.h>
#include <libraries/dosextens.h>
#include <devices/audio.h>
#include <functions.h>

/* Less than WorkBench 1.2 need not apply. That's nobody, right? ;-> */

#define  REVISION   33L

/* We'll need 4 buffers in CHIP memory, all else in FAST, if ya got it */

#define  BUFSIZE  1024L

/* Some MANX specific, byte saving, (usually), string routines */

#define  strcpy     _BUILTIN_strcpy
#define  strcmp     _BUILTIN_strcmp
#define  strlen     _BUILTIN_strlen

/* A pretty little HELP message showing valid variable ranges */

#define  USAGE      "\
\033[42m\033[31m\
 USAGE: Sound <file> [l/r]  [0-55]   [56-65535]  [on/off]  [QUIET]  \
\033[m\n\033[42m\
 ABORT: <CTRL> 'c'\033[33m\
  CHANNEL CYCLES SAMPLES/SECOND STEREO  NO-WINDOW \
\033[m\n\033[42m\033[31m\
 20-JAN-90 03:08 \xA9 1990 Richard Lee Stockton - freely distributable \
\033[m\n"

/* Probably more GLOBALS than are required. 'C' is funny stuff. */

extern struct   IconBase      *IconBase=NULL;
extern struct   IntuitionBase *IntuitionBase=NULL;
extern struct   WBStartup     *WBenchMsg=NULL;
extern struct   WBArg         *argp=NULL;
       struct   IntuiMessage  *message=NULL;
       struct   Window        *StatusWindo=NULL;
       struct   DiskObject    *infofile=NULL;
       struct   IOAudio       *sound[4]={NULL,NULL,NULL,NULL};
                long          sactual=0L, sstart=0L, savelock=0L,
                               atol(), sps=0L, cycles=1L, lock=0L;
                short         k=0, stereo=0, left=1, right=1,
                               statusline=1;
                UBYTE         sunit[4]={12,10,5,3},
                               sunitL[2]={1,8}, sunitR[2]={2,4};
                BOOL          help=FALSE;
                char          *sbuffer=NULL, *ltoa(), title[50]="Sound: ",
                               *cbuf[4]={NULL,NULL,NULL,NULL},
                               *sname[108]=NULL, *SafeAllocMem();
                void          loadSound(), setStatusWindo(),
                               soundSound(), cleanup(), quit();

/*********** quit, give-up, go home, finish... Neatness counts! ******/

void quit(qstring)
char    *qstring;
{ cleanup(qstring);  exit(0); }

void cleanup(string)
char    *string;
{
   if(sound[0])      /* This cleans up the audio device stuff */
   {
      for(k=3;k>(-1);k--)
      {
         if((sound[k])&&(sound[k]->ioa_Request.io_Device))
            AbortIO(sound[k]);
      }
      if(sound[0]->ioa_Request.io_Device) CloseDevice(sound[0]);
      for(k=3;k>(-1);k--)
      {
         if(sound[k]->ioa_Request.io_Message.mn_ReplyPort)
            DeletePort(sound[k]->ioa_Request.io_Message.mn_ReplyPort);
      }
      for(k=3;k>(-1);k--)
      {
         if(sound[k]) FreeMem(sound[k],(long)sizeof(struct IOAudio));
         if(cbuf[k])  FreeMem(cbuf[k],BUFSIZE);
      }
      sound[0]=sound[1]=sound[2]=NULL;
   }

/* Write any message to out. May be error or could be samples/second */
/* You'll be sorry if you try to Write(Output()) to WorkBench!  8-)  */

   if(help) Write(Output(),string,(long)strlen(string));
   else if(strlen(string))
   {
      strcpy(title,"Sound Error: ");   strcat(title,string);
      setStatusWindo();  Delay(150L);
   }

/* Clean up everything else */

   if(StatusWindo)      CloseWindow(StatusWindo);
   if(IntuitionBase)    CloseLibrary(IntuitionBase);
   if(sbuffer)          FreeMem(sbuffer,sactual);
   if(infofile)         FreeDiskObject(infofile);
   if(IconBase)         CloseLibrary(IconBase);
   if(!sound[3]) exit(10);
   StatusWindo=NULL;    IntuitionBase=NULL;
   sbuffer=NULL;   infofile=NULL;   IconBase=NULL;
   sactual=0L; sstart=0L; savelock=0L; sps=0L; cycles=1L;
   lock=0L;  k=0; stereo=0; left=1; right=1;
}


/*  Don't Allocate if Low Mem - by Bryce Nesbitt  */
/* Aberations by RLS. 4096 should be fudge enough */

char *SafeAllocMem(size,flags)
long size, flags;
{
   register char *p;
   
   if(p=(char *)AllocMem(size,flags))
      if(AvailMem(MEMF_CHIP)<4096L)
         { FreeMem(p,size);  return(NULL); }
   return(p);
}

/**** Status Window. Uses <CTRL> 'c' to abort.  ****/

void setStatusWindo()
{
    struct NewWindow w;
    
/* Try to Open Intuition and if successful, make a 'status' window */

    if(StatusWindo) CloseWindow(StatusWindo); StatusWindo=NULL;
    if(!IntuitionBase)
    {
       IntuitionBase = (struct IntuitionBase *)
                       OpenLibrary("intuition.library",REVISION);
    }
    if(!IntuitionBase) quit("");

    w.LeftEdge    =   82L;
    w.TopEdge     =    0L;

/* if title==USAGE, the large Width value will abort the window open */
/* This is just fine, because we don't WANT it to open in this case. */

    w.Width       = (long)(strlen(title)*8+8);
    w.Height      =   10L;
    w.DetailPen   =  0x01;
    w.BlockPen    =  0x03;
    w.Title       = (UBYTE *)title;
    w.Flags       = SMART_REFRESH|NOCAREREFRESH|ACTIVATE|WINDOWDRAG;
    w.IDCMPFlags  = VANILLAKEY;   /* So we can get <CTRL> 'c' message */
    w.Type        = WBENCHSCREEN;
    w.FirstGadget = NULL;
    w.CheckMark   = NULL;
    w.Screen      = NULL;
    w.BitMap      = NULL;
    w.MinWidth    = 0;
    w.MinHeight   = 0;
    w.MaxWidth    = 0;
    w.MaxHeight   = 0;
    StatusWindo = OpenWindow(&w);

/* Abort ONLY if cycles = 0 (loop) AND the window failed to open. */
/* This set of conditions would otherwise result in an Endless and */
/* Escape-proof loop. If cycles != 0, sound will end, sometime... */

    if(!StatusWindo&&(cycles==0L)) quit("");
}


/******** Load SoundFile 'sPath' & set cycles-sps-stereo *******/

void loadSound(sPath)
char    *sPath[80];
{
   struct Filehandle    *sFile=NULL;
   struct FileInfoBlock *finfo=NULL;
   struct FileLock      *lock=NULL;
          long          i, j;
          char          string[5];

/* Allocate 256 bytes as work memory */

   if(!(sbuffer=SafeAllocMem(256L,MEMF_CLEAR|MEMF_PUBLIC)))
        quit("No Work Memory!");

/* Check for and parse IFF data in first 256 bytes of file */

   if(!(sFile=Open(sPath,MODE_OLDFILE)))
   {
      sactual=256L;      quit("Can't Find SoundFile!");
   }
   Read(sFile,sbuffer,256L);              /* load the 1st 256 bytes */
   for(sstart=0L, sps=0L, i=0L; i<252L; i+=4L)
   {
      strncpy(string,sbuffer+i,4);   string[4]=NULL;
      if(!(strcmp(string,"VHDR")))        /* get samples per second */
      {
         for(j=0;j<(long)((UBYTE)sbuffer[i+20]);j++) sps+=256L;
         sps += ((UBYTE)sbuffer[i+21L]);
      }
      if(!(strcmp(string,"CHAN")))            /* Channel Assignment */
      {
         if((sbuffer[i+7]==6)||(sbuffer[i+11]==6)) stereo=1;
      }
      if(!(strcmp(string,"BODY")))        /* get size of sound data */
      {
         for(j=0;j<4;j++) sactual+=(((UBYTE)sbuffer[i+7L-j])<<(8*j));
         sstart = i+8L; i=252L;
      }
   }

/* if not in IFF format, get filesize from FileInfoBlock */

   if(!sactual)
   {

/* Allocate a file info block, get size from it, and de-allocate */

      if((!(finfo=(struct FileInfoBlock *)
         SafeAllocMem((long)sizeof(struct FileInfoBlock),MEMF_CLEAR)))
       ||(!(lock=Lock(sname,ACCESS_READ)))||(!(Examine(lock,finfo))) )
                    quit("FileInfoBlock Problem!");
      sactual = finfo->fib_Size;      if(lock) UnLock(lock);
      if(finfo) FreeMem(finfo,(long)sizeof(struct FileInfoBlock));
   }

/* clean up work area */

   FreeMem(sbuffer,256L); sbuffer=NULL;

/* Allocate _contiguous_ memory for SOUND data. */
/* We'll transfer in BUFSIZE chunks to CHIP memory a little later. */
/* We have to do the contiguity(?) check since AllocMem() does not. */

   if((AvailMem(MEMF_LARGEST)<sactual) ||
     (!(sbuffer=SafeAllocMem(sactual,MEMF_CLEAR|MEMF_PUBLIC))))
        { Close(sFile); quit("Need Contiguous Memory!"); }

/* Load the data into sbuffer */

   Seek(sFile,sstart,OFFSET_BEGINNING);
   if((Read(sFile,sbuffer,sactual)) == -1L)
      {Close(sFile); quit("Read Error!");}
   Close(sFile);
}

/*****************  make a noise ******************/

void soundSound()
{
    ULONG   class;
    LONG    i, dactual, dlength, remaining;
    USHORT  code, count;

/* Put up a 'status' window on the top line. */

    strcpy(title,sname);     strcat(title," at ");
    strcat(title,ltoa(sps)); strcat(title," SAMPLES_PER_SECOND");
    if(stereo&&left&&right)  strcat(title," in STEREO");
    if(statusline) setStatusWindo();

/* Make sure we have valid values before 'sounding' */

    if(left==right) left=right=1;
    if((sps<56L)||(sps>65534L)) sps = 10000L;
    if(cycles>65535L) cycles = 1L;       /* 55 limit only applies to CLI */

/* Allocate sound data buffers from CHIP memory. Ports and */
/* Audio Request Structures do NOT require CHIP memory */

   for(k=0;k<4;k++)
   {
     if(!(cbuf[k]=SafeAllocMem(BUFSIZE,
           MEMF_CHIP|MEMF_CLEAR|MEMF_PUBLIC))) quit("No CHIP Memory!");
     if(!(sound[k]=(struct IOAudio *)SafeAllocMem((long)sizeof(struct IOAudio),
                     MEMF_CLEAR|MEMF_PUBLIC))) quit("No IOA Memory!");
   }
   if( (!(sound[0]->ioa_Request.io_Message.mn_ReplyPort =
           CreatePort("Sound0",0L))) ||
       (!(sound[1]->ioa_Request.io_Message.mn_ReplyPort =
           CreatePort("Sound1",0L))) ||
       (!(sound[2]->ioa_Request.io_Message.mn_ReplyPort =
           CreatePort("Sound2",0L))) ||
       (!(sound[3]->ioa_Request.io_Message.mn_ReplyPort =
           CreatePort("Sound3",0L))) )         quit("No Port Memory!");
   

/* Open Audio using the first IOAudio as the 'initializer' request */

   sound[0]->ioa_Request.io_Message.mn_Node.ln_Pri = 20;
   if(!right) sound[0]->ioa_Data   = &sunitL[0];
   else if(!left) sound[0]->ioa_Data   = &sunitR[0];
   else sound[0]->ioa_Data   = &sunit[0];
   sound[0]->ioa_Length = 4L;
   if((OpenDevice(AUDIONAME,0L,sound[0],0L))!=NULL)
   {
      sound[0]->ioa_Request.io_Device=NULL;  /* don't AbortIO if no open */
      quit("No Audio Device!");
   }

/* Set all IOAudios. */

   for(k=0;k<4;k++)
   {
      sound[k]->ioa_Request.io_Message.mn_Node.ln_Pri = 20;
      sound[k]->ioa_Request.io_Command = CMD_WRITE;
      sound[k]->ioa_Request.io_Flags   = ADIOF_PERVOL;

/* Note copies of Device & AllocKey from initializer. */

      sound[k]->ioa_Request.io_Device  = sound[0]->ioa_Request.io_Device;
      sound[k]->ioa_AllocKey  = sound[0]->ioa_AllocKey;

/* Each IOAudio has its own CHIP buffer, Port, and Unit (left/right) */

      sound[k]->ioa_Data   = (UBYTE *)cbuf[k];

/* 3579547 divided by 55 = 65083, nearly the maximum Period (65535) */

      sound[k]->ioa_Period = 3579547L/sps;

/* As LOUD as possible. Use your monitor/stereo volume. Rock 'n Roll! */

      sound[k]->ioa_Volume = 64L;

/* One time through this BUFSIZE (or smaller) part of the whole */

      sound[k]->ioa_Cycles = 1L;
   }

/* The compiler wants 'Unit' to be a structure, we just want to mask */
/* into the allocated left/right channels. left=1 or 8, right=2 or 4 */
/*        ...zap! You're a Unit structure! Feel any different?       */

   for(k=2;k>(-1);k-=2)
   {
      if(right) sound[k+1]->ioa_Request.io_Unit = (struct Unit *)
                      ((ULONG)(sound[0]->ioa_Request.io_Unit)&6L);
      if(left)    sound[k]->ioa_Request.io_Unit  = (struct Unit *)
                      ((ULONG)(sound[0]->ioa_Request.io_Unit)&9L);
   }

/* If in STEREO, split file. If in MONO, 'b' buffers use 'a' data */

   if(stereo) remaining=(sactual/2L)-(sactual&1L);
   else
   {
      remaining=sactual;
      sound[1]->ioa_Data   = (UBYTE *)cbuf[0];
      sound[3]->ioa_Data   = (UBYTE *)cbuf[2];
   }

/* dactual is the length of one channel's complete data */

   dactual=remaining;     k=count=0;

/* we be doing loops here */

   do
   {

/* be CERTAIN ioa_Length is an even number & set datalength */

      if(remaining>BUFSIZE) dlength=BUFSIZE;
       else  { dlength=remaining;  dlength-=(dlength&1L); }

/* Move the data into the proper CHIP buffer of BUFSIZE */

      movmem(sbuffer+(dactual-remaining),cbuf[k],(int)dlength);

/* Don't load or use the right CHIP buffers if MONO. Saves time. */

      if(stereo) movmem(sbuffer+(sactual-remaining),
                        cbuf[k+1],(int)dlength);

/* Data has been moved, so adjust 'remaining' */

      remaining-=dlength;

/* Left and Right Lengths are the same, no matter what! */

      sound[k]->ioa_Length = sound[k+1]->ioa_Length = dlength;

/* Start one set of Left/Right Channels. */

      if(left)  BeginIO(sound[k]);
      if(right) BeginIO(sound[k+1]);

/* If no QUIET flag, Check Intuition for the ABORT message */

      if(statusline)
      {
      while(message=(struct IntuiMessage *)GetMsg(StatusWindo->UserPort))
         {
            class = message->Class;  code = message->Code;

/* Hi Intuition! Thanks for the message, Have a nice day! */

            ReplyMsg(message);

/* <CTRL> 'c' abort.  1,2,3, easy as a,b,c, baby you and me! */

            if((class==VANILLAKEY)&&(code==3)) quit("");
         }
      }

/* Is this the last time AND the last cycle? If yes & no, reset. */

      if(remaining<2L) if(--cycles!=0L)
         { remaining=dactual;   dlength=BUFSIZE; }

/* Is this the last time, or what? */

      if(remaining<2L)  WaitIO(sound[k+right]);   /* wait for LAST request */
      else
      {
         if(k) k=0; else k=2;    /* switch buffers & wait for PREVIOUS */
         if(count++) WaitIO(sound[k+right]);
      }

/* Keep going until we run out of data */

   } while(remaining>1L);                   /* End of Loop */
}


/********** long int to char string (up to 7 digits) ***********/

char *ltoa(num)
          long  num;
{
   static char ostring[8]="";
          short  next = 7,  shift = 0;
   
   if (!num) ostring[next--] = '0';
   while ((num+9L)/10)
      { ostring[next--] = num % 10L + '0';   num /= 10L; }
   next+=1;
   while(next<8)       ostring[shift++] = ostring[next++];
   while(shift<8)      ostring[shift++] = '\0';
   return(ostring);
}


/************************  MAIN  ****************************/

main(argc,argv)
 int           argc;
 char          *argv[];
{
    long      temp=0L;
    short     i=0, WBargs=0;
    char      string[10];
    
/* If Called From CLI */

    if(argc==1) {help=TRUE; quit(USAGE);}    /* No SOUNDfile so quit */
    if(argc>1)
    {
       strcpy(sname,argv[1]);   /* 2nd arg MUST be filename */
       loadSound(sname);         /* Load 'em up! */
       for(i=2;i<argc;i++)
       {
          if(argv[i][0]=='o'||argv[i][0]=='O')
          {
             if(argv[i][1]=='n'||argv[i][1]=='N')   stereo = 1;
              else  stereo = 0;
          }
          else if(argv[i][0]=='l'||argv[i][0]=='L')      right = 0;
          else if(argv[i][0]=='r'||argv[i][0]=='R')       left = 0;
          else if(argv[i][0]=='q'||argv[i][0]=='Q') statusline = 0;
          else  /* must be sps or cycles, size of arg determines which one */
          {
             temp = atol(argv[i]);
             if(temp<56L)  cycles = temp;    else  sps = temp;
          }
       }
       soundSound();   quit("");
    }


/* If WorkBench, try to get info from the .info file */

    if(WBenchMsg->sm_NumArgs>1) WBargs=1;
    while((argc==0)&&(WBenchMsg->sm_NumArgs>WBargs)) /*  from WorkBench  */
    {
       argp=(WBenchMsg->sm_ArgList)+WBargs;      /*  CD to lock and  */
       if(lock=argp->wa_Lock)  savelock=(long)CurrentDir(lock);
       strcpy(sname,argp->wa_Name);              /* get the filename */
       loadSound(sname);
       if(!IconBase)
          IconBase=(struct IconBase *)OpenLibrary("icon.library",REVISION);
       if((!IconBase) ||   
            ((infofile=(struct DiskObject *)GetDiskObject(sname))==NULL))
                quit("");

/* then check 'ToolTypes' to set cycles, stereo, & samples per second. */
/* Note use of strncpy to copy only what we need. This is an attempt to */
/* take care of a .info editing bug which may cause extra long strings. */

       strncpy(string,(char *)FindToolType(infofile->do_ToolTypes,
                       "CYCLES"),3);

/* We do this check for '0' since atol() thinks ' ', or NULL = '0'  */
/* But if data is blank, or NULL, we want cycles set to 1 (default) */

       if(string[0]=='0') cycles = 0L;
       temp=atol(string);  if(temp) cycles = temp;

/* Valid STEREO strings will have o or O as the first letter. */

       strncpy(string,(char *)FindToolType(infofile->do_ToolTypes,
                       "STEREO"),4);
       if(string[0]=='o'||string[0]=='O')
       {
          if(string[1]=='n'||string[1]=='N')   stereo = 1;
           else  stereo = 0;
       }

/* Set left or right channel only (for special effects) */

       strncpy(string,(char *)FindToolType(infofile->do_ToolTypes,
                       "CHANNEL"),4);
       if(string[0]=='l'||string[0]=='L') right=0;
       if(string[0]=='r'||string[0]=='R')  left=0;

/* We'll take any old sps because we check for validity before playing */

       strncpy(string,(char *)FindToolType(infofile->do_ToolTypes,
                       "SAMPLES_PER_SECOND"),6);
       temp = atol(string);    if(temp) sps = temp;

       soundSound();                                    /* MAKE THAT NOISE! */

       if(lock) CurrentDir(savelock);  lock=NULL;    /* restore current DIR */

/* Then release everything you've used here. (Be a tidy camper.) */

       cleanup(NULL);    WBargs++;
    }
    exit(0);
}

/************************** end of Sound.c ******************************/
