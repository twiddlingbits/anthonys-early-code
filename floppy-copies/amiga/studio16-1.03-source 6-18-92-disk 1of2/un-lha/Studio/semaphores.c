#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"

struct SharedLock *FindLockGranted();
struct SharedLock *CreateLockPending();
void ReleaseAnySemaphore();
void SignalNextTask();

struct Task *FindTask();

/************************************************************/
/************************************************************/

void InitSharedSemaphore(ss)
struct SharedSemaphore *ss;
{
NewList(&ss->pending_shared);
NewList(&ss->pending_exclusive);
NewList(&ss->granted_shared);
ss->granted_exclusive=NULL;
}

/************************************************************/

void ObtainSharedSemaphore(ss)
struct SharedSemaphore *ss;
{
struct SharedLock *sl;

/** Has a shared or exclusive lock already been granted to this task? **/

Forbid();
sl = FindLockGranted(ss);
if (sl) {
	sl->count++;
	Permit();
	return;
	}

/** Is there another tasks' exlusive lock on on this semaphore? **/

if (ss->granted_exclusive) {
	sl = CreateLockPending(ss);	/** Create a pending shared lock **/
	if (sl) {
		/** Wait for ReleaseXXXXSemaphore to grant me access **/
		Permit();
		if (Wait(1<<sl->sigbit)!=(1<<sl->sigbit)) {
/*			kprintf("Semaphore Wait error!\n"); */
			}
		return;
		}
	}

/** No exlusive locks & no previous shared locks by this task **/

else {	
	sl = CreateLockPending(ss);	/** Create a pending shared lock **/
	if (sl) {
		/** Is someone requesting an exlusive lock? **/

		if (ss->pending_exclusive.mlh_Head->mln_Succ) {
			/** We can't grant anymore sharedlocks until the exlusives are granted **/
			Permit();
			if (Wait(1<<sl->sigbit)!=(1<<sl->sigbit)) {
				/* kprintf("Semaphore Wait error!\n"); */
				}
			return;
			}
		sl->count=1;
		Remove(sl);			/** remove from the pending list **/
		AddTail(&ss->granted_shared, sl);	/** add to granted list **/	
		}
	}
Permit();
}

/************************************************************/

struct SharedLock *FindLockGranted(ss)
struct SharedSemaphore *ss;
{
struct MinNode *minnode;
struct Task *task;
struct SharedLock *sl;

task = FindTask(0);

for (minnode=ss->granted_shared.mlh_Head; minnode->mln_Succ; minnode = minnode->mln_Succ) {
	sl = (struct SharedLock *)minnode;
	if (sl->task==task) {
		return(sl);
		}
	}

if (ss->granted_exclusive) {
	if (ss->granted_exclusive->task==task) {
		return(ss->granted_exclusive);
		}
	}

return(NULL);
}


/************************************************************/

struct SharedLock *CreateLockPending(ss)
struct SharedSemaphore *ss;
{
struct SharedLock *sl;

sl = (struct SharedLock *)AllocMem(sizeof(struct SharedLock),MEMF_CLEAR);
if (sl) {
	sl->task=FindTask(0);
	/*	sl->count=0; done by MEMF_CLEAR */
	sl->sigbit = AllocSignal(-1);
	AddTail(&ss->pending_shared, sl);
	}
return(sl);
}

/************************************************************/

void ReleaseSharedSemaphore(ss)
struct SharedSemaphore *ss;
{
ReleaseAnySemaphore(ss);
}

/************************************************************/

void ReleaseAnySemaphore(ss)
struct SharedSemaphore *ss;
{
struct SharedLock *sl;
/*char bug[80];*/

Forbid();

sl=FindLockGranted(ss);
if (sl==0) {		/** Can't find this semaphore?? **/
/*	sprintf(bug,"\007ReleaseSharedSemaphore Underflow A Task '%s'!!!\n",(struct Task *)FindTask(0)->tc_Node.ln_Name);
	kprintf(bug);*/
	Alert(0x01000008,FindTask(0));	/** AN_SemCorrupt **/
	Permit();
	return;
	}

sl->count--;

if (sl->count>0) {	/* Does this task have more nested locks? */
	Permit();
	return;
	}

if (sl->count==0) {
	if (sl==ss->granted_exclusive) {
		ss->granted_exclusive=NULL;
		}
	else {		/** must be shared sempahore **/
		Remove(sl);
		}
	SignalNextTask(ss);
	FreeSignal(sl->sigbit);
	FreeMem(sl, sizeof(struct SharedLock));
	}

else {	/** must be sl->count < 0 which can't happen **/
	Alert(0x01000008,FindTask(0));	/** AN_SemCorrupt **/
/*	kprintf("\007ReleaseSharedSemaphore Underflow B !!!\n");*/
	}

Permit();
}

/************************************************************/

void SignalNextTask(ss)
struct SharedSemaphore *ss;
{
struct SharedLock *sl;
struct MinNode *minnode;

/** Any pending exlusives get priority **/

if (ss->pending_exclusive.mlh_Head->mln_Succ) {

	/** Exclusive waiting, but can't trigger till all shared locks released **/

	if (ss->granted_shared.mlh_Head->mln_Succ==NULL) {
		sl=(struct SharedLock *)ss->pending_exclusive.mlh_Head;
		Remove(sl);		/** remove from pending list **/
		sl->count++;	/** count could be > 0 if sl used to be shared */
		ss->granted_exclusive=sl;
		Signal(sl->task, 1<<sl->sigbit);
		}
	else {
		/** Don't signal anyone.  Waiting for all shared locks to free **/
		return;
		}
	}

/** if no waiting exclusive then trigger all waiting shared **/

else {
	for (minnode=ss->pending_shared.mlh_Head; minnode->mln_Succ; minnode = minnode->mln_Succ) {
		sl = (struct SharedLock *)minnode;
		Remove(sl);		/** remove from pending list **/
		sl->count=1;	/** task is one level deep **/
		AddTail(&ss->granted_shared, sl);	/** add to granted list **/	
		Signal(sl->task, 1<<sl->sigbit);
		}		
	}
}

/************************************************************/

void ObtainExclusiveSemaphore(ss)
struct SharedSemaphore *ss;
{
struct SharedLock *sl;

/** Current task already have an exclusive lock ? **/

Forbid();

if (ss->granted_exclusive) {
	if (ss->granted_exclusive->task==FindTask(0)) {
		ss->granted_exclusive->count++;
		Permit();
		return;
		}
	}


/** If there is no granted exclusive locks, and there is only one **/
/** shared (lock who is this task), move the shared lock to exlusive **/

if (ss->granted_shared.mlh_Head->mln_Succ &&
		ss->granted_shared.mlh_Head->mln_Succ->mln_Succ==NULL &&
		ss->granted_exclusive==NULL) {
		sl=(struct SharedLock *)ss->granted_shared.mlh_Head;
		if (sl->task==FindTask(0)) {
			Remove(sl);
			ss->granted_exclusive=sl;
			sl->count++;
			Permit();
			return;
			}
		}

/** Is it OK for me to take the exclusive lock? **/
/** (no granted shared & no granted exlusive) **/

if (ss->granted_shared.mlh_Head->mln_Succ==NULL &&
		ss->granted_exclusive==NULL) {
	sl = (struct SharedLock *)AllocMem(sizeof(struct SharedLock),MEMF_CLEAR);
	if (sl) {
		sl->task=FindTask(0);
		sl->count=1;
		sl->sigbit = AllocSignal(-1);
		ss->granted_exclusive=sl;
		}
	Permit();
	return;
	}


/** Do we already have a shared lock?  If so, make it exclusive **/
sl = FindLockGranted(ss);	
if (sl==NULL)  {
	/** Add a pending exclusive lock and wait to be signaled **/
	sl = CreateLockPending(ss);
	}
if (sl) {
	Remove(sl);			/** Move it from shared to exlusive **/
	AddTail(&ss->pending_exclusive, sl);	

	/** Wait for ReleaseXXXXSemaphore to grant me access **/

	Permit();		
	if (Wait(1<<sl->sigbit)!=(1<<sl->sigbit)) {
/*		kprintf("Semaphore Wait error!\n");*/
		}
	return;
	}
Permit();
}

/************************************************************/

void ReleaseExclusiveSemaphore(ss)
struct SharedSemaphore *ss;
{
ReleaseAnySemaphore(ss);
}
