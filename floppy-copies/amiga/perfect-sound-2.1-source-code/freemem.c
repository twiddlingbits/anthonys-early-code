#include "exec/types.h"
#include "exec/memory.h"

main()
{
int c,f;
c=AvailMem(MEMF_CHIP);
f=AvailMem(MEMF_FAST);
printf("Free Chip Mem is: %d  (%x)\n",c,c);
printf("Free Fast Mem is: %d  (%x)\n",f,f);
}

