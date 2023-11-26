#include "/include/psound.h"

extern struct studioBase *studioBase;
extern short se_info_sample_deleted;

/***********************************************************/

/** General Disk_Samp manipulation functions **/

void DeleteSample(s)

struct Disk_Samp *s;

{
if (s==NULL)
   return;

CloseSample(s);
BroadcastEventParms(se_info_sample_deleted,s,0,0,0,0,0);
}

/***********************************************************/

/*** return pointer to a sample given its position in the chain **/

struct Disk_Samp *GetSamplePtr(i)
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

/***********************************************************/

/*** return position number of a sample **/

int GetSampleNum(s)
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

#if 0
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
#endif