#include "exec/types.h"
#include "psound.h"

extern int end_mark;
extern int start_mark;
extern int insert_mark;

void mix_samples(dest,src)

struct Samp *src;
struct Samp *dest;

{
int i,j,k;

i = insert_mark + end_mark - start_mark + 1;
if (i > dest->length) {
   i = dest->length;
   }

for (k = insert_mark; k < i; k++) {
   j = dest->lmem[k];
   j = j + src->lmem[k-insert_mark+start_mark];
   if (j < -126) j = -127;
   if (j > 126) j = 127;
   dest->lmem[k] = j;
   }

if (dest->type==STEREO && src->type==STEREO) {
   for (k = insert_mark; k < i; k++) {
      j = dest->rmem[k];
      j = j + src->rmem[k-insert_mark+start_mark];
      if (j < -126) j = -127;
      if (j > 126) j = 127;
      dest->rmem[k] = j;
      }
   }
}
