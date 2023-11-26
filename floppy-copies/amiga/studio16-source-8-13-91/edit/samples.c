#include "/include/psound.h"

extern struct studioBase *studioBase;
extern struct Disk_Samp *marked_samp;
extern int mark_drawn;               /** TRUE if part of graph outlined **/
extern struct list *open_list;

/** General Disk_Samp manipulation functions **/

void delete_sample(s)

struct Disk_Samp *s;

{
int err;

if (s==NULL)
   return;


/* printf("in delete_sample: s=%x %s\n",s,s->name);
printf("samplenum %d\n",get_sample_num(s)); */

delete_list_entry(open_list, get_sample_num(s));
err=close_sample(s);
printf("in delete_sample: (samples): close_samp returned: %d\n",err);

if (marked_samp==s) {
   marked_samp = NULL;
   mark_drawn = FALSE;
   }
}

/*** return pointer to a sample given its position in the chain **/

struct Disk_Samp *get_sample_ptr(i)
int i;
{
struct MinNode *node;

if (i < 0)        /** -1 means no entrys in list->current **/
   return(0);

for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ)
   if (i-- <= 0)
      return((struct Disk_Samp *)node);
return(NULL);
}

/*** return position number of a sample **/

int get_sample_num(s)
struct Disk_Sample *s;
{
struct MinNode *node;
int i;

i=0;
for (node=studioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   if (s==(struct Disk_Samp *)node)
      return(i);
   i++;
   }
return(-1);
}

/** copy samp parms **/

void copy_samp_parms(from, to)
struct Disk_Samp *from, *to;
{
to->length=from->length;
to->rate=from->rate;
to->type=from->type;
to->flags=from->flags;
to->version=from->version;
}
