
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

casetype(comcase,rk,winp1)
int comcase,rk;
struct winprint *winp1;
{

   extern char *typelist[MAXCODE];
   extern UBYTE *code;
   int a,b,f;

   f = 4;
   if (comcase == 0) {
      a = code[rk+1];
      b = code[rk+2]<<8;
      a = a + b;
      printx(winp1,typelist[a]);
      if (code[rk + 3] == 0)
         printx(winp1,"^n");

      }
   return(f);


}

