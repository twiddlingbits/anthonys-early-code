//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "basic.h"

bool IsBuiltinFunction(const unsigned char token)
{
	switch (token)
	{
	case TOK_ASC:		
	case TOK_CHR:		
	case TOK_FRE:			
	case TOK_INKEY:	
	case TOK_LEN:		
	case TOK_LEFT:	
	case TOK_MID:	
	case TOK_RIGHT:
	case TOK_STR:	
	case TOK_STRING:
	case TOK_VAL:
	case TOK_INSTR:

	case TOK_ABS:	
	case TOK_ATN:
	case TOK_CDBL:
	case TOK_CINT:
	case TOK_COS:
	case TOK_CSNG:
	case TOK_FEXP:
	case TOK_FIX:
	case TOK_INT:
	case TOK_LOG:	
	case TOK_RND:
	case TOK_SGN:
	case TOK_SIN:
	case TOK_SQR:
	case TOK_TAN:
	
	case TOK_ERR:
	case TOK_ERL:
	case TOK_MEM:
	case TOK_PEEK:
	case TOK_POINT:
	case TOK_POS:

		return TRUE;

	default:
		return FALSE;
	}
}

bool IsZeroParam(const unsigned char token)
{
	switch (token)
	{
	case TOK_INKEY:		
	case TOK_MEM:
	case TOK_ERR:
	case TOK_ERL:			
		return TRUE;
	default:
		return FALSE;
	}
}

bool IsOneParam(const unsigned char token)
{
	switch (token)
	{
		
	case TOK_ASC:		
	case TOK_CHR:		
	case TOK_FRE:			
	case TOK_LEN:		
	case TOK_STR:	
	case TOK_VAL:

	case TOK_ABS:	
	case TOK_ATN:
	case TOK_CDBL:
	case TOK_CINT:
	case TOK_COS:
	case TOK_CSNG:
	case TOK_FEXP:
	case TOK_FIX:
	case TOK_INT:
	case TOK_LOG:	
	case TOK_RND:
	case TOK_SGN:
	case TOK_SIN:
	case TOK_SQR:
	case TOK_TAN:

	case TOK_PEEK:
	case TOK_POS:

		return TRUE;

	default:
		return FALSE;
	}
}

int DoAsc(struct TypedValue* param1, struct TypedValue* result)
{
	if (param1->type_flags!=VT_STRING)
		return ERR_TM;

	result->type_flags=VT_INT;
	result->value.intval=((unsigned char*)param1->value.string)[0];
	return ERR_OKAY;
}

int DoChr(struct TypedValue* param1, struct TypedValue* result)
{
	short p1;
	
	if (!IsNumber(param1))
		return ERR_TM;

	p1 = IntVal(param1);
	if (p1 < 0 || p1 > 256)
		return ERR_FC;

	result->type_flags=VT_STRING;
	result->value.string=(char*)malloc(2);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;

	((unsigned char*)result->value.string)[0]=(unsigned char)p1;
	result->value.string[1]=0;

	return ERR_OKAY;
}

int DoFre(struct TypedValue* param1, struct TypedValue* result)
{
	result->type_flags=VT_FLOAT;
	result->value.floatval = 48000;		// Compatability value
	return ERR_OKAY;
}

int DoLen(struct TypedValue* param1, struct TypedValue* result)
{
	if (param1->type_flags!=VT_STRING)
		return ERR_TM;

	result->type_flags=VT_INT;
	result->value.intval=strlen(param1->value.string);
	return ERR_OKAY;
}

int DoStr(struct TypedValue* param1, struct TypedValue* result)
{
	char buffer[80];
	int k;

	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_STRING;

	/* generate with a space or a negative sign before the digits, and no space after the digits. */
	/* this is different than PRINT, which puts a space or negative sign before the digit, and always a space after the digits */
	/* some BASIC code relies on this behaviour (such as animal.bas, which has code like Right$(str$(.... to strip the leading space */

	if (param1->type_flags==VT_DOUBLE)
		k=_snprintf(buffer, sizeof(buffer),"% .14g", param1->value.doubleval);
	else
		k=_snprintf(buffer, sizeof(buffer),"% g", FloatVal(param1));

	if (k < 0)
		return ERR_INTERNAL;
	result->value.string=(char*)malloc(k+1);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;
	strcpy(result->value.string, buffer);
	
	return ERR_OKAY;
}

int DoVal(struct TypedValue* param1, struct TypedValue* result)
{
	if (param1->type_flags!=VT_STRING)
		return ERR_TM;

	result->type_flags=VT_FLOAT;
	result->value.floatval=(float)atof(param1->value.string);
	return ERR_OKAY;
}

int DoAbs(struct TypedValue* param1, struct TypedValue* result)
{
	double p1;
	
	if (!IsNumber(param1))
		return ERR_TM;

	p1 = DoubleVal(param1);

	result->type_flags=VT_DOUBLE;
	result->value.doubleval=fabs(p1);

	return ERR_OKAY;
}

int DoAtn(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval=atan(DoubleVal(param1));

	return ERR_OKAY;
}

int DoCdbl(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = DoubleVal(param1);

	return ERR_OKAY;
}

int DoCint(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_INT;
	result->value.intval = (int)floor(DoubleVal(param1));

	return ERR_OKAY;
}

int DoCos(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = cos(DoubleVal(param1));

	return ERR_OKAY;
}

int DoCsng(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_FLOAT;
	result->value.floatval = FloatVal(param1);

	return ERR_OKAY;
}

int DoExp(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = exp(DoubleVal(param1));

	return ERR_OKAY;
}

int DoFix(struct TypedValue* param1, struct TypedValue* result)
{
	double p1;

	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_INT;
	p1 = DoubleVal(param1);
	if (p1 > 0)
		p1 = floor(p1);
	else
		p1 = ceil(p1);

	result->value.intval = (int)p1;

	return ERR_OKAY;
}

int DoInt(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_INT;
	result->value.intval = IntVal(param1);

	return ERR_OKAY;
}

int DoLog(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = log(DoubleVal(param1));

	return ERR_OKAY;
}

int DoRnd(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	if (IntVal(param1)==0)
	{
		result->type_flags=VT_FLOAT;
		result->value.floatval = (float)rand()/(float)RAND_MAX;
	}
	else
	{
		result->type_flags=VT_INT;
		result->value.intval = (rand()*IntVal(param1))/(RAND_MAX+1)+1;
	}

	return ERR_OKAY;
}

int DoSgn(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_INT;
	float p1 = FloatVal(param1);

	if (p1 <0)
		result->value.intval = -1;
	else if (p1 > 0)
		result->value.intval = 1;
	else
		result->value.intval = 0;

	return ERR_OKAY;
}

int DoFExp(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = exp(DoubleVal(param1));

	return ERR_OKAY;
}

int DoSin(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = sin(DoubleVal(param1));

	return ERR_OKAY;
}

int DoSqr(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = sqrt(DoubleVal(param1));

	return ERR_OKAY;
}

int DoTan(struct TypedValue* param1, struct TypedValue* result)
{
	if (!IsNumber(param1))
		return ERR_TM;

	result->type_flags=VT_DOUBLE;
	result->value.doubleval = tan(DoubleVal(param1));

	return ERR_OKAY;
}

int DoLeft(struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* result)
{
	int len;
	int p2;

	if (param1->type_flags!=VT_STRING)
		return ERR_TM;
	if (!IsNumber(param2))
		return ERR_TM;

	p2 = IntVal(param2);
	if (p2 < 0)
		return ERR_FC;

	len = __min((int)strlen(param1->value.string), p2);
	result->type_flags=VT_STRING;
	result->value.string=(char*)malloc(len+1);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;

	strncpy(result->value.string, param1->value.string, len);
	result->value.string[len]=0;

	return ERR_OKAY;
}

int DoRight(struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* result)
{
	int len,p2;

	if (param1->type_flags!=VT_STRING)
		return ERR_TM;
	if (!IsNumber(param2))
		return ERR_TM;

	p2 = IntVal(param2);
	if (p2 < 0)
		return ERR_FC;

	len = __min((int)strlen(param1->value.string), p2);
	result->type_flags=VT_STRING;
	result->value.string=(char*)malloc(len+1);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;

	strncpy(result->value.string, param1->value.string+strlen(param1->value.string)-len, len);
	result->value.string[len]=0;

	return ERR_OKAY;
}

int DoString(struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* result)
{
	short len;
	unsigned char c;

	if (!IsNumber(param1))
		return ERR_TM;
	len = IntVal(param1);
	if (len < 0 || len > 32000)
		return ERR_FC;

	result->type_flags=VT_STRING;
	result->value.string=(char*)malloc(len+1);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;
	
	if (param2->type_flags==VT_STRING)
		c = ((unsigned char*)param2->value.string)[0];
	else
	{
		int i = IntVal(param2);
		if (i < 0 || i >255)
			return ERR_FC;

		c = (unsigned char)i;
	}

	for (int i=0; i < len; i++)
		((unsigned char*)result->value.string)[i]=c;
	result->value.string[len]=0;
	return ERR_OKAY;
}

/* 
 * MID(string, position, length).  
 * If optional length not given, entire string beginning at position returned
 */
int DoMid(struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* param3, struct TypedValue* result)
{
	int len, p2,p3;

	if (param1->type_flags!=VT_STRING)
		return ERR_TM;

	if (!IsNumber(param2))
		return ERR_TM;
	p2 = IntVal(param2);
	if (p2 < 1)
		return ERR_FC;
	
	if (param3)
	{
		if (!IsNumber(param3))
			return ERR_TM;
		p3 = IntVal(param3);
		if (p3 < 0)
			return ERR_FC;
	}
	else
	{
		p3 = strlen(param1->value.string);	// just a large value to force len to be the rest of string
	}

	p2 = p2-1;
	len = __min((int)strlen(param1->value.string+p2), p3);
	result->type_flags=VT_STRING;
	result->value.string=(char*)malloc(len+1);
	if (result->value.string==NULL)
		return ERR_OUTOFMEM;

	strncpy(result->value.string, param1->value.string+p2, len);
	result->value.string[len]=0;

	return ERR_OKAY;
}


/* 
 * BASIC: INSTR(position, text-to-search$, substring-to-find$). 
 * If position not given, entire string searched
 * Call Convention for this function: DoInstr(text-to-search$, substring-to-find$, position).
 */
int DoInstr(struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* param3, struct TypedValue* result)
{
	int p3;
	char* at;

	if (param1->type_flags!=VT_STRING)
		return ERR_TM;

	if (param2->type_flags!=VT_STRING)
		return ERR_TM;
	
	if (param3)
	{
		if (!IsNumber(param3))
			return ERR_TM;
		p3 = IntVal(param3)-1;
		if (p3 < 0)
			p3=0; 
	}
	else
	{
		p3 = 0; // start at the beginning of string
	}

	if (p3 >= (int)strlen(param1->value.string))
		at = NULL;
	else
		at = strstr(param1->value.string + p3, param2->value.string);



	result->type_flags=VT_INT;
	if (at==NULL)
		result->value.intval = 0;
	else
		result->value.intval = at - param1->value.string+1;

	return ERR_OKAY;
}
/*
 * A$=INKEY$
 *
 * Returns empty string if no key pressed, otherwise returns a string containing
 * the pressed key.  Once a key is returned, it is not returned again on the next call
 * until released and repressed.
 */
int DoInkey(struct BasicCode* code, struct TypedValue* result)
{
	unsigned char c;

	c = io_inkey(code->std_con);

	result->type_flags=VT_STRING;
	if (c==0)
	{
		result->value.string=(char*)malloc(1);
		if (result->value.string==NULL)
			return ERR_OUTOFMEM;
		result->value.string[0]=0;
	}
	else
	{
		result->value.string=(char*)malloc(2);
		if (result->value.string==NULL)
			return ERR_OUTOFMEM;
		((unsigned char*)result->value.string)[0]=c;
		result->value.string[1]=0;
	}

	return ERR_OKAY;
}

int DoMem(struct TypedValue* result)
{
	result->type_flags=VT_FLOAT;
	result->value.floatval=48338;	// ?MEM in a freshly booted L2 Basic returns this 
	return ERR_OKAY;
}


int DoErr(struct BasicCode* code, struct TypedValue* result)
{
	result->type_flags=VT_INT;
	result->value.intval=code->last_err;
	return ERR_OKAY;
}


int DoErl(struct BasicCode* code, struct TypedValue* result)
{
	result->type_flags=VT_FLOAT;
	result->value.floatval=code->last_erl;
	return ERR_OKAY;
}


int DoPeek(struct BasicCode *code, struct TypedValue* param1, struct TypedValue* result)
{
	long p1;
	unsigned char c;
	
	if (!IsNumber(param1))
		return ERR_TM;

	p1 = (long)FloatVal(param1);
	if (p1 < 0 || p1 > 0xFFFF)
		return ERR_FC;

	c=io_peek(code->std_con, (unsigned short)p1);

	result->type_flags=VT_INT;
	result->value.intval=c; 
	return ERR_OKAY;
}

int DoPos(struct BasicCode *code, struct TypedValue* param1, struct TypedValue* result)
{
	result->type_flags=VT_INT;
	result->value.intval=io_get_cursor(code->std_con); 
	return ERR_OKAY;
}

int DoPoint(struct BasicCode *code, struct TypedValue* param1, struct TypedValue* param2, struct TypedValue* result)
{
	short x,y;

	if (!IsNumber(param1) || !IsNumber(param2))
		return ERR_TM;

	x = IntVal(param1);
	y = IntVal(param2);
	if (x <0 || x>=128 || y<0 || y>=48)
		return ERR_FC;

	result->type_flags=VT_INT;
	result->value.intval = io_point(code->std_con, x, y);

	return ERR_OKAY;
}

int ExecuteBuiltinFunction(struct BasicCode* code, const unsigned char* expr, struct TypedValue* result, unsigned int* len)
{
	const unsigned char* expr_start = expr;
	struct TypedValue param1, param2, param3;
	int err;
	unsigned char token;

		
/*
 * No Parameters 
 */
	token = *expr++;

	if (IsZeroParam(token))
	{
		*len=1;

		if (token==TOK_INKEY)
			return DoInkey(code, result);

		else if(token==TOK_ERR)
			return DoErr(code, result);

		else if(token==TOK_ERL)
			return DoErl(code, result);

		else if(token==TOK_MEM)
			return DoMem(result);
		else
			return ERR_INTERNAL;
	}

	SkipWhiteSpace(&expr);
	if (*expr!='(')
		return ERR_SYNTAX;
	expr++;

/*
 * One Parameter
 */
	err = EvalExpression(expr, len, code, &param1);
	expr+=*len;
	if (err!=ERR_OKAY)
		return err;
	SkipWhiteSpace(&expr);

	if (IsOneParam(token))
	{

		if (*expr!=')')
			return ERR_SYNTAX;
		expr++;
		if (len)
			*len = expr - expr_start;

		switch (token)
		{
		case TOK_ASC:
			err = DoAsc(&param1, result);
			break;
		case TOK_CHR:
			err = DoChr(&param1, result);
			break;
		case TOK_FRE:
			err = DoFre(&param1, result);
			break;
		case TOK_LEN:
			err = DoLen(&param1, result);
			break;
		case TOK_STR:
			err = DoStr(&param1, result);
			break;
		case TOK_VAL:
			err = DoVal(&param1, result);
			break;
		case TOK_ABS:	
			err = DoAbs(&param1, result);
			break;
		case TOK_ATN:
			err = DoAtn(&param1, result);
			break;
		case TOK_CDBL:
			err = DoCdbl(&param1, result);
			break;
		case TOK_CINT:
			err = DoCint(&param1, result);
			break;
		case TOK_COS:
			err = DoCos(&param1, result);
			break;
		case TOK_CSNG:
			err = DoCsng(&param1, result);
			break;
		case TOK_FEXP:
			err = DoFExp(&param1, result);
			break;
		case TOK_FIX:
			err = DoFix(&param1, result);
			break;
		case TOK_INT:
			err = DoInt(&param1, result);
			break;
		case TOK_LOG:	
			err = DoLog(&param1, result);
			break;
		case TOK_RND:
			err = DoRnd(&param1, result);
			break;
		case TOK_SGN:
			err = DoSgn(&param1, result);
			break;
		case TOK_SIN:
			err = DoSin(&param1, result);
			break;
		case TOK_SQR:
			err = DoSqr(&param1, result);
			break;
		case TOK_TAN:
			err = DoTan(&param1, result);
			break;
		case TOK_PEEK:
			err = DoPeek(code, &param1, result);
			break;
		case TOK_POS:
			err = DoPos(code, &param1, result);
			break;

		default:
			err = ERR_INTERNAL;	// should never happen
			break;
		}

		if (param1.type_flags&VT_STRING && param1.value.string)
			free(param1.value.string);

		return err;

	}

/*
 * Two Parameters
 */
	if (*expr++!=',')
		return ERR_SYNTAX;

	err = EvalExpression(expr, len, code, &param2);
	expr+=*len;
	if (err!=ERR_OKAY)
		return err;
	SkipWhiteSpace(&expr);

	if (token==TOK_LEFT || token==TOK_RIGHT || token==TOK_STRING || token==TOK_POINT) 
	{
		if (*expr!=')')
			return ERR_SYNTAX;
		expr++;
		if (len)
			*len = expr - expr_start;

		switch(token)
		{
		case TOK_LEFT:
			err = DoLeft(&param1, &param2, result);
			break;
		case TOK_RIGHT:
			err = DoRight(&param1, &param2, result);
			break;
		case TOK_STRING:
			err = DoString(&param1, &param2, result);
			break;
		case TOK_POINT:
			err = DoPoint(code, &param1, &param2, result);
			break;

		default:
			err = ERR_INTERNAL;	// should never happen
			break;
		}

		if (param1.type_flags&VT_STRING && param1.value.string)
			free(param1.value.string);

		if (param2.type_flags&VT_STRING && param2.value.string)
			free(param2.value.string);

		return err;
	}
	else if ((token==TOK_MID || token==TOK_INSTR) && *expr==')')	// MID or INSTR can have two or three params
	{
		expr++;
		if (len)
			*len = expr - expr_start;

		if (token==TOK_MID)
			err = DoMid(&param1, &param2, NULL, result);
		else
			err = DoInstr(&param1, &param2, NULL, result);

		if (param1.type_flags&VT_STRING && param1.value.string)
			free(param1.value.string);

		if (param2.type_flags&VT_STRING && param2.value.string)
			free(param2.value.string);

		return err;
	}

/*
 * three Parameters
 */
	if (*expr++!=',')
		return ERR_SYNTAX;

	err = EvalExpression(expr, len, code, &param3);
	expr+=*len;
	if (err!=ERR_OKAY)
		return err;
	SkipWhiteSpace(&expr);

	if (token==TOK_MID || token==TOK_INSTR) 
	{
		if (*expr!=')')
			return ERR_SYNTAX;
		expr++;
		if (len)
			*len = expr - expr_start;

		if (token==TOK_MID)
			err = DoMid(&param1, &param2, &param3, result);
		else
			err = DoInstr(&param2, &param3, &param1, result);

		if (param1.type_flags&VT_STRING && param1.value.string)
			free(param1.value.string);

		if (param2.type_flags&VT_STRING && param2.value.string)
			free(param2.value.string);

		if (param3.type_flags&VT_STRING && param3.value.string)
			free(param3.value.string);

		return err;
	}

	return ERR_INTERNAL;		// should never get here
}
