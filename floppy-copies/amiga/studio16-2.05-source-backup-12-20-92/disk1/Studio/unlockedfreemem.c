#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"


/** PUT AUTODOCS - AW **/

/****************************************************************/

static struct MinList locked;		/** Locks that are not ok to free **/
static struct MinList unlocked;	/** Locks that are ok to free **/
static struct MinList chunks;		/** Mem chunks that want freeing **/

/****************************************************************/

struct MemLock *AllocMemLock()
{
struct MemLock *ml;

ml=(struct MemLock *)AllocMem(sizeof(struct MemLock),MEMF_CLEAR);
Forbid();
AddTail(&locked, ml);
Permit();
return(ml);
}

/****************************************************************/

int InitUnlockedFreeMem()
{
NewList(&locked);
NewList(&unlocked);
NewList(&chunks);
return(NO_ERROR);
}		

/****************************************************************/

void UnInitUnlockedFreeMem()
{
struct MinNode *node;

for (node=chunks.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	Alert(S16A_LOCK_FREE_MEM1,FindTask(0));

for (node=unlocked.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	Alert(S16A_LOCK_FREE_MEM2,FindTask(0));

for (node=locked.mlh_Head; node->mln_Succ; node = node->mln_Succ)
	Alert(S16A_LOCK_FREE_MEM3,FindTask(0));

}

/****************************************************************/

void UnlockMem(ml)
struct MemLock *ml;
{
struct MinNode *node;
struct ChunkToFree *c;

/** A lock is now totaly unlocked, so first see if a previous **/
/** call to UnlockFreeMem() has tried to free this memory **/

Forbid();
for (node=chunks.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	c=(struct ChunkToFree *)node;
	if (c->mem==ml->mem) {
		Remove(c);
		Remove(ml);
		Permit();
		SlowFreeMem(c->mem, c->size);
		FreeMem(c, sizeof(struct ChunkToFree));
		FreeMem(ml, sizeof(struct MemLock));
		return;
		}
	}

/** No call to UnlockFreeMem, so add to list of locks ready to free **/
Remove(ml);
AddTail(&unlocked, ml);
Permit();
}

/*############################################################
NAME	
	UnlockedFreeMem
FUNCTION
	This function is similar to the Exec FreeMem() call, except
	it will wait for the passed memory pointer to become
	"unlocked" if it notices that the memory pointer is locked.
	
	Memory becomes locked by using it as an argument in a call
	to BroadcastAndLock().  Once locked, the memory becomes
	unlocked when all broadasted messages have been replyed
	to.

	This allows you to allocate memory, broacast (via
	BroadcastAndLock()) a pointer to this memory, then
	free the memory via UnlockFreeMem() without worrying if
	the receiving tasks have processed their messages yet.

	All receiving tasks must not ReplyMsg() untill they
	have finished using the locked memory completely.
INPUTS
	mem	- pointer to memory to free
	size	- number of bytes to free
RESULTS
	NONE
##############################################################*/

void UnlockedFreeMem(mem, size)
APTR mem;
unsigned long size;
{
struct MinNode *node;
struct ChunkToFree *c;
struct MemLock *ml;

/** Program wants to free memory.  Before we can free it however, **/
/** We must make sure it has been registered as unlocked **/

Forbid();
for (node=unlocked.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	ml=(struct MemLock *)node;
	if (ml->mem==mem) {
		Remove(ml);
		Permit();
		SlowFreeMem(mem, size);
		FreeMem(ml, sizeof(struct MemLock));
		return;
		}
	}

/** memory not in the unlocked list, so check the locked list **/

for (node=locked.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
	ml=(struct MemLock *)node;
	if (ml->mem==mem) {
		/** No call to UnlockMem, so add to list of mem chunks waiting to **/
		/** be freeed **/
		c=(struct ChunkToFree *)AllocMem(sizeof(struct ChunkToFree), MEMF_CLEAR);
		c->mem=mem;
		c->size=size;
		AddTail(&chunks, c);
		Permit();
		return;
		}
	}

/** This memory has never been Locked so free it now **/
/** This occurs if no call to BroadcastAndLock() was ever made **/

SlowFreeMem(mem, size);
Permit();
}

