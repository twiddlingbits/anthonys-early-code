#include "exec/types.h"
#include "exec/exec.h"
#include "/include/studio16.h"

/** do not use -b0 when compiling this module **/

struct studioBase *studioBase;

void main()

{
struct MinNode *node;
struct Node *bignode; 
struct Module *mod;
struct Disk_Samp *samp;
struct NotifyLink *link;
int i;
struct AudioHandler *ah;

studioBase=(struct StudioBase *)OpenLibrary("studio.library",0);
if (studioBase==NULL) {
	exit(10);
	}

ObtainSemaphore(&studioBase->lock);
for (node=studioBase->modules.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   mod=(struct Module *)node;
	printf("mod %s %x\n",mod->name, mod);
	}

for (node=studioBase->samps.mlh_Head; node->mln_Succ; node = node->mln_Succ) {
   samp=(struct Disk_Samp *)node;
	printf("samp %s %x\n",samp->name, samp);
	}

for (bignode=studioBase->handlers.lh_Head; bignode->ln_Succ; bignode = bignode->ln_Succ) {
   ah=(struct AudioHandler *)bignode;
	printf("audio handler %s %x bits %d\n",ah->name, ah, ah->num_bits);
	}

for (i=0; i < studioBase->max_registered_ids; i++) {
	node=studioBase->notifyarray[i].mlh_Head;
	while(node->mln_Succ) {
		link=(struct NotifyLink *)node;
		printf("Notify mod %x on %s\n",link->notifyme,studioBase->eventidarray[i]);
		node=node->mln_Succ;
		}
	}

ReleaseSemaphore(&studioBase->lock);


if (studioBase)
	CloseLibrary(studioBase);
}

