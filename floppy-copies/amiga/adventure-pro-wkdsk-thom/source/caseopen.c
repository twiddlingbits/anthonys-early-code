
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseopen(comcase,rk)
int comcase,rk;
{

   extern char *extlist[MAXLIST];
   extern char userfile[10];
   extern UBYTE *code;
   FILE *fp;

   if (comcase == 0) {
      if (userfile[0] == 0) {
         strncpy(&userfile[0],extlist[code[rk + 1]],10);
         if (code[rk + 2] == 0)
            fp = fopen(&userfile[0],"r");
         else
            fp = fopen(&userfile[0],"w");
         }
      else
         fp = 0;
      }
   else
      fp = 0;

   return(fp);


}

