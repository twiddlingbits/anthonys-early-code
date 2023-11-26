#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"

/***********************************************************/

/** Test program **/

#if 0
#define TESTSIZE 8

long *AllocDmaMem();

main()
{
long *mem;

mem=AllocDmaMem(TESTSIZE);
kprintf("AllocDmaMem=%04x%04x\n",mem);
if (mem)
	FreeMem(mem, TESTSIZE);
}
#endif

/**********************************************************
	AllocDmaMem(size)

		Scan the freemem list (ExecBase->MemList) looking for
		memory that an hard disk controller can
		DMA into. Two kinds of memory qualify:
			1. Memory who's attribute flags include MEMF_LOCAL
			2. Memory who's position is less than 0x01000000;
***********************************************************/

#define MASK24BIT 0x00FFFFFF

#ifndef MEMF_LOCAL		/** 2.0 defines **/
	#define MEMF_LOCAL  (1L<<8)
	#define MEMF_24BITDMA (1L<<9)
#endif

long *AllocDmaMem(size)
unsigned long size;
{
struct MemChunk *chunk;
struct MemHeader *mem;
struct ExecBase *eb = *((struct ExecBase **)4);
long *it_ptr, *allocmem;
unsigned long last_size;
unsigned long attr;

it_ptr=NULL;
last_size=0xFFFFFFFF;

Forbid();
for (mem = (struct MemHeader *) eb -> MemList.lh_Head;
	mem -> mh_Node.ln_Succ; mem = (struct MemHead *)mem -> mh_Node.ln_Succ) {
	attr=mem->mh_Attributes;
	for (chunk = mem -> mh_First; chunk; chunk = chunk -> mc_Next) {
		if (!(attr&MEMF_CHIP)) {	/** don't want chip ram till last **/
			if (((long)chunk&MASK24BIT)==(long)chunk || (attr&MEMF_LOCAL)) {
				if (chunk->mc_Bytes >= size) {
					if (chunk->mc_Bytes < last_size) {
						it_ptr=(long *)chunk;
						last_size=chunk->mc_Bytes;
						}
					}
				}
			}
		}
	}

if (it_ptr)
	allocmem=(long *)AllocAbs(size, it_ptr);
else
	allocmem=(long *)AllocMem(size, MEMF_CHIP);

Permit();
return(allocmem);
}


