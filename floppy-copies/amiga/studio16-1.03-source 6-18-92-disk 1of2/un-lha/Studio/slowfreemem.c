#include <exec/types.h>
#include <exec/exec.h>
#include "/include/studio16.h"

struct MsgPort *freemem_port=0;
struct Task *freemem_task;

void HandleFreeNoticeTask();
void UnInitSlowFreeMem();
void SendFreeMem();
void HandleFreeNotice();
void SlowFreeMem();

#if 0 /*** TEST CODE **/
main()
{
int i;
char *p[10];

InitSlowFreeMem();

for (i=0; i < 10; i++)
	p[i]=(char *)AllocMem((i+1)*1000, MEMF_CLEAR);

for (i=0; i < 10; i++)
	if (p[i])
		FreeMem(p[i],(i+1)*1000);

for (i=0; i < 10; i++)
	p[i]=(char *)AllocMem((i+1)*1000, MEMF_CLEAR);

for (i=0; i < 10; i++)
	if (p[i])
		FreeMem(p[i],(i+1)*1000);

UnInitSlowFreeMem();
}

#endif

/****************************************************************/

int InitSlowFreeMem()
{
freemem_task=(struct Task *)CreateTask("HandleFreeNoticeTask",-120,HandleFreeNoticeTask,4096);
if (freemem_task) {
	while(freemem_port==NULL) {
		WaitTOF();
		}
	return(NO_ERROR);
	}
return(OUT_OF_MEMORY);
}		

/****************************************************************/

void UnInitSlowFreeMem()
{
SendFreeMem(0,0);
while(freemem_port) {
	WaitTOF();
	}
}

/****************************************************************/

void SlowFreeMem(mem, size)
char *mem;
unsigned long size;
{
if (size)
	SendFreeMem(mem, size);
}

/****************************************************************/

void SendFreeMem(mem, size)
char *mem;
unsigned long size;
{
struct FreeNotice *e;

e=(struct FreeNotice *)AllocMem(sizeof(struct FreeNotice), MEMF_CLEAR);
if (e) {
	e->message.mn_Node.ln_Type = NT_MESSAGE;
	e->message.mn_Length = sizeof(struct FreeNotice);
	e->mem=mem;
	e->size=size;
	PutMsg(freemem_port, e);
	}
else {
	FreeMem(mem, size);
	}
}

/****************************************************************/

void HandleFreeNoticeTask()
{
struct FreeNotice *e;

freemem_port=(struct MsgPort *)CreatePort(0,0);

while (TRUE) {
	Wait(1<<freemem_port->mp_SigBit);
	while (e = (struct FreeNotice *)GetMsg(freemem_port)) {
		HandleFreeNotice(e);
		}
	}
}

/****************************************************************/

void HandleFreeNotice(e)
struct FreeNotice *e;
{
if (e->size) {
/*kprintf("Did Slow Freemem %04x%04x\n",e->size);*/
	FreeMem(e->mem, e->size);
	FreeMem(e, sizeof(struct FreeNotice));
	}
else {		/** size==NULL means kill task **/
	FreeMem(e, sizeof(struct FreeNotice));
	DeletePort(freemem_port);
	freemem_port=NULL;
	DeleteTask(freemem_task);	/** Pop! **/
	}
}