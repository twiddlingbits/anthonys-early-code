#include <exec/types.h>
#include <exec/exec.h>
#include <exec/tasks.h>
#include "/include/studio16.h"

/************************************************************/

void StackCheck(str)
char *str;
{
struct Task *task;
unsigned long use,size;
unsigned long thisfunc=16;	/** stack space used by this func **/

task=(struct Task *)FindTask(0);
size=(unsigned long)task->tc_SPUpper-(unsigned long)task->tc_SPLower;
use = ((unsigned long)task->tc_SPUpper-(unsigned long)GetSP()+thisfunc)*100/size;

if (use+4000 > size)
	Alert(S16A_STACK_WARNING,FindTask(0));
}

/************************************************************/

void MarkStack()
{
struct Task *task;
short *ptr;
short code = 0xABCD;

task=(struct Task *)FindTask(0);
for (ptr=(short *)task->tc_SPLower; (unsigned long)ptr < GetSP(); ptr++)
	*ptr=code;
}

/************************************************************/

unsigned long FindMaxStackSize()
{
char s[80];
struct Task *task;
short *ptr;
short code = 0xABCD;
unsigned long use,size;
unsigned long thisfunc=102;	/** stack space used by this func **/

task=(struct Task *)FindTask(0);

for (ptr=(short *)task->tc_SPLower; (unsigned long)ptr < GetSP(); ptr++)
	if (*ptr!=code)
		break;

size=(unsigned long)task->tc_SPUpper-(unsigned long)task->tc_SPLower;
use = ((unsigned long)task->tc_SPUpper-(unsigned long)ptr+thisfunc)*100/size;
sprintf(s,"Task '%s' used %d percent of stack size %d\n",task->tc_Node.ln_Name,use,size);
/*kprintf(s);*/
if (use+4000 > size)
	Alert(S16A_STACK_WARNING,FindTask(0));
return(use);
}
