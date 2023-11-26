


#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"



caseassign(comcase,rk)
int comcase,rk;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int i,r,t,n1,n2;

     i = 5;
     if (code[rk + 2] == VARIABLE)
        varval[code[rk + 1]] = varval[code[rk + 3]];
     else
        varval[code[rk + 1]] = code[rk + 3] + (code[rk + 4]<<8);

     while (code[rk + i] != 0) {
        if (code[rk + i] <1 || code[rk + i] >4) {
           printf("bad symbolized operator!!");
           exit(10);
           }
        if (code[rk + i + 1] == VARIABLE)
           r = varval[code[rk + i + 2]];
        else
           r = code[rk + i + 2] + (code[rk + i + 3]<<8);
        if (code[rk + i] == 1)
           varval[code[rk + 1]] = varval[code[rk + 1]] + r;
        if (code[rk + i] == 2)
           varval[code[rk + 1]] = varval[code[rk + 1]] - r;
        if (code[rk + i] == 3)
           varval[code[rk + 1]] = varval[code[rk + 1]] * r;
        if (code[rk + i] == 4)
           varval[code[rk + 1]] = varval[code[rk + 1]] / r;
        i = i + 4;
        }
     return(i);


}
