

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseifvarlt(comcase,comif,rk)
int *comcase,*comif,rk;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int i;


    if (*comcase == 5)
       (*comif)++;
      if (*comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3] + (code[rk + 4]<<8);
      if (varval[code[rk + 1]] >= i) {
         *comif = 0;
         *comcase = 5;
         }
      }

}



caseifvareq(comcase,comif,rk)
int *comcase,*comif,rk;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int i;


    if (*comcase == 5)
       (*comif)++;
      if (*comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3] + (code[rk + 4]<<8);
      if (varval[code[rk + 1]] != i) {
         *comif = 0;
         *comcase = 5;
         }
      }

}



caseifvargt(comcase,comif,rk)
int *comcase,*comif,rk;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int i;


    if (*comcase == 5)
       (*comif)++;
      if (*comcase == 0) {
      if (code[rk + 2] == VARIABLE)
         i = varval[code[rk + 3]];
      else
         i = code[rk + 3] + (code[rk + 4]<<8);
      if (varval[code[rk + 1]] <= i) {
         *comif = 0;
         *comcase = 5;
         }
      }

}

