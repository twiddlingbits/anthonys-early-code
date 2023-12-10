//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include "basic.h"
#include <string.h>

/*
 * For all Stack routines, SP points to an empty entry that is
 * the next entry to use when pushing on to the stack.
 */

bool PushStack(struct Stack* stack, struct StackFrame* frame)
{	
	if (stack->sp == MAX_STACKSIZE)
		return FALSE;

	stack->entrys[stack->sp++] = *frame;
	return TRUE;
}


bool PopStack(struct Stack* stack, StackFrame** rp)
{	
	if (stack->sp == 0)
		return FALSE;

	--stack->sp;

	if (rp)
		*rp = &stack->entrys[stack->sp];
	return TRUE;
}

bool PeekStack(struct Stack* stack, StackFrame** rp)
{	
	if (stack->sp == 0)
		return FALSE;

	 *rp = &stack->entrys[stack->sp-1];
	return TRUE;
}

/*
 * search the stack backwards for a specific FOR index.
 *
 * If a GOSUB stack is encountered, or end of stack, stop searching, and return err
 */
bool SearchForStack(struct Stack* stack, VariableValue* variable, unsigned int* frame_index)
{
	if (stack->sp == 0)
		return FALSE;
	
	*frame_index=stack->sp-1;
	while (TRUE)
	{
		if (stack->entrys[*frame_index].type==TOK_GOSUB)
			return FALSE;

		if (stack->entrys[*frame_index].for_frame.variable==variable)
		{	
			return TRUE;
		}

		if (*frame_index == 0)
			return FALSE;
		(*frame_index)--;
	}  
}

void UnwindStack(struct Stack* stack, unsigned int frame_index)
{
	stack->sp = frame_index;
}
