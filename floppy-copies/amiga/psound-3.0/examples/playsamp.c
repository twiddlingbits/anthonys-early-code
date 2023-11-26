#include "exec/types.h"
#include "exec/exec.h"
#include "devices/audio.h"
#include "fcntl.h"

#define SOUNDPREC 20

struct Sample {
   int size;
   UBYTE *waveform;
};

struct Channel {
   struct IOAudio *IOB;
   struct Sample samp;
};



main()
{
struct Channel ch;
int rate;
char buf[80];

printf("file:");
scanf("%s\n", buf);
load_sample (buf, &ch.samp);
allocate_channel (&ch);

rate = 1;

while (rate != 0) {
   printf ("sample rate:");
   scanf ("%d\n", &rate);

   if (rate != 0)
      play (&ch, rate);
}

deallocate_channel (&ch);
unload_sample (&ch.samp);

error ("Done");
}


load_sample (name, sample)
char *name;
struct Sample *sample;
{
int status;
int sampfile;

sampfile = open(name, O_RDONLY);
if (sampfile == -1) {
   close(sampfile);
   error ("can't open sample file");
}

status = read (sampfile, &(sample->size), sizeof (sample->size));
if (status != sizeof(sample->size)) {
   close(sampfile);
   error ("can't read sample file");
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


chan->IOB = (struct IOAudio *)AllocMem (sizeof(struct IOAudio),
                                         MEMF_PUBLIC | MEMF_CLEAR);
if (chan->IOB == NULL)
   error ("can't allocate mem for IOB");

if (OpenDevice(AUDIONAME, 0, chan->IOB, 0) != 0)
   error ("can't open audio device");

/* now allocate a channel */

chan->IOB->ioa_Request.io_Message.mn_Node.ln_Pri = SOUNDPREC;
if ((port = CreatePort("samples", 0)) == 0)
   error ("can't open port");
chan->IOB->ioa_Request.io_Message.mn_ReplyPort = port;
chan->IOB->ioa_Request.io_Command = ADCMD_ALLOCATE;
chan->IOB->ioa_Data = allocation_map;
chan->IOB->ioa_Length = sizeof (allocation_map);

SendIO (chan->IOB);
if (WaitIO (chan->IOB))
   error ("error in allocation");

}


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


unload_sample(samp)
struct Sample *samp;
{

FreeMem (samp->waveform, samp->size);
}



play (chan, rate)
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
WaitIO (chan->IOB);

}



error (s)
char *s;
{
printf ("%s\n", s);
exit(0);
}
    
