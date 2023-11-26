#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"

mem_type(p)
APTR p;
{
extern struct ExecBase *SysBase;
struct MemHeader *mh;
int memtype;

memtype=0;
Forbid();
for (mh=(struct MemHeader *)(SysBase->MemList.lh_Head);
     mh->mh_Node.ln_Succ; mh = mh->mh_Node.ln_Succ) {
      if (p < mh->mh_Upper && p>=mh->mh_Lower)
         memtype = mh->mh_Attributes&(MEMF_CHIP|MEMF_FAST);
   }
Permit();
return(memtype);
}



