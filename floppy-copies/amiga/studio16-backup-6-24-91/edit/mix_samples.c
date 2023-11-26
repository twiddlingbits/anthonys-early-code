#include "exec/types.h"
#include "/include/psound.h"

extern int end_mark;
extern int start_mark;
extern int insert_mark;

void mix_samples(dest,src)

struct Disk_Samp *src;
struct Disk_Samp *dest;

{
long i,j,k;

i = insert_mark + end_mark - start_mark + 1;
if (i >= dest->length)
   i = dest->length;

init_sample1(dest, insert_mark);
init_sample2(src, start_mark);

for (k = insert_mark; k < i; k++) {
   j = get_left_sample1(dest, k);
   j = j + get_samplel2();
   if (j < -32767) j = -32768;
   if (j > 32766) j = 32767;
   put_samplel1(j);
   }

if (dest->type==src->type==STEREO) {
   init_sample1(dest, k);
   init_sample2(src, start_mark);
   for (k = insert_mark; k < i; k++) {
      j = get_right_sample1(dest, k);
      j = j + get_sampler2();
      if (j < -32767) j = -32768;
      if (j > 32766) j = 32767;
      put_sampler1(j);
      }
   }
}
