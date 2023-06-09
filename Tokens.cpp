//	  awBASIC interpreter (c) 2002 Anthony J. Wood
//    www.awsoftware.org
//
//    This file is part of awbasic.

//    awbasic is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

#include <ctype.h> 		// isdigit
#include <string.h>		// strlen
#include "basic.h"

struct Tokens {
		unsigned char token;
		char* word;
	};

	static struct Tokens TokenList[] = {
		TOK_END, "END",
		TOK_LET, "LET",
		TOK_PRINT, "PRINT",
		TOK_PRINT,"?",			// alternate code for TOK_PRINT.  
		TOK_IF,"IF",
		TOK_ELSE,"ELSE",
		TOK_THEN,"THEN",
		TOK_APOS,"'",
		TOK_REM,"REM",
		TOK_GOTO,"GOTO",	
		TOK_FOR,"FOR",	
		TOK_TO,"TO",		
		TOK_STEP,"STEP",	
		TOK_NEXT,"NEXT",	
		TOK_GOSUB,"GOSUB",	
		TOK_RETURN,"RETURN",	
		TOK_DIM,"DIM",	
		TOK_NOT,"NOT",
		TOK_AND,"AND",
		TOK_OR,"OR",
		TOK_GT,">",
		TOK_LT,"<",
		TOK_EQ,"=",
		TOK_MUL,"*",
		TOK_DIV,"/",
		TOK_ADD,"+",
		TOK_SUB,"-",
		TOK_EXP,"^",
		TOK_EXP,"[",	// TRS80 uses ASCII 91 ( [ ) for power
		TOK_ASC,"ASC",	
		TOK_CHR,"CHR$",		
		TOK_FRE,"FRE",			
		TOK_INKEY,"INKEY$",		
		TOK_LEN,"LEN",			
		TOK_LEFT,"LEFT$",		
		TOK_MID,"MID$",		
		TOK_RIGHT,"RIGHT$",	
		TOK_STR,"STR$",		
		TOK_STRING,"STRING$",	
		TOK_VAL,"VAL",
		TOK_INSTR,"INSTR",
		TOK_ABS,"ABS",	
		TOK_ATN,"ATN",
		TOK_CDBL,"CDBL",
		TOK_CINT,"CINT",
		TOK_COS,"COS",
		TOK_CSNG,"CSGN",
		TOK_FEXP,"EXP",
		TOK_FIX,"FIX",
		TOK_INT,"INT",
		TOK_LOG,"LOG",	
		TOK_RND,"RND",
		TOK_SGN,"SGN",
		TOK_SIN,"SIN",
		TOK_SQR,"SQR",
		TOK_TAN,"TAN",
		TOK_INPUT,"INPUT",
		TOK_CLEAR,"CLEAR",
		TOK_CLS,"CLS",
		TOK_DEFSTR,"DEFSTR",
		TOK_DEFINT,"DEFINT",
		TOK_DEFSGN,"DEFSGN",
		TOK_DEFDBL,"DEFDBL",
		TOK_READ,"READ",
		TOK_DATA,"DATA",
		TOK_RESTORE,"RESTORE",
		TOK_RANDOM,"RANDOM",
		TOK_ERROR,"ERROR",
		TOK_ON,"ON",
		TOK_ERL,"ERL",
		TOK_ERR,"ERR",
		TOK_RESUME,"RESUME",
		TOK_MEM,"MEM",
		TOK_TAB,"TAB(",
		TOK_USING,"USING",
		TOK_POKE,"POKE",
		TOK_PEEK,"PEEK",
		TOK_SET,"SET",
		TOK_RESET,"RESET",
		TOK_POINT,"POINT",
		TOK_STOP,"STOP",
		TOK_POS,"POS",
		TOK_RUN,"RUN",
		TOK_LPRINT,"LPRINT",
		TOK_OUT,"OUT",
		TOK_LINE,"LINE",
		0x00, ""};


/*******************************************************/

char* TokenToAscii(unsigned char findtoken)
{
	for (int i=0; TokenList[i].token; i++)
		if (TokenList[i].token==findtoken)
			return TokenList[i].word;

	return 0;
}

/*******************************************************/

unsigned char GetTokenOrChar(char** source)
{
	if (**source == '\n' || **source == '\r' || **source == NULL)
		return **source;

	for (int i=0; TokenList[i].token; i++)
	{
		if (0==strnicmp(*source, TokenList[i].word, strlen(TokenList[i].word)))
		{
			*source += strlen(TokenList[i].word);
			return TokenList[i].token;
		}
	}
	(*source)++;
	return *(*source-1);
}

/*******************************************************/

bool GetLineNumber(char** source, unsigned short* value)
{
	while (**source==' ' || **source=='\r' || **source=='\n')
		(*source)++;

	if (!isdigit(**source))
		return FALSE;
	
	*value = 0;
	while (isdigit(**source))
	{
		*value = *value*10;
		*value = *value + **source-'0';
		(*source)++;
	}
	return TRUE;
}

void TokenizeQuote(unsigned char** dest, char** src)
{
	while (TRUE)
	{
		(*dest)++;
		**dest = **src;
		(*src)++;

		if (**dest=='\"' || **dest==NULL || **dest == '\n' || **dest == '\r')
			break;
	}
}

void TokenizeRem(unsigned char** dest, char** src)
{
	while (TRUE)
	{
		(*dest)++;
		**dest = **src;
		(*src)++;

		if (**dest==NULL || **dest == '\n' || **dest == '\r')
			break;
	}
}

void TokenizeData(unsigned char** dest, char** src)
{
	while (TRUE)
	{
		(*dest)++;
		**dest = **src;
		(*src)++;

		if (**dest=='\"')
			TokenizeQuote(dest, src);

		if (**dest==NULL || **dest == '\n' || **dest == '\r' || **dest == ':')
			break;
	}
}

void TokenizeLine(unsigned char* dest, char* src, int *destlen, int *sourcelen)
{
	unsigned char* dest_start = dest;
	char* src_start = src;

	do {
		*dest = GetTokenOrChar(&src);

		if (*dest==TOK_REM|| *dest=='\'')
			TokenizeRem(&dest, &src);

		else if (*dest=='\"')
			TokenizeQuote(&dest, &src);

		else if (*dest==TOK_DATA)
			TokenizeData(&dest, &src);

		if (*dest == '\n' || *dest == '\r')
			*dest = NULL;

	} while (*dest++ != NULL);

	*destlen = dest-dest_start;
	*sourcelen=src-src_start;
}

bool Tokenize(char* source, struct BasicCode* code)
{
	unsigned short line_number, last_line_number;
	int image_idx;
	int start_idx;
	int sourcelen;
	int destlen;


	image_idx = 0;

	/*
	 * image format is:
	 * <2 bytes> addr of next statement
	 * <2 bytes> line number in integer form
	 * <n bytes> BASIC statment in tokenized form
	 * <0> End-of-statment marker
	 * repeat as needed
	 *  <0 0> Two bytes of zero mark end
	 */

	while (GetLineNumber(&source, &line_number))
	{
		if (image_idx > 0 && line_number <= last_line_number)
		{
			io_printf1i(code->std_con, "Line number %d is out of sequence\n", line_number);
			return FALSE;
		}
		last_line_number = line_number;

		start_idx = image_idx;
		code->image[image_idx++] = 0;	// pointer to next statement
		code->image[image_idx++] = 0;	// pointer to next statement
		code->image[image_idx++] = (unsigned char)(line_number&0xFF);	
		code->image[image_idx++] = 0xFF&(line_number>>8);

		TokenizeLine(&code->image[image_idx], source, &destlen, &sourcelen);
		image_idx+=destlen;
		source+=sourcelen;

		code->image[start_idx] = image_idx&0xFF;	
		code->image[start_idx+1] = 0xFF&(image_idx>>8);
	};

	code->image[image_idx++] = 0;	// mark end
	code->image[image_idx++] = 0;	// mark end
	
	return TRUE;
}


