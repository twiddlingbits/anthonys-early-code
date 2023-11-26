
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseds(comcase,rk,win1)
int comcase,rk;
struct Window *win1;
{

   extern int varval[MAXLIST];
   extern UBYTE *code;
   int a,i,r,f,x,y,x2,y2;

   f = 6;
     if (code[rk + 1] == VARIABLE)
        a = varval[code[rk + 2]];
     else
        a = code[rk + 2];
     if (code[rk + 3] == VARIABLE)
        x = varval[code[rk + 4]];
     else {
        x = code[rk + 4];
        x2 = code[rk + 5];
        x = x + (x2<<8);
        f++;
        }
     if (code[rk + f -1] == VARIABLE)
        y = varval[code[rk + f]];
     else {
        y = code[rk + f];
        y2 = code[rk + f + 1];
        y = y + (y2<<8);
        f++;
        }

   if (comcase == 0) {
     startsec(win1,a,x,y);
     }

   return(f+1);


}

