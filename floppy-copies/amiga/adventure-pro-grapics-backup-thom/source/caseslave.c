
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "adventure.h"
#include "sed1.h"

caseslave(comcase,rk)
int comcase,rk;
{

   extern char *extlist[MAXCODE];
   extern UBYTE *code;
   extern struct IOAudio IOB[4];
   extern struct Samp samp[4];
   extern int slave[4][2];
   int a,i,f;
   char *file;

   f = 5;
   if (comcase == 0) {
      file = extlist[code[rk + 1]];
      i = 0;
      while ((a = strcmp(file,samp[i].name)) != 0 && i < 4)
         i++;
      if (i != 4) {
         slave[i][0] = code[rk + 2];
         slave[i][1] = code[rk + 3];
         IOB[i].ioa_Volume = code[rk + 4];
         }
      }
   return(f);


}

