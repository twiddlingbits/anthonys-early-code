
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "adventure.h"
#include "sed1.h"

caseunslave(comcase,rk)
int comcase,rk;
{

   extern char *extlist[MAXCODE];
   extern UBYTE *code;
   extern struct Samp samp[4];
   extern int slave[4][2];
   int a,i,r,f,x,y,x2,y2;
   char *file;

   f = 4;
   if (comcase == 0) {
      file = extlist[code[rk + 1]];
      i = 0;
      while ((a = strcmp(file,samp[i].name)) != 0 && i < 4)
         i++;
      if (i != 4) {
         slave[i][0] = 0;
         slave[i][1] = 0;
         }
      }
   return(f);


}

