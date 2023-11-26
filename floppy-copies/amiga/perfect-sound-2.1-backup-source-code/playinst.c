/***********************************************************************/
/*       playinst.c                                 July 27, 1986      */
/* Replaces an earlier version dated July 16, 1986.  It had a bug.     */
/*                                                                     */
/* Plays notes on the instruments from the Dealer Demos disks.         */
/* The documentation on the format of those sample files is in         */
/* a file named instrum.doc that was on the boards in February.        */
/* This program doesn't do much as such; it is intended to show how    */
/* to use the sampled sound files.  The program doesn't clean up after */
/* itself if an error occurs, since it is only a demo.  If all goes    */
/* well, however, it will free the memory and deallocate everything.   */
/* Improvements:                                                       */
/*    There are many possible, but the most glaring one has to do with */
/*    the play duration.  The duration is simply the number of times   */
/*    the looping segment is played.  It needs to be converted into a  */
/*    time, i.e. play for 3 seconds or something like that.  To do     */
/*    that, take the time and multiply by the sample rate to get the   */
/*    number of samples to play.  If that is shorter than the non-     */
/*    looping portion, play part of the non-looping portion.  If it is */
/*    longer, subtract the number of samples in the non-looping part   */
/*    and divide by the number of samples in the looping part to get   */
/*    the number of loops to play.  This may not be an integer, so you */
/*    will have to play part of a loop at the end.                     */
/*                                                                     */
/* This program is unconditionally placed in the public domain.  Feel  */
/* free to use code from it for anything you want (even for commercial */
/* purposes).  Let's see some music programs out there!!!              */
/* NOTE: the sound files on the Instruments disks are NOT IFF files.   */
/*                                                                     */
/* Author:  Bobby Deen                                                 */
/*          2506 Morning Glory                                         */
/*          Richardson, Tx. 75081                                      */
/*          (214) 235-4391  (voice)                                    */
/* Send Electronic Mail to:                                            */
/*          Rising Star BBS @ (214) 231-1372 (Echomail)                */
/*       or Amiga Scope BBS @ (214) 288-1537                           */
/***********************************************************************/

#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "fcntl.h"

#define MAX_OCTAVES 10
#define SOUNDPREC 20

struct octave {
   int start;
   int loopstart;
   int len;
   int nonlooplen;
   int looplen;
};

struct Sample {
   struct octave oct[MAX_OCTAVES];
   int num_octs;
   int size;
   UBYTE *waveform;
};

struct CompressedSample {
   UWORD technique;
   UWORD numSegments;
   UWORD loopStart;
   UBYTE highOctSize;
   UBYTE lowOctSize;
};

struct Channel {
   struct IOAudio *IOBA, *IOBB;
   struct Sample samp;
};



main()
{
struct Channel ch;
int p, l;
char buf[80];

printf("file:");
scanf("%s\n", buf);
load_sample (buf, &ch.samp);
allocate_channel (&ch);
printf("Enter -1 for pitch to get a scale\n");

l = 1;

while (l != 0) {
   printf ("pitch:");
   scanf ("%d\n", &p);
   printf ("length:");
   scanf ("%d\n", &l);

   if (p == -1 && l != 0) {
      for (p = 0; p < ch.samp.num_octs*12; p++)
         play (&ch, p, l);
   }
   else {
      if (l != 0)
         play (&ch, p, l);
   }
}

deallocate_channel (&ch);
unload_sample (&ch.samp);

error ("Done");
}



load_sample (name, sample)
char *name;
struct Sample *sample;
{
struct CompressedSample samp;
struct octave *o;
int status, i, segsize;
int sampfile;

sampfile = open(name, O_RDONLY);
if (sampfile == -1) {
   close(sampfile);
   error ("can't open sample file");
}

status = read (sampfile, &samp, sizeof (samp));
if (status != sizeof(samp)) {
   close(sampfile);
   error ("can't read sample file");
}

sample->size = 0;
sample->num_octs = samp.lowOctSize-samp.highOctSize+1;
printf ("%d octaves - notes range from 0 to %d\n",sample->num_octs,
                                    sample->num_octs*12-1);

for (i=0; i < sample->num_octs; i++) {
   o = &sample->oct[i];
   o->start = sample->size;
   segsize = 1 << (samp.highOctSize+i);
   o->len = segsize * samp.numSegments;
   sample->size += o->len;
   o->nonlooplen = samp.loopStart * segsize;
   o->loopstart = o->start + o->nonlooplen;
   o->looplen = o->len - o->nonlooplen;
}

sample->waveform = (UBYTE *)AllocMem (sample->size, MEMF_CHIP);
if (sample->waveform == NULL) {
   close (sampfile);
   error ("can't allocate waveform");
}

status = read (sampfile, sample->waveform, sample->size);
close (sampfile);
if (status != sample->size)
   error ("can't read waveform");

}



allocate_channel (chan)
struct Channel *chan;
{
struct MsgPort *port;
struct MsgPort *CreatePort();

static UBYTE allocation_map[] = { 1, 8, 2, 4 };


chan->IOBA = (struct IOAudio *)AllocMem (sizeof(struct IOAudio),
                                         MEMF_PUBLIC | MEMF_CLEAR);
if (chan->IOBA == NULL)
   error ("can't allocate mem for IOBA");

chan->IOBB = (struct IOAudio *)AllocMem (sizeof(struct IOAudio),
                                         MEMF_PUBLIC | MEMF_CLEAR);
if (chan->IOBB == NULL)
   error ("can't allocate mem for IOBB");

if (OpenDevice(AUDIONAME, 0, chan->IOBA, 0) != 0)
   error ("can't open audio device");

/* now allocate a channel */

chan->IOBA->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port = CreatePort("samp.A", 0)) == 0)
   error ("can't open port");
chan->IOBA->ioa_Request.io_Message.mn_ReplyPort = port;
chan->IOBA->ioa_Request.io_Command = ADCMD_ALLOCATE;
chan->IOBA->ioa_Data = allocation_map;
chan->IOBA->ioa_Length = sizeof (allocation_map);

SendIO (chan->IOBA);
if (WaitIO (chan->IOBA))
   error ("error in allocation");

/* initialize second IOB */

chan->IOBB->ioa_Request.io_Device = chan->IOBA->ioa_Request.io_Device;
chan->IOBB->ioa_Request.io_Unit = chan->IOBA->ioa_Request.io_Unit;
chan->IOBB->ioa_AllocKey = chan->IOBA->ioa_AllocKey;
if ((port = CreatePort("samp.B", 0)) == 0)
   error ("can't open port");
chan->IOBB->ioa_Request.io_Message.mn_ReplyPort = port;

}


deallocate_channel (chan)
struct Channel *chan;
{
struct Device *device;
struct MsgPort *port;

if (chan->IOBA != 0) {
   device = chan->IOBA->ioa_Request.io_Device;
   if (device != 0) {
      chan->IOBA->ioa_Request.io_Command = ADCMD_FREE;
      DoIO (chan->IOBA);
      CloseDevice (chan->IOBA);
   }
   port = chan->IOBA->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   FreeMem (chan->IOBA, sizeof (struct IOAudio));
}

if (chan->IOBB != 0) {
   port = chan->IOBB->ioa_Request.io_Message.mn_ReplyPort;
   if (port != 0)
      DeletePort (port);
   FreeMem (chan->IOBB, sizeof (struct IOAudio));
}
}


unload_sample(samp)
struct Sample *samp;
{

FreeMem (samp->waveform, samp->size);
}



play (chan, note, len)
struct Channel *chan;
int note, len;
{
int per, oct;
static int perval [] = {
   135, 143, 151, 160, 170, 180, 190, 202, 214, 226, 240, 254
};
/*   270, 286, 302, 320, 340, 360, 380, 404, 428, 452, 480, 508 */
/* ***NOTICE*** I had a bug in an earlier version where it wasn't       */
/* playing the lowest octave.  The top set of pitch values is what      */
/* the music demo program uses.  The bottom set will lower everything   */
/* an octave and give more low notes.  However, you probably should     */
/* use the top set and generate lower octaves by interpolating a        */
/* higher one (giving twice as many samples).                           */

oct = note / 12;
per = perval [note % 12];

printf ("oct = %d, per = %d\n", oct, per);

chan->IOBA->ioa_Request.io_Command = CMD_WRITE;
chan->IOBB->ioa_Request.io_Command = CMD_WRITE;

chan->IOBA->ioa_Request.io_Flags = ADIOF_PERVOL;
chan->IOBB->ioa_Request.io_Flags = ADIOF_PERVOL;

chan->IOBA->ioa_Data = chan->samp.waveform + chan->samp.oct[oct].start;
chan->IOBB->ioa_Data = chan->samp.waveform + chan->samp.oct[oct].loopstart;
chan->IOBA->ioa_Length = chan->samp.oct[oct].nonlooplen;
chan->IOBB->ioa_Length = chan->samp.oct[oct].looplen;

chan->IOBA->ioa_Period = per;
chan->IOBB->ioa_Period = per;
chan->IOBA->ioa_Volume = 64;
chan->IOBB->ioa_Volume = 64;

chan->IOBA->ioa_Cycles = 1;
chan->IOBB->ioa_Cycles = len;

/* Play the note, making sure we actually have some segments */
if (chan->IOBA->ioa_Length || chan->IOBB->ioa_Length) {
   if (chan->IOBA->ioa_Length)
      BeginIO (chan->IOBA);         /* play non-looping segments */
   if (chan->IOBB->ioa_Length) {
      BeginIO (chan->IOBB);         /* play looping segments */
      WaitIO (chan->IOBB);
   }
   else
      WaitIO (chan->IOBA);          /* no looping segments */
}
}



error (s)      /* this needs to be improved to clean up memory, etc. */
char *s;
{
printf ("%s\n", s);
exit(0);
}
    
