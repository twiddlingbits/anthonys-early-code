/* availmem from cli */
#include "exec/types.h"
#include "lattice/fcntl.h"
#include "exec/execbase.h"
#include "exec/memory.h"
#include "exec/interrupts.h"
#include "exec/exec.h"
#include "exec/errors.h"
#include "lattice/stdio.h"

main()
{
long l;
l=AvailMem(MEMF_CHIP);
printf("AvailMem says: %ld");
}
