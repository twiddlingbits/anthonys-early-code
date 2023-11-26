#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"

/**********************************/
/** find largest memory fragment  */
/**********************************/

extern struct ExecBase *SysBase;

frags()

{
int    newmax=0;
struct MemChunk *chunk;
struct MemHeader *mem;
struct ExecBase *eb = SysBase;


Forbid();
for (mem = (struct MemHeader *) eb -> MemList.lh_Head;
mem -> mh_Node.ln_Succ; mem = mem -> mh_Node.ln_Succ) {
   for (chunk = mem -> mh_First; chunk; chunk = chunk -> mc_Next) {
   if (chunk->mc_Bytes>newmax) newmax=chunk->mc_Bytes;
   }
}
Permit();
if (newmax<40000) newmax=40000;
return (newmax-40000);    /* leave some memory to prevent crashes */
}


