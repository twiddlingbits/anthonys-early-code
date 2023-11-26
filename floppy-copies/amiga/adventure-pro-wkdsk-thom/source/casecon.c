
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"


casecon(rk)
int rk;
{
   extern UBYTE *code;
   extern struct loc2 rm;


      if (code[rk + 2] == 1)
         rm.n[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 2)
         rm.s[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 3)
         rm.e[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 4)
         rm.w[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 5)
         rm.u[code[rk + 1]] = code[rk + 3];
      if (code[rk + 2] == 6)
         rm.d[code[rk + 1]] = code[rk + 3];

      if (code[rk + 4] == 1)
         rm.n[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 2)
         rm.s[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 3)
         rm.e[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 4)
         rm.w[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 5)
         rm.u[code[rk + 3]] = code[rk + 1];
      if (code[rk + 4] == 6)
         rm.d[code[rk + 3]] = code[rk + 1];


}
