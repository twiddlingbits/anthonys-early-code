#include "exec/types.h"
#include "/include/psound.h"

/**************************/
/** flip a sample        **/
/**************************/

flip (samp, s, e)

struct Disk_Samp *samp;
int s,e;
{
static struct Disk_Samp temp;
int k;
int buf_pt;

temp = *samp;
temp.version++;
create_sample(&temp);   /** CHECK FOR ERROR!! **/

for (k=0; k < s; k++)
   put_samplel1(get_left_sample2(samp, k));

k = e;
buf_pt = k-BLOCK_SIZE+1;
get_left_sample2(samp, buf_pt);

while (k >=s) {
   put_samplel1(get_left_sample2(samp, k));
   if (k==buf_pt) {  /** stop get_left_samp from reloading buffer alot */
      buf_pt = buf_pt - BLOCK_SIZE;
      if (buf_pt < 0) buf_pt = 0;
      get_left_sample2(samp, buf_pt);
      }
   k--;
   }

for (k=e+1; k < samp->length; k++)
   put_samplel1(get_left_sample2(samp, k));

if (samp->type==STEREO) {
   init_sample1(&temp,0);
   for (k=0; k < s; k++)
      put_samplel1(get_left_sample2(samp, k));
   k = samp->length-1;
   buf_pt = k-BLOCK_SIZE+1;
   get_left_sample2(samp, buf_pt);
   while (k >=s) {
      put_samplel1(get_left_sample2(samp, k));
      if (k==buf_pt) {  /** stop get_left_samp from reloading buffer alot */
         buf_pt = buf_pt - BLOCK_SIZE;
         if (buf_pt < 0) buf_pt = 0;
         get_left_sample2(samp, buf_pt);
         }
      k--;
      }
   for (k=e+1; k < samp->length; k++)
      put_samplel1(get_left_sample2(samp, k));
   }
samp->flags |= KEEP_NAME;
close_sample(samp);
*samp = temp;
return(0);
}
   
