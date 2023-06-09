//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <malloc.h>
#include <string.h>
#include <ctype.h> 	
#include "basic.h"

void SetVariable(union VariableValue* storage, union VariableValue newvalue, unsigned char type)
{
	if (type&VT_STRING)
		free(storage->string);

	CopyVariableValue(storage, &newvalue);
}

unsigned int CountArrayDims(struct BasicCode* code, const unsigned char* expr)
{
	int err;
	unsigned int numdims, len;
	struct TypedValue dummy;

	numdims=0;
	while (TRUE)
	{
		err = EvalExpression(expr, &len, code, &dummy);
		expr+=len;
		if (err!=ERR_OKAY)
			return numdims;

		numdims++;
		SkipWhiteSpace(&expr);
		if (*expr!=',')
			 return numdims;
		expr++;
	}
}


int CalcIndexValues(struct BasicCode* code, const unsigned char* expr, unsigned int* numdims, unsigned int** indexvals, unsigned int* len)
{
	int err, isize;
	unsigned int i, ii;
	struct TypedValue size;
	const unsigned char* expr_start = expr;

	*numdims = CountArrayDims(code, expr);
	*indexvals = (unsigned int*)malloc(*numdims*sizeof(int));
	if (*indexvals==0)
		return ERR_OUTOFMEM;
	for (i=1; i <= *numdims; i++)
	{
		err = EvalExpression(expr, &ii, code, &size);
		expr+=ii;
		if (err!=ERR_OKAY)
			return err;
		if (!IsNumber(&size))
			return ERR_TM;
		isize = IntVal(&size);
		if (isize < 0)
			return ERR_FC;
		(*indexvals)[*numdims-i]=isize;		// put in reverse order, compatible with L2 Basic
		SkipWhiteSpace(&expr);
		if (*expr!=',')
			 if (i!=*numdims)
				return ERR_SYNTAX;
		expr++;
	}

	if (*(expr-1)!=')')
		return ERR_SYNTAX;

	if (len)
		*len = (expr - expr_start);

	return ERR_OKAY;
}

int SimpleParseVariable(unsigned char deftab[], const unsigned char* expr, unsigned char* type, unsigned short* label, unsigned int* len)
{
	const unsigned char* expr_start = expr;

/*
 * First, decode variable label (up to two significant chars)
 */
	SkipWhiteSpace(&expr);
	if (!isalpha(*expr))
		return ERR_SYNTAX;

	*label = tolower(*expr);
	expr++;
	
	if (isalnum(*expr))
	{
		*label = (*label)|(tolower(*expr)<<8);
		expr++;
		while (isalnum(*expr))	// skip end (non-significant) portion of variable name
			expr++;
	}

/*
 * Next, What Type is this variable?
 */
	
	SkipWhiteSpace(&expr);		// Allow A $ 

	if (*expr=='$')
	{
		*type = VT_STRING;
		expr++;
	}
	else if (*expr=='%')
	{
		*type = VT_INT;
		expr++;
	}
	else if (*expr=='#')
	{
		*type = VT_DOUBLE;
		expr++;
	}
	else if (*expr=='!')
	{
		*type = VT_FLOAT;
		expr++;
	}
	else 
		*type = deftab[((*label)&0xFF)-'a'];

	if (len)
		*len = (expr - expr_start);

	return ERR_OKAY;
}

int ParseVariable(struct BasicCode* code, const unsigned char* expr, union VariableValue** value_ptr, unsigned char* type, unsigned int* len)
{
	const unsigned char* expr_start = expr;
	unsigned short label;
	unsigned int i, *ap;
	int err;
/*
 * Parse label and type.  EG A1$
 */
	err=SimpleParseVariable(code->deftab, expr, type, &label, len);
	if (err!=ERR_OKAY)
		return err;
	expr+=*len;
/*
 * Check if this is an array
 */
	SkipWhiteSpace(&expr);		// Allow A $ (4) = "fred"  

	if (*expr=='(')
	{
		(*type)|= VT_ARRAY;
		expr++;
	}

/*
 * Look up the variable to get is storage location.  
 */
	for (i=0; i < MAX_VARIABLES && code->variables[i].label!=0; i++)
	{
		if (code->variables[i].label == label && code->variables[i].value.type_flags==(*type))
			break;
	}

	if (i >= MAX_VARIABLES)
		return ERR_OUTOFMEM;

/*
 * If variable doesn't exist in table, create it.
 */
	if (code->variables[i].label==0)
	{
		code->variables[i].value.type_flags=*type;
		code->variables[i].label=label;
		
		if ((*type)&VT_ARRAY)
		{
			/*
			 * create array stucture with default of DIM A(10, ...,10)
			 * Note that 10 is 0 through 10, inclusive (11 total)
			 * int 0: number of dimensions (n) 
			 * int 1..n: size for each dimension
			 * n X VariableValue's
			 */
			unsigned int numdims,allocsize;
			unsigned int* ap;

			numdims = CountArrayDims(code, expr);

			allocsize = 0;		
			for (unsigned int ii=0; ii < numdims; ii++)
				allocsize = allocsize*(11) + (11);
			allocsize*=sizeof(union VariableValue);
			allocsize+=(1+numdims)*sizeof(int);			

			ap=(unsigned int*)malloc(allocsize);
			if (ap==0)
				return ERR_OUTOFMEM;

			memset(ap, 0, allocsize);

			ap[0]=numdims;
			while (numdims)
			{
				ap[numdims]=11;
				numdims--;
			}

			code->variables[i].value.value.array = ap;

		}
	}
/*
 * Set value_ptr to the variable locations storage.  
 * If this is an array, the calc the correct array entry
 */
	if ((*type)&VT_ARRAY)
	{
		unsigned int index =0;
		unsigned int numdims, *indexvals;

		ap = code->variables[i].value.value.array;

		err = CalcIndexValues(code, expr, &numdims, &indexvals, &i);
		expr+=i;
		if (err!=ERR_OKAY)
		{
			free(indexvals);
			return err;
		}

		for (i=0; i < numdims; i++)
		{
			if (indexvals[i] >= ap[i+1])
			{
				free(indexvals);
				return ERR_BS;
			}

			index = index*ap[i+1]+indexvals[i];
		}

		*value_ptr =  (union VariableValue *)((unsigned char*)ap+sizeof(int)*(numdims+1)+sizeof(union VariableValue)*index);
		free(indexvals);
	}
	else
	{
		*value_ptr =  &code->variables[i].value.value;
	}

	if ((*type)&VT_STRING)	// if this string location uninitilized, initilize it.
	{
		if ((*value_ptr)->string==NULL)
		{
			(*value_ptr)->string = (char*)malloc(1);
			(*value_ptr)->string[0]=0;
		}
	}

/*
 * Calc length of source characters parsed and return
 */
	if (len)
		*len = (expr - expr_start);

	(*type)&= (~VT_ARRAY);		// This routine returns a single typed value, never an "array"

	return ERR_OKAY;
}

int CreateArray(struct Variable vartab[], unsigned short label, unsigned char type, unsigned int* ap)
{
	for (int i=0; i < MAX_VARIABLES && vartab[i].label!=0; i++)
		if (vartab[i].label == label && vartab[i].value.type_flags==(type|VT_ARRAY))
			return ERR_DD;

	if (i >= MAX_VARIABLES)
		return ERR_OUTOFMEM;

	vartab[i].label=label;
	vartab[i].value.type_flags=type|VT_ARRAY;
	vartab[i].value.value.array=ap;
	return ERR_OKAY;
}

short IntVal(struct TypedValue* tv)
{
	if (tv->type_flags&VT_INT)
		return tv->value.intval;

	else if (tv->type_flags&VT_FLOAT)
		return (int)tv->value.floatval;
	
	else if (tv->type_flags&VT_DOUBLE)
		return (int)tv->value.doubleval;
	
	else return 0;	// Should never happen
}

float FloatVal(struct TypedValue* tv)
{
	if (tv->type_flags&VT_FLOAT)
		return tv->value.floatval;

	else if (tv->type_flags&VT_INT)
		return (float)tv->value.intval;
	
	else if (tv->type_flags&VT_DOUBLE)
		return (float)tv->value.doubleval;
	
	else return 0.0;	// Should never happen
}

double DoubleVal(struct TypedValue* tv)
{
	if (tv->type_flags&VT_DOUBLE)
		return tv->value.doubleval;

	else if (tv->type_flags&VT_FLOAT)
		return (double)tv->value.floatval;

	else if (tv->type_flags&VT_INT)
		return (double)tv->value.intval;
	
	else return 0.0;	// Should never happen
}

bool IsNumber(struct TypedValue* tv)
{
	return !(tv->type_flags&VT_STRING);
}

bool IsNumberT(unsigned char type)
{
	return !(type&VT_STRING);
}

bool IsInt(struct TypedValue* tv)
{
	return tv->type_flags&VT_INT;
}

void CopyVariableValue(VariableValue* d, VariableValue* s)
{
	memcpy(d, s, sizeof(VariableValue));
}

void ConvertType(struct TypedValue* v, unsigned char newtype)
{
	if (newtype&VT_INT)
		v->value.intval = IntVal(v);
	else if (newtype&VT_FLOAT)
		v->value.floatval = FloatVal(v);
	else if (newtype&VT_DOUBLE)
		v->value.doubleval=DoubleVal(v);
	else
		return;

	v->type_flags=newtype;
}

// ***************************** OLD CODE
#if 0
float LookupFloat(struct Variable vartab[], struct Variable *v)
{
	for (int i=0; i < MAX_VARIABLES && vartab[i].label!=0; i++)
	{
		if (vartab[i].label == v->label && vartab[i].value.type_flags&VT_FLOAT)
			return vartab[i].value.value.floatval;
	}

	return 0.0;		// un-initilized variables are zero
}

int SetFloat(struct Variable vartab[], struct Variable *v, Variable** vp)
{
	for (int i=0; i < MAX_VARIABLES && vartab[i].label!=0; i++)
	{
		if (vartab[i].label == v->label && vartab[i].value.type_flags&VT_FLOAT)
		{
			vartab[i].value.value.floatval = v->value.value.floatval;
			if (vp)
				*vp = &vartab[i];
			return ERR_OKAY;
		}
	}
	
	if (i < MAX_VARIABLES)
	{
		vartab[i] = *v;
		if (vp)
			*vp = &vartab[i];
		return ERR_OKAY;
	}

	return ERR_OUTOFMEM;
}

char* LookupString(struct Variable vartab[], struct Variable *v)
{
	for (int i=0; i < MAX_VARIABLES && vartab[i].label!=0; i++)
	{
		if (vartab[i].label == v->label && vartab[i].value.type_flags&VT_STRING)
			return vartab[i].value.value.string;
	}

	return "";		// un-initilized variable
}

int SetString(struct Variable vartab[], struct Variable *v)
{
	for (int i=0; i < MAX_VARIABLES && vartab[i].label!=0; i++)
		if (vartab[i].label == v->label && vartab[i].value.type_flags&VT_STRING)
			break;

	if (i < MAX_VARIABLES)
	{
		vartab[i]=*v;		// NOTE: This takes the pointer in v, so it shouldn't be free
		v->value.value.string = NULL;
		return ERR_OKAY;
	}

	return ERR_OUTOFMEM;
}

#endif
