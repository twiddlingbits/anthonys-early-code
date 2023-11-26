#include "exec/types.h"
#include "/include/psound.h"

/********************************************/
/** insert a range from one samp to another */
/********************************************/

move(Window,start,end,position,from,to)

struct Window *Window;
int position,from,to,start,end;

{
static struct Disk_Samp temp_samp;
extern struct Disk_Samp samp[MAX_SAMPLES];
int len;
int i;

   if (start>=end) {
      show_msg(Window,195);
      return(0);
      }

   if ((samp[to].type==STEREO || samp[from].type==STEREO)
        && (samp[to].type!=samp[from].type)) {
      show_msg(Window,196);
      return(0);
      }

   show_msg(Window,112);

   len=samp[to].length+end-start;
   temp_samp = samp[to];
   temp_samp.version++;
   create_sample(&temp_samp);    /** NEED TO CHECK FOR ERROR!!! **/
   init_sample2(&samp[to], 0);

   for (i=0; i < position; i++)
      put_samplel1(get_samplel2());

   init_sample2(&samp[from], start);
   for (i=start; i <= end; i++)
      put_samplel1(get_samplel2());

   init_sample2(&samp[to], position);
   for (i=0; i < samp[to].length-position; i++)
      put_samplel1(get_samplel2());


   if (samp[to].type==STEREO) {
      init_sample1(&temp_samp, 0);
      init_sample2(&samp[to], 0);

      for (i=0; i < position; i++)
         put_sampler1(get_sampler2());

      init_sample2(&samp[from], start);
      for (i=start; i <= end; i++)
         put_sampler1(get_sampler2());

      init_sample2(&samp[to], position);
      for (i=0; i < samp[to].length-position; i++)
         put_sampler1(get_sampler2());
      }

   samp[to].flags |= KEEP_NAME;
   close_sample(&samp[to]);
   samp[to] = temp_samp;

   samp[to].length=len;
   msg(Window,"");
   }

