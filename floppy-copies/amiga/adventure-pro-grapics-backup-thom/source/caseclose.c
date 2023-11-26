
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include "adventure.h"

caseclose(comcase,rk,fp)
int comcase,rk;
FILE *fp;
{

   extern char userfile[10];
   extern UBYTE *code;

   if (comcase == 0) {
      if (userfile[0] != 0) {
         fclose(fp);
         userfile[0] = 0;
         }
      }

   return(1);


}


