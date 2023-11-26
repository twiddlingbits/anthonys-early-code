#include "exec/types.h"
#include "psound.h"

extern struct Samp samp[MAX_SAMPLES];

void delete_sample(val)

int val;

{
if (samp[val].lmem==0)
   return;

FreeMem(samp[val].lmem,samp[val].length);

if (samp[val].type==STEREO)
  FreeMem(samp[val].rmem,samp[val].length);

FreeMem(samp[val].name,strlen(samp[val].name)+1);

samp[val].lmem=0;
samp[val].rmem=0;
samp[val].length=0;
samp[val].repeat=0;
samp[val].oneshot=0;
samp[val].cycles=0;
samp[val].ctoctave=0;
}
