#include "exec/types.h"
#include "exec/exec.h"
#include "exec/execbase.h"
#include "/include/studio16.h"

/***********************************************************/

/** Test program **/
#if 0
#define TESTSIZE 64

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
		DMA into. Memory is searched in the following order:
			1. Extended Memory (Greater than 0xFFFFFF) if 
				SBD_USE_EXTENDED_MEMORY is set in StudioBase.
			2. AutoConfig Fast Ram
			3. Chip Ram.
***********************************************************/

#define MASK24BIT 0x00FFFFFF

#ifndef MEMF_LOCAL		/** 2.0 defines **/
	#define MEMF_LOCAL  (1L<<8)
	#define MEMF_24BITDMA (1L<<9)
#endif

long *FindChunkType();
unsigned long MemoryAddressIsOkay();

/***********************************************************/

long *AllocDmaMem(size)
unsigned long size;
{
extern struct StudioBase *StudioBase;
long *it_ptr, *allocmem;

it_ptr=NULL;

Forbid();

if (StudioBase->defaults.flags & SBD_USE_EXTENDED_MEMORY)
	it_ptr=FindChunkType(size,TRUE,0);

if (it_ptr==NULL)
	it_ptr=FindChunkType(size,FALSE,MEMF_FAST);

if (it_ptr)
	allocmem=(long *)AllocAbs(size, it_ptr);
else
	allocmem=(long *)AllocMem(size, MEMF_CHIP);

Permit();
return(allocmem);
}

/***********************************************************/

long *FindChunkType(size,usexmem,flags)
unsigned long size;
unsigned long usexmem;
unsigned long flags;
{
struct MemChunk *chunk;
struct MemHeader *mem;
struct ExecBase *eb = *((struct ExecBase **)4);
long *it_ptr;
unsigned long last_size;
unsigned long attr;

it_ptr=NULL;
last_size=0xFFFFFFFF;

for (mem = (struct MemHeader *) eb -> MemList.lh_Head;
	mem -> mh_Node.ln_Succ; mem = (struct MemHead *)mem -> mh_Node.ln_Succ) {
	attr=mem->mh_Attributes;
	if ((attr&flags)==flags) {	/** E.G. CHIP_RAM? **/
		if (MemoryAddressIsOkay(mem->mh_Lower,usexmem)) {
			for (chunk = mem -> mh_First; chunk; chunk = chunk -> mc_Next) {
				if (chunk->mc_Bytes >= size) {
					if (chunk->mc_Bytes < last_size) {	/** pick smallest chunk that will hold 'size' **/
						it_ptr=(long *)chunk;
						last_size=chunk->mc_Bytes;
						}
					}
				}
			}
		}
	}
return(it_ptr);
}
/***********************************************************/

unsigned long MemoryAddressIsOkay(memaddr,x)
unsigned long memaddr;
unsigned long x;
{
if (x)  /** TRUE==Require Extended Memory! **/
	return(memaddr&0xFF000000);
else		/** FALSE==Require AUTOCONFIG memory **/
	return((unsigned long)((memaddr&MASK24BIT)==memaddr));
}

/***********************************************************/
