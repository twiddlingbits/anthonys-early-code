
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseread(comcase,rk,fp)
int comcase,rk;
FILE *fp;
{

   extern int varval[MAXLIST];
   extern char userfile[10];
   extern UBYTE *code;

   if (comcase == 0) {
      if (userfile[0] != 0)
         fscanf(fp,"%d,",&varval[code[rk + 1]]);
      }

   return(2);


}


