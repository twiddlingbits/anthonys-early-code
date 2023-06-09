//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>
#include <ctype.h> 		// isdigit
#include <malloc.h>
#include <memory.h>		// memset
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "basic.h"

void SkipWhiteCode(struct BasicCode *code)
{
	while (isspace(*code->pc))	// skip white space (any control code or space)
		code->pc++;  
}


bool IsEosToken(unsigned char token)
{
	return (token == 0 || token == ':' || token==TOK_ELSE);	// else can terminate a THEN statement
}


void SkipToEOS(struct BasicCode *code)
{
	while (!IsEosToken(*code->pc))	// skip to EOS
			code->pc++;
}

unsigned char NextTokenOrChar(struct BasicCode *code)
{
	while (isspace(*code->pc))	// skip white space
		code->pc++;
	
	return *code->pc;
}

int GotoLine(unsigned int linenum, struct BasicCode *code)
{
	unsigned int next_addr = 0;
	unsigned int this_ln, i;

	while (TRUE) 
	{
		i = next_addr;
		next_addr = code->image[i] + (code->image[i+1]<<8);
		if (next_addr == 0)
			break;
		this_ln = code->image[i+2] + (code->image[i+3]<<8);

		if (this_ln == linenum)
		{
			code->pc = &code->image[i];		
			return ERR_NORMAL_EOL;	
		}

	} 

	return ERR_MISSING_LN;
}

int NextAtoI(struct BasicCode *code)
{
	int value = 0;
	
	while (uisdigit(*code->pc))
	{
		value = value*10;
		value = value + *code->pc-'0';
		code->pc++;
	}

	return value;
}

int DoPrint(struct BasicCode *code)
{
	unsigned int len;
	int err;
	unsigned char token;
	struct TypedValue v, v2;
	unsigned char b[256];
	short sp;
	bool suppress_newline = FALSE;
	struct IoConsole *io;

	/* determine which console to print to */

	if (*(code->pc-1)==TOK_LPRINT)
	{
		if (code->print_con==NULL)
		{
			if (WinLprintMessage()) // Print to file? Msg
				code->print_con=IoFileWriteOpen(NULL);

			if (code->print_con==NULL)
				code->print_con=IoNullOpen();

			if (code->print_con==NULL)
				return ERR_INTERNAL;
		}

		io = code->print_con;
	}
	else
	{
		io = code->std_con;
	}


	/*
	 * PRINT @ expression?
	 */

	token = NextTokenOrChar(code);

	if (token=='@')	
	{
		code->pc++;	// skip @
		err = EvalExpression(code->pc, &len, code, &v);
		if (err!=ERR_OKAY)
			return err;

		code->pc = code->pc + len;

		if (v.type_flags&VT_STRING)
		{
			free(v.value.string);
			return ERR_TM;

		}
		else if (IsNumber(&v))
		{
			int i = IntVal(&v);
			if (i < 0 || i >= io->io_width*io->io_height)
				return ERR_FC;
			io_set_cursor(io, i);
		}
		else
			return ERR_SYNTAX;

		if (IsEosToken(token=NextTokenOrChar(code)))	/* L2 Basic handels "PRINT@256" by moving cursor then SN Error*/
			return ERR_SYNTAX;
		
		if (token!=',')
			return ERR_SYNTAX;

		code->pc++;		/* skip comma */

	}

	/*
	 * PRINT USING has two forms:
	 *   PRINT USING <STRING>;<NUMBER>  e.g PRINT USING "##.#";3.4
	 *   PRINT USING <STRING>;<STRING>;...   e.g PRINT USING "!";"ABC";"DEF"
	 *   currently only support the first one
	 */

	token = NextTokenOrChar(code);

	if (NextTokenOrChar(code)==TOK_USING)	
	{
		code->pc++;
		err = EvalExpression(code->pc, &len, code, &v);
		if (err!=ERR_OKAY)
			return err;

		code->pc = code->pc + len;

		if (NextTokenOrChar(code)==';')
			code->pc++;

		err = EvalExpression(code->pc, &len, code, &v2);
		if (err!=ERR_OKAY)
		{
			if (v.type_flags&VT_STRING)	
				free(v.value.string);
			return err;
		}

		code->pc = code->pc + len;

		if (v.type_flags&VT_STRING && IsNumber(&v2))
			io_printusingnum(io, v.value.string, DoubleVal(&v2));
		else 
		{
			io_putstr(io, "? UNSUPORTED USE OF PRINT USING");
			SkipToEOS(code);
		}
		if (v.type_flags&VT_STRING)
			free(v.value.string);

		if (v2.type_flags&VT_STRING)
			free(v2.value.string);
		
		token = NextTokenOrChar(code);
		if (token!=';')
			io_putc(io, '\n');
		else
		{
			code->pc++;
			token = NextTokenOrChar(code);
		}

		if (IsEosToken(token))
			return ERR_OKAY;
		else
			return ERR_SYNTAX;
	}

	/*
	 * <expression | TAB> [; | ,] [<expression | TAB>] ...
	 * E.g:  PRINT
	 *       PRINT A;
	 *       PRINT "x=";x
	 *       PRINT "x="x
	 *		 PRINT @100,"fred"
	 *		 PRINT ATAB(5)B
	 *       PRINT ;;;;;;A
	 *		 PRINT ,,A
	 */

	while (TRUE)
	{
		token = NextTokenOrChar(code);
		if (IsEosToken(token))	
		{
			if (!suppress_newline)
				io_putc(io, '\n');
			return ERR_OKAY;
		}

		suppress_newline = FALSE;
		
		/*
		 * a comma advances to next 16 char print zone
		 */
		if (token==',')
		{
			code->pc++;

			sp = 16-(io_get_cursor(io)%16);

			for (int i=0; i < sp; i++)
				io_putc(io, ' ');

			suppress_newline = TRUE;
		}

		/*
		 * TAB(expression) moves the cursor on the current line to position <expression>
		 */
		else if (token==TOK_TAB)  
		{
			code->pc++;	/* Skip TAB( */
			err = EvalExpression(code->pc, &len, code, &v);
			if (err!=ERR_OKAY)
				return err;

			code->pc = code->pc + len;

			if (v.type_flags&VT_STRING)
			{
				free(v.value.string);
				return ERR_TM;

			}
			else if (IsNumber(&v))
			{
				short i = IntVal(&v);
				if (i < 0 || i > 255)
					return ERR_FC;
				
				if (io->io_width>0)
					sp = i-(io_get_cursor(io)%io->io_width);
				else
					sp = i-io_get_cursor(io);

				for (i=0; i < sp; i++)
					io_putc(io, ' ');
			
			}
			else
				return ERR_SYNTAX;

			if (NextTokenOrChar(code)!=')')
				return ERR_SYNTAX;
			code->pc++;	// skip )
		}
		else if (token==';')
		{
			code->pc++;
			suppress_newline = TRUE;
		}

		/*
		 * eval and print expression 
		 */
		else
		{
			err = EvalExpression(code->pc, &len, code, &v);
			if (err!=ERR_OKAY)
				return err;

			code->pc = code->pc + len;

			if (v.type_flags&VT_STRING)
			{
				if (strlen(v.value.string) > sizeof(b)-1)
					sprintf((char*)b, "<INTERNAL ERR: STRING TOO LONG>");
				else
					sprintf((char*)b, "%s", v.value.string);
				free(v.value.string);

			}
			else if (IsNumber(&v))
			{
				if (v.type_flags==VT_DOUBLE)
					sprintf((char*)b, "% .14g ", v.value.doubleval); 
				else
					sprintf((char*)b, "% g ",FloatVal(&v));
			}
			else
				return ERR_SYNTAX;

			io_putstr(io, (char*)b);	
		}
	}
}


int DoIf(struct BasicCode *code)
{
	unsigned int len;
	struct TypedValue r;
	int err;
	unsigned char token;
	
	err = EvalExpression(code->pc, &len, code, &r);
	if (err!=ERR_OKAY)
		return err;
	
	code->pc = code->pc + len;

	if (r.type_flags&VT_STRING && r.value.string)
		free (r.value.string);

	if (!IsNumber(&r))
		return ERR_TM;
	
	if (IntVal(&r)!=0)  // expression evaluated TRUE
	{
		token = NextTokenOrChar(code);
		if (token == TOK_THEN)
		{
			code->pc++;		// skip optional THEN
			token = NextTokenOrChar(code);
		}

		if (uisdigit(token))
			return GotoLine(NextAtoI(code), code);

		else
			return ERR_NORMAL_CONT;	// resume execution on code at PC
	}

	// search for else (allow nested if-then-else)
	short nest = 0;
	while (*code->pc!=0x00)	// Find end of line
	{
		if (*code->pc==TOK_IF)
			nest++;
		else if (*code->pc==TOK_ELSE)
		{
			if (nest == 0)
			{
				code->pc++;	// skip ELSE token
				token = NextTokenOrChar(code);

				if (uisdigit(token))	// ELSE line_number
					return GotoLine(NextAtoI(code), code);
				else
					return ERR_NORMAL_CONT;	// resume execution on code at PC
			}
			nest--;
		}
		code->pc++;
	}

	return ERR_OKAY;
}

int DoRem(struct BasicCode *code)
{
	while (*code->pc)
		code->pc++;		// Skip to end-of-line marker (NULL)
	return ERR_OKAY;
}

int DoLet(struct BasicCode *code, VariableValue** varptr, unsigned char* typeptr)
{
	int err;
	unsigned int len;
	unsigned char type;
	struct TypedValue vval;
	VariableValue* vlocptr;
	unsigned char token;

/*
 * Decode variable name
 */
	err = ParseVariable(code, code->pc, &vlocptr, &type, &len);
	if (err!=ERR_OKAY)
		return err;

	code->pc+= len;
	if (varptr)
		*varptr = vlocptr;
	if (typeptr)
		*typeptr = type;

/*
 * Make sure equal sign is next:  LET A1 = EXPRESSION
 */
	token = NextTokenOrChar(code);

	if (token!=TOK_EQ)
		return ERR_SYNTAX;

	code->pc++;			// skip over equal sign
	
/*
 * evaluate expression & Set variable
 */

	err = EvalExpression(code->pc, &len, code, &vval);
	if (err!=ERR_OKAY)
		return err;
	
	code->pc+=len;

	if (IsNumberT(type) && IsNumber(&vval))
		ConvertType(&vval, type);
	else if (type!=vval.type_flags)	// one number, one string
		return ERR_TM;

	SetVariable(vlocptr, vval.value, type);

	return ERR_OKAY;
}


int DoGoto(struct BasicCode *code)
{
	unsigned char token;

	token = NextTokenOrChar(code);
	if (uisdigit(token))
		return GotoLine(NextAtoI(code), code);
	else
		return ERR_SYNTAX;
}

int GosubLine(unsigned int linenum, struct BasicCode *code)
{
	struct StackFrame frame;
	
	// SkipWhiteCode(code); Is the right command here, but it detects syntax errors that Real L2 TRS Basic doesn't
	SkipToEOS(code);
	
	frame.type = TOK_GOSUB;
	frame.gosub_frame.return_pc=code->pc;
	frame.gosub_frame.current_line_start_pc=code->current_line_start_pc;
	frame.gosub_frame.current_statement_start_pc=code->current_statement_start_pc;
	if (!PushStack(&code->stack, &frame))
		return ERR_OUTOFMEM;

	return GotoLine(linenum, code);
}

int DoGosub(struct BasicCode *code)
{
	unsigned char token;
	int dest;

	token = NextTokenOrChar(code);
	if (!uisdigit(token))
		return ERR_SYNTAX;

	dest = NextAtoI(code);

	return GosubLine(dest, code);
}

/*
 * GOSUB RETURN
 * Pop the return value of the stack.  Unwind any FOR stack entries
 * until a gosub return is found.
 */

int DoReturn(struct BasicCode *code)
{
	StackFrame *frame;

	do
	{
		if (!PopStack(&code->stack, &frame))
			return ERR_RG;

	} while (frame->type!=TOK_GOSUB);


	code->pc=frame->gosub_frame.return_pc;
	code->current_line_start_pc=frame->gosub_frame.current_line_start_pc;
	code->current_statement_start_pc=frame->gosub_frame.current_statement_start_pc;

	return ERR_OKAY;
}

int DoFor(struct BasicCode *code)
{
	struct StackFrame frame;
	unsigned char token;
	unsigned int len;
	int err;
	struct TypedValue r;
/*
 * Start to build the FOR stack frame
 * First, set the variable index & assign the variable name
 */
	err=DoLet(code, &frame.for_frame.variable, &frame.for_frame.type_flags);
	if (err!=ERR_OKAY)
		return err;

/*
 * TO should be next, check for it then set the 'to' frame value
 */
	token = NextTokenOrChar(code);
	if (token!=TOK_TO)
		return ERR_SYNTAX;

	code->pc++;		// skip TO
	
	err = EvalExpression(code->pc, &len, code, &r);
	if (err!=ERR_OKAY)
		return err;
	
	code->pc = code->pc + len;

	if (r.type_flags&VT_STRING && r.value.string)
		free (r.value.string);

	if (!IsNumber(&r))
		return ERR_TM;

	ConvertType(&r, frame.for_frame.type_flags);
	CopyVariableValue(&frame.for_frame.to_val, &r.value);

/*
 * Check for optional STEP.  Set step value
 */
	
	r.type_flags=VT_INT;		// default step is 1
	r.value.intval=1;

	token = NextTokenOrChar(code);
	if (token==TOK_STEP)
	{
		code->pc++;		// skip STEP
	
		err = EvalExpression(code->pc, &len, code, &r);
		if (err!=ERR_OKAY)
			return err;
	
		code->pc = code->pc + len;

		if (r.type_flags&VT_STRING && r.value.string)
			free (r.value.string);

		if (!IsNumber(&r))
			return ERR_TM;
	}

	ConvertType(&r, frame.for_frame.type_flags);
	CopyVariableValue(&frame.for_frame.step_val, &r.value);
	SkipWhiteCode(code);
	frame.for_frame.start_pc = code->pc;
/*
 * FOR frame is built, push it on the stack.
 * If this FOR is already on the stack (in use), then reset stack to that point.
 * This handles cases like:
 *    10 For a=1 to 20
 *    20 For b=1 to 20
 *    30 IF x=1 then goto 10
 *    40 next b:next a
 */

	unsigned int frame_index;

	frame.type=TOK_FOR;
	frame.for_frame.current_line_start_pc=code->current_line_start_pc;
	frame.for_frame.current_statement_start_pc=code->current_statement_start_pc;

	if (SearchForStack(&code->stack, frame.for_frame.variable, &frame_index))
		UnwindStack(&code->stack, frame_index);

	PushStack(&code->stack, &frame);
	return ERR_OKAY;

}

/*
 * Handle:  "NEXT" or "NEXT X" or "NEXT X,Y,Z"
 * if a variable is given, and the last FOR frame pushed on the stack doesn't match,
 * then the stack is unwound while looking for a FOR that matches.
 * EG:
 *    FOR X=1 to 5
 *    FOR Y=1 to 10
 *    NEXT X
 * OR
 *    FOR X=1 to 5
 *    FOR Y=1 to 10
 *    IF Y < 3 THEN NEXT Y
 *    NEXT X
 *
 * both are legal.
 */

int DoNext(struct BasicCode *code)
{
	unsigned char token;
	VariableValue* value;
	unsigned char type;
	bool is_var;
	bool isdone;
	StackFrame* frame;
	int err;
	unsigned int i;

	token = NextTokenOrChar(code);
	if (isalpha(token))				// NEXT with no variable.
	{
		is_var = TRUE;
	}
	else
	{
		is_var = FALSE;
		if (!IsEosToken(token))
			return ERR_SYNTAX;
	}

	while (TRUE)
	{
		if (is_var)
		{
			err=ParseVariable(code, code->pc, &value, &type, &i);
			if (err!=ERR_OKAY)
				return err;
			code->pc+= i;
		}

		while (TRUE) {
			if (!PeekStack(&code->stack, &frame))
				return ERR_NF;

			if (frame->type!=TOK_FOR)
				return ERR_NF;

			if (!is_var)
				break;

			if (frame->for_frame.variable==value)
				break;
			
			if (!PopStack(&code->stack, 0))	// Next with wrong variable type uwinds stack, looking for correct kind
				return ERR_NF;
		}

		is_var = TRUE;

		if (frame->for_frame.type_flags&VT_INT)
		{
			frame->for_frame.variable->intval+= frame->for_frame.step_val.intval;
			if (frame->for_frame.step_val.intval > 0)
				isdone = frame->for_frame.variable->intval > frame->for_frame.to_val.intval;
			else
				isdone = frame->for_frame.variable->intval < frame->for_frame.to_val.intval;

		}
		else if (frame->for_frame.type_flags&VT_FLOAT)
		{
			frame->for_frame.variable->floatval+= frame->for_frame.step_val.floatval;
			if (frame->for_frame.step_val.floatval > 0)
				isdone = frame->for_frame.variable->floatval > frame->for_frame.to_val.floatval;
			else
				isdone = frame->for_frame.variable->floatval < frame->for_frame.to_val.floatval;
		}
		else if (frame->for_frame.type_flags&VT_DOUBLE)
		{
			frame->for_frame.variable->doubleval+= frame->for_frame.step_val.doubleval;
			if (frame->for_frame.step_val.doubleval > 0)
				isdone = frame->for_frame.variable->doubleval > frame->for_frame.to_val.doubleval;
			else
				isdone = frame->for_frame.variable->doubleval < frame->for_frame.to_val.doubleval;
		}
		else return ERR_INTERNAL;  // should never happen

		if (isdone)
			PopStack(&code->stack, &frame);
		else
		{
			code->pc=frame->for_frame.start_pc;
			code->current_line_start_pc=frame->for_frame.current_line_start_pc;
			code->current_statement_start_pc=frame->for_frame.current_statement_start_pc;
			return ERR_OKAY;
		}

		token = NextTokenOrChar(code);
		if (token==',')
		{
			code->pc++;
			token = NextTokenOrChar(code);
		}
		else
		{
			if (IsEosToken(token))
				return ERR_OKAY;
			else
				return ERR_SYNTAX;
		}
	}
}


int DoDim(struct BasicCode *code)
{
	unsigned char token;
	unsigned char type;
	unsigned short label;
	unsigned int i;
	unsigned int numdims;
	unsigned int* dimsizes;
	unsigned int* ap;
	unsigned int allocsize;
	int err;

	while (TRUE)
	{
/*
 * get variable label that will be an array
 */
		err=SimpleParseVariable(code->deftab, code->pc, &type, &label, &i);
		if (err!=ERR_OKAY)
			return err;
		code->pc+=i;

/*
 * make sure a ( is next
 */

		token = NextTokenOrChar(code);
		if (token!='(')
			return ERR_SYNTAX;
		code->pc++;
/*
 * determine the number of array dimensions (allow multi-dimensional arrays)
 * and value of each index
 */
		err = CalcIndexValues(code, code->pc, &numdims, &dimsizes, &i);
		if (err!=ERR_OKAY)
			return err;
		code->pc+=i;
/*
 * create array stucture
 * int 0: number of dimensions (n) 
 * int 1..n: size for each dimension
 * n X VariableValue's
 */
		allocsize = 0;		
		for (i=0; i <numdims; i++)
			allocsize = allocsize*(dimsizes[i]+1) + (dimsizes[i]+1);
	
		allocsize*=sizeof(union VariableValue);
		allocsize+=(1+numdims)*sizeof(int);

		ap=(unsigned int*)malloc(allocsize);
		if (ap==0)
			return ERR_OUTOFMEM;
		memset(ap, 0, allocsize);

		ap[0]=numdims;
		for (i=0; i <numdims; i++)
			ap[i+1]=dimsizes[i]+1;		// range is zero -> dim value, inclusive (1+dim value)

		free(dimsizes);

/*
 * create the array variable entry
 */

		err = CreateArray(code->variables, label, type, ap);
		if (err!=ERR_OKAY)
			return err;
/*
 * A , indicates another array to be dimensioned
 */
		token = NextTokenOrChar(code);
		if (token!=',')
			break;
		code->pc++;	// skip comma
	}		
	return ERR_OKAY;
}

/*
 * Used by input and READ to parse a user entered string
 * If the string is surrounded by quotes, it may contain any characters.
 * If it has no quotes, it is terminated by a return (null) or a comma
 */

int ParseInputString(const unsigned char* expr, struct TypedValue* v, unsigned int* len)
{
	const unsigned char* expr_start = expr;
	unsigned short k;

	if (*expr == '\"')
		return ParseStringLiteral(expr, v, len);

	k=0;
	while (expr[k] && expr[k]!=',')
		k++;

	v->value.string = (char*)malloc(k+1);
	if (v->value.string == 0)
		return ERR_OS;

	strncpy(v->value.string, (char*)expr, k);
	v->value.string[k] = 0;

	if (len)
		*len = k;

	v->type_flags = VT_STRING;

	return ERR_OKAY;
}


int DoInput(struct BasicCode *code)
{
	unsigned char token;
	char buffer[160];
	int err, bi;
	unsigned int len;
	unsigned char type;
	struct TypedValue vval;
	VariableValue* vlocptr;

	
	/*
	 * INPUT "optional quote";<variable> [, <variable>] ...
	 */

	token = NextTokenOrChar(code);
	if (token=='\"')
	{
		while (TRUE)
		{
			token = *(++code->pc); 
			if (token == '\"' || token==NULL)
				break;
			io_putc(code->std_con, token);
		}
		if (token!='\"')
			return ERR_SYNTAX;
		++code->pc;	// skip end quote
		token = NextTokenOrChar(code);
		if (token!=';')
			return ERR_SYNTAX;
		code->pc++;		// skip semicolon
	}

	io_putstr(code->std_con, "? ");
	if (io_gets(code->std_con, buffer)==0)		// the only error currently supported in gets is break triggered during gets
		return ERR_BREAK;

	bi =0;
	while (TRUE)
	{
		/*
		* Decode variable name
		*/
			err = ParseVariable(code, code->pc, &vlocptr, &type, &len);
			if (err!=ERR_OKAY)
				return err;

			code->pc+= len;

		/*
		* Parse user input and then Set variable value
		*/

		do
		{
			if (IsNumberT(type))
			{
				if (buffer[bi]==0)	// allow user to press return with no number entered for zero
				{
					vval.type_flags=VT_INT;
					vval.value.intval=0;
					err=ERR_OKAY;
					len =0;
				}
				else
					err = ParseNumberFull((unsigned char*)&buffer[bi], &vval, &len, type==VT_DOUBLE);
			}
			else
				err = ParseInputString((unsigned char*)&buffer[bi], &vval, &len);
			if (err!=ERR_OKAY)
			{
				io_putstr(code->std_con, "?REDO\n? ");
				if (io_gets(code->std_con, buffer)==0)		// the only error currently supported in gets is break triggered during gets
					return ERR_BREAK;
				bi = 0;
			}
		} while (err!=ERR_OKAY);

		bi+=len;

		if (IsNumberT(type) && IsNumber(&vval))
			ConvertType(&vval, type);
		else if (type!=vval.type_flags)	// one number, one string
			return ERR_INTERNAL;		// shouldn't happen

		if (type==VT_STRING)	// TRS80 L2 Compatibility Hack: If user press return on string input, keep string var the same
			if (strlen(vval.value.string)==0)
			{
				free(vval.value.string);
				vval.value.string=NULL;
			}

		if (!(type==VT_STRING && vval.value.string==NULL))
			SetVariable(vlocptr, vval.value, type);

		/*
		* More variables to INPUT?
		*/
			token = NextTokenOrChar(code);

			if (token!=',')
				break;

			code->pc++;			// skip over equal sign

			if (buffer[bi]==0)
			{
				io_putstr(code->std_con, " ?? ");
				if (io_gets(code->std_con, buffer)==0)		// the only error currently supported in gets is break triggered during gets
					return ERR_BREAK;
				bi=0;
			}
			else
				bi++;
	}

	if (buffer[bi]!=0)
		io_putstr(code->std_con, "?EXTRA IGNORED\n");

	SkipToEOS(code);	// For copatibility with L2 BASIC:  Allows garbage to terminate the input statement.
						// EG, INPUT PP;  Is tolerated.
	return ERR_OKAY;
}
	
int DoClear(struct BasicCode *code)
{
	BasicClear(code);
	
	SkipToEOS(code);

	return ERR_OKAY;
}

int DoCls(struct BasicCode *code)
{
	io_cls(code->std_con);
	return ERR_OKAY;
}

int DoDef(struct BasicCode* code, unsigned char type)
{
	unsigned char a,b;

	while (TRUE)
	{
		a =  NextTokenOrChar(code);
		if (!isalpha(a))
			return ERR_SYNTAX;
		a = tolower(a);
		code->pc++;
		b =  NextTokenOrChar(code);
		if (b==TOK_SUB)	// e.g. DEFINT a-z
		{
			code->pc++;
			b =  NextTokenOrChar(code);
			if (!isalpha(b))
				return ERR_SYNTAX;
			b = tolower(b);
			code->pc++;
			if (b<a)
				return ERR_SYNTAX;

			for ( ; a <= b; a++)
				code->deftab[a-'a'] = type;
		}
		else
		{
			code->deftab[a-'a'] = type;
		}	
		b =  NextTokenOrChar(code);
		if (IsEosToken(b))
			return ERR_OKAY;
		if (b!=',')
			return ERR_SYNTAX;
		code->pc++;	// skip comma
	}
}

int DoDefInt(struct BasicCode *code)
{
	return DoDef(code, VT_INT);
}

int DoDefSng(struct BasicCode *code)
{
	return DoDef(code, VT_FLOAT);
}

int DoDefDbl(struct BasicCode *code)
{
	return DoDef(code, VT_DOUBLE);
}

int DoDefStr(struct BasicCode *code)
{
	return DoDef(code, VT_STRING);
}

/*
 * The DATA statement is used by READ. 
 * If encoutered during execution, skip over it
 */

int DoData(struct BasicCode *code)
{
	SkipToEOS(code);

	return ERR_OKAY;
}

int NextReadData(struct BasicCode *code)
{
/* 
 * Initilize if READ never called before
 */
	if (code->readptr==0)						// never initilized before
	{
		code->readptr=&code->image[4];			// start of first statement
		if (*code->readptr==',')
			return ERR_SYNTAX;
	}
	
/* 
 * comma means in a read list (EG READ 10,15,"fred fred",frog)
 */
	if (*code->readptr==',' )
	{
		code->readptr++;
		return ERR_OKAY;
	}
/* 
 * In a statement, but need to search for another READ
 * Scan each statemenet looking for a TOK_READ, but ignoring
 * those found in quotes.  (some L2 BASIC programs have graphics
 * and other characters inside quoted strings.)
 */
	bool inquote;
	while (TRUE)
	{
		inquote = FALSE;
		while (*code->readptr)
		{
			if (*code->readptr==TOK_DATA && !inquote)
			{
				code->readptr++;
				return ERR_OKAY;
			}
			else if (*code->readptr=='\"')
				inquote = !inquote;

			code->readptr++;
		}
		
		code->readptr++;	// skip NULL EOS marker
/*
 * EOS. If the begining of the next statement is 0,0, then we have reached end of program
 */
		if (code->readptr >= &code->image[MAX_IMAGESIZE])
			return ERR_INTERNAL;
		
		if (*code->readptr==0 && *(code->readptr+1)==0)
			return ERR_OD;

		code->readptr+=4;  // skip next addr & line number
	}
}

int DoRestore(struct BasicCode *code)
{
	code->readptr=0;

	return ERR_OKAY;
}

int DoRandom(struct BasicCode *code)
{
	time_t timebuffer;

	time(&timebuffer );
	srand(timebuffer);

	return ERR_OKAY;
}

int DoRead(struct BasicCode *code)
{
	unsigned char token;
	int err;
	unsigned int len;
	unsigned char type;
	struct TypedValue vval;
	VariableValue* vlocptr;

	
	/*
	 * READ <variable> [, <variable>] ...
	 */

	while (TRUE)
	{
		/*
		* Decode variable name
		*/
			err = ParseVariable(code, code->pc, &vlocptr, &type, &len);
			if (err!=ERR_OKAY)
				return err;

			code->pc+= len;

		/*
		* evaluate expression & Set variable
		*/

		err=NextReadData(code);
		if (err!=ERR_OKAY)
			return err;	

		SkipWhiteSpace((const unsigned char**)&code->readptr);
			
		if (IsNumberT(type))
		{
			if (*code->readptr==',')	// DATA 1,,2 is allowed (,, returns zero)
			{
				len = 0;
				vval.value.intval=0;
				vval.type_flags=VT_INT;
				err = ERR_OKAY;
			}
			else
			{
				err = ParseNumberFull(code->readptr, &vval, &len, type==VT_DOUBLE);
			}
		}
		else
		{
			err = ParseInputString(code->readptr, &vval, &len);
		}
		if (err!=ERR_OKAY)
			return err;

		code->readptr+=len;

		if (IsNumberT(type) && IsNumber(&vval))
			ConvertType(&vval, type);
		else if (type!=vval.type_flags)	// one number, one string
			return ERR_INTERNAL;

		SetVariable(vlocptr, vval.value, type);

		/*
		* More variables to READ?
		*/
		token = NextTokenOrChar(code);

		if (token!=',')
			break;

		code->pc++;			// skip over equal sign
	}

	return ERR_OKAY;
}

/*
 * ON variable GOTO line1,line2, ...
 * ON variable GOSUB line1, line2, ...
 * ON ERROR GOTO linenumber
 */

int DoOn(struct BasicCode *code)
{
	unsigned int len;
	int k;
	struct TypedValue r;
	int err;
	unsigned char ontype;

	/* 
	 * First see if this is ON ERROR GOTO (different format)
	 */
	if (NextTokenOrChar(code)==TOK_ERROR)
	{
		code->pc++;
		if (NextTokenOrChar(code)!=TOK_GOTO)
			return ERR_SYNTAX;
		code->pc++;
		if (!uisdigit(NextTokenOrChar(code)))
			return ERR_SYNTAX;
		code->onerror=NextAtoI(code);
		return ERR_OKAY;
	}

	/*
	 * Evalute the the expression that selects the line number
	 */
	err = EvalExpression(code->pc, &len, code, &r);
	if (err!=ERR_OKAY)
		return err;
	
	code->pc = code->pc + len;

	if (r.type_flags&VT_STRING && r.value.string)
		free (r.value.string);

	if (!IsNumber(&r))
		return ERR_TM;

	/* 
	 * should be GOTO, GOSUB, or ERROR
	 */
	ontype = NextTokenOrChar(code);
	code->pc++;		// skip token

	/*
	 * get correct line number to jump to
	 * Zero or too large goes to next line, negative is an error.
	 */

	k = IntVal(&r);
	if (k < 0)
		return ERR_FC;

	if (k==0)
	{
		SkipToEOS(code);
		return ERR_OKAY;
	}

	while (k > 1)
	{
		while (*code->pc!=',' && !IsEosToken(*code->pc))
			code->pc++;

		if (IsEosToken(*code->pc))
			return ERR_OKAY; //	branch value too large (no matching line number), so just go to next line.
		k--;
		code->pc++;	// Skip comma
	}

	if (!uisdigit(NextTokenOrChar(code)))
		return ERR_SYNTAX;

	if (ontype==TOK_GOSUB)
		return GosubLine(NextAtoI(code), code);	
	else if (ontype==TOK_GOTO)
		return GotoLine(NextAtoI(code), code);	
	else
		return ERR_SYNTAX;
}

/*
 * RESUME			re-execute error line
 * RESUME 0			re-execute error line
 * RESUME line#		resume at line#
 * RESUME NEXT		resume at next statment after error statment
 */ 
int DoResume(struct BasicCode *code)
{
	unsigned char token;
	
	token = NextTokenOrChar(code);	

	if (IsEosToken(token))
		code->pc=code->err_stmt;

	else if (uisdigit(token))
	{
		int i = NextAtoI(code);

		if (i==0)
			code->pc=code->err_stmt;
		else
		{
			int err = GotoLine(i, code);

			if (err==ERR_MISSING_LN)
				code->onerror=0;	// turn off ON ERROR to prevent endless loop

			return err;
		}
	}

	else if (token==TOK_NEXT)
	{
		code->pc=code->err_stmt;
		SkipToEOS(code);
		return ERR_OKAY;
	}
	
	else
	{
		code->onerror=0;
		return ERR_SYNTAX;
	}

	return ERR_NORMAL_CONT;	// ExecuteStatment assumes that we return at the end of a statment,
							// unless ERR_NORMAL_CONT
}

/*
 * POKE only supported for screen memory
 */

int DoPoke(struct BasicCode *code)
{
	struct TypedValue v,v2;
	unsigned int len;
	int err;
	long loc;

	err = EvalExpression(code->pc, &len, code, &v);
	if (err!=ERR_OKAY)
		return err;

	code->pc = code->pc + len;

	if (v.type_flags&VT_STRING)
		free(v.value.string);

	if (NextTokenOrChar(code)==',')
		code->pc++;
	else
		return ERR_SYNTAX;

	err = EvalExpression(code->pc, &len, code, &v2);
	if (err!=ERR_OKAY)
	{
		if (v.type_flags&VT_STRING)	
			free(v.value.string);
		return err;
	}

	code->pc = code->pc + len;

	if (v2.type_flags&VT_STRING)
		free(v2.value.string);

	if (!(IsNumber(&v) && IsNumber(&v2)))
		return ERR_TM;

	loc = (long)FloatVal(&v);

	if (loc >= 15360 && loc < (15360+1024))
		io_set_c(code->std_con, loc-15360, (unsigned char)IntVal(&v2));

	return ERR_OKAY;

}

int DoSetReset(struct BasicCode *code, bool isset)
{
	struct TypedValue v,v2;
	unsigned int len;
	short x,y;
	int err;

	if (NextTokenOrChar(code)!='(')
		return ERR_SYNTAX;
	code->pc++;

	err = EvalExpression(code->pc, &len, code, &v);
	if (err!=ERR_OKAY)
		return err;

	code->pc = code->pc + len;

	if (v.type_flags&VT_STRING)
		free(v.value.string);

	if (NextTokenOrChar(code)==',')
		code->pc++;
	else
		return ERR_SYNTAX;

	err = EvalExpression(code->pc, &len, code, &v2);
	if (err!=ERR_OKAY)
	{
		if (v.type_flags&VT_STRING)	
			free(v.value.string);
		return err;
	}

	code->pc = code->pc + len;

	if (v2.type_flags&VT_STRING)
		free(v2.value.string);

	if (!(IsNumber(&v) && IsNumber(&v2)))
		return ERR_TM;

	x=IntVal(&v);
	y=IntVal(&v2);

	if (x <0 || x>=128 || y<0 || y>=48)
		return ERR_FC;

	if (!io_setreset(code->std_con, x, y, isset))
		return ERR_INTERNAL;	// x or y out of range

	if (NextTokenOrChar(code)!=')')
		return ERR_SYNTAX;

	code->pc++;

	return ERR_OKAY;
}

int DoSet(struct BasicCode *code)
{
	return DoSetReset(code, TRUE);
}

int DoReset(struct BasicCode *code)
{
	return DoSetReset(code, FALSE);
}

int DoRun(struct BasicCode *code)
{
	unsigned char token;
	int nextln;

	token = NextTokenOrChar(code);
	if (uisdigit(token))					// e.g RUN 500
	{
		nextln = NextAtoI(code);
		BasicSetDefaultState(code);		// resets PC
		return GotoLine(nextln, code);
	}
	else if (IsEosToken(token))			// RUN
	{
		BasicSetDefaultState(code);
		code->pc = &code->image[0];		
		return ERR_NORMAL_EOL;	
	}
	else
		return ERR_SYNTAX;	// either a syntax error or unsuported Run (e.g. run "filename")
}

/*
 * OUT port, value is not supported, but ignore it.
 */

int DoOut(struct BasicCode *code)
{
	SkipToEOS(code);

	if (code->debug_con)
		io_putstr(code->debug_con,"OUT unsupported and ignored\n");

	return ERR_OKAY;
}

/*
 * LINE starts a LINEINPUT A$
 * A DOS BASIC comand to input a string unaltered
 */

int DoLine(struct BasicCode *code)
{
	unsigned char token;
	char buffer[160];
	int err;
	unsigned int len;
	unsigned char type;
	struct TypedValue v;
	VariableValue* vlocptr;

	
	/*
	 * LINE INPUT "optional quote";<variable$> 
	 */
	if (NextTokenOrChar(code)!=TOK_INPUT)
		return ERR_SYNTAX;
	code->pc++;	// skip INPUT

	token = NextTokenOrChar(code);
	if (token=='\"')
	{
		while (TRUE)
		{
			token = *(++code->pc); 
			if (token == '\"' || token==NULL)
				break;
			io_putc(code->std_con, token);
		}
		if (token!='\"')
			return ERR_SYNTAX;
		++code->pc;	// skip end quote
		token = NextTokenOrChar(code);
		if (token!=';')
			return ERR_SYNTAX;
		code->pc++;		// skip semicolon
	}

	if (io_gets(code->std_con, buffer)==0)		// the only error currently supported in gets is break triggered during gets
		return ERR_BREAK;

	/*
	* Decode variable name
	*/
	err = ParseVariable(code, code->pc, &vlocptr, &type, &len);
	if (err!=ERR_OKAY)
		return err;

	code->pc+= len;

	/*
	* Parse user input and then Set variable value
	*/

	if (IsNumberT(type))
		return ERR_TM;

	v.value.string = (char*)malloc(strlen(buffer)+1);
	if (v.value.string == 0)
		return ERR_OS;

	strcpy(v.value.string, buffer);
	v.type_flags = VT_STRING;

	SetVariable(vlocptr, v.value, VT_STRING);

	return ERR_OKAY;
}


int ExecuteToken(unsigned char token, struct BasicCode *code)
{
	switch (token)
	{
	case TOK_PRINT:
	case TOK_LPRINT:
		return DoPrint(code);
	
	case TOK_IF:
		return DoIf(code);

	case TOK_APOS:
	case TOK_REM:
		return DoRem(code);

	case TOK_END:
		return ERR_NORMAL_END;

	case TOK_ELSE:
		return DoRem(code);		// probably the end of a THEN, terminate line.

	case TOK_LET:
		return DoLet(code, NULL, NULL);

	case TOK_GOTO:
		return DoGoto(code);

	case TOK_GOSUB:
		return DoGosub(code);

	case TOK_RETURN:
		return DoReturn(code);

	case TOK_FOR:
		return DoFor(code);

	case TOK_NEXT:
		return DoNext(code);

	case TOK_DIM:
		return DoDim(code);

	case TOK_INPUT:
		return DoInput(code);

	case TOK_CLEAR:
		return DoClear(code);

	case TOK_CLS:
		return DoCls(code);

	case TOK_DEFINT:
		return DoDefInt(code);

	case TOK_DEFSGN:
		return DoDefSng(code);

	case TOK_DEFDBL:
		return DoDefDbl(code);

	case TOK_DEFSTR:
		return DoDefStr(code);

	case TOK_DATA:
		return DoData(code);

	case TOK_READ:
		return DoRead(code);

	case TOK_RESTORE:
		return DoRestore(code);

	case TOK_RANDOM:
		return DoRandom(code);

	case TOK_ON:
		return DoOn(code);

	case TOK_RESUME:
		return DoResume(code);

	case TOK_POKE:
		return DoPoke(code);

	case TOK_SET:
		return DoSet(code);

	case TOK_RESET:
		return DoReset(code);

	case TOK_STOP:
		return ERR_BREAK;

	case TOK_RUN:
		return DoRun(code);

	case TOK_LINE:
		return DoLine(code);

	case TOK_OUT:
		return DoOut(code);

	case TOK_TO:
	case TOK_STEP:
	case TOK_THEN:
		return ERR_SYNTAX;

	default:
		if (IsBuiltinFunction(token))
			return ERR_SYNTAX;

		return ERR_UNDEFINED_TOKEN;
	}
}

int ExecuteStatement(struct BasicCode *code)
{
	unsigned char token;
	int err;

	WinTrsDelay();
	
	code->current_statement_start_pc = code->pc;

	if (code->tron)
		DebugShowTrace(code);

	if ((err=io_chk_brk(code->std_con))!=ERR_OKAY)
		return err;
	
	token = NextTokenOrChar(code);

	if (token&0x80)		// all tokens have the high bit set
	{
		code->pc++;
		err = ExecuteToken(token, code);

	}
	else if (token==':' || token==0) // NOP
	{
		err = ERR_OKAY;
	}
	else
		err = ExecuteToken(TOK_LET, code);

	if (err==ERR_NORMAL_CONT)	// See DoIf() for example uses
		return ERR_OKAY;

	if (err!=ERR_OKAY)
		return err;

	SkipWhiteCode(code);

	if (*code->pc==':')	// colon seperates statements
	{
		code->pc++;  
		return ERR_OKAY;			// return pointing to next statement
	}

	else if (*code->pc==TOK_ELSE || *code->pc==TOK_APOS)	
	{
		return ERR_OKAY;			// return pointing else or ' (REM shortcut), which will skip reset of line
	}

	else if (*code->pc==0)	// zero at the end of the line
	{
		code->pc++;  
		return ERR_NORMAL_EOL;		// return point past the end of line
	}
	
	return ERR_SYNTAX;
}

/*
 * image format is:
 * <2 bytes> addr of next statement
 * <2 bytes> line number in integer form
 * <n bytes> BASIC statment in tokenized form
 * <0> End-of-statment marker
 * repeat as needed
 *  <0 0> Two bytes of zero mark end
 */

int Interpret(struct BasicCode *code)
{

	int err;

	do 
	{
		if (*code->pc == 0 && code->pc[1]==0)
			return ERR_NORMAL_END;		// No END statment, but end of code

		code->current_line_start_pc = code->pc;
		code->pc = code->pc + 4;	// skip pointer & line number

		while ((err = ExecuteStatement(code)) == ERR_OKAY) ;

		/*
		 * Hanndle ON ERROR linenum
		 */

		if (!(err == ERR_NORMAL_END || err == ERR_NORMAL_EOL || err==ERR_BREAK || 
			  err==ERR_WINDOWCLOSED || err==ERR_INTERNAL || err==ERR_UNDEFINED_TOKEN))
		{
			if (code->onerror)
			{
				code->err_stmt = code->current_statement_start_pc;
				code->last_err = err;
				code->last_erl = code->current_line_start_pc[2] + (code->current_line_start_pc[3]<<8);
				err = ERR_OKAY;
				err = GotoLine(code->onerror, code);
			}
		}		
	} while (err==ERR_NORMAL_EOL);

	return err;
}

