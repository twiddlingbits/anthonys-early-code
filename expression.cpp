//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>		// printf
#include <stdlib.h>		// exit()
#include <ctype.h> 		// isdigit
#include <string.h>		// strlen
#include <math.h>		// pow

#include "basic.h"



//************************
//
// Function: EvalExpression
// Example Use:	error = EvalExpression("3*4+(1>2)", &len, &result)
//

int EvalExpression(const unsigned char* expr, unsigned int* len, struct BasicCode* code, struct TypedValue* vr)
{

	int err;

	const unsigned char* expr1;
	expr1 = expr;

	err=DoEvalExpression(&expr, 0, code, vr);

	if (len)
		*len = (expr - expr1);

	return err;
}

//************************
// Function: DoEvalExpression
//
// Evaluate a numerical and/or string expression.
//
// How it works:
//
// DoEvalExpression scans left to right, looking for "number1", operator, "number2". 
// eg "3+2...".  After geting number 2, it peeks ahead to look at the next operator.
// If the peeked operator is of a higher precedence, then DoEvalExpression backs
// up before number2, and calls itself to evaluate the term.  Thus "1+2*3" would
// see the "*" and cause DoEvalExpression to be recursivly called with "2*3".  
// The result is the new "number2" and the plus operator is now performed.
//
// if the expression was "1+2*3^4", then the recursion just goes deeper.  Thus
// DoEvalExpression would say: 
// 1. number1 is 1
// 2. operator is +
// 3. number2 is 2
// 4. peeked operator is *, so backup an call self to get number2
// 5. first level of recursive call says number1 is 2
// 6. first level of recursive call says operator is *
// 7. frist level of recursive call says number2 is 3
// 8. peeking next operator, ^, of higher precedence causes another level of recusion
// 9. and so on
// 
// If during a recursive call, a lower precedence operator is encountered,
// the recursion unwinds one level, giving a valid "number2" to the caller.
//
// Parenthesies are handled in GetValue().  Their contents are evaluated
// via a call to EvalExpression() and the result returned by GetNumber() as a
// number. 
//
// String Literals & Variables are also resolved in GetValue()
//

int DoEvalExpression(const unsigned char** expr, int call_level, struct BasicCode* code, struct TypedValue* result)
{
	struct TypedValue x, y,tmp;
	int level, next_level;
	unsigned char op;
	const unsigned char* pushed_expr;
	int err;

	x.value.string = 0;
	y.value.string = 0;

	err = GetValue(expr, code, &x);
	if (err!=ERR_OKAY)
		return err;

	while (TRUE)
	{
		err = GetOperator(expr, &level, FALSE, &op);
		if (err != ERR_OKAY)	// end of expression
		{
			*result = x;
			x.value.string = NULL;	// don't free it
			err = ERR_OKAY;
			(*expr)--;		// fixup GetOperator wich always increments, even if no valid operator
			break;
		}

		pushed_expr = *expr;
		err = GetValue(expr, code, &y);
		if (err!=ERR_OKAY)
			break;

		GetOperator(expr, &next_level, TRUE, 0);	// next_level set to -1 if no operator
		if (next_level > level)
		{
			*expr = pushed_expr;
			err = DoEvalExpression(expr, level, code, &y);
			if (err!=ERR_OKAY)
				break;
		}
		else if (call_level >= next_level )	// e.g -1 OR 0-1 > 7 OR 2   Then the 2nd OR would trigger this fall back to the prior recursion level 
		{						 
			err = DoOperator(&x, &y, op, result);
			break;
		}
		
		err = DoOperator(&x, &y, op, &tmp);
		if (err!=ERR_OKAY)
			break;
		
		if (x.type_flags&VT_STRING && x.value.string)
		{	
			free(x.value.string);
			x.value.string = 0;
		}
		if (y.type_flags&VT_STRING && x.value.string)
		{	
			free(y.value.string);
			y.value.string = 0;
		}

		x = tmp;
	}

	/*
	 * GetValue() can malloc a string.  Free if if it was created.
	 */

	if (x.type_flags&VT_STRING && x.value.string)
	{	
		free(x.value.string);
		x.value.string = 0;
	}

	if (y.type_flags&VT_STRING && x.value.string)
	{	
		free(y.value.string);
		y.value.string = 0;
	}

	return err;
}

//************************

int GetOperator(const unsigned char** expr, int* level, int peek_mode, unsigned char* result)
{
	unsigned char op;
	int err = ERR_OKAY;

	SkipWhiteSpace(expr);
	
	op = **expr;

	if (op==TOK_OR)
		*level=1;
	else if (op==TOK_AND)
		*level=2;
	else if (op==TOK_GT || op==TOK_LT || op==TOK_EQ)
		*level=3;
	else if (op==TOK_ADD || op==TOK_SUB)
		*level = 4;
	else if (op==TOK_MUL || op==TOK_DIV)
		*level = 5;
	else if (op==TOK_EXP)
		*level = 6;
	else
	{
		*level =-1;
		err =  ERR_UNDEFINED_OP;
	}


	/*
	 * Handle two token operators <=, <>, >=
	 */
	if (op == TOK_LT)
	{
		if (*((*expr)+1)==TOK_EQ)
		{
			op = OPR_LE;
			if (!peek_mode)
				(*expr)++;
		}
		else if (*((*expr)+1)==TOK_GT)
		{
			op = OPR_NE;
			if (!peek_mode)
				(*expr)++;
		}
	}
	else if (op == TOK_GT && *((*expr)+1)==TOK_EQ)
	{
		op = OPR_GE;
		if (!peek_mode)
			(*expr)++;
	}

	/*
	 * Handle operators that are syntax errors, but still
	 * supported by L2 BASIC:  =<, =>
	 */
	if (op == TOK_EQ)
	{
		if (*((*expr)+1)==TOK_LT)
		{
			op = OPR_LE;
			if (!peek_mode)
				(*expr)++;
		}
		else if (*((*expr)+1)==TOK_GT)
		{
			op = OPR_GE;
			if (!peek_mode)
				(*expr)++;
		}
	}

	/*
	 * wrap up
	 */

	if (!peek_mode)
		(*expr)++;

	if (result)
		*result = op;
	return err;
}


/*
 * PERFORM X OP Y
 * Handles varius types, as follows:
 * If either operand is string, make sure both are string
 * If either operand is double, then promote other operand to double
 * If either operand is float, then promote other operand to float
 * If both operands are int but operator is /, perform float operators (for float result)
 * If both operands are int, perform int operators
 *
 * NOTE: when types are convertd, the type_flag is not changed (typedvalues of operands are not valid on return).
 */

int DoOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result)
{
	if (x->type_flags&VT_INT && y->type_flags&VT_INT && op==TOK_DIV)
	{
		x->value.floatval = FloatVal(x); 
		y->value.floatval = FloatVal(y);

		return DoFloatOperator(x, y, op, result);
	}
	else if (x->type_flags&VT_INT && y->type_flags&VT_INT)
	{
		return DoIntOperator(x, y, op, result);
	}
	else if (x->type_flags&VT_STRING || y->type_flags&VT_STRING)
	{
		if (!(x->type_flags&VT_STRING && y->type_flags&VT_STRING))
			return ERR_TM;

		return DoStringOperator(x, y, op, result);
	}
	else if (x->type_flags&VT_FLOAT || y->type_flags&VT_FLOAT)
	{
		x->value.floatval = FloatVal(x); 
		y->value.floatval = FloatVal(y);
		x->type_flags=VT_FLOAT;
		y->type_flags=VT_FLOAT;

		return DoFloatOperator(x, y, op, result);
	}
	else if (x->type_flags&VT_DOUBLE || y->type_flags&VT_DOUBLE)
	{
		x->value.doubleval = DoubleVal(x); 
		y->value.doubleval = DoubleVal(y);
		x->type_flags=VT_DOUBLE;
		y->type_flags=VT_DOUBLE;

		return DoDoubleOperator(x, y, op, result);
	}
	else
	{
		return ERR_INTERNAL;		// should never happen
	}
}

int DoStringOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result)
{
	if (op==TOK_ADD)
	{
		short l1, l2;

		l1 = strlen(x->value.string);
		l2 = strlen(y->value.string);

		result->type_flags=VT_STRING;
		result->value.string = (char*)malloc(l1+l2+1);
		if (result->value.string==0)
			return ERR_OS;

		strcpy(result->value.string, x->value.string);
		strcpy(result->value.string+l1, y->value.string);
		return ERR_OKAY;
	}

	int i;

	i = strcmp(x->value.string, y->value.string);
	result->type_flags=VT_INT;
	result->value.intval = 0;	// false

	switch (op)
	{
		case TOK_LT:
			if (i < 0)
				result->value.intval = -1;	// true
			return ERR_OKAY;
		case TOK_GT:
			if (i > 0)
				result->value.intval = -1;	// true
			return ERR_OKAY;
		case TOK_EQ:
			if (i==0)
				result->value.intval = -1;	// true
			return ERR_OKAY;
		case OPR_LE:
			if (i <=0)
				result->value.intval = -1;	// true
			return ERR_OKAY;
		case OPR_GE:
			if (i >=0)
				result->value.intval = -1;	// true
			return ERR_OKAY;
		case OPR_NE:
			if (i!=0)
				result->value.intval = -1;	// true
			return ERR_OKAY;		

		default:
			return ERR_TM;
	}
}

int DoFloatOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result)
{
	result->type_flags=VT_FLOAT;

	switch (op)
	{
		case TOK_ADD:
			result->value.floatval= x->value.floatval+y->value.floatval;
			return ERR_OKAY;
		case TOK_SUB:
			result->value.floatval= x->value.floatval-y->value.floatval;
			return ERR_OKAY;
		case TOK_MUL:
			result->value.floatval= x->value.floatval*y->value.floatval;
			return ERR_OKAY;
		case TOK_DIV:
			if (y->value.floatval==0)
			{
				result->value.floatval = 0;
				return ERR_DIV_ZERO;
			}
			else
			{
				result->value.floatval =  x->value.floatval/y->value.floatval;
				return ERR_OKAY;
			}
		case TOK_EXP:
			result->value.floatval = (float)pow(x->value.floatval, y->value.floatval);
			return ERR_OKAY;
		case TOK_LT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval<y->value.floatval ? -1 : 0;
			return ERR_OKAY;
		case TOK_GT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval>y->value.floatval ? -1 : 0;
			return ERR_OKAY;
		case TOK_EQ:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval==y->value.floatval ? -1 : 0;
			return ERR_OKAY;
		case OPR_LE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval<=y->value.floatval ? -1 : 0;
			return ERR_OKAY;
		case OPR_GE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval>=y->value.floatval ? -1 : 0;
			return ERR_OKAY;
		case OPR_NE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.floatval!=y->value.floatval ? -1 : 0;
			return ERR_OKAY;		
		case TOK_AND:
			result->type_flags=VT_INT;
			result->value.intval= IntVal(x) & IntVal(y);
			return ERR_OKAY;
		case TOK_OR:
			result->type_flags=VT_INT;
			result->value.intval= IntVal(x) | IntVal(y);
			return ERR_OKAY;
		default:
			result->value.floatval = 0.0;
			return ERR_UNDEFINED_OP;
	}
}

int DoDoubleOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result)
{
	result->type_flags=VT_DOUBLE;

	switch (op)
	{
		case TOK_ADD:
			result->value.doubleval= x->value.doubleval+y->value.doubleval;
			return ERR_OKAY;
		case TOK_SUB:
			result->value.doubleval= x->value.doubleval-y->value.doubleval;
			return ERR_OKAY;
		case TOK_MUL:
			result->value.doubleval= x->value.doubleval*y->value.doubleval;
			return ERR_OKAY;
		case TOK_DIV:
			if (y->value.doubleval==0)
			{
				result->value.doubleval= 0;
				return ERR_DIV_ZERO;
			}
			else
			{
				result->value.doubleval=  x->value.doubleval/y->value.doubleval;
				return ERR_OKAY;
			}
		case TOK_EXP:
			result->value.doubleval = pow(x->value.doubleval, y->value.doubleval);
			return ERR_OKAY;
		case TOK_LT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval<y->value.doubleval? -1 : 0;
			return ERR_OKAY;
		case TOK_GT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval>y->value.doubleval? -1 : 0;
			return ERR_OKAY;
		case TOK_EQ:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval==y->value.doubleval? -1 : 0;
			return ERR_OKAY;
		case OPR_LE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval<=y->value.doubleval? -1 : 0;
			return ERR_OKAY;
		case OPR_GE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval>=y->value.doubleval? -1 : 0;
			return ERR_OKAY;
		case OPR_NE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.doubleval!=y->value.doubleval? -1 : 0;
			return ERR_OKAY;		
		case TOK_AND:
			result->type_flags=VT_INT;
			result->value.intval= IntVal(x) & IntVal(y);
			return ERR_OKAY;
		case TOK_OR:
			result->type_flags=VT_INT;
			result->value.intval= IntVal(x) | IntVal(y);
			return ERR_OKAY;
		default:
			result->value.doubleval= 0.0;
			return ERR_UNDEFINED_OP;
	}
}

int DoIntOperator(struct TypedValue* x, struct TypedValue* y, unsigned char op, struct TypedValue* result)
{
	result->type_flags=VT_INT;

	switch (op)
	{
		case TOK_ADD:
			result->value.intval= x->value.intval+y->value.intval;
			return ERR_OKAY;
		case TOK_SUB:
			result->value.intval= x->value.intval-y->value.intval;
			return ERR_OKAY;
		case TOK_MUL:
			result->value.intval= x->value.intval*y->value.intval;
			return ERR_OKAY;
		case TOK_DIV:
			if (y->value.intval==0)
			{
				result->value.intval= 0;
				return ERR_DIV_ZERO;
			}
			else
			{
				result->value.intval=  x->value.intval/y->value.intval;
				return ERR_OKAY;
			}
		case TOK_EXP:
		{
			result->value.intval= 1;
			while (y->value.intval--)
				(result->value.intval)*=x->value.intval;
			return ERR_OKAY;
		}
		case TOK_LT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval<y->value.intval? -1 : 0;
			return ERR_OKAY;
		case TOK_GT:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval>y->value.intval? -1 : 0;
			return ERR_OKAY;
		case TOK_EQ:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval==y->value.intval? -1 : 0;
			return ERR_OKAY;
		case OPR_LE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval<=y->value.intval? -1 : 0;
			return ERR_OKAY;
		case OPR_GE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval>=y->value.intval? -1 : 0;
			return ERR_OKAY;
		case OPR_NE:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval!=y->value.intval? -1 : 0;
			return ERR_OKAY;		
		case TOK_AND:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval & y->value.intval;
			return ERR_OKAY;
		case TOK_OR:
			result->type_flags=VT_INT;
			result->value.intval= x->value.intval | y->value.intval;
			return ERR_OKAY;
		default:
			result->value.intval= 0;
			return ERR_UNDEFINED_OP;
	}
}


//************************

bool uisdigit(unsigned char c)
{
	if (c&0x80)		// isdigit ignores high order bit!
		return FALSE;

	return isdigit(c)!=0;
}

bool ishex(unsigned char c1)
{
	unsigned char c;

	c=tolower(c1);
	if (uisdigit(c))
		return TRUE;
	
	return isalpha(c) && c >='a' && c<='f';
}

float htof(unsigned char c1)
{
	unsigned char c;

	c=tolower(c1);
	if (uisdigit(c))
		return (float)(c-'0');
	else
		return (float)(c-'a'+10);
}

//************************

int GetValue(const unsigned char** expr, struct BasicCode* code, struct TypedValue* result)
{
	float preresult=0;
	bool negative = FALSE;
	bool not = FALSE;

	SkipWhiteSpace(expr);

/*
 * Uniary operators NOT, -
 */
	if (**expr==TOK_SUB)
	{
		negative = TRUE;
		(*expr)++;
		SkipWhiteSpace(expr);
	}
	else if (**expr==TOK_NOT)
	{
		not = TRUE;
		(*expr)++;
		SkipWhiteSpace(expr);
	}

/*
 * Parenthesis evaluate to a value
 */

	if (**expr=='(')
	{
		int err;
		err = EvalParen(expr, code, result);
		if (err!=ERR_OKAY)
			return err;
		if (result->type_flags&VT_STRING)
			return err;
	}
/*
 * Variables evaluate to a value
 */
	else if (isalpha(**expr))
	{
		VariableValue* value;
		unsigned char type;
		unsigned int len;
		int err;

		err = ParseVariable(code, *expr, &value, &type, &len);
		if (err!=ERR_OKAY)
			return err;

		*expr+= len;
		result->type_flags=type;
		CopyVariableValue(&result->value, value);

		if (type&VT_STRING)
		{
			result->value.string = (char*)malloc(strlen(value->string)+1);
			if (result->value.string==NULL)
				return ERR_OS;
			strcpy(result->value.string, value->string);
			result->type_flags=VT_STRING;
			return ERR_OKAY;
		}	
	}

/*
 * Strings literals evaluate to a value
 */
	else if (**expr == '\"')
	{
		unsigned int len;
		int err;

		err=ParseStringLiteral(*expr, result, &len);
		(*expr)+=len;
		return err;
	}

/*
 * Bultin Functions (e.g. Tan() or Left$) eval to a value
 */

	else if (IsBuiltinFunction(**expr))
	{
		unsigned int len;
		int err;

		err=ExecuteBuiltinFunction(code, *expr, result, &len);
		(*expr)+=len;

		if (!IsNumber(result))	// string results return immediatly
			return err;
		else
			if (err!=ERR_OKAY)
				return err;		// errors also terminate now
	}
/*	
 * &Hdddd for hex number
 */
	else if (**expr=='&')
	{
		(*expr)++;

		if (tolower(**expr)!='h')
			return ERR_SYNTAX;

		(*expr)++;

		result->type_flags=VT_FLOAT;
		result->value.floatval=0;

		while (ishex(**expr))
		{
			result->value.floatval = result->value.floatval*16+htof(**expr);
			(*expr)++;
		}

		return ERR_OKAY;
	}
/*	
 * numbers evaluate to a value
 */
	else
	{	
		unsigned int len;
		int err;

		err=ParseNumber(*expr, result, &len, FALSE);
		(*expr)+=len;
		if (err!=ERR_OKAY)
			return err;		
	}

/*
 * apply uniary operators and return
 */

	if (!negative && !not)
		return ERR_OKAY;

	if (result->type_flags&VT_INT)
	{
		if (negative)
			result->value.intval =  -result->value.intval;
		else if (not)
			result->value.intval =  ~result->value.intval;
	}
	else if (not)
	{
		result->value.intval =  ~IntVal(result); 
		result->type_flags=VT_INT;
	}
	else if (result->type_flags&VT_FLOAT)
	{
		if (negative)
			result->value.floatval =  -result->value.floatval;
	}
	else if (result->type_flags&VT_DOUBLE)
	{
		if (negative)
			result->value.doubleval =  -result->value.doubleval;
	}
	else 
		return ERR_INTERNAL;

	return ERR_OKAY;
}

/*
 * ParseNumbeFull
 *   is the same as ParseNumber(), except that it handles leading plus, minus, or spaces
 */

int ParseNumberFull(const unsigned char* expr, struct TypedValue* v, unsigned int* len, bool force_double)
{
	int err;
	unsigned int len2;
	const unsigned char* expr_start = expr;
	bool negative=FALSE;

	SkipWhiteSpace(&expr);
	if (expr[0]=='+' || expr[0]==TOK_ADD)
		expr++;
	else if (expr[0]=='-' || expr[0]==TOK_SUB )
	{
		negative=TRUE;
		expr++;
	}

	err = ParseNumber(expr, v, &len2, force_double);
	if (err!=ERR_OKAY)
		return err;

	if (negative)
	{
		if (v->type_flags&VT_INT)
			v->value.intval =  -v->value.intval;
		else if (v->type_flags&VT_FLOAT)
			v->value.floatval =  -v->value.floatval;
		else if (v->type_flags&VT_DOUBLE)
			v->value.doubleval =  -v->value.doubleval;
		else
			return ERR_INTERNAL;
	}

	*len = len2+(expr - expr_start);

	return ERR_OKAY;
}

/* myatof() - same as atof() except ignores spaces and allows TOK_ADD, TOK_SUB as well as +, - */

double myatof(const unsigned char* a, int len)
{
	unsigned char b[80];
	int i, p=0;

	if (len >= sizeof(b)-1)
		return 0;

	for (i=0; i < len; i++)
	{
		b[p]=a[i];
		if (b[p]==TOK_ADD)
			b[p]='+';
		else if (b[p]==TOK_SUB)
			b[p]='-';
		if (b[p]!=' ')
			p++;		// don't copy spaces
	}

	b[p]=0;

	return atof((char*)b);
}

long myatol(const unsigned char* a, int len)
{
	unsigned char b[80];
	int i, p=0;

	if (len >= sizeof(b)-1)
		return 0;

	for (i=0; i < len; i++)
	{
		b[p]=a[i];
		if (b[p]==TOK_ADD)
			b[p]='+';
		else if (b[p]==TOK_SUB)
			b[p]='-';
		if (b[p]!=' ')
			p++;		// don't copy spaces
	}

	b[p]=0;

	return atol((char*)b);
}

//************************

/* ASCII to TypedValue
 *   INT    if ends in % or no decimal point and inside the allowable range(see FLOAT)
 *   DOUBLE if ends in # or 8 or more digits or force_double set on call
 *   FLOAT  if ends in ! or has a decimal with < 8 digits or has no decimal but is outside
 *          the int range (-32768 to 32767)
 * Does not expect any leading +,- or space
 * White spaces inside the number are ignored.  IE, "3  4" is 34
 */

int ParseNumber(const unsigned char* expr, struct TypedValue* v, unsigned int* len, bool force_double)
{
	bool hasdecimal=FALSE, hasd=FALSE, hase=FALSE;
	short i, digitcount=0;

	if (!(uisdigit(*expr) || *expr=='.'))
		return ERR_SYNTAX;

	for (i=0; uisdigit(expr[i]) || expr[i]=='.' || tolower(expr[i])=='d' || tolower(expr[i])=='e' || expr[i]==' '; i++)
	{
		if (uisdigit(expr[i]))
			digitcount++;
		else if (expr[i]=='.')
			hasdecimal=TRUE;
		else if (tolower(expr[i])=='d')
		{
			hasd=TRUE;
			break;
		}
		else if (tolower(expr[i])=='e')
		{
			hase=TRUE;
			break;
		}
	}

	/* find end (for length) if contains an E or D */
	if (hase || hasd)
	{
		i++;	// skip e or d
		while (expr[i]==' ')	// skip any white space
				i++;
		if (expr[i]==TOK_SUB || expr[i]==TOK_ADD || expr[i]=='-' || expr[i]=='+')
			i++;	// skip +/-
		while (expr[i]==' ')	// skip any white space
			i++;
		while (uisdigit(expr[i]))
			i++;
	}

	if (expr[i]=='#' || digitcount >=8 || hasd || force_double)
		v->type_flags=VT_DOUBLE;
	else if (expr[i]=='%' && hasdecimal)	//eg 2.2% is not allowed
		return ERR_SYNTAX;
	else if (hase)
		v->type_flags=VT_FLOAT;
	else if (expr[i]=='%' || !hasdecimal)
		v->type_flags=VT_INT;
	else 
		v->type_flags=VT_FLOAT;

	
	*len = i;
	if (expr[i]=='#' || expr[i]=='!' || expr[i]=='%')
		*len=*len+1;

	if (v->type_flags==VT_INT)
	{
		long l = myatol(expr, *len);

		if (l >= -32768 && l <= 32767)
			v->value.intval=(short)l;
		else
		{
			v->type_flags=VT_FLOAT;
			v->value.floatval=(float)myatof(expr, *len);
		}
	}
	else if (v->type_flags==VT_FLOAT)
		v->value.floatval=(float)myatof(expr, *len);
	else if (v->type_flags==VT_DOUBLE)
		v->value.doubleval=myatof(expr, *len);
	else
		return ERR_INTERNAL;

	return ERR_OKAY;

}	
	
//************************

int ParseStringLiteral(const unsigned char* expr, struct TypedValue* v, unsigned int* len)
{
	const unsigned char* expr_start = expr;
	unsigned short k;

	if (*expr != '\"')
		return ERR_INTERNAL;

	expr++;
	k=0;
	while (expr[k] && expr[k]!='\"')
		k++;

	v->value.string = (char*)malloc(k+1);
	if (v->value.string == 0)
		return ERR_OS;

	strncpy(v->value.string, (char*)expr, k);
	v->value.string[k] = 0;

	if (len)
	{
		*len = k+2;
		if (expr[k]==0)	   // unterminated double quote is allowed by BASIC L2
			(*len)--;			// Don't skip NULL EOL
	}

	v->type_flags = VT_STRING;

	return ERR_OKAY;
}

//************************

int EvalParen(const unsigned char** expr, struct BasicCode* code, struct TypedValue* result)
{
	int i=0;
	int nest = 1;
	unsigned char* b;
	unsigned int len;
	bool inquote=FALSE;
	int err = ERR_OKAY;

	b = new unsigned char[strlen((const char*)*expr)+1];
		
	while (TRUE)
	{
		(*expr)++;
		if (**expr=='\"')
			inquote=!inquote;

		if (!inquote)
		{
			if (**expr==')')
			{
				nest--;
				if (nest==0)
				{
					(*expr)++;
					b[i]='\0';
					err = EvalExpression(b, &len, code, result);	
					break;
				}
			}
			else if (**expr=='(')
			{
				nest++;
			}
		}

		if (**expr=='\0')
		{
			err = ERR_MISSING_PARN;		//'(' without a matching ')'
			break;
		}
		b[i]=**expr;
		i++;
	}
	delete b;
	return err;
}

//************************

void SkipWhiteSpace(const unsigned char** expr)
{
	while (**expr==' ')
		(*expr)++;
}

