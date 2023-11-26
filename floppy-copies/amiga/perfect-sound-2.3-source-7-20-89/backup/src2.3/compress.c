#include "exec/types.h"
#include "psound.h"

/*****************************/
/** compress a sound sample **/
/*****************************/

compress_4 (Window, samp, i)

struct Window *Window;
struct Samp *samp;
int i;   /** the sample number **/

{
short val;
int j;

for (j=0; j < samp->length[i]; j++) {
   val = *(samp->lmem[i]+j);
   val = val + 128;
   val = val>>1;
   val = val <<1;
   val = val - 128;
   *(samp->lmem[i]+j) = val;
   }
}

compress_6 (Window, samp, i)

struct Window *Window;
struct Samp *samp;
int i;   /** the sample number **/

{
short val;
int j;

for (j=0; j < samp->length[i]; j++) {
   val = *(samp->lmem[i]+j);
   val = val + 128;
   val = val>>2;
   val = val <<2;
   val = val - 128;
   *(samp->lmem[i]+j) = val;
   }
}

compress_all (Window, samp, i)

struct Window *Window;
struct Samp *samp;
int i;   /** the sample number **/

{
UBYTE val, upper;
int kount,kount2,j;

val = *(samp->lmem[i]);
upper = val&0xF0;
kount = 2;
kount2 = 0;

for (j=1; j < samp->length[i]; j++) {
   val = *(samp->lmem[i]+j);
   if ( (val&0xF0)!=upper) {
      upper = val&0xF0;
      kount = kount + 2;
      kount2++;
      }
   else {
      kount++;
      }
   }

printf("uncompressed len = %d\n",samp->length[i]);
printf("total compressed bytes = %d\n",kount/2);
printf("total nyble switches = %d\n\n",kount2);
}


