#include <windows.h>
#include "stack.h"

/*
 * For all Stack routines, SP points to an empty entry that is
 * the next entry to use when pushing on to the stack.
 */

BOOL PushStack(struct Stack* stack, unsigned short x, unsigned short y)
{	
	if (stack->sp == MAX_STACKSIZE)
		return FALSE;

	stack->x[stack->sp] = x;
	stack->y[stack->sp++] = y;
	return TRUE;
}


BOOL PopStack(struct Stack* stack, unsigned short *x, unsigned short *y)
{	
	if (stack->sp == 0)
		return FALSE;

	--stack->sp;

	*x = stack->x[stack->sp];
	*y = stack->y[stack->sp];
	return TRUE;
}

void InitStack(struct Stack* stack)
{
	stack->sp=0;
}
