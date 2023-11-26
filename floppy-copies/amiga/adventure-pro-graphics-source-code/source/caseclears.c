
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <stdio.h>
#include <graphics/gfx.h>
#include <devices/audio.h>
#include "adventure.h"
#include "sed1.h"

caseclears(comcase,rk,win1)
int comcase,rk;
struct Window *win1;
{

   extern char *extlist[MAXCODE];
   extern UBYTE *code;
   extern struct IOAudio IOB[4];
   extern struct Samp samp[4];
   int a,i,r,f,x,y,x2,y2;
   char *file;

   f = 2;
   if (comcase == 0) {
      file = extlist[code[rk + 1]];
      i = 0;
      while ((a = strcmp(file,samp[i].name)) != 0 && i < 4)
         i++;
      if (i < 4) {
         wipe_it(&samp[i]);
         deallocate_channel(&IOB[i]);
         }
      }
   return(f);


}

