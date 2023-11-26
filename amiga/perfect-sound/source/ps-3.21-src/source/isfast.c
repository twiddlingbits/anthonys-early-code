#include "exec/types.h"
#include "exec/execbase.h"

/** return TRUE if a 68020/030/010 is in use **/

isfast()
{
struct ExecBase *eb;
struct ExecBase **eeb;

eeb = (struct ExecBase **)4;
eb = *eeb;
return(eb->AttnFlags&(AFF_68010|AFF_68020));
}

