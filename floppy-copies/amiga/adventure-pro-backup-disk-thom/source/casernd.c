
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casernd(comcase,rk)
int comcase,rk;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;

   if (comcase == 0) {
     do {
        varval[code[rk + 1]] = rand();
        } while(varval[code[rk + 1]] > code[rk + 2]);
     }

   return(3);


}

