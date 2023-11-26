#include <exec/types.h>
#include <exec/exec.h>
#include <exec/tasks.h>

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
/**char s[80];**/
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
/**sprintf(s,"Task '%s' used %d percent of stack size %d\n",task->tc_Node.ln_Name,use,size);**/
/*kprintf(s);*/
if (use+1000 > size)
	Alert(0x35100007,FindTask(0));
return(use);
}
