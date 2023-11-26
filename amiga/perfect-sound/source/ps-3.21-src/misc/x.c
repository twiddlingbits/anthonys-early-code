#include "exec/types.h"
#include "exec/execbase.h"

main()
{
struct ExecBase *eb;
struct ExecBase **eeb;

eeb = 4;
eb = *eeb;

printf("flags %x\n",eb->AttnFlags);
}

