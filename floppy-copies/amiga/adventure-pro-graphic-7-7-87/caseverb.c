

#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <lattice/stdio.h>
#include "adventure.h"


caseverb(comcase,pointer,verb)
int *comcase;
char *pointer;
int verb;
{
   int i,comlen;

   *comcase = 1;
   i = 1;
   pointer++;
   while (*pointer  > 0)  {
      if (*pointer  == verb)
         *comcase = 0;
      i++;
      pointer++;
      }
   comlen = i+1;
   return(comlen);

}

