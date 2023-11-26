#include "exec/types.h"
#include "/include/psound.h"

/********************************************/
/** insert a range from one samp to another */
/********************************************/

move(Window,start,end,position,from,to)

struct Window *Window;
int position,start,end;
struct Disk_Samp *from,*to;
{
extern struct list *open_list;
static struct Disk_Samp *temp_samp;
int len;
int i;

   if (start>=end) {
      show_msg(Window,195);
      return(0);
      }

   if ((to->type==STEREO || from->type==STEREO)
        && (to->type!=from->type)) {
      show_msg(Window,196);
      return(0);
      }

   show_msg(Window,112);

   len=to->length+end-start;
   create_sample(to->name, to->version+1, &temp_samp);    /** NEED TO CHECK FOR ERROR!!! **/
   copy_samp_parms(to, temp_samp);
   init_sample2(to, 0);

   for (i=0; i < position; i++)
      put_samplel1(get_samplel2());

   init_sample2(from, start);
   for (i=start; i <= end; i++)
      put_samplel1(get_samplel2());

   init_sample2(to, position);
   for (i=0; i < to->length-position; i++)
      put_samplel1(get_samplel2());


   if (to->type==STEREO) {
      init_sample1(temp_samp, 0);
      init_sample2(to, 0);

      for (i=0; i < position; i++)
         put_sampler1(get_sampler2());

      init_sample2(from, start);
      for (i=start; i <= end; i++)
         put_sampler1(get_sampler2());

      init_sample2(to, position);
      for (i=0; i < to->length-position; i++)
         put_sampler1(get_sampler2());
      }

   delete_sample(to);
   add_list_entry(open_list, temp_samp->name, temp_samp);

   msg(Window,"");
   }

