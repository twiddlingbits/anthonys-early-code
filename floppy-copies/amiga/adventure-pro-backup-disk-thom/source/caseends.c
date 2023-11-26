
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseends(comcase,rk,win1)
int comcase,rk;
struct Window *win1;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int a;

   if (comcase == 0) {
     if (code[rk + 1] == VARIABLE)
        a = varval[code[rk + 2]];
     else
        a = code[rk + 2];
     endsec(a,win1);
     }

   return(3);


}

